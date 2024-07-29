'''
DK H Inspetor Monitor
@author Byunghun Hwang<bh.hwang@iae.re.kr>
'''

import sys, os
import pathlib
try:
    # using PyQt5
    from PyQt5.QtGui import QImage, QPixmap, QCloseEvent, QStandardItemModel, QStandardItem
    from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QPushButton, QMessageBox, QFileDialog, QFrame, QVBoxLayout, QComboBox, QLineEdit, QCheckBox
    from PyQt5.uic import loadUi
    from PyQt5.QtCore import QObject, Qt, QTimer, QThread, pyqtSignal
except ImportError:
    # using PyQt6
    from PyQt6.QtGui import QImage, QPixmap, QCloseEvent, QStandardItemModel, QStandardItem
    from PyQt6.QtWidgets import QApplication, QMainWindow, QLabel, QPushButton, QMessageBox, QFileDialog, QFrame, QVBoxLayout, QComboBox, QLineEdit, QCheckBox
    from PyQt6.uic import loadUi
    from PyQt6.QtCore import QObject, Qt, QTimer, QThread, pyqtSignal
    
from datetime import datetime
from PIL import ImageQt, Image
from sys import platform
# import paho.mqtt.client as mqtt
import pyqtgraph as graph
import zmq
import json
import threading

from console import ConsoleLogger

'''
Main Window
'''

class AppWindow(QMainWindow):
    def __init__(self, config:dict):
        super().__init__()
        
        self.__console = ConsoleLogger.get_logger()
        
        self.__frame_win_defect_layout = QVBoxLayout()
        self.__frame_win_defect_plot = graph.PlotWidget()

        # op trigger test simulation pipeline
        self.op_trigger_context = zmq.Context()
        self.op_trigger_socket = self.op_trigger_context.socket(zmq.PUB)
        self.op_trigger_socket.setsockopt(zmq.SNDHWM, 1000)
        self.op_trigger_socket.bind("tcp://*:5008")

        # camera monitoring worker thread
        cam_monitor_thread = threading.Thread(target=self.cam_view_monitoring)
        cam_monitor_thread.start()
        
        
        try:            
            if "gui" in config:
                
                # load gui file
                ui_path = pathlib.Path(config["app_path"]) / config["gui"]
                if os.path.isfile(ui_path):
                    loadUi(ui_path, self)
                else:
                    raise Exception(f"Cannot found UI file : {ui_path}")
                
                # frame window components preparation
                self.__frame_win_defect = self.findChild(QFrame, name="frame_defect_view")
                self.__frame_win_defect_layout.addWidget(self.__frame_win_defect_plot)
                self.__frame_win_defect_layout.setContentsMargins(0, 0, 0, 0)
                self.__frame_win_defect_plot.setBackground('w')
                self.__frame_win_defect_plot.showGrid(x=True, y=True)
                self.__frame_win_defect.setLayout(self.__frame_win_defect_layout)

                # button component connection
                self.btn_op_trigger_on.clicked.connect(self.on_click_op_trigger_on)
                self.btn_op_trigger_off.clicked.connect(self.on_click_op_trigger_off)

                
        except Exception as e:
            self.__console.critical(f"{e}")
            
        # member variables
        self.__configure = config   # configure parameters
        
    # clear all guis
    def clear_all(self):
        try:
            self.__frame_win_defect_plot.clear()

            self.op_trigger_socket.close()
            self.op_trigger_context.term()
            
        except Exception as e:
            self.__console.critical(f"{e}")

        
    def closeEvent(self, a0: QCloseEvent | None) -> None:
        
        self.__console.info("Terminated Successfully")
        return super().closeEvent(a0)
    

    def __send_op_trigger_request(self, topic:str, msgdata:dict) -> None:
        try:
            json_data = json.dumps(msgdata)
            self.op_trigger_socket.send_multipart([topic.encode(), json_data.encode()])
        except json.JSONDecodeError as e:
            print(f"json parse error : {e}")
        

    # trigger on button event
    def on_click_op_trigger_on(self):
        msg = {"op_trigger": True }
        self.__send_op_trigger_request("simulation", msg)
        print("Trigger ON")

    def on_click_op_trigger_off(self):
        msg = {"op_trigger": False }
        json_data = json.dumps(msg)
        self.__send_op_trigger_request("simulation", msg)
        print("Trigger OFF")

    
    # camera monitoring thread function
    def cam_view_monitoring(self):
        cam_monitor_context = zmq.Context()
        cam_monitor_socket = cam_monitor_context.socket(zmq.SUB)
        cam_monitor_socket.setsockopt(zmq.RCVHWM, 5000)
        cam_monitor_socket.setsockopt_string(zmq.SUBSCRIBE, "basler_gige_cam_linker/image_stream_monitor")
        cam_monitor_socket.connect("tcp://192.168.0.50:5557")

        count = 0
        try:
            while True:
                print(f"camera image received : {count}")
                count = count + 1
        except KeyboardInterrupt:
            print("Interrupted")
        finally:
            cam_monitor_socket.close()
            cam_monitor_context.term()