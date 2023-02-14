import sys
import time

from PyQt5.QtCore import ( #logging library (to perform multithreading)
    QObject,
    QThreadPool, 
    QRunnable,  
    pyqtSignal, 
    pyqtSlot,
    QTimer
) #tools to manage the multithreading

from PyQt5.QtWidgets import (
  QApplication,
  QLabel,
  QMainWindow,
  QPushButton,
  QVBoxLayout,
  QWidget,
)

class Worker(QRunnable):
  """
  Worker thread
  :param args: Arguments to make available to the run code
  :param kwargs: Keywords arguments to make available to the run
  :code
  :
  """
  def __init__(self, *args, **kwargs):
    super().__init__()
    self.args = args
    self.kwargs = kwargs

  @pyqtSlot()
  def run(self):
    """
    Initialize the runner function with passed self.args,
    self.kwargs.
    """
    print(self.args, self.kwargs)

class MainWindow(QMainWindow):
  def __init__(self):
    super().__init__()

    self.threadpool = QThreadPool()
    print(  "Multithreading with maximum %d threads" % self.threadpool.maxThreadCount())

    self.counter = 0
    layout = QVBoxLayout()
    self.l = QLabel("Start")
    b = QPushButton("DANGER!")
    b.pressed.connect(self.oh_no)
    layout.addWidget(self.l)
    layout.addWidget(b)
    w = QWidget()
    w.setLayout(layout)
    self.setCentralWidget(w)
    self.show()
    self.timer = QTimer()
    self.timer.setInterval(1000)
    self.timer.timeout.connect(self.recurring_timer)
    self.timer.start()

  def oh_no(self):
    worker = Worker("some", "arguments", keywords=2)
    self.threadpool.start(worker)

  def recurring_timer(self):
    self.counter += 1
    self.l.setText("Counter: %d" % self.counter)

app = QApplication(sys.argv)
window = MainWindow()
app.exec_()
