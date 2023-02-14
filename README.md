# GlucoSense&reg;  Project
## LTEBS Group 2 (2022-2023)

> NB this is the **private development** folder to use after the submission of Feb. 13th for further development that's not going to be graded. What we submitted and all the commits history before Feb. 13th is in [**this repo**](https://github.com/ltebs-polimi/AY2223_I_Project-2)

Hi! Welcome to our project repository. 
We are **Pedro Gianjoppe, Gabriele Paroli, Anna Sparapani** and **Pietro Zerboni** and we worked with or tutor Andrea Rescalli for the "Laboratorio di tecnologie elettroniche e biosensori", a course from professor Cerveri @PoliMi. 

We worked with a PSoC 5 microcontroller to develop a device that could perform Cylic Voltammetry (CV) and Chrono Amperommetry (CA) [^1] procedures with the aim to determine glucose concentration in a solution. 

In this repository you will find all the code we developed. Here's a guide on how to use the GlucoSense and after that you can find more detailed info about all the development steps and choices we made in the past months. We hope this paints a clear picture of our project, but please let us know if you have any doubts! 

###### Table of Contents 
1. [User Guide](#user-guide)
2. [Development Info](#development-info)
3. [References and Acknowledgements](#references-and-acknowledgements)

--------
## User Guide
The device presents a switch, a connector for the glucose strip and a LED. To use it you will also need a computer running the GUI. 
![GlucoSens](/README_pics/vista%20completa.png)
1. Switch on the device, the LED should switch on
2. Start the GUI ([GUI.py](/GUI/GUI.py) file)
3. Press the `Connect to Bluetooth` button on the GUI and wait for the button to become `Disconnect from COM port`. This means that the GUI is connected to the PSoC
4. Press `TIA initialize` and wait a few seconds - *this is needed for the GUI to receive initialization data the device creates when switched on*
6. Insert a glucose strip in the connector (electrodes facing up, see [picture](#glucose-strips-and-connector))
5. Now you can start with the measurements 
   - **Option 1 : Measure Glucose**
   This allows you to perform a simple glucose measure, from the Home Screen button `Measure Glucose`. When this is pressed the device will perform a CA with standard parameters and then evaluate the glucose concentration based on the calibration curve we built.
      1. Add a drop of solution to the strip active site and make sure it turns from yellow to black
      2. Press the `Measure Glucose` button, the LED will start blinking
      3. A couple of seconds later the LED wil stop blinking and you can read the evaluated glucose concentration next to the button 
   - **Option 2 : Cyclic Voltammetry**
   This allows you to perform a CV procedure
      1. Press the `Cyclic Voltammetry` tag in the upper bar of the GUI, this will take you to the CV page
      2. Choose the parameters you want to use 
         - `Min` and `max Voltage` $\in [\pm 2500mV]$ range
         - `Scan Rate` $\in [0, 10] mV/s$ controls the speed of the procedure
         - Select the `Type of CV`, the default is a linear CV but also a Square Wave Voltammetry can be performed
         - If a SWV is chosen, select also the pulse increment and pulse height parameters
      3. Add a drop of solution to the strip active site and make sure it turns from yellow to black and wait for about 20s 
      4. Press `Start`, and wait for the procedure to finish
      5. At the end the GUI will display 
         - the graph of the measured current (mA) against the imposed voltage (mV)
         - the graph of imposed voltages (mV) against time (ms)
         - the optimal working point voltage in the bottom left corner. This is the voltage at which the biggest peak in the duck-plot is observed
   - **Option 3 : Chronoamperommetry**
      1. Press the `Chronoamperommetry` tag in the upper bar of the GUI, this will take you to the CA page
      2. Choose the parameters you want to use 
         - `Fixed Voltage` (mV) is the voltage of the low part of the square wave imposed during the CV
         - `Pulse Voltage` (mV) is the voltage of the pulse part of the CA square wave 
         - `Duration` (ms) is the period of the square wave
         - alternatively, the CA can be performed with standard parameters, which were obtained during calibration and are saved in the PSoC's EEPROM
      3. Add a drop of solution to the strip active site and make sure it turns from yellow to black and wait for about 20s 
      4. Press `Start`, and wait for the procedure to finish
      5. At the end the GUI will display 
         - the graph of the measured current (mA) against the time (ms)
         - the graph of imposed voltages (mV) against time (ms)
         - the glucose concentration based on the current at 500ms. This is accurate only if the `Pulse Voltage` is the standard one (56mV), since the calibration curve has been built with this.

If there are issues you can always disconnect and reconnect the device, as well as switching it off and then back on. Remeber that if you switch the device off, you will have to connect it again and initialize it again (pressing the `Connect to Blutooth` and `Initialize TIA` buttons). 

----
## Development Info 
1. [PsoC C Code](#1-psoc-c-code)  
   - [Files](#c-files)
   - [Interrupts](#interrupt-routines) 
2. [GUI](#2-gui)
   - [User Interface](#user-interface)
   - [Device Calibration](#device-calibration)
3. [PCB](#3-pcb)
4. [Case and Strip Interface](#4-case-and-strip-interface)
   - [Case](#case)
   - [Strip and Connector](#glucose-strips-and-connector)

Overwiew of our development stages and interactions:
![Overview](/README_pics/Overview.png)

### 1. [PSoC C code](/PSoC_Project)
The PSoC microcontroller was programmed through its proprietary software, *PSoC creator*. The code manages all CV and CA measurements and communication with the GUI via UART Bluetooth. 
The main has a "switch cases" structure based on the input coming via BT from the GUI. Here's an example of workflow: 
1. PSoC is switched on and performs initialization procedures
2. when a byte is received on the RX of the UART BT an ISR is called, which saves it in the global variable data_buffer[]
3. when a TAIL is received on the RX of the UART BT, meaning the incoming data is finished, a flag is raised from the ISR
4. in the while(1) of the main, when the flag is raised one of the cases is entered based on data_buffer[0] and the flag is lowered
   > we are using the headers of the incoming data to switch cases in the main.c
5. the code inside the case is exectued

The code relies on a variety of .c files, which group the functions called by the main, according to their objective.

#### .c Files 

- [**`hardware_management.c`**](/PSoC_Project/PSoC_Project.cydsn/hardware_management.c) contains all functions that manage the hardware initialization, sleep, wakeup etc. during the execution of the code. The measurements and procedured rely heavily on the embedded DAC and ADC, as well as a TIA, OpAmps and other components that complete the circuit.
- [**`DAC_management.c`**](/PSoC_Project/PSoC_Project.cydsn/DAC_management.c) two DACs are be used to impose voltage (VDAC with 8 bit resolution = 16 mV resolution, and a DVDAC with 12 bits resolution ~ 1 mV resolution)
This file is called by `hardware_management.c` and simply checks which DAC is selected and performs the start, wakeup etc. function on that component (so that `hardware_management.c` does not have to check every time which DAC is selected). In our project, at this point, the user cannot select which DAC to use via GUI. Wr are always using the 8-bit VDAC; however we built the code so that for future developments, a simple change in the GUI code would allow this option.
- [**`BT_protocols.c`**](/PSoC_Project/PSoC_Project.cydsn/BT_protocols.c) this file contains the function needed to write on the BT to send data to the GUI. The reading is implemented in the BT RX ISR. 
Since when the CV and CA are performed long arrays of data have to be sent, functions in this file manage these long arrays by splitting them up in shorter arrays and sending them in consecutive iterations.
- [**`lut_protocols.c`**](/PSoC_Project/PSoC_Project.cydsn/parametric_lut.c)
the values imposed by the DAC during the procedures are either triangular waves (during CV) or a square wave (during CA). To allow a fast switching between two subsequent steps of imposing the voltage, the values are previosly saved in a Look Up Table (LUT, a global array `uint16 waveform_lut[]`). The values expressed not in mV but in levels of the DAC (255 in case of the VDAC-8 bit and 4096 in case of the DVDAC-12 bit), so they can be directly fed in input to the `DAC_SetValue(value)` API functions. 
- [**`user_inputs.c`**](/PSoC_Project/PSoC_Project.cydsn/user_inputs.c) this file contains functions that are often called by the `main.c` cases and act as a midman between the main and the technical functions contained in the previously discussed files.

#### Interrupt Routines 
- **`dacInterrupt`** called on the rising edge of the PWM wave during the CV and CA procedures, it imposes a DAC value on the counter electrode. It uses the `DAC_SetValue(uint8 value)` API function, and the input is given as a level of the DAC. The conversion from mV to DAC levels is perfomed in the GUI before sending the parameters, so that the LUT is already built in terms of DAC levels.
When the measure is finished (all the wave saved in the LUT has been imposed) it sends the data to the GUI via BT.
- **`adcInterrupt`** called on the falling edge of the PWM wave during CV and CA procedures, it reads the voltage at the working electrode and saves in in th global array `data_long[]` which will be send to the GUI at the end of the procedure.
   > **PWM called ISRs** \\
   the `dacInterrupt` and the `adcInterrupt` are called on the falling and rising edges od the PWM squared wave respectively. Since the user can selected the *Scan Rate* parammeter in the CV procedure, the speed at which the traingular wave is imposed needs to the changed. To allow this, we adapt the PWM period to the scan rate (done by a function from `user_inputs.c`)
- **`Custom_UART_BT_RX_Interrupt`** manages the RX of the BT UART. It is called every time there is an incoming byte on the RX and saves the data in the global array `data_buffer[]`. When byte equals to `TAIL` is received, it raises a flag to signal the main that there is some ready data.

### 2. GUI
#### User Interface
The Graphical User Interface (GUI) has been implemented in Python - `PyQt5`. To run it you just need to open the [GUI.py](/GUI/GUI.py) file in the GUI folder and run the .py code using the PyQt virtual environment. It is necessary to install the [`scipy`](https://pypi.org/project/scipy/), [`PyQt5`](https://pypi.org/project/PyQt5/), [`pyqtgraph`](https://pypi.org/project/pyqtgraph/), [`pyserial`](https://pypi.org/project/pyserial/) and `serial` libraries to run the code.

The GUI performs all interactions between the user (which is the patient or the doctor) and the device, since there are no buttons or LCD screens in the hardware except from the turn ON / OFF button, which can be used to restart the PSoC.

Those interactions are implemented by serial communication between the Bluetooth Module connected to PSoC and the Python `serial` library. Multithreading in PyQt was implemented to establish the connection of the COM port and also for the reading and writing data from PyQt to Bluetooth.

The GUI is divided in 3 different tabs: "Home screen", "Cyclic voltametry" and "Chronoamperometry", each one with its own funcionalities and widgets.

##### 1. Home screen
performs bluetooth connection and one-click glucose measurement (only page that the patient can access).

![image](https://user-images.githubusercontent.com/115043749/218341538-b2fb5780-36c9-4bde-a620-c880835a4c07.png)

When the GUI is started, the connection is switched on by pressing the `Connect to Bluetooth` button which triggers the GUI to scan all serial ports by sending a specific character. When the serial port corresponding to the PSoC BT module responds with the proper character, the port is opened and the device results connected to the GUI. After that, the `TIA initialize` button is used to signal the PSoC that it can send back the values from the initialization of the TIA, in order for the GUI to calculate and store the TIA resistance value.

Below, in this same screen, the `Measure Glucose` button to performs the one-click glucose concentration measurement by performing Chronoamperometry with the default parameters stored found during calibration. Then, the proper current value is retrieved from the measurement and applied to the linear regression model obtained from the calibration of the device. The resulting value, corresponding to the glucose concentration, is displayed.

##### 2. Cyclic Voltammetry
performs and displays Cyclic or Square-Wave Voltammetry procedures with desired parameters, imports and exports measurements to external .csv files and computes optimal voltage for Chronoamperometry 
> Our idea is that this page is accessed only by a clinician who knows how to set the parameters. The patient only uses the Home page and performs simple glucose measurements.

![image](https://user-images.githubusercontent.com/115043749/218313394-8fa9345d-5f5e-4d3a-be1e-b808fb6c6f4d.png)

In the CV screen, the user can choose the CV parameters (*scan rate, start and end values, type of CV*) and start the procedure. It is also possible to save the parameters in the EEPROM (`Save changes`) and retrieve parameters from the EEPROM (`Restore default`).

When the procedure button (`Start`) is pressed, the parameters are sent to the PSoC, which creates the LUT and adapts the period of the PWM. It then starts the procedure and it is possible to interrupt the measurements by pressing the same button (which now displays the label `Stop` in red). When the procedure is finished (in case it is not interrupted by the user), PSoC sends all measured values back to the GUI. The GUI receives two arrays, the first with all measured voltages and the second with the LUT (all imposed voltages), converts the values in mA and mV ranges and then plots them in two graphs: 
- The duck-plot of the Current (mA) in respect to the imposed Voltage (mV)
   > we tested the device, and obtained duck-plots for concentrations in the [50, 200] mg/L range. If you use a much higher or lower concentration the parameters the GUI allows you to choose will probably not lead to a duck-plot.
- The imposed Voltage (mV) with respect to the procedure time (ms) 

It also retrieves the optimal voltage for the Chronoamperometry by computing the value of imposed voltage for which we have the highest current measurement in absolute value (the highest or lowest peak in the duck-plot).

#### 3. Chrono Amperometry
performs and displays ChronoAmperometry procedure with desired parameters, imports and exports measurements to external file and computes the Glucose Concentration (page accessed only by the doctor).
> As for the CV page, our idea is that this page is accessed only by a clinician who knows how to set the parameters.

![image](https://user-images.githubusercontent.com/115043749/218313436-6051896b-81ae-4ca4-9796-01ea3e4653bf.png)

Very similar to the CV screen, the CA screen allows selection of CA parameters and starting of procedure and then plots the measurements data coming from the PSoC in two graphs: 
- The exponential decay plot of the Current (mA) in respect to the procedure Time (ms)
   > as for the CV, we tested the device for concentrations in the [50, 200] mg/L range.
- The imposed Voltage (mV) in respect to the to the procedure Time (ms)

It also calculates the glucose concentration by retrieving the proper current value from the measurements and appling it to the linear regression model obtained from the [calibration](#device-calibration) of the device. The resulting value, corresponding to the glucose concentration, is displayed.

Both CV and CA screens allow for **export** and **import** of data. 
- Data is exported into a `.csv` file and named with the time and date of the measure. 
- Data can be imported to be graphed in the corresponding window

Both CV and CA screens allow to **Save changes to EEPROM** and also to retrieve the **default values from EEPROM**.

#### DEVICE CALIBRATION
In order to allow the computation of the glucose concentration by means of the CA measurements, a calibration step was necessary for the device. The calibration was done using the GUI and followed the steps:
1. Perform Cyclic Voltammetry procedure to find the optimal voltage as the imposed voltage for which the measured current has maximum magnitude 

   ![CV to find the optimal voltage](/README_pics/CV.png)
   The optimal voltage was found at **56mV**

2. Set up the parameter `Pulse Voltage (ms)` of the CA as the optimal voltage from the CV

3. Perform CA procedure for 4 different solutions with known glucose concentration (50, 100, 150 and 200 mg / dL) 

4. For each resulting graph of Current (mA) over Time (ms), retrieve the current measurement at a st time stamp (current measured at 500 ms, so the 50th value, since the CA is performed with 10ms intervals) to be used in the linear regression.
Here are the current vs time graphs we obtained: 
![CA for 4 glucose concentrations](/README_pics/CA.png)

6. Plot the 4 values of current measured at 500ms with respect to the known glucose concentration

7. Apply Linear Regression to find the **Calibration curve** of the device 
![Calibration curve](/README_pics/RL.png)

These plots and the linear regression were done in *MatLab*.


### 3. [PCB](/PCB)
Most of the electrical design needed to perform the procedures is embedded in the PSoC (DACs, ADC, TIA etc.), so the PCB only presents interface elements. 
The PCB was developed in *Eagle* by Autodesk. Here is a simplified schematic created on *Fritzing*:

![Fritzing Schematic](/README_pics/Schematic.png)

As you can see from the image, the board includes:
1. Connections for the BT module. We use a voltage divider to lower the voltage as the TX and RX pins works at 3,3V.
2. The USB breakout board, used to interface the glucose strip with the board
3. One LED
3. The power circuit, we use a 9V DC battery and a standard linearization circuit and a switch (so that the device doesn't need to be connected to the computer running the GUI or to any other power source)

We self made the board in the lab, printing the Eagle schematic and then imprinting it on a copper covered board. We then cut it and pierced it to solder the components. We soldered directly only resistances and capacitors, and placed plugs for all other elements, so that they can be easily removed and reused by the lab.

Here's an image of our printed board 
![Board](/README_pics/Board_front.png) 

and here the back with the elements soldered on
![Board](/README_pics/Board_back.png) 

### 4. [Case and strip interface](/3D/)
Our idea for the case was to have a clean design with only the essential interface elements on the outside. As you can see from the following pics the device presents itself as a box with a switch, connector for the glucose strip, and a LED on the side. 
All 3D modeling was performed using *Autodesk Inventor*, followed by slicing of all parts using *Cura* software. Finally, the parts were 3D printed in PLA using an *Ultimaker 3*.

#### Case
The case was designed and constructed to be compact while allowing separate and easy access to both the PSoC and battery. The PSoC can be accessed by removing the four screws holding the top portion of the case in place, providing access to the PCB and components soldered onto it. 
A dedicated compartment was created in the lower portion of the case to hold the battery, and access to it can be obtained by removing a cover held in place by two screws.
![Case Open](/README_pics/vista%20esplosa.png)
![GlucoSens](/README_pics/GlucoSens.png)

#### Glucose strips and connector 
We chose the [*OneTouch Ultra*](https://www.amazon.it/One-Touch-Ultra-Strisce-Test/dp/B002FU5WHY/ref=sr_1_1?keywords=one+touch+ultra+strisce&qid=1676288893&sr=8-1) glucose strips to develop our device around. They are readily available on Amazon, quite cheap and provide three electrodes easily accessible. 

To have an easy and realiable way to interface the strips with the device electrical pins, we decided to adapt a female USB connector so that three out of four of the electrical lines of the USB would turn into our electrodes lines.
We designed and 3D printed a custom adapter, which leaves the correct space to insert the strip and ensures electrical contact of the electrodes on the strip with three electrodes of the USB, as you can see from the following diagram.
![Connector](/README_pics/Connector.png)

---
### References and Acknowledgements

Our work was inspired by the previous work of Lopin et al. (2018) [^2], which proposed to use the PSoC as a potentiostat and developed PSoC anf GUI code to perform CV and CA procedures.
Our requirements differ in some ways from the aim of Lopin et al., so we had to adapt and change significantly our development, but their code was of great help.

For the choice of the OneTouch Ultra as for glucose strips, we relied on the project by M. Bindhammer, published on *hackaday.io* [^3]. The project is then developed on an ArduinoUNO microcontroller. 

We would like to thank our tutor Andrea Rescalli for his help and insights, as well as the lab's team. 

[^1]:"A Practical Beginner’s Guide to Cyclic Voltammetry"
Noémie Elgrishi, Kelley J. Rountree, Brian D. McCarthy, Eric S. Rountree, Thomas T. Eisenhart, and Jillian L. Dempsey
Journal of Chemical Education 2018 95 (2), 197-206
DOI: 10.1021/acs.jchemed.7b00361

[^2]:"PSoC-Stat: A single chip open source potentiostat based on a Programmable System on a Chip"
Lopin P, Lopin KV (2018) PSoC-Stat: A single chip open source potentiostat based on a Programmable System on a Chip. PLOS ONE 13(7): e0201353. https://doi.org/10.1371/journal.pone.0201353 

[^3]:https://hackaday.io/project/11719-open-source-arduino-blood-glucose-meter-shield
# Gruppo2TEBS_privateDevelopment
