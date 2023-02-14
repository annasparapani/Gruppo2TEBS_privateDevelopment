/* ========================================
 *
 * FILE with all the PROTOOLS FOR THE UART
 * BLUETOOTH SERIAL COMMUNICATION
 *
 * ========================================
*/

/*******************************************************
  *** UART INITIALIZATION ***
  * switch on the HW needed by the UART for communication 
  * > already implemented in the UART_Start() API  
*/

#include <project.h>
#include "stdio.h"
#include "stdlib.h"
#include "BT_protocols.h"

/* **************************************************************
   ******************   UART RECEIVE DATA ***********************
   **************************************************************

   the incoming data are parameteres to set the measuring procedures, and instructions 
   from the GUI. They come as : 
                    [HEADER - 1 byte] [DATA - 1 byte] ... [DATA - 1 byte] [TAIL - 1 byte]
   in the main we veirfy if there is incoming data, and if there is we switch the case according 
   to the header. 
                    
   - reading is perfomed by the interrupt of the rx bluetooth, each incoming byte is saved in data_buffer[]
     until the TAIL is received. When tail is received a flag is raised and the main reads the data buffer               
                    
*/

/* **************************************************************
   ******************   UART SEND DATA ***********************
   **************************************************************

   the outgoing data are readings of the ADC during the CV and CA procedures 
   to the GUI and parameters. They come as : 
                    [HEADER - 1 byte] [DATA - 1 byte] ... [DATA - 1 byte] [TAIL - 1 byte]
   they are sent when the measure is finished, or when the buffer reaches MAX_SIZE_BUFFER 
   
   the buffer size is "counted" when the buffer is filled
                    
*/

void writeBT(int sending_size){
    
    data_to_send[sending_size] = TAIL; //tail added at the end of the array
    
    
    UART_BT_PutArray(data_to_send, sending_size+1); // pass the data_buffer[0] address and the buffer size 
                                                    // +1 because we added the tail 
    // clean the sending array
    for(uint8_t i=0; i<DATA_MAX_SENDING_SIZE ; i++){
        data_to_send[i] = 0;
    }
}

void errorBT(void){
    
    UART_BT_PutString("Error");
    
}

/* **************************************************************
   ******************   BT SENDING MANAGER **********************
   **************************************************************

   the UART sends 1 byte at each iteration, and should not send more 
   than DATA_MAX_SENDING_SIZE, otherwise the cnncetion requires too long and 
   it's easier to lose data. This function is used for when all the data from 
   CV and CA needs to be sent. Takes the entire array of data (1000-2000 bytes) 
   and splits it up into arrays of an appropriate sending size which are
   sent by calling the writeBT function 
                    
*/

void BT_sending_manager(uint8_t* data_long_BT_man, int sending_size){
    
    int index=0;
    for(index = 0; index < sending_size; index++){
        if(index<DATA_MAX_SENDING_SIZE){
            data_to_send[index] = data_long_BT_man[index];
        } else {
            if((index%DATA_MAX_SENDING_SIZE)==0&&(index!=0)){ // filled one array to send, we call the sending function
                //writeBT(DATA_MAX_SENDING_SIZE);
                UART_BT_PutArray(data_to_send, DATA_MAX_SENDING_SIZE); // pass the data_buffer[0] address and the buffer size 
                                                    // +1 because we added the tail 
            // clean the sending array
            for(uint8_t i=0; i<DATA_MAX_SENDING_SIZE ; i++){
                data_to_send[i] = 0;
            }
        }
            
            data_to_send[index%DATA_MAX_SENDING_SIZE] = data_long_BT_man[index]; 
        }
    }
    // sending of extra "last piece" of array which does not reach the SENDING SIZE
    // this is not needed in out case, our measures arrays will always be 2*lut_legth
    
    data_to_send[index%DATA_MAX_SENDING_SIZE] = TAIL;
    data_to_send[(index%DATA_MAX_SENDING_SIZE)+1] = TAIL; //tail added at the end of the array
    
    UART_BT_PutArray(data_to_send, (index%DATA_MAX_SENDING_SIZE)+2); // pass the data_buffer[0] address and the buffer size 
                                                    // +1 because we added the tail 
    // clean the sending array
    for(uint8_t i=0; i<DATA_MAX_SENDING_SIZE ; i++){
        data_to_send[i] = 0;
    }
}


void sendMeasures(void){ // called by the ISR when the measures are finished 
    
    BT_sending_manager(data_long, lut_length*2); // send the measured voltages  
    
    for(int i=0; i<DATA_LONG_SIZE; i++){
        data_long[i]=0; 
    }
    
    for(int i = 0; i < lut_length; i++){ //create the array and send the imposed voltages
        uint8_t LSB_data = waveform_lut[i] & 0xFF;
        uint8_t MSB_data = waveform_lut[i] >> 8;
        data_long[2*i] = MSB_data; // i = 0 -> 0 e 1 , i = 2 -> 
        data_long[2*i+1] = LSB_data;
    }
    CyDelay(500);
    
    BT_sending_manager(data_long, lut_length*2); // incompatible pointers, if it's an issue copy 
                                                  // waveform lut inside on data long 
}
/* [] END OF FILE */
