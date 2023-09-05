'''
Sensor Monitor App for Vibration Monitor
@author bh.hwang@iae.re.kr
'''

import sys, os
import typing
from PyQt6 import QtCore, QtGui
import pathlib
import json
from PyQt6.QtGui import QImage, QPixmap, QCloseEvent, QStandardItem, QStandardItemModel, QIcon, QColor
from PyQt6.QtWidgets import QApplication, QMainWindow, QTableView, QLabel, QPushButton, QMessageBox, QWidget
from PyQt6.QtWidgets import QFileDialog
from PyQt6.uic import loadUi
from PySide6.QtCharts import QChart, QChartView, QLineSeries
from PyQt6.QtCore import QModelIndex, QObject, Qt, QTimer, QThread, pyqtSignal, QAbstractTableModel
import timeit
import paho.mqtt.client as mqtt
from datetime import datetime
import csv
import math
import argparse
import numpy as np
import matplotlib.pyplot as plt
import io
from PIL import Image
from scipy import signal

WORKING_PATH = pathlib.Path(__file__).parent # working path
APP_UI = WORKING_PATH / "MainWindow.ui" # Qt-based UI file
APP_NAME = "vib-sensor-monitor" # application name


class SensorMonitor(QMainWindow):
    def __init__(self, broker_ip:str):
        super().__init__()
        loadUi(APP_UI, self)
        
        self.canvas = {}
        self.message_api = {
            "flame/sensor/receive_data" : self.mapi_receive_data
        }
        
        self.mq_client = mqtt.Client(client_id=APP_NAME,transport='tcp',protocol=mqtt.MQTTv311, clean_session=True)
        self.mq_client.on_connect = self.on_mqtt_connect
        self.mq_client.on_message = self.on_mqtt_message
        self.mq_client.on_disconnect = self.on_mqtt_disconnect
        self.mq_client.connect_async(broker_ip,port=1883,keepalive=60)
        self.mq_client.loop_start()
    
    # data received
    def mapi_receive_data(self, payload:dict):
        
        # compute fft
        if "fs" in payload.keys():
            _fs = payload["fs"]
            _ts = 1/_fs
        
        if "data" in payload.keys():
            _data = np.array(payload["data"])
            _data_mean = _data.mean()
            _normalized_data = _data - _data_mean
            
            fx = np.fft.fft(_normalized_data, n=None, axis=-1, norm=None)/len(_normalized_data)
            amplitude = abs(fx)*2/len(fx)
            frequency = np.fft.fftfreq(len(fx), _ts)
            peak_frequency = frequency[amplitude.argmax()]
            print(f"Peak Frequenct : {peak_frequency}")
            
            plt.clf()
            plt.subplot(2, 1, 1)                # nrows=2, ncols=1, index=1
            plt.plot(_normalized_data, '-')
            plt.title('Vibration Raw Data')
            plt.xlabel(f"Time({_ts}sec)")
            plt.ylabel('Magnitude')

            plt.subplot(2, 1, 2)                # nrows=2, ncols=1, index=2
            f, tt, Sxx = signal.spectrogram(_normalized_data, fs=_fs)
            plt.pcolormesh(tt, f, Sxx, shading='gouraud')
            plt.title('Spectogram')
            plt.xlabel('Time(s)')
            plt.ylabel('Frequency(Hz)')
            plt.tight_layout()
            #plt.show()
            
            buf = io.BytesIO()
            plt.savefig(buf, format='png')
            buf.seek(0)
            im = Image.open(buf)
            buf.close()
            
            pixmap = QImage(im, 800, 600, QImage.Format.Format_ARGB32)
            self.canvas.setPixmap(pixmap)
            self.setCentralWidget(self.canvas)
            self.resize(pixmap.width(), pixmap.height())            
        
        
     # show message on status bar
    def show_on_statusbar(self, text):
        self.statusBar().showMessage(text)
        
    # update image frame on label area
    def update_frame(self, image):
        pixmap = QPixmap.fromImage(image)
        window = self.findChild(QLabel, "canvas")
        window.setPixmap(pixmap.scaled(window.size(), Qt.AspectRatioMode.KeepAspectRatio))

    # MQTT callbacks
    def on_mqtt_connect(self, mqttc, obj, flags, rc):
        
        # subscribe message api
        for topic in self.message_api.keys():
            self.mq_client.subscribe(topic, 0)
        
        self.show_on_statusbar("Connected to Broker({})".format(str(rc)))
        
    def on_mqtt_disconnect(self, mqttc, userdata, rc):
        self.show_on_statusbar("Disconnected to Broker({})".format(str(rc)))
        
    def on_mqtt_message(self, mqttc, userdata, msg):
        mapi = str(msg.topic)
        
        try:
            if mapi in self.message_api.keys():
                payload = json.loads(msg.payload)
                if "data" in payload:
                    self.message_api[mapi](payload)
                else:
                    print("Message payload does not contain the data key")
                    return
            else:
                print("Unknown MAPI was called : {}".format(mapi))
        except json.JSONDecodeError as e:
            print("MAPI message payload connot be converted : {}".format(str(e)))
        

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--broker', nargs='?', required=False, help="Broker Address")
    args = parser.parse_args()
    
    broker_address = "127.0.0.1"
    if args.broker is not None:
        broker_address = args.broker

    app = QApplication(sys.argv)
    window = SensorMonitor(broker_address)
    window.show()
    sys.exit(app.exec())
