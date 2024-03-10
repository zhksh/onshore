import serial
import time
import sys

from PyQt5.QtWidgets import (QApplication, QWidget,
QPushButton, QGridLayout, QLabel, QVBoxLayout)
from PyQt5.QtCore import QTimer


class SubUI(QWidget):

    update_interval = 300    
    baud_rate = 9600
    datadelim = 'DATA|'
    sensor_names = [('Tauchsensor','tb'), ('Temperatur0','tmp0'), 
                    ('Temperatur1','tmp1'), ('Aux','aux')]


    def __init__(self, port):
        super().__init__()
        self.port = port
        self.initUI()


    def connect(self):
        print("Connecting to Arduino on {}".format(self.port))
        try:        
            self.cnx = serial.Serial(self.port, self.baud_rate)
            time.sleep(2)
        except Exception as e:
            print("that didnt work:" + str(e))
            exit(1)


    def setup_loop(self):
        self.qTimer = QTimer()
        self.qTimer.setInterval(self.update_interval) # 1000 ms = 1 s
        self.qTimer.timeout.connect(self.getSensorValues)
        self.qTimer.start()


    def initUI(self):   
        grid = QGridLayout()
        # grid.setRowStretch(0, 2) 
        grid.setColumnStretch(0,2) 
        self.setLayout(grid)
        self.connect()
        self.sensor_map = {}

        self.setFixedSize(700, 480)
        font_val = self.font()
        font_val.setPointSize(30)
        font_label = self.font()
        font_label.setPointSize(15)      

        positions = [(i, j) for i in range(2) for j in range(2)]
        for position, (lname,name) in zip(positions, self.sensor_names):
            val = QLabel("0")
            label = QLabel(lname)
            layout = QVBoxLayout()
            val.setFont(font_val)
            label.setFont(font_label)
            label.setStyleSheet("border-bottom: 1px solid black;")
            layout.addWidget(label)
            layout.addWidget(val)
            grid.addLayout(layout, *position)
            self.sensor_map[name] = val

        self.move(400, 500)
        self.setWindowTitle('SUB Ui')  
        self.show()
        self.setup_loop()


    def getSensorValues(self):
        try:
            pl = self.cnx.readline().decode()
            print(pl)
            decoded = self.decode_(pl)
            if (len(decoded)):
                for s, v in decoded.items(): self.sensor_map[s].setText(v)
        except Exception as e:
            pass

    def decode_(self, bytestr):
        data = {}
        if bytestr.startswith(self.datadelim):
            sv_pairs = bytestr[len(self.datadelim):].rstrip().split(";")
            # print(sv_pairs)
            for pair in sv_pairs: 
                t = tuple(pair.split(":"))            
                data[t[0]] = t[1]
            
        return data


if __name__ == '__main__':
    if len(sys.argv) < 2:
        path = 'COM8'
    else:
        path = sys.argv[1]
    app = QApplication(sys.argv)
    subUI = SubUI(path)
    sys.exit(app.exec_())
    subUI.cnx.close()
