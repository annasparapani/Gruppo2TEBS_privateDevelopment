/*******************************************************************************
* File Name: globals.h
*
* Description:
*  Global variables, unions and structures to use in the project
*********************************************************************************/

#if !defined(GLOBALS_H)
#define GLOBALS_H
    
#include "cytypes.h"
#include "stdio.h"  // gets rid of the type errors
    
#define FREQ_CLOCK_PWM 2400000
#define T_PWM_STD_CA 10*FREQ_CLOCK_PWM //10ms, here it is expressed in clock repetitions
    
#define TIA_RESISTOR_DEFAULT_VALUE_INDEX 0
#define Number_calibration_points 5
    
/**************************************
*        BT OUTPUT OPTIONS 
**************************************/ 
    
#define DATA_MAX_SENDING_SIZE       120 // max number of bytes to send with BT 
#define DATA_MAX_READING_SIZE       20
#define DATA_LONG_SIZE              5000 // max number of bytes to send with BT
#define PARAMS_SENDING_SIZE         2 // bytes sent when parameters are read 
#define BT_SET                      'F'
#define CV_PARAMS_SET               'B'
#define CA_PARAMS_SET               'C'
#define TIA_SET                     'A'
#define EEPROM_SET                  'R'
#define WHICH_DAC_IS_SET            'S'
#define HEADER_CHECK                'Y'

#define CV_DATA                     'M'
#define CA_DATA                     'M'
#define EEPROM_DATA_CV              'E'
#define EEPROM_DATA_CA              'E'
// TO DO aggiungere header per LUT quando viene inviata 


/**************************************
*        BT INPUT OPTIONS
**************************************/ 
// redefine these with BT [header] options 

#define TIA_CALIBRATE           'A'
#define CHANGE_CV_PARAMETERS    'B'
#define CHANGE_CA_PARAMETERS    'C'
#define RUN_CV                  'D'
#define RUN_CA                  'E'
#define TAIL                    'Z'
#define EEPROM_MANAGEMENT       'R'  
#define DAC_MANAGEMENT          'S'
#define CONNECT_BT              'F' 
#define TIA_INITIALIZATION      'I'    


/**************************************
*        EEPROM ADDRESSES
**************************************/ 

#define V_DEFAULT_ADD            0x00
#define T_DEFAULT_ADD            0x01
#define SCAN_RATE_DEFAULT_ADD    0x02
#define START_DEFAULT_ADD        0x03
#define END_DEFAULT_ADD          0x04
#define INCREMENT_DEFAULT_ADD    0x05
#define HEIGH_DEFAULT_ADD        0x06
#define CA_PERIOD_DEFAULT_ADD    0x07
#define EEPROM_FLAG_ADD          0xFF
    
#define V_DEFAULT_ADD_USER            0x08
#define T_DEFAULT_ADD_USER            0x0A
#define SCAN_RATE_DEFAULT_ADD_USER    0x0B
#define START_DEFAULT_ADD_USER        0x0C
#define END_DEFAULT_ADD_USER          0x0D
#define INCREMENT_DEFAULT_ADD_USER    0x0E
#define HEIGH_DEFAULT_ADD_USER        0x0F
#define CA_PERIOD_DEFAULT_ADD_USER    0x10

#define R_TIA_CALIBRATION             0x11
#define Q_TIA_CALIBRATION             0x12

#define V_DEFAULT                0x38 // 56mv 
#define CA_PERIOD_DEFAULT        0x0A // 10 tenths of a second
#define SCAN_RATE_DEFAULT        0x05 // mV/s
#define START_DEFAULT            0b11100010 // -30 (*10)mV binary signed (2's complement)
#define END_DEFAULT              0x1E // 30 (*10)mV
#define INCREMENT_DEFAULT        0x01 // 1mV 
#define HEIGH_DEFAULT            0x02 // 2mV
    
#define EEPROM_FLAG              0
#define EEPROM_FLAG_DONE         1   

/**************************************
*           ADC Constants
**************************************/  
    
// define how big to make the arrays for the lut for dac and how big
// to make the adc data array     
#define MAX_LUT_SIZE 5000
#define ADC_CHANNELS 4
 
    
/**************************************
*           API Constants
**************************************/
#define true                        1
#define false                       0
    
#define VIRTUAL_GROUND              2048

// Define the AMux channels
#define two_electrode_config_ch     0 //** we are not giving the user the option to change this parameter yet **
//#define three_electrode_config_ch   1 // called by helper_hardwareSetup()
#define AMux_TIA_working_electrode_ch 1 // called by helper_hardwareSetup

/**************************************
*        EEPROM API Constants
**************************************/

#define VDAC_NOT_SET 0 //these are called by DAC_checkVoltageSource
#define VDAC_IS_VDAC 1
#define VDAC_IS_DVDAC 2
    
#define VDAC_ADDRESS 0 // saving which DAC is selected in the EEPROM address 0x00 
    
#define EEPROM_READ_TEMPERATURE_CORRECT        0
    
    
/**************************************
*        AMuX API Constants
**************************************/
    
//#define DVDAC_channel 1       already defined in DAC_management.h
//#define VDAC_channel 0
    
    
/**************************************
*        Global Variables
**************************************/   
    
uint16_t    dac_ground_value;  // value to load in the DAC
                               // **also defined as extern in parametric_lut.h**
cystatus    return_value; // return value used every time we want to read from the EEPROM
    
/* Make global variables needed for the DAC/ADC interrupt service routines */
    
// LUT VARIABLES
uint16_t    lut_value;  // value need to load DAC, also defined as extern 
// add 5 to the lut to add a buffer cause a few functions go over the MAX_LUT_SIZE
// by 1, and will use MAX_LUT_SIZE to check for over runs
    
volatile uint16_t waveform_lut[MAX_LUT_SIZE+5];  // **also defineed as extern in parametric_lut.h**
uint16_t    lut_index;  
uint16_t    lut_length;

uint8_t tia_calibration_values[(Number_calibration_points*4)+1];

// DAC VARIABLES 
uint8_t selected_voltage_source;

// BT VARIABLES


volatile static uint8_t buffer_index;
volatile static uint8_t connection_state; //static because it is used inside the ISR
volatile static uint8_t input_flag; //static because it is used inside the ISR

volatile uint8_t data_buffer[DATA_MAX_READING_SIZE]; //static because it is used inside the ISR
volatile uint8_t temp[DATA_MAX_READING_SIZE]; //static because it is used inside the ISR

volatile uint8_t data_to_send[DATA_MAX_SENDING_SIZE];
volatile uint8_t data_long[DATA_LONG_SIZE];


uint8_t finished_procedure_flag; // DEBUG CHANGE -- remove later


#endif    
/* [] END OF FILE */
