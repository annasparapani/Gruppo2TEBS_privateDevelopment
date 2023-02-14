/*******************************************************************************
* File Name: TIA_calibrate.c
*
* Description:
*  Protocols to calibrate the current measuring circuitry i.e. TIA / delta sigma ADC with an IDAC
*
*********************************************************************************/
#include <project.h>
#include "math.h"
#include <stdio.h>
#include "stdlib.h"

#include "TIA_calibrate.h"
#include "BT_protocols.h"

//extern char LCD_str[];  // for debug
const uint16_t calibrate_TIA_resistor_list[]= {20, 30, 40, 80, 120, 250, 500, 1000};
uint16_t static ADC_value;

int16_t valore;
int16_t valore_s;

/***************************************
* Forward function references
***************************************/
static void Calibrate_Hardware_Wakeup(void);
static void calibrate_step(uint16_t IDAC_value, uint8 IDAC_index);
static void Calibrate_Hardware_Sleep(void);

/******************************************************************************
* Function Name: calibrate_TIA
*******************************************************************************
*
* Summary:
*  Calibrate the TIA circuit each time the current gain settings are changed
*
* Global variables:
*  uint8 TIA_resistor_value_index: index of which TIA resistor to use, Supplied by UART input
*  uint8 ADC_buffer_index: which ADC buffer is used, gain = 2**ADC_buffer_index
*
* Return:
*  array of 20 bytes is loaded into the USB IN ENDPOINT (into the computer)
*
*******************************************************************************/

void calibrate_TIA(uint8 TIA_resistor_value_index /*uint8 ADC_buffer_index -> this maybe remove*/) {
    // The IDAC only 
    IDAC_calibrate_Start();
    IDAC_calibrate_SetValue(0);
    // start the hardware required
    Calibrate_Hardware_Wakeup();
    CyDelay(100);
    // decide what currents to use based on TIA resistor and ADC buffer settings
    uint16_t resistor_value = calibrate_TIA_resistor_list[TIA_resistor_value_index];
    //uint var = 2;
    uint16_t IDAC_setting = 0;
    
    /* ADC_buffer_index management */
//  uint8 ADC_buffer_value = pow(var, ADC_buffer_index); ** we are not giving the ADC_buffer_index to the function**
    uint8_t ADC_buffer_value =  2*1; // ADC_buffer_index = 1 
    
    // set input current to zero and read ADC
    ADC_SigDel_StartConvert();
    calibrate_step(IDAC_setting, 2);
    // calculate the IDAC value needed to get a 1 Volt in the ADC
    // the 8000 is because the IDAC has a 1/8 uA per bit and 8000=1000mV/(1/8 uA per bit)
    float32 transfer = 8000./(ADC_buffer_value*resistor_value);
    int transfer_int = (int) transfer;
    if (transfer_int > 250) {  // the TIA needs too much current, reduce needs by half.  Is needed for the 20k resistor setting
        transfer_int /= 2;
    }
    //    LCD_Position(0,0);
//    sprintf(LCD_str, "in:%d |%d| ", resistor_value, ADC_buffer_value);
//    LCD_PrintString(LCD_str);
    // is not DRY but not sure how to fix
    IDAC_calibrate_SetPolarity(IDAC_calibrate_SINK);
    calibrate_step(transfer_int, 0);
    calibrate_step(transfer_int/2, 1);
    IDAC_calibrate_SetPolarity(IDAC_calibrate_SOURCE);
    calibrate_step(transfer_int/2, 3);
    calibrate_step(transfer_int, 4);
    IDAC_calibrate_SetValue(0);
    Calibrate_Hardware_Sleep();
    
    
    /* regressione lineare per calcolare R (V = R*I + q)
    float sum_x = 0, sum_x2 = 0, sum_xy = 0, sum_y = 0, R, q;
    for(int i=0; i<Number_calibration_points; i++){
        sum_x = sum_x + calibrate_array[i];
        sum_x2 = sum_x2 + calibrate_array[i]*calibrate_array[i];
        sum_y = calibrate_array[i+Number_calibration_points];
        sum_xy = sum_xy + calibrate_array[i]*calibrate_array[i+Number_calibration_points]; 
    }
    
    q = (Number_calibration_points*sum_xy - sum_x*sum_y)/(Number_calibration_points*sum_x2 - (sum_x*sum_x));
    R = (sum_y - q*sum_x)/Number_calibration_points;
    */
    // dobbiamo mandarla alla GUI
    if(connection_state){ //TIA_calibrate -- BT already connected
        for(int i=0; i<Number_calibration_points*2; i++){
            uint8_t LSB_data = calibrate_array[i] & 0xFF;
            uint8_t MSB_data = calibrate_array[i] >> 8;
            data_to_send[2*i+1] = MSB_data;
            data_to_send[2*i+2] = LSB_data;
        }
        data_to_send[0] = TIA_SET; // header 
        writeBT((Number_calibration_points*4)+1); 
    } else { //TIA_initialize -- BT not connected
        for(int i=0; i<Number_calibration_points*2; i++){
            uint8_t LSB_data = calibrate_array[i] & 0xFF;
            uint8_t MSB_data = calibrate_array[i] >> 8;
            tia_calibration_values[2*i+1] = MSB_data;
            tia_calibration_values[2*i+2] = LSB_data;
        }
        tia_calibration_values[0] = TIA_SET;
    }
    
    // se necessario salvare localmente, salvare in EEPROM 
  
}

/******************************************************************************
* Function Name: calibrate_step
*******************************************************************************
*
* Summary:
*  Gets a single calibration data point by setting the calibration IDAC and reading
*  the ADC count and saving them in the calibration_array
*
* Parameters:
*  uint16_t IDAC_value: value to set the calibration  IDAC to before measuring with the ADC
*  uint8 IDAC_index: index of where in calibration_array to save the IDAC and ADC data
*
* Global variables:
*  calibration_array: array of saved IDAC and ADC values
*
*******************************************************************************/

static void calibrate_step(uint16_t IDAC_value, uint8 IDAC_index) {
    IDAC_calibrate_SetValue(IDAC_value);
    CyDelay(100);  // allow the ADC to settle
    ADC_value = ADC_SigDel_GetResult16(); //la funzione restituisce un int16, noi lo salviamo in un uint16, il uC come fa a capire?
    valore = ADC_SigDel_GetResult16();
    valore_s = valore & 0x0FFF;
   
    calibrate_array[IDAC_index] = IDAC_value;
    calibrate_array[IDAC_index + Number_calibration_points] = ADC_value;
    // visto che usiamo la UART salviamo i dati in un array, salviamo le correnti da posiz. 0 a 4 e le tensioni da posiz. 5 a 10
}

/******************************************************************************
* Function Name: Calibrate_Hardware_Wakeup
*******************************************************************************
*
* Summary:
*  Wakeup all the hardware needed for the calibration routine and set the AMux to 
*  the correct channel
*
*******************************************************************************/

static void Calibrate_Hardware_Wakeup(void) {
    AMux_TIA_input_Select(AMux_TIA_calibrat_ch);
    TIA_Wakeup();
    VDAC_TIA_Wakeup();
    ADC_SigDel_Wakeup();
}

/******************************************************************************
* Function Name: Calibrate_Hardware_Sleep
*******************************************************************************
*
* Summary:
*  Put to sleep all the hardware needed for the calibration routine, stop  
*  the IDAC, and set the AMux to the correct channel
*
*******************************************************************************/

static void Calibrate_Hardware_Sleep(void) {
    AMux_TIA_input_Select(AMux_TIA_measure_ch);
    TIA_Sleep();
    VDAC_TIA_Sleep();
    ADC_SigDel_Sleep();
    IDAC_calibrate_Stop();
}

/* [] END OF FILE */

