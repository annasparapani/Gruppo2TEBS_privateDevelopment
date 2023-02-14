import sys
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QApplication, QMainWindow, QSpinBox, QDoubleSpinBox
class MainWindow(QMainWindow):
  def __init__(self):
    super().__init__()
    self.setWindowTitle("My App")
    widget = QDoubleSpinBox()
    # Or: widget = QSpinBox()
    widget.setMinimum(-10)
    widget.setMaximum(3)
    # Or: widget.setRange(-10,3)
    widget.setPrefix("$")
    widget.setSuffix("c")
    widget.setSingleStep(0.5) # Or e.g. 0.5 for QDoubleSpinBox
    widget.valueChanged.connect(self.value_changed)
    widget.valueChanged[str].connect(self.value_changed_str)
    self.setCentralWidget(widget)
  def value_changed(self, i):
    print(i)
  def value_changed_str(self, s):
    print(s)  
app = QApplication(sys.argv)
window = MainWindow()
window.show()
app.exec_()