/*******************************************************************************
* File Name: hardware_management.h
*
* Description:
*  Functions prototypes and their variables used by main.
*********************************************************************************/

#if !defined(HARDWARE_MANAGEMENT_H)
#define HARDWARE_MANAGEMENT_H

#include <project.h>
#include "cytypes.h"
#include "stdio.h"  // gets rid of the type errors
#include "DAC_management.h"
#include "BT_protocols.h"
#include "globals.h"
    
/***************************************
*        Variables
***************************************/     
    
    
    
/***************************************
*        Function Prototypes
***************************************/ 

uint8_t helper_check_voltage_source(void);
void helper_set_voltage_source(uint8_t voltage_source);

uint8_t helper_Writebyte_EEPROM(uint8_t data, uint16_t address);
uint8_t helper_Readbyte_EEPROM(uint16_t address);

void helper_HardwareSetup(void);
void helper_HardwareStart(void);
void helper_HardwareSleep(void);
void helper_HardwareWakeup(void);
uint16_t helper_Convert2Dec(const uint8_t array[], const uint8_t len);

void initialize_default_values(void);

#endif

/* [] END OF FILE */

