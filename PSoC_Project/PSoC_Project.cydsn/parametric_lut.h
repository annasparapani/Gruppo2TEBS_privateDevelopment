/*******************************************************************************
* File Name: parametric_lut.h
*
* Description:
*  This file contains the function prototypes and constants used for
*  the protocols to create look up tables
*
*********************************************************************************/
#if !defined(PARAMETRIC_LUT_H)
#define PARAMETRIC_LUT_H

//#include <project.h>
#include "stdlib.h"  // remove after testing
#include "stdio.h"  // remove after testing
    
// Local files
#include "globals.h"

/***************************************
*        Function Prototypes
***************************************/    
uint16_t LUT_MakeTriangle_Wave(volatile uint8_t * data_buffer);
uint16_t LUT_MakePulse(uint16_t base, uint16_t pulse, uint16_t ca_period_ms);
uint16_t LUT_make_line(uint16_t start, uint16_t end, uint16_t index);
uint16_t LUT_make_swv_line(uint16_t start, uint16_t end, uint16_t pulse_inc,
                         uint16_t pulse_height, uint16_t index);

/***************************************
* Global variables external identifier
***************************************/

// these should be deleted from here and left just as global variables, let's see if it works this way 

extern uint16_t lut_value;  // value we need to load DAC
volatile extern uint16_t waveform_lut[MAX_LUT_SIZE+5]; // look up table
extern uint16_t dac_ground_value;  // value to load in the DAC -> why is is defined both in globals and in the 
extern uint16_t lut_length;

    
#endif

/* [] END OF FILE */

