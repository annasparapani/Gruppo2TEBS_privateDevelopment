#ifndef __ INTERRUPT_ROUTINES_H //include prototipi una sola volta
    // if n def -> se non è definita la definisce, altrimenti no
    // stringa vuota on associata a nulla -> la definisco solo per dire "esiste o meno" 
    // -> se non esiste la creo e vado avanti a includere le librerie 
    
    #include "cytypes.h"
    #include "stdio.h"
    
    // call ISR function of the bluetooth reception (ADC and DAC are already defined in their .h files 
    CY_ISR_PROTO(Custom_UART_BT_RX_Interrupt);
    CY_ISR_PROTO(isr_dac_Interrupt);
    CY_ISR_PROTO(isr_adc_Interrupt);
    
#endif


