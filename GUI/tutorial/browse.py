import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog, QLabel, QPushButton

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Create a label to display the selected file
        self.label = QLabel(self)
        self.label.setText("No file selected")
        self.label.move(200, 200)

        # Create a "Browse" button
        self.browse_button = QPushButton("Browse", self)
        self.browse_button.move(20, 60)
        self.browse_button.clicked.connect(self.show_file_dialog)

    def show_file_dialog(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getOpenFileName(self, "QFileDialog.getOpenFileName()", "",
                                                  "All Files (*);;Text Files (*.txt)", options=options)
        if file_name:
            self.label.setText(file_name)

app = QApplication(sys.argv)
window = MainWindow()
window.show()
sys.exit(app.exec_())