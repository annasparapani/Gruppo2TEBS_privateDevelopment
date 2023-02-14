# Only needed for access to command line arguments
import sys

import random

from PyQt5 import QtCore 

# We import the PyQt5 classes that we need for the application
# from the QtWidgets module
from PyQt5.QtWidgets import ( #dynamic list (if we need a new package during the GUI development, we need to add here)
    QApplication, #Event loop application
    QLabel,
    QMainWindow,
    QPushButton,
    QVBoxLayout,
    QHBoxLayout,
    QWidget,
) #Each object used in the GUI must be imported


###############
# MAIN WINDOW #
###############
# This is a pre-made widget which provides a lot of standard window 
# features you’ll make use of in your apps, including toolbars, menus, 
# a statusbar, dockable widgets and more.

class MainWindow(QMainWindow): #QMainWindow is the widget imported previously 
    def __init__(self): #child class initialization
        """!
        @brief Init MainWindow.
        """
        # If you want to create a custom window, the best approach is 
        # to subclass QMainWindow and then include the setup for the 
        # window in this __init__ block.

        super(MainWindow, self).__init__() #parent class initialization (inheritance) -> needs MainWindow and self as parameters

        # title and geometry
        self.setWindowTitle("GUI")
        width = 300
        height = 300
        self.setMinimumSize(width, height)

        self.initUI() #standard function to define the graphic structure, function defined below

    #####################
    # GRAPHIC INTERFACE #
    #####################
    def initUI(self):
        """!
        @brief Set up the graphical interface structure.
        """
        # Define buttons (pulsante)
        self.plus_btn = QPushButton( #Button is from the class QPushButton (must be imported in the beginning)
            text="+1", #text of the button
            clicked=self.add_one #there is always the "self" before attributes and methods due to the need of reference
            #clicked is a signal generated when we click in the button
        )
        
        # alternatively, signals can be connected in this way (to activate it, uncomment next line and comment previous comand clicked=...)
        # self.plus_btn.clicked.connect(self.add_one)
        
        self.minus_btn = QPushButton(
            text="-1",
            clicked=self.remove_one #function remove_one is ran after clicking the button
        )

        # alternatively, signals can be connected in this way
        # self.plus_btn.clicked.connect(self.add_one)
        
        # Define lable (etichetta) to visualize numbers
        self.label_number = int(random.random()*100)
        self.display_label = QLabel( #QLabel is object from class that must be imported previously
            str(self.label_number), #content (value) of label
            alignment=QtCore.Qt.AlignCenter #layout (alignment) of label
        )

        # layout (design)
        #we have to create the horizontal and the vertical layout
        button_hlay = QHBoxLayout() #Horizontal Box Layout
        button_hlay.addWidget(self.plus_btn) #first -> left
        button_hlay.addWidget(self.minus_btn) #second -> right

        vlay = QVBoxLayout() #Vertical Box Layout
        vlay.addLayout(button_hlay) #1st -> above: add horizontal layout in the vertical layout
        vlay.addWidget(self.display_label) #2nd -> below
        widget = QWidget()
        widget.setLayout(vlay) #assign vertical and horizontal layout to the final page layout
        
        #.setCentralWidget is a QMainWindow specific function that 
        # allows you to set the widget that goes in the middle of the window.
        self.setCentralWidget(widget)

    def add_one(self):
        """!
        @brief Add 1 to the number displayed in the label.
        """
        self.label_number = self.label_number+1 #add 1 to label_number attribute
        self.display_label.setText(str(self.label_number)) #change label text; setText() is a method from class QLabel

    def remove_one(self):
        """!
        @brief Subtract 1 to the number displayed in the label.
        """
        self.label_number = self.label_number-1 #subtract 1 from label_number attribute
        self.display_label.setText(str(self.label_number)) #change label text; setText() is a method from class QLabel


#############
#  RUN APP  #
#############
if __name__ == '__main__':
    # You need one (and only one) QApplication instance per application.
    # Pass in sys.argv to allow command line arguments for your app.
    # If you know you won't use command line arguments QApplication([])
    # works too.

    app = QApplication(sys.argv) #Instance of QApplication
    
    # Create a Qt widget, which will be our window.
    w = MainWindow()
    w.show() # IMPORTANT!!!!! Windows are hidden by default, so we always need to apply the method .show()
    
    # Start the event loop.
    sys.exit(app.exec_())

    # Your application won't reach here until you exit and the event
    # loop has stopped.