'''
Time-series Data Analyzer Application Window Class
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
import paho.mqtt.client as mqtt
import pyqtgraph as graph

from console import ConsoleLogger

'''
Main Window
'''

class AppWindow(QMainWindow):
    def __init__(self, config:dict):
        super().__init__()
        
        self.__console = ConsoleLogger.get_logger()
        
        self.__frame_win_series_layout = QVBoxLayout()
        self.__frame_win_series_plot = graph.PlotWidget()
        self.__frame_win_fft_layout = QVBoxLayout()
        self.__frame_win_fft_plot = graph.PlotWidget()
        self.__frame_win_spectorgram_layout = QVBoxLayout()
        self.__frame_win_spectogram_plot = graph.PlotWidget()
        
        # model load
        # self.__model = PurgeFanFaultClassification_Resnet("resnet9_pfc.pth")
        # if not self.__model.exist():
        #     QMessageBox.critical(self, "Error", f"Model does not exist")
        
        # local variables
        self.__current_csv_file = None
        
        try:            
            if "gui" in config:
                
                # load gui file
                ui_path = pathlib.Path(config["app_path"]) / config["gui"]
                if os.path.isfile(ui_path):
                    loadUi(ui_path, self)
                else:
                    raise Exception(f"Cannot found UI file : {ui_path}")
                
                # frame window components preparation
                self.__frame_win_series = self.findChild(QFrame, name="frame_series_view")
                self.__frame_win_series_layout.addWidget(self.__frame_win_series_plot)
                self.__frame_win_series_layout.setContentsMargins(0, 0, 0, 0)
                self.__frame_win_series_plot.setBackground('w')
                self.__frame_win_series_plot.showGrid(x=True, y=True)
                self.__frame_win_series.setLayout(self.__frame_win_series_layout)
                
                self.__frame_win_fft = self.findChild(QFrame, name="frame_fft_view")
                self.__frame_win_fft_layout.addWidget(self.__frame_win_fft_plot)
                self.__frame_win_fft_layout.setContentsMargins(0, 0, 0, 0)
                self.__frame_win_fft_plot.setBackground('w')
                self.__frame_win_fft_plot.showGrid(x=True, y=True)
                self.__frame_win_fft.setLayout(self.__frame_win_fft_layout)
                
                self.__frame_win_spectogram = self.findChild(QFrame, name="frame_spectogram_view")
                self.__frame_win_spectorgram_layout.addWidget(self.__frame_win_spectogram_plot)
                self.__frame_win_spectorgram_layout.setContentsMargins(0, 0, 0, 0)
                self.__frame_win_spectogram_plot.setBackground('w')
                self.__frame_win_spectogram.setLayout(self.__frame_win_spectorgram_layout)

                
        except Exception as e:
            self.__console.critical(f"{e}")
            
        # member variables
        self.__configure = config   # configure parameters
        
    # clear all guis
    def clear_all(self):
        try:
            self.__frame_win_series_plot.clear()
            self.__frame_win_fft_plot.clear()
            self.__frame_win_spectogram_plot.clear()
            
        except Exception as e:
            self.__console.critical(f"{e}")

        
    def closeEvent(self, a0: QCloseEvent | None) -> None:
        
        self.__console.info("Terminated Successfully")
        return super().closeEvent(a0)