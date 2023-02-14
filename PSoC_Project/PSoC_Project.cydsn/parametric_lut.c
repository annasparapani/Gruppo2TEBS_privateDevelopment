/*******************************************************************************
* File Name: parametric_lut.c
*
* Description:
*  This file contains the protocols to create look up tables
*
*********************************************************************************/

#include "parametric_lut.h"

/******************************************************************************
* Function Name: LUT_MakeTriangleWave
*******************************************************************************
*
* Summary:
*  Fill in the look up table (waveform_lut) for the DACs to perform a cyclic voltammetry experiment.
*  Start the CV protocol at the user defined start value
*
* Parameters:
*  uint16_t start_value: first value to put in the dac
*  uint16_t end_value: to peak dac value
*
* Return:
*  uint16_t: how long the look up table is
*
* Global variables:
*  waveform_lut: Array the look up table is stored in
*
*******************************************************************************/

uint16_t LUT_MakeTriangle_Wave(volatile uint8_t * data_buffer) {
    uint8_t start_value  = data_buffer[2];
    uint8_t end_value    = data_buffer[3];
    uint8_t cv_type      = data_buffer[4];
    
    uint16_t _lut_index = 0;  // start at the beginning of the lut
    
    if(!cv_type) { //cv_type == 0 perform linear CV
         _lut_index = LUT_make_line(start_value, end_value, 0); //retta da start a end salvata nella prima metà di waveform_lut
         _lut_index = LUT_make_line(end_value, start_value, _lut_index-1); //retta da end a start salvata nella seconda metà di waveform_lut
    
    } else{ // cv_type == 1 perform sqw  
        uint8_t pulse_inc    = data_buffer[5];
        uint8_t pulse_height = data_buffer[6];
        _lut_index = LUT_make_swv_line(start_value, end_value, pulse_inc, pulse_height, 0); 
        _lut_index = LUT_make_swv_line(end_value, start_value, pulse_inc, pulse_height, _lut_index-1); 
    }
    waveform_lut[_lut_index] = start_value;  // the DAC is changed before the value is checked in the isr so it will go 
                                            // 1 over so make it stay at last voltage
    _lut_index++;

    return _lut_index;  
}

/******************************************************************************
* Function Name: LUT_make_line
*******************************************************************************
*
* Summary:
*  Make a ramp from start to end in waveform_lut starting at index
*  Does not matter if start or end is higher
*
* Parameters:
*  uint16_t start: first value to put in the look up table
*  uint16_t end: end value to put in the look up table
*  uint16_t index: the place to start putting in numbers in the look up table
*
* Return:
*  uint16_t: first place after the filled in area of the look up table
*
* Global variables:
*  waveform_lut: Array the look up table is stored in
*
*******************************************************************************/

uint16_t LUT_make_line(uint16_t start, uint16_t end, uint16_t index) {
    //printf("start: %i, end: %i\n", start, end);
    if (start < end) {
        for (int16_t value = start; value <= end; value++) {
            waveform_lut[index] = value;
            index ++;
            //printf("l: %i, %i\n", index, value);
            if (index >= MAX_LUT_SIZE) { //questo sarebbe un errore da gestire, per ora non viene comunicato
                return index;
            }
        }
    }
    else {
        for (int16_t value = start; value >= end; value--) {
            waveform_lut[index] = value;
            index ++;
            //printf("b: %i, %i\n", index, value);
            if (index >= MAX_LUT_SIZE) {
                return index;
            }
        }
    }
    //printf("m: %i\n", index);
    return index;
}

/******************************************************************************
* Function Name: LUT_make_swv_side
*******************************************************************************
*
* Summary:
*  Make a ramp with a square wave super imposed, from start to end in waveform_lut
*  starting at index
*  Does not matter if start or end is higher
*
* Parameters:
*  uint16_t start: first value to put in the look up table
*  uint16_t end: end value to put in the look up table
*  uint16_t pulse_inc: increment between the square pulse steps
*  uint16_t pulse_height: height of each square wave pulse
*  uint16_t index: the place to start putting in numbers in the look up table
*
* Return:
*  uint16: first place after the filled in area of the look up table
*
* Global variables:
*  waveform_lut: Array the look up table is stored in
*
*******************************************************************************/

uint16_t LUT_make_swv_line(uint16_t start, uint16_t end, uint16_t pulse_inc, uint16_t pulse_height, uint16_t index) {
    
    if (index > MAX_LUT_SIZE) {
        return index;
    }
    uint16_t half_pulse = pulse_height / 2;
    if (start < end) {
        for (int16_t value = start; value <= end; value+=pulse_inc) {
            waveform_lut[index] = value + half_pulse;
            index ++;
            waveform_lut[index] = value - half_pulse;
            index ++;
            if (index > MAX_LUT_SIZE) {
                break;
            }
        }
    }
    else {
        for (int16_t value = start; value >= end; value-=pulse_inc) {
            waveform_lut[index] = value + half_pulse;
            index ++;
            waveform_lut[index] = value - half_pulse;
            index ++;
            printf("b: %i, %i\n", index, value);
            if (index > MAX_LUT_SIZE) {
                break;
            }
        }
    }
    return index;
}

/******************************************************************************
* Function Name: LUT_MakePulse
*******************************************************************************
*
* Summary:
*  Make a look up table that stores a square pulse sequence
*  Quick hack for chronoamperomerty
*  TODO: THIS SHOULD BE REPLACED
*
* Parameters:
*  uint16_t base: value to be placed in the DAC to maintain the baseline potential
*  uint16_t pulse: value to put in the DAC for the voltage pulse
*
* Global variables:
*  waveform_lut: Array the look up table is stored in
*
*******************************************************************************/

uint16_t LUT_MakePulse(uint16_t base, uint16_t pulse, uint16_t ca_period_ms) {
    int _lut_index = 0;
    int counter_ca = ca_period_ms/(T_PWM_STD_CA/FREQ_CLOCK_PWM);
    while (_lut_index < 20) { 
        waveform_lut[_lut_index] = base;
        _lut_index++;
    }
    while (_lut_index < 20+counter_ca) {
        waveform_lut[_lut_index] = pulse;
        _lut_index++;
    }
    while (_lut_index < 40+counter_ca) { 
        waveform_lut[_lut_index] = base;
        _lut_index++;
    }
    return _lut_index;
}

/* [] END OF FILE */

