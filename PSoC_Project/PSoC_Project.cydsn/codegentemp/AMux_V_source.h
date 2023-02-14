/*******************************************************************************
* File Name: AMux_V_source.h
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

#if !defined(CY_AMUX_AMux_V_source_H)
#define CY_AMUX_AMux_V_source_H

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

void AMux_V_source_Start(void) ;
#define AMux_V_source_Init() AMux_V_source_Start()
void AMux_V_source_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void AMux_V_source_Stop(void); */
/* void AMux_V_source_Select(uint8 channel); */
/* void AMux_V_source_Connect(uint8 channel); */
/* void AMux_V_source_Disconnect(uint8 channel); */
/* void AMux_V_source_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define AMux_V_source_CHANNELS  2u
#define AMux_V_source_MUXTYPE   1
#define AMux_V_source_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define AMux_V_source_NULL_CHANNEL 0xFFu
#define AMux_V_source_MUX_SINGLE   1
#define AMux_V_source_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if AMux_V_source_MUXTYPE == AMux_V_source_MUX_SINGLE
# if !AMux_V_source_ATMOSTONE
#  define AMux_V_source_Connect(channel) AMux_V_source_Set(channel)
# endif
# define AMux_V_source_Disconnect(channel) AMux_V_source_Unset(channel)
#else
# if !AMux_V_source_ATMOSTONE
void AMux_V_source_Connect(uint8 channel) ;
# endif
void AMux_V_source_Disconnect(uint8 channel) ;
#endif

#if AMux_V_source_ATMOSTONE
# define AMux_V_source_Stop() AMux_V_source_DisconnectAll()
# define AMux_V_source_Select(channel) AMux_V_source_FastSelect(channel)
void AMux_V_source_DisconnectAll(void) ;
#else
# define AMux_V_source_Stop() AMux_V_source_Start()
void AMux_V_source_Select(uint8 channel) ;
# define AMux_V_source_DisconnectAll() AMux_V_source_Start()
#endif

#endif /* CY_AMUX_AMux_V_source_H */


/* [] END OF FILE */
