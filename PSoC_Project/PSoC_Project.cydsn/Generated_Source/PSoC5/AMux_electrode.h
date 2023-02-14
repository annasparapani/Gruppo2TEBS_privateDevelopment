/*******************************************************************************
* File Name: AMux_electrode.h
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

#if !defined(CY_AMUX_AMux_electrode_H)
#define CY_AMUX_AMux_electrode_H

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

void AMux_electrode_Start(void) ;
#define AMux_electrode_Init() AMux_electrode_Start()
void AMux_electrode_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void AMux_electrode_Stop(void); */
/* void AMux_electrode_Select(uint8 channel); */
/* void AMux_electrode_Connect(uint8 channel); */
/* void AMux_electrode_Disconnect(uint8 channel); */
/* void AMux_electrode_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define AMux_electrode_CHANNELS  2u
#define AMux_electrode_MUXTYPE   1
#define AMux_electrode_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define AMux_electrode_NULL_CHANNEL 0xFFu
#define AMux_electrode_MUX_SINGLE   1
#define AMux_electrode_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if AMux_electrode_MUXTYPE == AMux_electrode_MUX_SINGLE
# if !AMux_electrode_ATMOSTONE
#  define AMux_electrode_Connect(channel) AMux_electrode_Set(channel)
# endif
# define AMux_electrode_Disconnect(channel) AMux_electrode_Unset(channel)
#else
# if !AMux_electrode_ATMOSTONE
void AMux_electrode_Connect(uint8 channel) ;
# endif
void AMux_electrode_Disconnect(uint8 channel) ;
#endif

#if AMux_electrode_ATMOSTONE
# define AMux_electrode_Stop() AMux_electrode_DisconnectAll()
# define AMux_electrode_Select(channel) AMux_electrode_FastSelect(channel)
void AMux_electrode_DisconnectAll(void) ;
#else
# define AMux_electrode_Stop() AMux_electrode_Start()
void AMux_electrode_Select(uint8 channel) ;
# define AMux_electrode_DisconnectAll() AMux_electrode_Start()
#endif

#endif /* CY_AMUX_AMux_electrode_H */


/* [] END OF FILE */
