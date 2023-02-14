
from PyQt5.QtGui import QColor, QPalette
from PyQt5.QtWidgets import QWidget

class Color(QWidget):
  def __init__(self, color):
    super().__init__()
    self.setAutoFillBackground(True)
    palette = self.palette()
    palette.setColor(QPalette.Window, QColor(color))
    self.setPalette(palette)

import sys
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QApplication, QMainWindow
#from layout_colorwidget import Color

class MainWindow(QMainWindow):
  def __init__(self):
    super().__init__()
    self.setWindowTitle("My App")
    widget = Color("red")
    self.setCentralWidget(widget)
    
app = QApplication(sys.argv)
window = MainWindow()
window.show()
app.exec_()