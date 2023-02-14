import sys
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (
  QApplication,
  QLabel,
  QMainWindow,
  QPushButton,
  QTabWidget,
  QWidget,
)
from layout_color import Color

class MainWindow(QMainWindow):
  def __init__(self):
    super().__init__()
    self.setWindowTitle("My App")
    tabs = QTabWidget()
    tabs.setTabPosition(QTabWidget.North) #can be West, South or East
    tabs.setMovable(True) #can be false
    for n, color in enumerate(["red", "green", "blue", "yellow"]):
      tabs.addTab(Color(color), color)
      self.setCentralWidget(tabs)

app = QApplication(sys.argv)
window = MainWindow()
window.show()
app.exec_()
