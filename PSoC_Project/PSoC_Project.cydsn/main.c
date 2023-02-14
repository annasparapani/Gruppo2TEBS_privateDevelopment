/* ========================================
 * GlucoSens project
 * We worked with a PSoC 5 microcontroller to develop a device that could 
 * perform Cylic Voltammetry (CV) and Chrono Amperommetry (CA) procedures 
 * with the aim to determine glucose concentration in a solution.
 * The main has a switch cases structure, and you an find an in depth
 * description of the code in the README.md file in the PSoC code section.
 * 
 * P. Gianjoppe, G. Paroli, A. Sparapani, P. Zerboni
 * Gruppo 2 - Laboratorio di Tecnologie Elettroniche e Biosensori 
 * Tutor: A. Rescalli 
 * Professor: P. Cerveri
 * 
 * Politecnico di Milano, a.a. 2022-2023
 * ========================================
*/

#include "project.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"


// local files
#include "TIA_calibrate.h"
#include "DAC_management.h"
#include "globals.h"
#include "hardware_management.h"
#include "user_inputs.h"
#include "parametric_lut.h"
#include "BT_protocols.h"
#include "Interrupt_Routines.h"


/************************************
******* ISRs Custom Defined *********
************************************/

CY_ISR(dacInterrupt) // enabled by function that start CV and CA procedures 
{
    Opamp_Aux_Start();  
    DAC_SetValue(lut_value);
    LED_DAC_Write(1); 
    LED_ADC_Write(0);

    lut_index++;
    
    if (lut_index >= lut_length) { // all the data points have been given and sent 
        isr_adc_Disable();
        isr_dac_Disable();
        finished_procedure_flag=1;
        
        UART_BT_PutChar(CV_DATA);
        // we are sending first one Byte with the header, and after that the array with all the data
        // otherwise we would have to shift everything to the right
        
        sendMeasures();
        helper_HardwareSleep();
        lut_index = 0; 
    }
    lut_value = waveform_lut[lut_index];
}

CY_ISR(adcInterrupt){ // enabled by function that starts CV and CA procedures 
    LED_DAC_Write(0); 
    LED_ADC_Write(1);
    
    //ADC_SigDel_Start();
    //ADC_SigDel_StartConvert();
    
    int16 measure = ADC_SigDel_GetResult16(); 
    
    uint8_t LSB_data = measure & 0xFF;
    uint8_t MSB_data = measure >> 8;
    data_long[2*lut_index] = MSB_data;
    data_long[2*lut_index+1] = LSB_data;
}

CY_ISR(Custom_UART_BT_RX_Interrupt){ // called when incoming data is available on the RX of the UART 
    //WARNING: this function is called each time a new byte arrive, so if we receive the byte array 'AB' the function will be called twice
    // to change it, we must change "Interrupt Source" parameter on the TopDesign
    LED_DAC_Write(1);
    temp[buffer_index] = UART_BT_GetByte();   
    
    if(temp[buffer_index] == 'Z'){
        input_flag = 1; // raises input flag, so that te main calls the BT reading at the next while(1) iteration  
        
        
        for(uint16_t i=0; i<DATA_MAX_READING_SIZE ; i++){
            data_buffer[i] = temp[i];
            temp[i] =0; //clearing data buffer
        }   
    }
    
    if(input_flag){
        buffer_index = 0;
    } else {
        buffer_index++;
    }
}

int main(void){
    
    CyGlobalIntEnable; // Enable global interrupts
    
    input_flag = 0; 
    connection_state = 0;
    buffer_index = 0;
    lut_index=0; 
    finished_procedure_flag=0; // DEBUG CHANGE -- delete later 

    /* *********************************
       ******* INITIALIZATION CODE *****
       *********************************
       all the steps to perform when the device is switched on 
       1. hardware wakeup (bluetooth and all the other hw) 
       2. enable of interrupts
       3. calibration of the TIA with default value of R */
    
    helper_HardwareSetup(); /* from hardware_management.c 
                               1. calls Init() API functions of all AMUX
                               2. calls Start() from DAC_management.c, which starts the DAC selected 
                                -> in the beginning no DAC is selected - we can change this 
                               3. selects 3 electrodes configuration
                               4. connects the TIA to the working electrode */
    
    ADC_SigDel_SelectConfiguration(2, DO_NOT_RESTART_ADC); /* selects configuration of the ADC 
                                                              config2 has +- 1.024 V of input range*/
    
    /* Start end then disable the CV and CA ISR */
    isr_dac_StartEx(dacInterrupt);
    isr_dac_Disable();  // disable interrupt until a voltage signal needs to be given
    isr_adc_StartEx(adcInterrupt);
    isr_adc_Disable();
    isr_UART_BT_RX_StartEx(Custom_UART_BT_RX_Interrupt); 
   
    
    UART_BT_Start(); // switch on the communication with the Bluetooth 
    CyDelay(100); // give a little time to the BT module to tune and set
    
    //Clear the data_buffer and the data_to_send arrays
    for(uint16_t i=0; i<DATA_MAX_SENDING_SIZE ; i++){
        data_buffer[i] = 0;
        data_to_send[i] = 0;
    }
    
    for(uint16_t i=0; i<DATA_LONG_SIZE ; i++){
        data_long[i] = 0;
    }
    
    // TIA INITIALIZATION
    TIA_SetResFB(TIA_RESISTOR_DEFAULT_VALUE_INDEX); 
    calibrate_TIA(TIA_RESISTOR_DEFAULT_VALUE_INDEX); // calibration of the TIA with default R = 20 kOhm
    
    // EEPROM INITIALIZATION
    EEPROM_Start();
    if(EEPROM_ReadByte(EEPROM_FLAG_ADD)!=1){
        EEPROM_WriteByte(EEPROM_FLAG, EEPROM_FLAG_ADD);
    }
    
    if(EEPROM_ReadByte(EEPROM_FLAG_ADD) == 0){
       initialize_default_values(); // the default values in the EEPROM are initialized
    }
    
    //CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE); -- si può toglierlo
    
    
    
    for(;;) {
        
        //input_flag = 1; //DEBUG CHANGE -- delete later
        
        CyDelay(100);
        
    
        //CyWdtClear(); // clears the watchdog timer -> stops here
        
        /* ************************
           ******* CASES CODE *****
           ************************ 
           during the while(1) - normal functioning - of the device, the state of the device is set with a 
           switch(cases) structure which depends on the input coming from the Bluetooth UART 
           - the Bluetooth packets are defined in such a way [header][data][tail] that the header is a unique 
             identifier for the information the data is carrying and can be used to enter the correct state 
             >> [headers] need to be declared and defined in globals.h for accessing the cases
           - for the timing, at each while(1) iteration the input is checked, and if a new input is available the 
             state is changed 
             >> for this reason when before exiting a state and repeating the while(1) all the opearations of said
                state need to be concluded 
             >> in the GUI the user should not be able to send data on the BT while the device is performing the 
                processes of a specific case 
        */ 
        if(input_flag==1){ // we have a new  iput -> go to the related state (case) 
            
            input_flag=0;
            
            
            
            // DEBUG CHANGE -- hard coding of data buffer 
            //data_buffer[0]='D'; // CV change parameters
            //data_buffer[1]= 10; // scan rate
            //data_buffer[2]= 20; // start value 
            //data_buffer[3]= 150; // end value
            //data_buffer[4]= 0; // cv type = linear

            switch (data_buffer[0]) { //switch based on the header of the incoming packet  
               
            case CONNECT_BT:;
                    
                    data_to_send[0]=BT_SET; // send back to Py the 0xFF flag, so that Py can open the connection with the serial port 
                    writeBT(1); 
                    CyDelay(100);
            break;
                    
            case TIA_INITIALIZATION:;
                    connection_state=1;
                    for(int i=0; i<(Number_calibration_points*4)+1; i++){
                        data_to_send[i] = tia_calibration_values[i];
                    }
                    writeBT((Number_calibration_points*4)+1);               
            break;    
                    
            case TIA_CALIBRATE: ; // user has changed some parameters regarding the TIA (impedance, ADC configuration) 
                                  // the TIA is calibrated again
                if(data_buffer[1] == 0x00){
                    user_setup_TIA_ADC(data_buffer);
                    calibrate_TIA(data_buffer[2]); 
                    break;
                }else if(data_buffer[1] == 0x01){
                    EEPROM_WriteByte(data_buffer[2], R_TIA_CALIBRATION);
                    EEPROM_WriteByte(data_buffer[3], Q_TIA_CALIBRATION);
                    break;
                }
            break;

            
            //****************** CHANGE VOLTAMMETRY PARMS CASE ******************************************
            case CHANGE_CV_PARAMETERS: ; /* user has changed parameters of the CV from the GUI need to
                                            (1) update the T_PWM according to the scan rate,
                                            (2) create the LUT according to start and end values */ 
                user_set_isr_timer(data_buffer); /*give the scan rate in ms (from UART)*/
                lut_length = LUT_MakeTriangle_Wave(data_buffer);
                    /*start and end values in bits (mv->bit processing in Py) from UART
                                        and which type of cv to perform*/
                                      // for now, pulse height and increment of SWV are hard coded 
                                      // can add later option to change them 
                                      // to keep the same function, can pass 0 and 0 in case of simple cv
                
                
                // send confirmation that parameters have been set to Python 
                data_to_send[0] = CV_PARAMS_SET; 
                writeBT(1);   
                CyDelay(100);
     
            break; 
           
            //****************** CHANGE CHRONOAMPEROMETRY PARMS CASE *************************************  
            case CHANGE_CA_PARAMETERS: ; /* can set 
                                            - which CA to make (1) with parameters (2) measure with dft
                                            - if parameters: Voltage and duration of stimulation*/
                lut_length = user_chrono_lut_maker(data_buffer);
                                       /*type of CV, voltage and duration
                                        if measure -> duration = 0 (not used by user_chrono_lut_maker), V = 0 
                                                       >> this in Py! 
                                        if not CA  -> duration and V chosen by the user */

                //send confirmation that parameters have been set to Python 
                data_to_send[0] = CA_PARAMS_SET; 
                writeBT(1); 
                
                //if (data_buffer[1]==1) goto case E; // go directly to running the CA if measuring (just one button to press)
                                                      // look up  the correct syntax for this!
            break; 
                
            case RUN_CV : ; // user has pressed "start" on the CV (available after setting parameters) 
                //user_set_isr_timer(data_buffer); // DEBUG CHANGE -- remove later
                //lut_length = LUT_MakeTriangle_Wave(data_buffer); // DEBUG CHANGE -- remove later 
                
                user_run_procedure(); 
                
            break; 
                
            case RUN_CA : ; // user has pressed "start" on the CA interface (or coming from MEASURE)
                //lut_length = user_chrono_lut_maker(data_buffer);
                user_run_procedure(); 
                /* using the same function for CV and CA, since a differentiated look up table has been created */
            break;
            
            case EEPROM_MANAGEMENT: ;             
                user_EEPROM_management(data_buffer);
                data_to_send[0]=EEPROM_SET; 
                writeBT(PARAMS_SENDING_SIZE); 
            break;
                
            case DAC_MANAGEMENT:;
                user_voltage_source_funcs(data_buffer); 
            break; 
        } 
    }
        if(finished_procedure_flag){ // DEBUG CHANGE -- delete later the if case
            
            for (int i = 0; i<5 ; i++){
                LED_ADC_Write(1); 
                LED_DAC_Write(1); 
                CyDelay(500);
                LED_ADC_Write(0); 
                LED_DAC_Write(0); 
                CyDelay(500); 
            }  
            finished_procedure_flag=0; 
        }
    }
}

/* [] END OF FILE */
