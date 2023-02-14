/* ========================================
 *
 * HEADER FILE FOR THE BT_protocols.c FILE 
 *
 * ========================================
*/


#if !defined(BT_PROTOCOLS_H)
#define DAC_H

#include <project.h>
#include "globals.h"
#include <stdio.h>
    
/***************************************
*        Function Prototypes
***************************************/  
   
void readBT(void);
void writeBT(int);
void errorBT(void);
void BT_sending_manager(uint8_t* data_long_BT_man, int sending_size);
void sendMeasures(void);


/***************************************
* Global variables external identifier
***************************************/

//extern uint8_t input_flag; -- it was added into the globals.c as static 
//extern uint8_t buffer_index;-- it was added into the globals.c as static
//extern uint8_t connection state;-- it was added into the globals.c as static 

volatile extern uint8_t data_buffer[DATA_MAX_READING_SIZE]; // input buffer -- it was added into the globals.c as static 
volatile extern uint8_t temp[DATA_MAX_READING_SIZE]; //-- it was added into the globals.c as static 


volatile extern uint8_t data_long[DATA_LONG_SIZE]; // entire data to send array
volatile extern uint8_t data_to_send[DATA_MAX_SENDING_SIZE]; // one seding iteration array

volatile extern uint16_t waveform_lut[MAX_LUT_SIZE+5];
    
#endif



/* [] END OF FILE */
