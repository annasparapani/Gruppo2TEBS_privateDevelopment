import sys

from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QPushButton,
    QHBoxLayout,
    QVBoxLayout,
    QWidget
)

# We import library dedicated to data plot
import pyqtgraph as pg
from pyqtgraph import PlotWidget #impory PlotWidget to deal with plots



###############
# MAIN WINDOW #
###############
class MainWindow(QMainWindow): #Parent class QMainWindow, child class MainWindow
    def __init__(self): #init of child class
        """!
        @brief Init MainWindow.
        """
        super(MainWindow, self).__init__() #init of parent class (MainWindow)

        # title and geometry
        self.setWindowTitle("GUI")
        width = 200
        height = 160
        self.setMinimumSize(width, height)

        self.initUI() #standard function to run GUI

    #####################
    # GRAPHIC INTERFACE #
    #####################
    def initUI(self):
        """!
        @brief Set up the graphical interface structure.
        """
        # Create the plot widget
        self.graphWidget = PlotWidget() #class imported previously

        # Define buttons (pulsante / buttone)
        self.clear_btn = QPushButton( #clear button connected to the built-in method from PlotWidget class
            text="Clear",
            clicked=self.graphWidget.clear # .clear() is a method of the PlotWidget class that is activated when the button is clicked
            #clicked is the signal generated when the button is clicked
        )
        self.draw_btn = QPushButton(
            text="Draw",
            clicked=self.draw #instead, for drawing there is no built-in method so we have to define its function
        )

        #Add a third button that allows to update/aa data in graph

        self.add_btn = QPushButton( #It's a QPushButton widget
            text="Add", #keyword for the text shown in the button
            clicked=self.add_data #when clicked, calls the method .add_data()
        )

        # layout
        button_hlay = QHBoxLayout() #horizontal layout
        button_hlay.addWidget(self.clear_btn) #1st layer -> left
        button_hlay.addWidget(self.draw_btn) #2nd layer -> middle
        button_hlay.addWidget(self.add_btn) #3rd layer -> right

        vlay = QVBoxLayout() #vertical layout
        vlay.addLayout(button_hlay) #add horizontal layout as first layer of vertical layout -> up
        vlay.addWidget(self.graphWidget) #add graph as 2nd layer -> down

        widget = QWidget()
        widget.setLayout(vlay) #final layout
        self.setCentralWidget(widget) #set as central widget

        # Some random data
        self.hour = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10] #pack data
        self.temperature1 = [30, 32, 34, 32, 33, 31, 29, 32, 35, 45]
        self.temperature2 = [16, 20, 17, 23, 30, 25, 28, 26, 22, 32]

        # Plot settings
        # It's all really well documented

            # Add grid
        self.graphWidget.showGrid(x=True, y=True)
            # Set background color
        self.graphWidget.setBackground('w')
            # Add title
        self.graphWidget.setTitle("Temperature measurement")
            # Add axis labels
        styles = {'color':'k', 'font-size':'15px'}
        self.graphWidget.setLabel('left', 'Temperature [°C]', **styles)
        self.graphWidget.setLabel('bottom', 'Time [h]', **styles)
            # Add legend
        self.graphWidget.addLegend()

        # Plot data: x, y values
        self.draw() #non-parameters -> empty parentesis
        
    #Define methods called by buttons
    def draw(self): #Define draw method (called when we click on draw_btn) 
        """!
        @brief Draw the plots.
        """
        self.temp1line = self.plot(self.graphWidget, self.hour, self.temperature1, 'Temp 1', 'r')
        self.temp2line = self.plot(self.graphWidget, self.hour, self.temperature2, 'Temp 2', 'b')
        #We can use the same method to plot 2 graphs

    def plot(self, graph, x, y, curve_name, color): #Standard function (can be repeat for any signal to be plotted)
        """!
        @brief Draw graph.
        """
        pen = pg.mkPen(color=color)
        line = graph.plot(x, y, name=curve_name, pen=pen)
        return line

    def add_data(self):

        #Example of random data to be addaded
        self.hour.append(11)
        self.temperature1.append(41)
        self.temperature2.append(34)

        #update plot by calling the EXISTING line
        #update the previous line of data
        self.temp1line.setData(x=self.hour, y=self.temperature1)
        self.temp2line.setData(x=self.hour, y=self.temperature2)


        return

    
#############
#  RUN APP  #
#############

#Launch the application
if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MainWindow()
    w.show() #show the main window since it is hidden by default
    sys.exit(app.exec_())