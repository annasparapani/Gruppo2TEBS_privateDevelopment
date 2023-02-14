/*******************************************************************************
* File Name: TIA_calibrate.h
*
* Description:
*  This file contains the function prototypes and constants used for
*  the protocols to calibrate a TIA / delta sigma ADC with an IDAC
*
**********************************************************************************
 * Copyright Kyle Vitautas Lopin, Naresuan University, Phitsanulok Thailand
 * Released under Creative Commons Attribution-ShareAlike  3.0 (CC BY-SA 3.0 US)
*********************************************************************************/

#if !defined(TIA_CALIBRATE_H)
#define TIA_CALIBRATE_H

#include <project.h>
#include "cytypes.h"
#include "math.h"
#include "stdio.h"  // gets rid of the type errors
#include "globals.h"
    
/**************************************
*      Constants
**************************************/

#define AMux_TIA_calibrat_ch 0
#define AMux_TIA_measure_ch 1    

uint16_t calibrate_array[2* Number_calibration_points ];


/***************************************
* Global variables identifier 
***************************************/

uint8_t TIA_resistor_value_index;
//uint8_t ADC_buffer_index; già presente in user_inputs.h
extern float32 uA_per_adc_count;
extern float32 R_analog_route;

/***************************************
*        Function Prototypes
***************************************/  
void calibrate_TIA(uint8_t resistor_value_index /* uint8 ADC_buffer_index >> this maybe remove*/);

#endif
/* [] END OF FILE */



