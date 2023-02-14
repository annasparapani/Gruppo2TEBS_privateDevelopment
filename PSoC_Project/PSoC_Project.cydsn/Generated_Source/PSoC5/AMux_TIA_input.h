/*******************************************************************************
* File Name: AMux_TIA_input.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUX_AMux_TIA_input_H)
#define CY_AMUX_AMux_TIA_input_H

#include "cyfitter.h"
#include "cyfitter_cfg.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))    
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif /* ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) */


/***************************************
*        Function Prototypes
***************************************/

void AMux_TIA_input_Start(void) ;
#define AMux_TIA_input_Init() AMux_TIA_input_Start()
void AMux_TIA_input_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void AMux_TIA_input_Stop(void); */
/* void AMux_TIA_input_Select(uint8 channel); */
/* void AMux_TIA_input_Connect(uint8 channel); */
/* void AMux_TIA_input_Disconnect(uint8 channel); */
/* void AMux_TIA_input_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define AMux_TIA_input_CHANNELS  3u
#define AMux_TIA_input_MUXTYPE   1
#define AMux_TIA_input_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define AMux_TIA_input_NULL_CHANNEL 0xFFu
#define AMux_TIA_input_MUX_SINGLE   1
#define AMux_TIA_input_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if AMux_TIA_input_MUXTYPE == AMux_TIA_input_MUX_SINGLE
# if !AMux_TIA_input_ATMOSTONE
#  define AMux_TIA_input_Connect(channel) AMux_TIA_input_Set(channel)
# endif
# define AMux_TIA_input_Disconnect(channel) AMux_TIA_input_Unset(channel)
#else
# if !AMux_TIA_input_ATMOSTONE
void AMux_TIA_input_Connect(uint8 channel) ;
# endif
void AMux_TIA_input_Disconnect(uint8 channel) ;
#endif

#if AMux_TIA_input_ATMOSTONE
# define AMux_TIA_input_Stop() AMux_TIA_input_DisconnectAll()
# define AMux_TIA_input_Select(channel) AMux_TIA_input_FastSelect(channel)
void AMux_TIA_input_DisconnectAll(void) ;
#else
# define AMux_TIA_input_Stop() AMux_TIA_input_Start()
void AMux_TIA_input_Select(uint8 channel) ;
# define AMux_TIA_input_DisconnectAll() AMux_TIA_input_Start()
#endif

#endif /* CY_AMUX_AMux_TIA_input_H */


/* [] END OF FILE */
