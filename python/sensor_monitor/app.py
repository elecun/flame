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
from PIL.ImageQt import ImageQt

import matplotlib
matplotlib.use('agg')

WORKING_PATH = pathlib.Path(__file__).parent # working path
APP_UI = WORKING_PATH / "MainWindow.ui" # Qt-based UI file
APP_NAME = "vib-sensor-monitor" # application name

def gaussian_spectrogram(x, fs, window_dur=0.005, step_dur=None, dyn_range=120, 
                         cmap=None, ax=None):
    from scipy.signal import spectrogram, gaussian
    from matplotlib.colors import LogNorm
    from matplotlib.cm import get_cmap
    
    # set default for step_dur, if unspecified. This value is optimal for Gaussian windows.
    if step_dur is None:
        step_dur = window_dur / np.sqrt(np.pi) / 8.
    
    # convert window & step durations from seconds to numbers of samples (which is what
    # scipy.signal.spectrogram takes as input).
    window_nsamp = int(window_dur * fs * 2)
    step_nsamp = int(step_dur * fs)
    
    # make the window. A Gaussian filter needs a minimum of 6σ - 1 samples, so working
    # backward from window_nsamp we can calculate σ.
    window_sigma = (window_nsamp + 1) / 6
    window = gaussian(window_nsamp, window_sigma)
    
    # convert step size into number of overlapping samples in adjacent analysis frames
    noverlap = window_nsamp - step_nsamp
    
    # compute the power spectral density
    freqs, times, power = spectrogram(x, detrend=False, mode='psd', fs=fs,
                                      scaling='density', noverlap=noverlap,
                                      window=window, nperseg=window_nsamp)

    p_ref = 2e-5  # 20 μPa, the standard reference pressure for sound in air

    # set lower bound of colormap (vmin) from dynamic range. The upper bound defaults
    # to the largest value in the spectrogram, so we don't need to set it explicitly.
    dB_max = 10 * np.log10(power.max() / (p_ref ** 2))
    vmin = p_ref * 10 ** ((dB_max - dyn_range) / 10)

    # set default colormap, if none specified
    if cmap is None:
        cmap = get_cmap('Greys')
    # or if cmap is a string, get the actual object
    elif isinstance(cmap, str):
        cmap = get_cmap(cmap)

    # create the figure if needed
    if ax is None:
        fig, ax = plt.subplots()

    # other arguments to the figure
    extent = (times.min(), times.max(), freqs.min(), freqs.max())

    # plot
    ax.imshow(power, origin='lower', aspect='auto', cmap=cmap,
              norm=LogNorm(), extent=extent, vmin=vmin, vmax=None)
    return ax

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
            print(f"Sample Frequency : {_fs}")
        
        if "data" in payload.keys():        
            _data = np.array(payload["data"])
            print(f"Data size : {_data.size}")
            _data_mean = _data.mean()
            _normalized_data = _data - _data_mean
            
            fx = np.fft.fft(_normalized_data, n=None, axis=-1, norm=None)/len(_normalized_data)
            amplitude = abs(fx)*2/len(fx)
            frequency = np.fft.fftfreq(len(fx), _ts)
            peak_frequency = frequency[amplitude.argmax()]
            print(f"Peak Frequenct : {peak_frequency}")
            
            fig, ax = plt.subplots(2,1, dpi=200)
            ax[0].plot(_normalized_data, '-')
            ax[0].set_title('Vibration Raw Data')
            ax[0].set_xlabel(f"Time({_ts}sec)")
            ax[0].set_ylabel('Magnitude')
            
            freqs, times, power = signal.spectrogram(_normalized_data, detrend=False, mode='psd', fs=_fs, nperseg=int(_fs/4), scaling='density', noverlap=1)
            c = ax[1].pcolormesh(times, freqs, power, shading='gouraud', cmap='plasma')
            
            ax[1].set_title('Spectogram')
            ax[1].set_xlabel('Time(s)')
            ax[1].set_ylabel('Frequency(Hz)')
            fig.tight_layout()
            fig.canvas.draw()
            
            fig.savefig("test.png")
            
            width, height = fig.canvas.get_width_height()
            image = QImage(fig.canvas.buffer_rgba(), width, height, QImage.Format.Format_RGBA8888)
            
            pixmap = QPixmap.fromImage(image)
            window = self.findChild(QLabel, "canvas")
            window.setPixmap(pixmap.scaled(window.size(), Qt.AspectRatioMode.KeepAspectRatio))
            
            label_fs = self.findChild(QLabel, "value_fs")
            label_fs.setText(_fs)
        
        
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
        
        self.show_on_statusbar(f"Connected to Broker({str(rc)})")
        
    def on_mqtt_disconnect(self, mqttc, userdata, rc):
        self.show_on_statusbar(f"Disconnected to Broker({str(rc)})")
        
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
