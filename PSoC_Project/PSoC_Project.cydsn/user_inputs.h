/*******************************************************************************
* File Name: user_inputs.h
*********************************************************************************/

#if !defined(USER_INPUTS_H)
#define USER_INPUTS_H
    
#include <project.h>
#include "stdio.h"  // gets rid of the type errors
    
#include "globals.h"
#include "hardware_management.h"
#include "BT_protocols.h"
#include "parametric_lut.h"
    
#define DO_NOT_RESTART_ADC      0
   
/***************************************
*        Function Prototypes
***************************************/  
   
void user_setup_TIA_ADC(volatile uint8_t data_buffer[]);
void user_voltage_source_funcs(uint8_t data_buffer[]);
void user_run_procedure(void);
void user_reset_device(void);
void user_set_isr_timer(volatile uint8_t data_buffer[]);
void user_EEPROM_management(uint8_t data_buffer[]);
uint16_t user_chrono_lut_maker(volatile uint8_t data_buffer[]);

/***************************************
* Global variables external identifier
***************************************/

extern uint8_t TIA_resistor_value_index;
extern uint8_t ADC_buffer_index;
extern uint8_t voltage_source;
    
#endif

/* [] END OF FILE */

