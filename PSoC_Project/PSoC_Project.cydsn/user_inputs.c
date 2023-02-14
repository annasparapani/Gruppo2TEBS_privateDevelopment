/*******************************************************************************
* File Name: user_inputs.c
*
* Description:
*  Make some of functions the user can call through the USB. 
*  Only for the longer functions to keep main clearer
*
**********************************************************************************
 * Copyright Kyle Vitautas Lopin, Naresuan University, Phitsanulok Thailand
 * Released under Creative Commons Attribution-ShareAlike  3.0 (CC BY-SA 3.0 US)
*********************************************************************************/

#include <project.h>
#include "stdio.h"  // gets rid of the type errors

#include "user_inputs.h"


/******************************************************************************
* Function Name: user_setup_TIA_ADC
*******************************************************************************
*
* Summary:
*  Set up the TIA and ADC circuit by changing the TIA resistor value, the ADC gain
*    setup if the user wants to use an external resistor
*  Parameters:
*  uint8 data_buffer[]: array of chars with the parameters to set the adc and tia components
*  input is A|U|X|Y|Z|W: where 
*  U is ADC configuration to use where config 1 uses a Vref of +-2.048 V and config 2 uses +-1.024 V
*  X - the TIA resistor value index, a string between 0-7 that sets the TIA resisot value
*  Y - the adc buffer gain setting
*  Z - T or F for if an external resistor is to be used and the AMux_working_electrode should be set according
*  W - 0 or 1 for which user resistor should be selected by AMux_working_electrode
*  
* Global variables: - found in calibrate.h and used by calibrate.c
*  uint8 TIA_resistor_value_index: index of whick TIA resistor to use, Supplied by USB input
*  uint8 ADC_buffer_index: which ADC buffer is used, gain = 2**ADC_buffer_index
*
* Return:
*  array of 20 bytes is loaded into the USB in endpoint (into the computer)
*
*******************************************************************************/

void user_setup_TIA_ADC(volatile uint8_t data_buffer[]) {
    //const uint16_t calibrate_TIA_resistor_list[]= {20, 30, 40, 80, 120, 250, 500, 1000};
    
    TIA_resistor_value_index = data_buffer[2];
    if (TIA_resistor_value_index >= 0 && TIA_resistor_value_index <= 7) {
        TIA_SetResFB(TIA_resistor_value_index);  // see TIA.h for how the values work, basically 0 - 20k, 1 -30k, etc.
    }
    
    // I'M LEAVING ONKY THE OPTION TO CHANGE THE TIA RESISTOR, not the ADC configuration and the external R option
    // they are still here, can be added later 
    
    /*uint8_t adc_config = data_buffer[2];
    if (adc_config == 1 || adc_config == 2) {
        ADC_SigDel_SelectConfiguration(adc_config, DO_NOT_RESTART_ADC); 
    }
    
    ADC_buffer_index = data_buffer[6]-'0';
    if (ADC_buffer_index >= 0 || ADC_buffer_index <= 3) { //se l'utente non usa R ext, manda 0 in automatico
        ADC_SigDel_SetBufferGain(ADC_buffer_index); 
    }
    if (data_buffer[8] == 'T') {
        tia_mux.use_extra_resistor = true;
        tia_mux.user_channel = data_buffer[10]-'0';  // not used yet, forse da togliere
        data_buffer[8] = 0;
        AMux_TIA_resistor_bypass_Connect(0);
    }
    else {
        AMux_TIA_resistor_bypass_Disconnect(0);
    }*/
}

/******************************************************************************
* Function Name: user_voltage_source_funcs
*******************************************************************************
*
* Summary:
*  Check if the user wants to read what type of DAC is to be used or the user can set the DAC to be used

* Parameters:
*  uint8 data_buffer[]: array of chars used to setup the DAC or to read the DAC settings
*  input is VXY: where X is either 'R' or 'S' for read or set
*  Y is '0' or '1'
*
* Return:
*  export the DAC information to the USB, or set the DAC source depending on user input
*
*******************************************************************************/

void user_voltage_source_funcs(uint8_t data_buffer[]) {
    if (data_buffer[1] == 'R') {  // User wants to read status of VDAC, forse da togliere
        data_to_send[0] = WHICH_DAC_IS_SET;
        data_to_send[1] = helper_check_voltage_source();
        writeBT(PARAMS_SENDING_SIZE);
    }
    else if (data_buffer[1] == 'S') {  // User wants to set the voltage source
        helper_set_voltage_source(data_buffer[2]);
        DAC_Start();
        DAC_Sleep();
    }
}

/******************************************************************************
* Function Name: user_run_procedure
*******************************************************************************
*
* Summary:
*  Start a cyclic voltammetry experiment.  The look up table in waveform_lut should
*  already be created.  If the dac isr is already running this will not start and throws
*  and error through the USB.  
*  
* Global variables:
*  uint16_t lut_value: value gotten from the look up table that is to be applied to the DAC
*  uint16_t lut_index: current index of the look up table
*  uint16_t waveform_lut[]:  look up table of the waveform to apply to the DAC
*
* Parameters:
*  None
*
* Return:
*  Starts the isr's used to perform an experiment else if the dac is already running,
*  possibly because another experiment is already running, return an error through the USB
*
*******************************************************************************/

void user_run_procedure(void){
    helper_HardwareWakeup(); 
    if (!isr_dac_GetState()){  // enable the dac isr if it isnt already enabled
        if (isr_adcAmp_GetState()) {  // User has started cyclic voltammetry while amp is already running so disable amperometry
            isr_adcAmp_Disable();
        }
        lut_index = 0;  // start at the beginning of the look up table
        lut_value = waveform_lut[0];
        
        
        helper_HardwareWakeup();  // start the hardware
        Opamp_Aux_Start(); 
        ADC_SigDel_Start(); 
        
        DAC_SetValue(lut_value);  // let the electrode equilibriate
        CyDelay(20);  // let the electrode voltage settle
        ADC_SigDel_StartConvert();  // start the converstion process of the delta sigma adc so it will be ready to read when needed
        CyDelay(10);
   
        
        int16 measure = ADC_SigDel_GetResult16(); 
        
        data_long[lut_index]= measure >> 8;
        data_long[lut_index+1]= measure & 0xFF;
        
        lut_index++; 
        
        isr_dac_Enable();  // enable the interrupts to start the dac
        isr_adc_Enable();  // and the adc
        
    }
}

/******************************************************************************
* Function Name: user_reset_device
*******************************************************************************
*
* Summary:
*  Stop all operations by disabling all the isrs and reset the look up table index to 0
*  
* Global variables:
*  uint16_t lut_index: current index of the look up table
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/

void user_reset_device(void) {
    isr_dac_Disable();
    isr_adc_Disable();
    isr_adcAmp_Disable();
    helper_HardwareSleep();
    
    lut_index = 0;  
}

/******************************************************************************
* Function Name: user_set_isr_timer
*******************************************************************************
*
* Summary:
*  Set the PWM that is used as the isr timer
* 
* Parameters:
*  uint8 data_buffer[]: array of chars used to setup the DAC or to read the DAC settings
*  input is T|XXXXX
*  XXXXX - uint16_t with the number to put in the period register
*  the compare register will be loaded with XXXXX / 2
*  
* Return:
*  Set the compare and period register of the pwm 
*
*******************************************************************************/


void user_set_isr_timer(volatile uint8_t data_buffer[]) {
    PWM_isr_Wakeup();
    uint16_t scan_rate = data_buffer[1];  //arriva il valore di scan rate
    uint16_t resolution_dac=1;
    
    uint8_t voltage_source = helper_check_voltage_source();
    
    if(voltage_source){
       resolution_dac = 16;
    }
    else if(voltage_source==2){
       resolution_dac = 1;
    }
    uint16_t timer_period = (resolution_dac/scan_rate)*FREQ_CLOCK_PWM;
    PWM_isr_WriteCompare((uint16_t)(timer_period / 2));  // not used in amperometry run so just set in the middle
    PWM_isr_WritePeriod(timer_period-1);
    PWM_isr_Sleep();
}

/******************************************************************************
* Function Name: user_chrono_lut_maker
*******************************************************************************
*
* Summary:
*  Make a look up table that will run a chronoamperometry experiment.  Hackish now
* 
* Parameters:
*  uint8 data_buffer[]: array of chars used to make the look up table
*  input is Q|XXXX|YYYY|ZZZZZ: 
*  XXXX - uint16_t with the number to put in the DAC for the baseline voltage and the 
*  voltage to be applied after the pulse.
*  YYYY - uint16_t with the number to put in the dac during the pulse of the experiment
*  ZZZZZ - uint16_t to put in the period of the PWM timer to set the sampling rate
*  
* Global variables:
*  uint16_t lut_value: value gotten from the look up table that is to be applied to the DAC
*  uint16_t waveform_lut[]:  look up table of the waveform to apply to the DAC
*  
* Return:
*  4000 - how long the look up table will be
*
*******************************************************************************/

uint16_t user_chrono_lut_maker(volatile uint8_t data_buffer[]) {
    
    PWM_isr_Wakeup(); 
    
    if(!data_buffer[1]){ // data_buffer[1]==0 da il tipo di misura 
        uint16_t ca_period; 
        
        ca_period = (data_buffer[2] << 8) | data_buffer[3]; 
        
        lut_length = LUT_MakePulse(data_buffer[5], data_buffer[4], ca_period);
    }
    else{
        uint16_t baseline = 0b01111111; // 0V = DAC value equal to 128
        uint16_t pulse = EEPROM_ReadByte(V_DEFAULT_ADD); 
        uint16_t ca_period = EEPROM_ReadByte(CA_PERIOD_DEFAULT_ADD); 
        lut_length = LUT_MakePulse(baseline, pulse, ca_period);
    }
    
    //cambio il periodo del PWM utilizzando 10ms (default), leggerò la misura ogni 10ms

    PWM_isr_WriteCompare((uint16_t)(T_PWM_STD_CA / 2));  
    PWM_isr_WritePeriod((uint16_t) T_PWM_STD_CA - 1);
                
    lut_value = waveform_lut[0];  // setup the dac so when it starts it will be at the correct voltage
           
    PWM_isr_Sleep();
    return lut_length; // ritorna la lunghezza della lut creata (varia in base al tempo in cui il voltaggio è alto)
}

/******************************************************************************
* Function Name: user_EEPROM_management
*******************************************************************************
*
* Summary:
*  We use this function in order to read the default values stored in the EEPROM (if data_buffer[1]==1)
*  or to write new default values (if data_buffer[2]==0).
* 
* Parameters: 
*  uint8 data_buffer[]: array of chars that is used to pass the values that we want to write in the EEPROM  
*  
* Global variables:
*  cystatus return_value: return value of the EEPROM_WriteByte function, if it is != CYRET_SUCCESS an error has occurred
*  
* Return:
*  None
*
*******************************************************************************/

void user_EEPROM_management(uint8_t data_buffer[]){
    uint8_t i = 1;
    if (!data_buffer[i]){  // if data_buffer[1] is equal to 0 the user reads the default CV values from the EEPROM
        data_to_send[0] = EEPROM_DATA_CV;
        data_to_send[i++] = EEPROM_ReadByte(SCAN_RATE_DEFAULT_ADD);
        data_to_send[i++] = EEPROM_ReadByte(START_DEFAULT_ADD);
        data_to_send[i++] = EEPROM_ReadByte(END_DEFAULT_ADD);
        data_to_send[i++] = EEPROM_ReadByte(INCREMENT_DEFAULT_ADD);
        data_to_send[i++] = EEPROM_ReadByte(HEIGH_DEFAULT_ADD);
        
        writeBT(i);
    }
    else if (data_buffer[i] == 1){
        data_to_send[0] = EEPROM_DATA_CA;
        data_to_send[i++] = EEPROM_ReadByte(V_DEFAULT_ADD);
        data_to_send[i++] = EEPROM_ReadByte(T_DEFAULT_ADD);
    }
    else if (data_buffer[i] == 2){ // if data_buffer[1] is equal to 2 the user writes new CV default values in the EEPROM
        return_value = EEPROM_WriteByte(data_buffer[2],SCAN_RATE_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
        return_value = EEPROM_WriteByte(data_buffer[3],START_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
        return_value = EEPROM_WriteByte(data_buffer[4],END_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
        return_value = EEPROM_WriteByte(data_buffer[5],INCREMENT_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
        return_value = EEPROM_WriteByte(data_buffer[6],HEIGH_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
    }
    else if (data_buffer[i] == 3){ // if data_buffer[1] is equal to 3 the user writes new CV default values in the EEPROM
        return_value = EEPROM_WriteByte(data_buffer[2],V_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
        return_value = EEPROM_WriteByte(data_buffer[3],CA_PERIOD_DEFAULT_ADD_USER);
             if(return_value != CYRET_SUCCESS){
               errorBT();
        }
    }
}

/* [] END OF FILE */

