import sys # Only needed for access to command line arguments

import time

import logging

import traceback


from PyQt5 import QtCore
from PyQt5.QtCore import ( #logging library (to perform multithreading)
    QObject,
    QThreadPool, 
    QRunnable,  
    pyqtSignal, 
    pyqtSlot
) #tools to manage the multithreading

from PyQt5.QtWidgets import (
    QApplication, # Application handler: You need one (and only one) QApplication instance per application.
    QMainWindow,
    QPushButton,
    QComboBox,
    QHBoxLayout,
    QVBoxLayout,
    QGridLayout,
    QTabWidget,
    QDoubleSpinBox,
    QSpinBox,
    QLabel,
    QLCDNumber,
    QFileDialog,
    QWidget, # Basic empty GUI widget: Create a Qt widget, which will be our window.
)

from layout_color import Color #color palette

from PyQt5.QtGui import QIcon #Icons

import serial #Serial Communication Protocol 
import serial.tools.list_ports #list all COM ports

import pyqtgraph as pg #Plotting tool
from pyqtgraph import PlotWidget #impory PlotWidget to deal with plots

import numpy as np
import scipy 
from scipy.signal import find_peaks

from datetime import datetime

from scipy.signal import savgol_filter


# Globals
CONN_STATUS = False


# Logging config -> equivalent to print() but for multithreading
logging.basicConfig(format="%(message)s", level=logging.INFO)


#########################
# SERIAL_WORKER_SIGNALS #
#########################
class SerialWorkerSignals(QObject): #parent class QObject, child class SerialWorkerSignals
    """!
    @brief Class that defines the signals available to a serialworker.

    Available signals (with respective inputs) are:
        - device_port:
            str --> port name to which a device is connected
        - status:
            str --> port name
            int --> macro representing the state (0 - error during opening, 1 - success)
    """

    #Define 2 signals
    device_port = pyqtSignal(str) #COM PORT where the PSOC is connected
    status = pyqtSignal(str, int) #pyqtSignal


#################
# SERIAL_WORKER #
#################
class SerialWorker(QRunnable): #Open serial port; QRunnable is parent class, SerialWorker is our child class
    """!
    @brief Main class for serial communication: handles connection with device.
    """
    def __init__(self, serial_port_name): #we will set the serial_port_name when creating the object SerialWorker
        """!
        @brief Init worker.
        """
        self.is_killed = False # global Boolean variable to handle multi-threading (see slides from lecture) -> it terminates QRunnable subclasses when it turns TRUE
        super().__init__()
        # init port, params and signals
        self.port = serial.Serial()
        self.port_name = serial_port_name
        self.baudrate = 9600 # hard coded but can be a global variable, or an input param
        self.signals = SerialWorkerSignals()

    @pyqtSlot() #Slot in multithreading application -> receive signals
    def run(self): #What the parallel thread does MUST be inside the run method
        """!
        @brief Estabilish connection with desired serial port.
        """
        global CONN_STATUS

        if not CONN_STATUS: #if not connected, try to connect to serial port
            try:
                self.port = serial.Serial(port=self.port_name, baudrate=self.baudrate,
                                        write_timeout=0, timeout=2)
                time.sleep(0.1)                 
                if self.port.is_open: #if the port is open
                    CONN_STATUS = True
                    self.signals.status.emit(self.port_name, 1) 
                    time.sleep(0.01)     
            except serial.SerialException:
                logging.info("Error with port {}.".format(self.port_name))
                self.signals.status.emit(self.port_name, 0)
                time.sleep(0.01)

    @pyqtSlot()
    def send(self, char):
        """!
        @brief Basic function to send a single char (byte) on serial port.
        """
        try:
            self.port.write(char)
            logging.info("Written {} on port {}.".format(char, self.port_name))
        except:
            logging.info("Could not write {} on port {}.".format(char, self.port_name))

    @pyqtSlot()
    def read(self, data_size):
        """!
        @brief Basic function to read a single char on serial port.
        """
        try:
            char = self.port.read(data_size)
            logging.info("Read {} on port {}.".format(char, self.port_name))
            return char
            
        except:
            logging.info("Could not read on port {}.".format(self.port_name))

        #We need to call the method read() outside this function as: self.serial_worker.read()

   
    @pyqtSlot()
    def killed(self):
        """!
        @brief Close the serial port before closing the app.
        """
        global CONN_STATUS
        if self.is_killed and CONN_STATUS:
            self.port.close()
            time.sleep(0.01)
            CONN_STATUS = False
            self.signals.device_port.emit(self.port_name)

        logging.info("Killing the process SerialWorker")


#########################
# READ_WORKER_SIGNALS #
#########################
class Read_WorkerSignals(QObject):
  """
  Defines the signals available from a running worker thread.
  Supported signals are:
  finished:  No data
  error:  `tuple` (exctype, value, traceback.format_exc() )
  result:  `object` data returned from processing, anything
  data: tuple data point (x, y)
  """
  finished = pyqtSignal()
  error = pyqtSignal(tuple)
  result = pyqtSignal(object)
  data = pyqtSignal(tuple)

#################
# READ_WORKER #
#################
class ReadWorker(QRunnable):
    """
    Handles with the idle state of the GUI while waiting for finish of CV or CA
    :param callback: The function callback to run on this worker
    :thread. Supplied args and kwargs will be passed through to the runner.
    :type callback: function
    :param args: Arguments to pass to the callback function
    :param kwargs: Keywords to pass to the callback function
    :
    """
    def __init__(self, fn, *args, **kwargs):

        self.is_killed = False

        super().__init__()
        # Store constructor arguments (re-used for processing)
        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = Read_WorkerSignals()
        # Add the callback to our kwargs
        kwargs["signals"] = self.signals

    @pyqtSlot()
    def run(self):
        """
        Initialize the runner function with passed args, kwargs.
        """
        # Retrieve args/kwargs here; and fire processing using them
        try:
            result = self.fn(*self.args, **self.kwargs)
        except Exception:
            traceback.print_exc()
            exctype, value = sys.exc_info()[:2]
            self.signals.error.emit((exctype, value, traceback.format_exc()))
        else:
            self.signals.result.emit(result) # Return the result resulting of the processing
        finally:
            self.signals.finished.emit() # Done
    
    def killed(self):
        self.is_killed = True
        logging.info("Killing the process ReadWorker")

def read_PSoC(self, signals): #we can add the parameter signals if we wanna do signals.progress.emit() inside the function
    """
    Read from PSoC
    """ 

    #CASE SWITCH
    #if char_buffer == b'F': #CONNECT_BT
    #    char_buffer = self.serial_worker.read(2)
        
    #elif char_buffer == 'case2':

    #elif char_buffer == 'case3':

    #else:

    #define BT_SET                      'F'
    #define CV_PARAMS_SET               'B'
    #define CA_PARAMS_SET               'C'
    #define TIA_SET                     'A'
    #define EEPROM_SET                  'R'
    #define WHICH_DAC_IS_SET            'S'
    #define CV_DATA                     'M'
    #define CA_DATA                     'M'
    #define EEPROM_DATA                 'E'

    char_buffer = bytearray() 
    flag_CV_CA = 0 #0 if CV, 1 if CA
    
    while self.read_worker.is_killed == False:
        char_buffer = self.serial_worker.read(1)
        time.sleep(0.1)  
        if char_buffer == b'F':
            logging.info('F')

        elif char_buffer == b'B':
            logging.info('B')
            self.serial_worker.send(b'DZ') #When B is received (CV parameters are SET), we send the D (to start procedure)
            flag_CV_CA = 0 #CV

        elif char_buffer == b'C':
            logging.info('C')
            self.serial_worker.send(b'EZ') #When C is received (CA parameters are SET), we send the E (to start procedure)
            flag_CV_CA = 1 #CA

        elif char_buffer == b'A':
            logging.info('A')
            data_buffer = bytearray()                

            while True:
                char_buffer = self.serial_worker.read(1)
                
                if char_buffer != b'Z':
                    data_buffer += char_buffer
                else:
                    break   
            
            current_vector = np.zeros(5)
            voltage_vector = np.zeros(5)

            index_current = 0
            index_voltage = 0


            for i in range(0, len(data_buffer), 2):
                two_bytes = data_buffer[i:i+2]
                logging.info(two_bytes)

                #byte_16 = data_buffer[2*i] + data_buffer[2*i+1] #MSB + LSB
                #logging.info(data_buffer)
                #logging.info(byte_16)

                int_16 = int.from_bytes(two_bytes, 'big', signed=True)

                logging.info(int_16)

                if index_current < 5:
                    int_16 = self.convert_range(int_16, 0, 255, 0, 31.9)
                    if index_current == 3 or index_current == 4:
                        current_vector[index_current] = -int_16
                    else:
                        current_vector[index_current] = int_16
                    index_current+=1
                elif index_voltage < 5:
                    int_16 = self.convert_range(int_16, -2048, 2048, -1024, 1024)
                    voltage_vector[index_voltage] = int_16
                    index_voltage+=1

            current_vector =  current_vector / 1000
            [R, Q] = np.polyfit(np.fliplr([current_vector])[0],np.fliplr([voltage_vector])[0], deg=1)

            self.TIA_resistance = R

            logging.info(current_vector)
            logging.info(voltage_vector)
            logging.info("R: {}.".format(R))
            logging.info("Q: {}.".format(Q))

            R_byte = int(np.round(R/1000)).to_bytes(1, 'big')
            Q_byte = int(np.round(Q)).to_bytes(1, 'big', signed=True)
            logging.info("R_byte: {}.".format(R_byte))
            logging.info("Q_byte: {}.".format(Q_byte))

            data_to_send = b'A' + b'\x01' + R_byte + Q_byte + b'Z'
            logging.info("data_to_send: {}.".format(data_to_send))    

            self.serial_worker.send(data_to_send)

            #Kill read worker
            self.TIA_initialize_btn.setText("Re-Initialize TIA") 
            self.read_worker.is_killed = True
            self.read_worker.killed()


        elif char_buffer == b'M':
            # code block to be executed if condition_1 is True
            logging.info('M')

            data_buffer = bytearray() 

            count_Z = 0    
            lenght = 0    

            current_received_flag=False
            Z_flag=False        

            while True:
                char_buffer = self.serial_worker.read(1)
                
                if char_buffer!=b'Z':
                    if Z_flag:
                        data_buffer+=b'Z'
                        lenght+=1
                    data_buffer+=char_buffer
                    lenght+=1
                    Z_flag=False
                elif char_buffer==b'Z' and not Z_flag:
                    Z_flag=True
                elif char_buffer==b'Z' and Z_flag and not current_received_flag:
                    Z_flag=False
                    current_received_flag=True
                elif char_buffer == b'Z' and Z_flag and current_received_flag:
                    break

                '''if char_buffer != b'Z':
                    data_buffer += char_buffer
                    lenght+=1
                elif char_buffer == b'Z' and count_Z == 0:
                    count_Z+=1
                else:
                    break '''


            
            logging.info(lenght)
            current_vector = np.zeros(int(lenght/4))
            voltage_vector = np.zeros(int(lenght/4))
            
            index_current = 0
            index_voltage = 0

            for i in range(0, lenght, 2):
                two_bytes = data_buffer[i:i+2]

                if index_current < len(current_vector):
                    int_16 = int.from_bytes(two_bytes, 'big', signed=True)  
                    logging.info(int_16)  
                    int_16 = self.convert_range(int_16, -2048, 2048, -1024, 1024)
                    if index_current < len(current_vector) : # changing sign to the first half of current vector (when negative tensions)
                                                               # are imposed -> CHECK if calculations are right  
                        int_16 = -int_16
                    
                    #•int_16 = int_16 / 19468
                    current_vector[index_current] = int_16 / self.TIA_resistance #mA, mV -> do it inside the draw
                    index_current+=1
                elif index_voltage < len(voltage_vector):
                    int_16 = int.from_bytes(two_bytes, 'big', signed=False)  
                    logging.info(int_16)
                    int_16 = self.convert_range(int_16, 0, 255, -2048, 2048)
                    voltage_vector[index_voltage] = int_16 #mV
                    index_voltage+=1

            logging.info(current_vector)
            logging.info(voltage_vector)

            if(flag_CV_CA == 0): #CV
                #Plot CV curve
                self.draw_CV(current_vector, voltage_vector)

                #Re-Activate buttons
                self.start_stop_btn.setText("Start") 
                self.start_stop_btn.setStyleSheet("background-color: green") 
                self.end_voltage_field.setDisabled(False)
                self.scan_rate_field.setDisabled(False)
                self.start_voltage_field.setDisabled(False)
                self.pulse_inc_field.setDisabled(False)
                self.pulse_height_field.setDisabled(False)
                self.save_changes_btn.setDisabled(False)
                self.restore_default_btn.setDisabled(False)
                self.type_cv.setDisabled(False)
                self.import_data_btn.setDisabled(False)
                self.export_data_btn.setDisabled(False)

                #Kill read worker
                self.read_worker.is_killed = True
                self.read_worker.killed()

            else: #CA
                #Plot CA curve
                self.draw_CA(current_vector, voltage_vector)
                
                #Re-Activate buttons
                self.start_stop_btn_ca.setText("Start")
                self.start_stop_btn_ca.setStyleSheet("background-color: green") 
                self.fixed_voltage_field.setDisabled(False)
                self.pulse_voltage_field.setDisabled(False)
                self.duration_field.setDisabled(False)
                self.type_ca.setDisabled(False)
                self.save_changes_btn_ca.setDisabled(False)
                self.restore_default_btn_ca.setDisabled(False)
                self.import_data_btn_ca.setDisabled(False)
                self.export_data_btn_ca.setDisabled(False)

                #Kill read worker
                self.read_worker.is_killed = True
                self.read_worker.killed()
                  


        elif char_buffer == b'R':
            logging.info('R')
        elif char_buffer == b'S':
            logging.info('S')
        else:
            1==1


    x = []
    y = []
    self.read_worker.signals.data.emit((x, y))

    data_array = [] #TBD

    return data_array #this will be the result signal

def conv16_8(MSB, LSB):

    value16 = b''
    value16 = (MSB << 8 ) + LSB
    

    return value16



###############
# MAIN WINDOW #
###############
class MainWindow(QMainWindow):
    def __init__(self):
        """!
        @brief Init MainWindow.
        """

        # define worker for serial communication and for reading signal from PSoC
        self.serial_worker = SerialWorker(None) #start empty SerialWorker since we don't know the serial_port_name yet
        self.read_worker = ReadWorker(None)

        super(MainWindow, self).__init__() #initialize parent class: When you subclass a Qt class you must always call the super __init__ function to allow Qt to set up the object.

        # title and geometry
        self.setWindowTitle("GUI")
        width = 1000
        height = 700
        self.setMinimumSize(width, height) #it can be setFixedSize() or setMaximumSize() instead

        #global varibles
        self.TIA_resistance = 5

        # create thread handler
        self.threadpool = QThreadPool() #initialize the contenitore of the thread pool

        self.connected = CONN_STATUS

        self.serialscan() #method defined below for serial communication

        self.TIA_initialization() #method defined below for serial communication

        self.glucoseMeasurement() #method for "one-click" glucose measurement with standard values (stored on EEPROM)

        self.cyclicVoltammetry() #method for cyclicVoltammetry

        self.chronoAmperometry() #method for chronoAmperometry

        self.initUI() #standard method for layout



    ####################
    # SERIAL INTERFACE #
    ####################
    def serialscan(self):
        """!
        @brief Scans all serial ports and create a list.
        """
        # create the combo box to host port list
        self.port_text = ""
        #self.com_list_widget = QComboBox()
        #self.com_list_widget.currentTextChanged.connect(self.port_changed)
        
        # create the connection button
        self.conn_btn = QPushButton(
            text=("Connect to bluetooth"), 
            checkable=True,
            toggled=self.on_toggle #toggle updates everytime a button is pressed or released
        )

        # acquire list of serial ports and add it to the combo box
        self.serial_ports = [
                p.name
                for p in serial.tools.list_ports.comports()
            ]
        #self.com_list_widget.addItems(self.serial_ports) #addItems to object QComboBox()

    ##################
    # SERIAL SIGNALS #
    ##################
    #def port_changed(self):
        """!
        @brief Update conn_btn label based on selected port.
        """
        #self.port_text = #self.com_list_widget.currentText()
        #self.conn_btn.setText("Connect to port {}".format(self.port_text))

    def on_toggle(self, checked):
        """!
        @brief Allow connection and disconnection from selected serial port.
        """
        if checked:
            #Send “F” to all ports and wait (idle) for “xFF”, the COM that returns it is connected 
            logging.info(self.serial_ports)
            for port_check in self.serial_ports:
                try:
                    test_port = serial.Serial(port=port_check, baudrate=9600,
                                            write_timeout=0, timeout=2)
                    time.sleep(0.1)                   
                    if test_port.is_open: #if the port is open
                        test_char = b'FZ'
                        test_port.write(test_char)
                        #time.sleep(0.1)     
                        test_char = test_port.read(1)
                        #time.sleep(0.1)   
                        logging.info(port_check)
                        logging.info(test_char)
                        if test_char == b'F':
                            self.port_text = port_check                            
                            # setup reading worker
                            self.serial_worker = SerialWorker(self.port_text) # needs to be re defined
                            # connect worker signals to functions
                            self.serial_worker.signals.status.connect(self.check_serialport_status)
                            self.serial_worker.signals.device_port.connect(self.connected_device)
                            # execute the worker
                            self.threadpool.start(self.serial_worker)                             
                            self.conn_btn.setText("Disconnect")
                            break #don't try other ports
                        else:
                            logging.info("Port {} not available".format(port_check))
                
                except serial.SerialException:
                    logging.info("Error with port {}.".format(port_check))
                    time.sleep(0.01)

           
            
        else:
            # kill thread
            self.serial_worker.is_killed = True
            self.serial_worker.killed()
            #self.com_list_widget.setDisabled(False) # enable the possibility to change port
            self.conn_btn.setText("Connect to bluetooth")

    def check_serialport_status(self, port_name, status):
        """!
        @brief Handle the status of the serial port connection.

        Available status:
            - 0  --> Error during opening of serial port
            - 1  --> Serial port opened correctly
        """
        if status == 0:
            self.conn_btn.setChecked(False)
        elif status == 1:
            # enable all the widgets on the interface
            #self.com_list_widget.setDisabled(True) # disable the possibility to change COM port when already connected
            self.conn_btn.setText(
                "Disconnect from port {}".format(port_name)
            )

    def connected_device(self, port_name):
        """!
        @brief Checks on the termination of the serial worker.
        """
        logging.info("Port {} closed.".format(port_name))


    def ExitHandler(self):
        """!
        @brief Kill every possible running thread upon exiting application.
        """
        self.serial_worker.is_killed = True
        self.serial_worker.killed()

        self.read_worker.is_killed = True
        self.read_worker.killed()

    #######################
    # GLUCOSE MEASUREMENT #
    #######################
    def TIA_initialization(self):
        #Measure Glucose button
        self.TIA_initialize_btn = QPushButton(
            text=("Initialize TIA"), 
            checkable=True,
            toggled=self.TIA_initialize
        )

    def TIA_initialize(self, checked):
        if checked: #START
            self.serial_worker.send(b'IZ')

            self.TIA_initialize_btn.setText("Stop")

            # Pass the function to execute
            self.read_worker = ReadWorker(read_PSoC, self)
            self.read_worker.signals.result.connect(self.print_output) #just a test function
            #self.read_worker.signals.data.connect(self.draw) #Draw for each data (x,y) received

            #read_worker.signals.finished.connect(self.thread_complete)
            # Execute
            self.threadpool.start(self.read_worker)

        else: #STOP
            1==1
            #self.TIA_initialize_btn.setText("Initialize TIA") 
            #self.read_worker.is_killed = True
            #self.read_worker.killed()



    #######################
    # GLUCOSE MEASUREMENT #
    #######################
    def glucoseMeasurement(self):
        """!
        @brief Method for "one-click" glucose measurement with standard values (stored on EEPROM)
        """

        #Measure Glucose button
        self.measure_glucose_btn = QPushButton(self)
        self.measure_glucose_btn.setText("Measure glucose") #text
        #self.measure_glucose_btn.setIcon(QIcon("SP_MediaPlay")) #icon
        self.measure_glucose_btn.clicked.connect(self.measure_glucose)

        ## Glucose concentration (mg/dL) LCD
        self.glucose_lcd = QLCDNumber()
        #self.glucose_lcd.setFixedWidth(100)
        self.glucose_lcd.display('')
        self.glucose_lcd.setStyleSheet("QLCDNumber {color: red;}")
        self.glucose_lcd.setSmallDecimalPoint(False)
        self.glucose_sufix = QLabel(" mg/dL")


    def measure_glucose(self):
        """!
        @brief Calculate glucose measurement by chronoamperometry and calibration with standard values (stored on EEPROM)
        """
        #Send CA parameters to PSoC
        data_to_send = b'C\x00\x02X\x82\x7fZ' #values from calibration
        logging.info(data_to_send)
        
        self.serial_worker.send(data_to_send)      

        self.read_worker = ReadWorker(read_PSoC, self)
        self.read_worker.signals.result.connect(self.print_output) #just a test function
        self.threadpool.start(self.read_worker)

        #self.glucose_stored = int(-122.0998 + (368393.2303*current_axis[50])) --> IT IS DONE INSIDE DRAW_CA
        #self.glucose_lcd.display(self.glucose_stored)
        
        
    #####################
    # CYCLIC VOLTAMMETRY #
    #####################
    def cyclicVoltammetry(self): 
        """
        @brief Set up the CyclicVoltammetry functionalities
        """

        ## Parameters to be send to PSoC
        self.data_buffer = bytearray()
        self.header_data = b'\x00'#data_buffer[0]
        self.scan_rate_data = b'\x05' #data_buffer[1]
        self.start_voltage_data = b'\x79' #data_buffer[2]
        self.end_voltage_data = b'\x85' #data_buffer[3]
        self.type_cv_data = b'\x00' #data_buffer[4]
        self.pulse_inc_data = b'\x01' #data_buffer[5]
        self.pulse_height_data = b'\x02' #data_buffer[6]
        self.tail_data = b'Z' #data_buffer[7]

        ## Array to store measurements
        self.current_stored = np.array([])
        self.voltage_stored = np.array([]) 
        self.optimal_voltage = 0

        ## CV plot: Current(mA) vs Voltage(mV)
        self.graph_cv = PlotWidget() 
        self.graph_cv.showGrid(x=True, y=True)
        self.graph_cv.setBackground('w')
        self.graph_cv.setTitle("Cyclic Voltammetry Graph")
        styles = {'color':'k', 'font-size':'15px'}
        self.graph_cv.setLabel('left', 'Current (mA)', **styles)
        self.graph_cv.setLabel('bottom', 'Voltage (mV)', **styles)
        self.graph_cv.addLegend()

        ## LUT plot: Voltage(mV) vs Time(s)
        self.graph_cv_LUT = PlotWidget() 
        self.graph_cv_LUT.showGrid(x=True, y=True)
        self.graph_cv_LUT.setBackground('w')
        self.graph_cv_LUT.setTitle("Imposed Voltage (mV)")
        styles = {'color':'k', 'font-size':'15px'}
        self.graph_cv_LUT.setLabel('left', 'Voltage (mV)', **styles)
        self.graph_cv_LUT.setLabel('bottom', 'Time (s)', **styles)
        self.graph_cv_LUT.addLegend()

        ## Parameters display

        #Start voltage (mV)
        self.start_voltage_label = QLabel("Starting voltage")
        self.start_voltage_field = QSpinBox()
        self.start_voltage_field.setMinimum(-2000)
        self.start_voltage_field.setMaximum(0)
        self.start_voltage_field.setSuffix(" mV")
        self.start_voltage_field.setValue(-100)
        self.start_voltage_field.valueChanged.connect(self.update_start_voltage)
        

        #Ending voltage (mV)
        self.end_voltage_label = QLabel("Ending voltage")
        self.end_voltage_field = QSpinBox()
        self.end_voltage_field.setMinimum(0)
        self.end_voltage_field.setMaximum(2000)
        self.end_voltage_field.setSuffix(" mV")
        self.end_voltage_field.setValue(100)
        self.end_voltage_field.valueChanged.connect(self.update_end_voltage)


        #Scan rate (mV/s)
        self.scan_rate_label = QLabel("Scan rate")

        self.scan_rate_field = QSpinBox()
        self.scan_rate_field.setMinimum(1)
        self.scan_rate_field.setMaximum(10)
        self.scan_rate_field.setSuffix(" mV/s")
        self.scan_rate_field.setValue(5)
        self.scan_rate_field.valueChanged.connect(self.update_scan_rate)


        ## Save changes button
        self.save_changes_btn = QPushButton(
            text=("Save Changes"), 
            checkable=True,
            clicked=self.save_changes_cv
        )

        ## Restore Default button
        self.restore_default_btn = QPushButton(
            text=("Restore Default"), 
            checkable=True,
            clicked=self.restore_default_cv
        )

        

        ## CV type combobox
        self.type_cv_label = QLabel("Voltammetry type")
        self.type_cv = QComboBox()
        self.type_cv.addItems(["Cyclic Voltammetry","Squared-Wave Voltammetry"])
        self.type_cv.currentIndexChanged.connect(self.change_type_cv)

        #Pulse increment mV
        self.pulse_inc_label = QLabel("Pulse increment")
        self.pulse_inc_field = QSpinBox()
        self.pulse_inc_field.setMinimum(0)
        self.pulse_inc_field.setMaximum(100)
        self.pulse_inc_field.setSuffix(" mV")
        self.pulse_inc_field.setValue(1)
        self.pulse_inc_field.valueChanged.connect(self.update_pulse_inc)


        #Pulse height mV
        self.pulse_height_label = QLabel("Pulse heigh")

        self.pulse_height_field = QSpinBox()
        self.pulse_height_field.setMinimum(0)
        self.pulse_height_field.setMaximum(100)
        self.pulse_height_field.setSuffix(" mV")
        self.pulse_height_field.setValue(2)
        self.pulse_height_field.valueChanged.connect(self.update_pulse_height)


        # Widgets are hidden by default and are shown if SWVoltammetry is chosen
        self.pulse_inc_label.hide()
        self.pulse_inc_field.hide()
        self.pulse_height_label.hide()
        self.pulse_height_field.hide()
        
        ## Start/stop button
        self.start_stop_btn = QPushButton(
            text=("Start"), 
            checkable=True,
            toggled=self.start_cv
        )
        self.start_stop_btn.setStyleSheet("background-color: green") 

        ## Export data button
        self.export_data_btn = QPushButton(
            text=("Export data"), 
            checkable=True,
            clicked=self.export_data_cv
        )

        ## Import data button
        self.import_data_btn = QPushButton(
            text=("Import data"), 
            checkable=True,
            clicked=self.import_data_cv
        )

        ## Optimal voltage (maximum sensitivity) LCD
        self.lcd_label = QLabel("Optimal voltage (max. sensitivity)")
        self.lcd = QLCDNumber()
        self.lcd.setFixedWidth(100)
        self.lcd.display('')
        self.lcd.setStyleSheet("QLCDNumber {color: red;}")
        self.lcd.setSmallDecimalPoint(False)
    
    def convert_range(self, val, input_min, input_max, output_min, output_max):
        return int(((val - input_min) / (input_max - input_min)) * (output_max - output_min) + output_min)

    def update_start_voltage(self):
        """
        @brief Save parameter into variable when changing it
        """

        start_voltage_analog = self.start_voltage_field.value()

        start_voltage_int = self.convert_range(start_voltage_analog, -2000, 2000, 0, 255) 

        self.start_voltage_data = start_voltage_int.to_bytes(1, 'big')

        logging.info(start_voltage_analog)
        logging.info(start_voltage_int)
        logging.info(self.start_voltage_data)



    def update_end_voltage(self):
        """
        @brief Save parameter into variable when changing it
        """
        end_voltage_analog = self.end_voltage_field.value()

        end_voltage_int = self.convert_range(end_voltage_analog, -2000, 2000, 0, 255) #I COULDN'T USE THE FUNCTION, IT IS GIVEN THE ERROR: convert_range() takes 5 positional arguments but 6 were given

        self.end_voltage_data = end_voltage_int.to_bytes(1, 'big')

        logging.info(end_voltage_analog)
        logging.info(end_voltage_int)
        logging.info(self.end_voltage_data)
        


    def update_scan_rate(self):
        """
        @brief Save parameter into variable when changing it
        """
        scan_rate_int = self.scan_rate_field.value()
        self.scan_rate_data = scan_rate_int.to_bytes(1, 'big')
        logging.info(scan_rate_int)
        logging.info(self.scan_rate_data)



    def update_pulse_inc(self):
        """
        @brief Save parameter into variable when changing it
        """
        pulse_inc_int = self.pulse_inc_field.value()
        self.pulse_inc_data = pulse_inc_int.to_bytes(1, 'big')
        logging.info(pulse_inc_int)
        logging.info(self.pulse_inc_data)


    def update_pulse_height(self):
        """
        @brief Save parameter into variable when changing it
        """
        pulse_height_int = self.pulse_height_field.value()
        self.pulse_height_data = pulse_height_int.to_bytes(1, 'big')
        logging.info(pulse_height_int)
        logging.info(self.pulse_height_data)


    def start_cv(self, checked):
        """
        @brief Start the cyclic voltammetry (voltage supply and current measurement)
        """
        if checked: #START

            self.graph_cv.clear()
            self.graph_cv_LUT.clear()
            self.start_stop_btn.setText("Stop")
            self.start_stop_btn.setStyleSheet("background-color: red") 
            self.end_voltage_field.setDisabled(True)
            self.scan_rate_field.setDisabled(True)
            self.start_voltage_field.setDisabled(True)
            self.pulse_inc_field.setDisabled(True)
            self.pulse_height_field.setDisabled(True)
            self.save_changes_btn.setDisabled(True)
            self.restore_default_btn.setDisabled(True)
            self.type_cv.setDisabled(True)
            self.import_data_btn.setDisabled(True)
            self.export_data_btn.setDisabled(True)

            #self.header_data => data_buffer[0]
            #self.scan_rate_data => data_buffer[1]
            #self.start_voltage_data => data_buffer[2]
            #self.end_voltage_data => data_buffer[3]
            #self.type_cv_data => data_buffer[4]
            #self.pulse_inc_data => data_buffer[5]
            #self.pulse_height_data => data_buffer[6]

            #Send CV parameters to PSoC
            self.header_data = b'B' #data_buffer[0] -> CV_parametri state
            self.tail_data = b'Z'
            self.data_buffer = self.header_data + self.scan_rate_data + self.start_voltage_data + self.end_voltage_data + self.type_cv_data + self.pulse_inc_data + self.pulse_height_data + self.tail_data
            logging.info(self.data_buffer)
            self.serial_worker.send(self.data_buffer)             

            #TBD: GUI must remain on-hold waiting for response from PSoC indicating the CV is finished
            # Pass the function to execute
            self.read_worker = ReadWorker(read_PSoC, self)
            self.read_worker.signals.result.connect(self.print_output) #just a test function
            #self.read_worker.signals.data.connect(self.draw) #Draw for each data (x,y) received

            #read_worker.signals.finished.connect(self.thread_complete)
            # Execute
            self.threadpool.start(self.read_worker)

        else: #STOP
            self.start_stop_btn.setText("Start") 
            self.start_stop_btn.setStyleSheet("background-color: green") 
            self.end_voltage_field.setDisabled(False)
            self.scan_rate_field.setDisabled(False)
            self.start_voltage_field.setDisabled(False)
            self.pulse_inc_field.setDisabled(False)
            self.pulse_height_field.setDisabled(False)
            self.save_changes_btn.setDisabled(False)
            self.restore_default_btn.setDisabled(False)
            self.type_cv.setDisabled(False)
            self.import_data_btn.setDisabled(False)
            self.export_data_btn.setDisabled(False)

            self.read_worker.is_killed = True
            self.read_worker.killed()
    
    def print_output(self, s): #Debugging function to see the "test" signal of the ReadWorker
        print('result: ',s)  
        
    def draw_CV(self, current_axis, voltage_axis): #Define draw method  --> TBD: IT IS STILL A FAKE FUNCTION; NEED TO CHANGE IT (Reference: The calculator on PSOC guide)
        """!
        @brief Draw the plots.
        """
        half = len(current_axis) // 2

        time_axis = np.arange(0,len(voltage_axis))
        self.line = self.plot(self.graph_cv_LUT, time_axis[:half],voltage_axis[:half],'','r')
        self.line = self.plot(self.graph_cv_LUT, time_axis[half-1:],voltage_axis[half-1:],'','b')

        # scan rate = mv/s -> one reading every one PWM and we have to call the PWM with a speed proportional
        # to the scan rate 
        
        self.line1 = self.plot(self.graph_cv, voltage_axis[3:half], current_axis[3:half], '', 'r')
        self.line2 = self.plot(self.graph_cv, voltage_axis[half:-3], current_axis[half:-3], '', 'b')

        self.current_stored = current_axis
        self.voltage_stored = voltage_axis

        
        #self.optimal_voltage = voltage_axis[current_axis.argmax()]
        #self.lcd.display(self.optimal_voltage)
    

        self.optimal_voltage = voltage_axis[self.find_peak(current_axis)]
        self.lcd.display(self.optimal_voltage)


    def find_peak(self, data):
        mid = len(data)//2
        growing_values = data[:mid]
        decreasing_values = -(data[mid:])
        peaks, _ = find_peaks(growing_values)
        valleys, _ = find_peaks(decreasing_values)
        peaks_data = [growing_values[peak] for peak in peaks]
        valleys_data = [decreasing_values[valley] for valley in valleys]
        max_peak = max(peaks_data, default=None, key=abs)
        max_valley = max(valleys_data, default=None, key=abs)
        if max_peak is None and max_valley is None:
            return None
        elif max_peak is None:
            return valleys[valleys_data.index(max_valley)] + mid
        elif max_valley is None:
            return peaks[peaks_data.index(max_peak)]
        elif abs(max_peak) >= abs(max_valley):
            return peaks[peaks_data.index(max_peak)]
        else:
            return valleys[valleys_data.index(max_valley)] + mid
        
    
    def plot(self, graph, x, y, curve_name, color): #Standard function (can be repeat for any signal to be plotted)
        """!
        @brief Draw graph.
        """
        pen = pg.mkPen(color=color)
        line = graph.plot(x, y, name=curve_name, pen=pen, size=2)   
        return line


    def save_changes_cv(self):
        """
        @brief Save the CV settings into the internal EEPROM 
        Può svolgere due funzioni differenti a seconda del valore di data_buffer[1]: 
        - data_buffer[1] = 1: l’utente vuole leggere i valori di default presenti nella EEPROM, a partire da data_buffer[1] 	vengono inseriti i valori di default e alla fine viene mandato data_buffer all’utente; 
        - data_buffer[1] = 0: l’utente vuole scrivere dei nuovi valori di default nella EEPROM, i quali sono contenuti in 	data_buffer a partire da data_buffer[1]. I nuovi valori vengono salvati in celle di memoria diverse rispetto a quelle 	dove sono salvati i valori di default inizialmente. 
        """

        self.header_data = b'R' #data_buffer[0] -> EEPROM_mng state
        self.tail_data = b'Z'
        self.data_buffer = self.header_data + b'\x00' + self.scan_rate_data + self.start_voltage_data + self.end_voltage_data + self.type_cv_data + self.pulse_inc_data + self.pulse_height_data + self.tail_data
        logging.info(self.data_buffer)
        self.serial_worker.send(self.data_buffer)

        #TBD communication with PSoC to save values in EEPROM


    def restore_default_cv(self):
        """
        @brief Restore the default values (defined by us) from the internal EEPROM
        Può svolgere due funzioni differenti a seconda del valore di data_buffer[1]: 
        - data_buffer[1] = 1: l’utente vuole leggere i valori di default presenti nella EEPROM, a partire da data_buffer[1] 	vengono inseriti i valori di default e alla fine viene mandato data_buffer all’utente; 
        - data_buffer[1] = 0: l’utente vuole scrivere dei nuovi valori di default nella EEPROM, i quali sono contenuti in 	data_buffer a partire da data_buffer[1]. I nuovi valori vengono salvati in celle di memoria diverse rispetto a quelle 	dove sono salvati i valori di default inizialmente. 
        """
        
        self.header_data = b'R' #data_buffer[0] -> EEPROM_mng state
        self.tail_data = b'Z'
        self.data_buffer = self.header_data + b'\x01' + self.tail_data
        logging.info(self.data_buffer)
        self.serial_worker.send(self.data_buffer)


        #TBD communication with PSoC to read values from EEPROM
        
        
    def change_type_cv(self):
        """
        @brief Select the type of CV and store it in a variable
        """
        if self.type_cv.currentIndex() == 0 : # "Cyclic Voltammetry":
            int_0 = 0 #byte: 00000000
            self.type_cv_data = int_0.to_bytes(1, 'big') #Go to data_buffer[4]
            logging.info(self.type_cv_data)

            #Disable the pulse_inc and pulse_height parameters and widgets since it's used only for SWC
            self.pulse_inc_data = int_0.to_bytes(1, 'big') #data_buffer[5]
            self.pulse_height_data = int_0.to_bytes(1, 'big') #data_buffer[6]
            self.pulse_inc_label.hide()
            self.pulse_inc_field.hide()
            self.pulse_height_label.hide()
            self.pulse_height_field.hide()
        else: #"Squared-Wave Voltammetry"
            int_1 = 1 #byte: 00000001
            self.type_cv_data = int_1.to_bytes(1, 'big') #Go to data_buffer[4]
            logging.info(self.type_cv_data)
            
            self.pulse_inc_label.show()
            self.pulse_inc_field.show()
            self.pulse_height_label.show()
            self.pulse_height_field.show()
            


    def export_data_cv(self):
        """
        @brief Export data (measurements and values) to external file
        """
        now = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        now_date = np.array([now])

        arr_concat = np.concatenate((now_date, self.current_stored, self.voltage_stored))

        # Export data to .txt
        string = np.array2string(arr_concat)
        filename = 'Measures_CV_' + now + '.txt'

        with open(filename, "w") as f:
            f.write(string)      

        logging.info("Exporting CV measures to file {}".format(filename))  
        

    def import_data_cv(self):
        """
        @brief Import data (measurements and values) to external file 
        """
        self.graph_cv.clear()
        options = QFileDialog.Options()
        filename, _ = QFileDialog.getOpenFileName(self, "QFileDialog.getOpenFileName()", "",
                                                  "All Files (*);;Text Files (*.txt)", options=options)

        logging.info("Importing CV measures from file {}.".format(filename))

        with open(filename, "r") as f:
            loaded_string = f.read()
        
        logging.info(loaded_string)

        loaded_array = np.fromstring(loaded_string[loaded_string.index(' ') + 2:], sep="' '", dtype=float)

        logging.info(loaded_array)

        half_ = len(loaded_array) // 2
        self.draw_CV(loaded_array[:half_], loaded_array[half_:])        
        
    
    #####################
    # CHRONO AMPEROMETRY #
    #####################
    def chronoAmperometry(self): 
        """
        @brief Set up the ChronoAmperometry functionalities
        """

        ## Parameters to be send to PSoC
        self.data_buffer = bytearray()
        self.header_data = b'\x00'#data_buffer[0]
        self.type_ca_data = b'\x00' #data_buffer[1]
        self.duration_data = b'\x03\xe8' #data_buffer[2] e [3]
        self.pulse_voltage_data = b'\x38' #data_buffer[4] -> 56mV from calibration
        self.fixed_voltage_data = b'\x7f' #data_buffer[5]
        self.tail_data = b'Z' #data_buffer[5]

        ## Array to store measurements
        self.currentCA_stored = np.array([])
        self.timeCA_stored = np.array([]) 
        self.glucoseCA_stored = 0

        ## CA plot: Current(mA) vs Time(s)
        self.graph_ca = PlotWidget() 
        self.graph_ca.showGrid(x=True, y=True)
        self.graph_ca.setBackground('w')
        self.graph_ca.setTitle("ChronoAmperometry Graph")
        styles = {'color':'k', 'font-size':'15px'}
        self.graph_ca.setLabel('left', 'Current (mA)', **styles)
        self.graph_ca.setLabel('bottom', 'Time (s)', **styles)
        self.graph_ca.addLegend()

        ## LUT plot: Voltage(mV) vs Time(s)
        self.graph_ca_LUT = PlotWidget() 
        self.graph_ca_LUT.showGrid(x=True, y=True)
        self.graph_ca_LUT.setBackground('w')
        self.graph_ca_LUT.setTitle("Imposed Voltage (mV)")
        styles = {'color':'k', 'font-size':'15px'}
        self.graph_ca_LUT.setLabel('left', 'Voltage (mV)', **styles)
        self.graph_ca_LUT.setLabel('bottom', 'Time (s)', **styles)
        self.graph_ca_LUT.addLegend()

        #Fixed voltage / Working potential (mV)
        self.fixed_voltage_label = QLabel("Fixed Voltage")
        self.fixed_voltage_field = QSpinBox()
        self.fixed_voltage_field.setMinimum(-2000)
        self.fixed_voltage_field.setMaximum(2000)
        self.fixed_voltage_field.setSuffix(" mV")
        self.fixed_voltage_field.setValue(0)
        self.fixed_voltage_field.valueChanged.connect(self.update_fixed_voltage)

        #Pulse voltage (mV)
        self.pulse_voltage_label = QLabel("Pulse Voltage")
        self.pulse_voltage_field = QSpinBox()
        self.pulse_voltage_field.setMinimum(-2000)
        self.pulse_voltage_field.setMaximum(2000)
        self.pulse_voltage_field.setSuffix(" mV")
        self.pulse_voltage_field.setValue(56)
        self.pulse_voltage_field.valueChanged.connect(self.update_pulse_voltage)

        #Acquisition time / Duration (s)
        self.duration_label = QLabel("Duration")
        self.duration_field = QSpinBox()
        self.duration_field.setMinimum(550)
        self.duration_field.setMaximum(4000)
        self.duration_field.setSuffix("ms")
        self.duration_field.setValue(1000)
        self.duration_field.valueChanged.connect(self.update_duration)

        ## Save changes button
        self.save_changes_btn_ca = QPushButton(
            text=("Save Changes"), 
            checkable=True,
            clicked=self.save_changes_ca
        )

        ## Restore Default button
        self.restore_default_btn_ca = QPushButton(
            text=("Restore Default"), 
            checkable=True,
            clicked=self.restore_default_ca
        )

        ## CA type combobox
        self.type_ca_label = QLabel("ChronoAmperometry type")
        self.type_ca = QComboBox()
        self.type_ca.addItems(["Use chosen parameters","Use standard parameters"])
        self.type_ca.currentIndexChanged.connect(self.change_type_ca)


        ## Start/stop button
        self.start_stop_btn_ca = QPushButton(
            text=("Start"), 
            checkable=True,
            toggled=self.start_ca
        )
        self.start_stop_btn_ca.setStyleSheet("background-color: green") 

        ## Export data button
        self.export_data_btn_ca = QPushButton(
            text=("Export data"), 
            checkable=True,
            clicked=self.export_data_ca
        )

        ## Import data button
        self.import_data_btn_ca = QPushButton(
            text=("Import data"), 
            checkable=True,
            clicked=self.import_data_ca
        )

        ## Glucose concentration (mg/dL) LCD
        self.glucoseCA_label = QLabel("Glucose concentration")
        self.glucoseCA_LCD = QLCDNumber()
        self.glucoseCA_LCD.setFixedWidth(100)
        self.glucoseCA_LCD.display('')
        self.glucoseCA_LCD.setStyleSheet("QLCDNumber {color: red;}")
        self.glucoseCA_LCD.setSmallDecimalPoint(False)
        self.glucoseCA_sufix = QLabel(" mg/dL")

    def start_ca(self, checked):
        """
        @brief Start the cyclic voltammetry (voltage supply and current measurement)
        """
        if checked:
            self.graph_ca.clear()
            self.graph_ca_LUT.clear()
            self.start_stop_btn_ca.setText("Stop")
            self.start_stop_btn_ca.setStyleSheet("background-color: red") 
            self.fixed_voltage_field.setDisabled(True)
            self.pulse_voltage_field.setDisabled(True)
            self.duration_field.setDisabled(True)
            self.type_ca.setDisabled(True)
            self.save_changes_btn_ca.setDisabled(True)
            self.restore_default_btn_ca.setDisabled(True)
            self.import_data_btn_ca.setDisabled(True)
            self.export_data_btn_ca.setDisabled(True)
#
            #self.data_buffer = bytearray()
            #self.header_data = b'\x00'#data_buffer[0]
            #self.type_ca_data = b'\x00' #data_buffer[1]
            #self.duration_data = b'\x00' #data_buffer[2]
            #self.pulse_voltage_data = b'\x00' #data_buffer[3]
            #self.fixed_voltage_data = b'\x00' #data_buffer[4]
            #self.tail_data = b'Z' #data_buffer[5]

            #Send CA parameters to PSoC
            self.header_data = b'C' #data_buffer[0] -> CA_parametri state
            self.tail_data = b'Z'
            self.data_buffer = self.header_data + self.type_ca_data + self.duration_data + self.pulse_voltage_data + self.fixed_voltage_data + self.tail_data
            logging.info(self.data_buffer)
            self.serial_worker.send(self.data_buffer)      

            #TBD: GUI must remain on-hold waiting for response from PSoC indicating the CV is finished
            # Pass the function to execute
            self.read_worker = ReadWorker(read_PSoC, self)
            self.read_worker.signals.result.connect(self.print_output) #just a test function
            #self.read_worker.signals.data.connect(self.draw) #Draw for each data (x,y) received

            #read_worker.signals.finished.connect(self.thread_complete)
            # Execute
            self.threadpool.start(self.read_worker)
            

        else:
            self.start_stop_btn_ca.setText("Start")
            self.start_stop_btn_ca.setStyleSheet("background-color: green") 
            self.fixed_voltage_field.setDisabled(False)
            self.pulse_voltage_field.setDisabled(False)
            self.duration_field.setDisabled(False)
            self.type_ca.setDisabled(False)
            self.save_changes_btn_ca.setDisabled(False)
            self.restore_default_btn_ca.setDisabled(False)
            self.import_data_btn_ca.setDisabled(False)
            self.export_data_btn_ca.setDisabled(False)

            self.read_worker.is_killed = True
            self.read_worker.killed()

    def draw_CA(self, current_axis, voltage_axis):
        """!
        @brief Draw the plots.
        """
      
        current_smooth = savgol_filter(current_axis, 10, 3) 
        time_axis = np.arange(0,len(voltage_axis)*10,10) ######IT NEED TO BE FIXED TO THE RIGHT TIME SCALE

        self.line = self.plot(self.graph_ca_LUT, time_axis,voltage_axis,'','r')
        self.line1 = self.plot(self.graph_ca, time_axis[3:], current_axis[3:], '', 'r')
        self.line2 = self.plot(self.graph_ca, time_axis[3:], current_smooth[3:], '', 'b')

        #We can use the same method to plot 2 graphs

        self.currentCA_stored = current_axis
        self.timeCA_stored = time_axis

        self.glucoseCA_stored = int(-122.0998 + (368393.2303*current_axis[50]))
        self.glucoseCA_LCD.display(self.glucoseCA_stored)

        self.glucose_lcd.display(self.glucoseCA_stored)



    def restore_default_ca(self):
        """
        @brief Restore the default values (defined by us) and save it into the internal EEPROM
        Può svolgere due funzioni differenti a seconda del valore di data_buffer[1]: 
        - data_buffer[1] = 1: l’utente vuole leggere i valori di default presenti nella EEPROM, a partire da data_buffer[1] 	vengono inseriti i valori di default e alla fine viene mandato data_buffer all’utente; 
        - data_buffer[1] = 0: l’utente vuole scrivere dei nuovi valori di default nella EEPROM, i quali sono contenuti in 	data_buffer a partire da data_buffer[1]. I nuovi valori vengono salvati in celle di memoria diverse rispetto a quelle 	dove sono salvati i valori di default inizialmente. 
        """
        
        self.header_data = b'R' #data_buffer[0] -> EEPROM_mng state
        self.tail_data = b'Z'
        self.data_buffer = self.header_data + b'\x01' + self.tail_data
        logging.info(self.data_buffer)
        self.serial_worker.send(self.data_buffer)

        #TBD communication with PSoC to read values from EEPROM
        
    
    def save_changes_ca(self):
        """
        @brief Save the CV settings into the internal EEPROM 
        """
        self.header_data = b'R' #data_buffer[0] -> EEPROM_mng state
        self.tail_data = b'Z'
        self.data_buffer = self.header_data + b'\x00' + self.type_ca_data + self.duration_data + self.pulse_voltage_data + self.fixed_voltage_data + self.tail_data
        logging.info(self.data_buffer)
        self.serial_worker.send(self.data_buffer)
        #TBD communication with PSoC to save values in EEPROM

    def export_data_ca(self):
        """
        @brief Export data (measurements and values) to external file
        """
        """DEBUGGING
        self.serial_worker.send(b'B\x03\x32\xAF\x00\x00\x00Z')
        time.sleep(5)  
        self.serial_worker.send(b'DZ')
        time.sleep(5) """
        now = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        now_date = np.array([now])

        arr_concat = np.concatenate((now_date, self.current_stored, self.voltage_stored))

        # Export data to .txt
        string = np.array2string(arr_concat)
        filename = 'Measures_CA_' + now + '.txt'

        with open(filename, "w") as f:
            f.write(string)      

        logging.info("Exporting CA measures to file {}.txt".format(filename))          

    def import_data_ca(self):
        """
        @brief Import data (measurements and values) to external file 
        """
        """DEBUGGING
        char_buffer = self.serial_worker.read(1)
        self.read_worker = ReadWorker(read_PSoC, self)
        self.read_worker.signals.result.connect(self.print_output) #just a test function
        #self.read_worker.signals.data.connect(self.draw) #Draw for each data (x,y) received

        #read_worker.signals.finished.connect(self.thread_complete)
        # Execute
        self.threadpool.start(self.read_worker)
        logging.info(char_buffer)"""
        self.graph_ca.clear()
        options = QFileDialog.Options()
        filename, _ = QFileDialog.getOpenFileName(self, "QFileDialog.getOpenFileName()", "",
                                                  "All Files (*);;Text Files (*.txt)", options=options)

        logging.info("Importing CA measures from file {}.".format(filename))

        with open(filename, "r") as f:
            loaded_string = f.read()
        
        logging.info(loaded_string)

        loaded_array = np.fromstring(loaded_string[loaded_string.index(' ') + 2:], sep="' '", dtype=float)

        logging.info(loaded_array)

        half_ = len(loaded_array) // 2
        self.draw_CA(loaded_array[:half_], loaded_array[half_:])  


    def change_type_ca(self):
        """
        @brief Select the type of CV and store it in a variable
        """
        if self.type_ca.currentIndex() == 0 : # "Use chosen parameters":
            int_0 = 0 #byte: 00000000
            self.type_ca_data = int_0.to_bytes(1, 'big') #Go to data_buffer[1]
            logging.info(self.type_ca_data)


        else: #"Use standard parameters"
            int_1 = 1 #byte: 00000001
            self.type_ca_data = int_1.to_bytes(1, 'big') #Go to data_buffer[1]
            logging.info(self.type_ca_data)


    def update_fixed_voltage(self):
        """
        @brief Save parameter into variable when changing it
        """
        fixed_voltage_analog = self.fixed_voltage_field.value()

        fixed_voltage_int = self.convert_range(fixed_voltage_analog, -2048, 2048, 0, 255) #I COULDN'T USE THE FUNCTION, IT IS GIVEN THE ERROR: convert_range() takes 5 positional arguments but 6 were given

        self.fixed_voltage_data = fixed_voltage_int.to_bytes(1, 'big')

        logging.info(fixed_voltage_analog)
        logging.info(fixed_voltage_int)
        logging.info(self.fixed_voltage_data)

    
    def update_pulse_voltage(self):
        """
        @brief Save parameter into variable when changing it
        """
        pulse_voltage_analog = self.pulse_voltage_field.value()

        pulse_voltage_int = self.convert_range(pulse_voltage_analog, -2048, 2048, 0, 255) #I COULDN'T USE THE FUNCTION, IT IS GIVEN THE ERROR: convert_range() takes 5 positional arguments but 6 were given

        self.pulse_voltage_data = pulse_voltage_int.to_bytes(1, 'big')

        logging.info(pulse_voltage_analog)
        logging.info(pulse_voltage_int)
        logging.info(self.pulse_voltage_data)


    def update_duration(self):
        """
        @brief Save parameter into variable when changing it
        """
        duration_int = self.duration_field.value()
        self.duration_data = duration_int.to_bytes(2, 'big')

        logging.info(duration_int)
        logging.info(self.duration_data)    
    

    #####################
    # GRAPHIC INTERFACE #
    #####################
    def initUI(self): 
        """!
        @brief Set up the graphical interface structure.
        """

        ##Tabs
        tabs = QTabWidget()
        tabs.setTabPosition(QTabWidget.North) #can be West, South or East
        tabs.setMovable(False) 

        ## "Home" tab

        #Serial communication widget
        button_hlay = QHBoxLayout()
        button_hlay.addWidget(self.conn_btn)
        button_hlay.addWidget(self.TIA_initialize_btn)
        widget_serial = QWidget()
        widget_serial.setLayout(button_hlay)

        #Glucose measurement widget
        layout_glucose=QGridLayout()
        layout_glucose.addWidget(self.measure_glucose_btn, 0, 0)
        layout_glucose.addWidget(self.glucose_lcd, 0, 1)
        layout_glucose.addWidget(self.glucose_sufix, 0, 2)
        widget_glucose = QWidget()
        widget_glucose.setLayout(layout_glucose)

        layout_home=QGridLayout()
        layout_home.addWidget(widget_serial, 0, 0) 
        layout_home.addWidget(widget_glucose, 1, 0)
        widget_home = QWidget()
        widget_home.setLayout(layout_home)

        tabs.addTab(widget_home, "Home")

        ## "Cyclic Voltammetry" tab
        layout_control_panel_cv=QGridLayout()
        layout_control_panel_cv.addWidget(self.start_voltage_label, 0, 0) 
        layout_control_panel_cv.addWidget(self.start_voltage_field, 0, 1) 
        layout_control_panel_cv.addWidget(self.end_voltage_label, 1, 0)
        layout_control_panel_cv.addWidget(self.end_voltage_field, 1, 1) 
        layout_control_panel_cv.addWidget(self.scan_rate_label, 2, 0) 
        layout_control_panel_cv.addWidget(self.scan_rate_field, 2, 1)
        layout_control_panel_cv.addWidget(self.type_cv_label, 3, 0) 
        layout_control_panel_cv.addWidget(self.type_cv, 3, 1) 
        layout_control_panel_cv.addWidget(self.pulse_inc_label, 4, 0) 
        layout_control_panel_cv.addWidget(self.pulse_inc_field, 4, 1) 
        layout_control_panel_cv.addWidget(self.pulse_height_label, 5, 0) 
        layout_control_panel_cv.addWidget(self.pulse_height_field, 5, 1) 
        layout_control_panel_cv.addWidget(self.save_changes_btn, 6, 0) 
        layout_control_panel_cv.addWidget(self.restore_default_btn, 6, 1)
        layout_control_panel_cv.addWidget(self.start_stop_btn, 7, 0)
        layout_control_panel_cv.addWidget(QWidget(), 7, 1)     
        widget_control_panel_cv = QWidget()
        widget_control_panel_cv.setLayout(layout_control_panel_cv)

        layout_import_export_cv=QGridLayout()
        layout_import_export_cv.addWidget(self.import_data_btn, 0, 0) 
        layout_import_export_cv.addWidget(self.export_data_btn, 0, 1)
        widget_import_export_cv = QWidget()
        widget_import_export_cv.setLayout(layout_import_export_cv)

        layout_lcd=QGridLayout()
        layout_lcd.addWidget(self.lcd_label, 0, 0)
        layout_lcd.addWidget(self.lcd, 0, 1)
        widget_lcd = QWidget()
        widget_lcd.setLayout(layout_lcd)

        layout_graphs_CV = QGridLayout()
        layout_graphs_CV.addWidget(self.graph_cv, 0, 0)
        layout_graphs_CV.addWidget(self.graph_cv_LUT, 1, 0)
        layout_graphs_CV.setRowStretch(0, 2)
        layout_graphs_CV.setRowStretch(1, 1)
        widget_graphs_CV = QWidget()
        widget_graphs_CV.setLayout(layout_graphs_CV)

        layout_cv=QGridLayout()
        layout_cv.addWidget(widget_graphs_CV, 0, 0) 
        layout_cv.addWidget(widget_control_panel_cv, 0, 1)
        layout_cv.addWidget(widget_import_export_cv, 1, 0)
        layout_cv.addWidget(widget_lcd, 1, 1)
        
        widget_cv = QWidget()
        widget_cv.setLayout(layout_cv)
        tabs.addTab(widget_cv, "Cyclic Voltammetry")

        ## "Chronoamperometry " tab
        layout_control_panel_ca=QGridLayout()
        layout_control_panel_ca.addWidget(self.fixed_voltage_label, 0, 0) 
        layout_control_panel_ca.addWidget(self.fixed_voltage_field, 0, 1)
        layout_control_panel_ca.addWidget(self.pulse_voltage_label, 1, 0) 
        layout_control_panel_ca.addWidget(self.pulse_voltage_field, 1, 1) 
        layout_control_panel_ca.addWidget(self.duration_label, 2, 0)
        layout_control_panel_ca.addWidget(self.duration_field, 2, 1) 
        layout_control_panel_ca.addWidget(self.type_ca_label, 3, 0) 
        layout_control_panel_ca.addWidget(self.type_ca, 3, 1) 
        layout_control_panel_ca.addWidget(self.save_changes_btn_ca, 4, 0) 
        layout_control_panel_ca.addWidget(self.restore_default_btn_ca, 4, 1)
        layout_control_panel_ca.addWidget(self.start_stop_btn_ca, 5, 0)
        layout_control_panel_ca.addWidget(QWidget(), 5, 1)        

        widget_control_panel_ca = QWidget()
        widget_control_panel_ca.setLayout(layout_control_panel_ca)

        layout_import_export_ca=QGridLayout()
        layout_import_export_ca.addWidget(self.import_data_btn_ca, 0, 0) 
        layout_import_export_ca.addWidget(self.export_data_btn_ca, 0, 1)
        widget_import_export_ca = QWidget()
        widget_import_export_ca.setLayout(layout_import_export_ca)

        layout_lcdCA=QGridLayout()
        layout_lcdCA.addWidget(self.glucoseCA_label, 0, 0)
        layout_lcdCA.addWidget(self.glucoseCA_LCD, 0, 1)
        layout_lcdCA.addWidget(self.glucoseCA_sufix, 0, 2)
        widget_lcdCA = QWidget()
        widget_lcdCA.setLayout(layout_lcdCA)

        layout_graphs_ca = QGridLayout()
        layout_graphs_ca.addWidget(self.graph_ca, 0, 0)
        layout_graphs_ca.addWidget(self.graph_ca_LUT, 1, 0)
        layout_graphs_ca.setRowStretch(0, 2)
        layout_graphs_ca.setRowStretch(1, 1)
        widget_graphs_ca = QWidget()
        widget_graphs_ca.setLayout(layout_graphs_ca)

        layout_amp=QGridLayout()
        layout_amp.addWidget(widget_graphs_ca, 0, 0) 
        layout_amp.addWidget(widget_control_panel_ca, 0, 1)
        layout_amp.addWidget(widget_import_export_ca, 1, 0)
        layout_amp.addWidget(widget_lcdCA, 1, 1)
        layout_amp.addWidget(QWidget(), 1, 1)

        widget_amp = QWidget()
        widget_amp.setLayout(layout_amp)
        tabs.addTab(widget_amp, "Chronoamperometry ")

        self.setCentralWidget(tabs) #This is a QMainWindow specific function that allows you to set th widget ethat goes in the middle of the window



#############
#  RUN APP  #
#############
if __name__ == '__main__':
    app = QApplication(sys.argv) # Pass in sys.argv to allow command line arguments for your app.
    w = MainWindow()
    app.aboutToQuit.connect(w.ExitHandler) #add this line due to multithreading
    w.show() # IMPORTANT!!!!! Windows are hidden by default.
    sys.exit(app.exec_()) # Start the event loop.
