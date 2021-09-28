import sys
import numpy as np
from PyQt5.QtWidgets import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
class MyWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.setLayout(self.layout)
        self.setGeometry(200, 200, 800, 600)
    def initUI(self):
        self.fig = plt.Figure()
        self.canvas = FigureCanvas(self.fig)
        
        layout = QVBoxLayout()
        layout.addWidget(self.canvas)
        cb = QComboBox()
        cb.addItem('Graph1')
        cb.addItem('Graph2')
        cb.activated[str].connect(self.onComboBoxChanged)
        layout.addWidget(cb)
        self.layout = layout
        self.onComboBoxChanged(cb.currentText())
    def onComboBoxChanged(self, text):
        if text == 'Graph1':
            self.doGraph1()
        elif text == 'Graph2':
            self.doGraph2()
    def doGraph1(self):
        x = np.arange(0, 10, 0.5)
        y1 = np.sin(x)
        
        self.fig.clear()
        ax = self.fig.add_subplot(111)
        ax.plot(x, y1, label="sin(x)")
        
        ax.set_xlabel("x")
        ax.set_xlabel("y")
        
        ax.set_title("sin")
        ax.legend()
        
        self.canvas.draw()
    def doGraph2(self):
        X = np.arange(-5, 5, 0.25)
        Y = np.arange(-5, 5, 0.25)
        X, Y = np.meshgrid(X, Y)
        Z = X**2 + Y**2
        
        self.fig.clear()
        
        ax = self.fig.gca(projection='3d')
        ax.plot_wireframe(X, Y, Z, color='black')
        self.canvas.draw() 
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MyWindow()
    window.show()
    app.exec_()