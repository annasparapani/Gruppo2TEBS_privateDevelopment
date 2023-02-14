/*******************************************************************************
* File Name: Working_Electrode.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Working_Electrode_ALIASES_H) /* Pins Working_Electrode_ALIASES_H */
#define CY_PINS_Working_Electrode_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Working_Electrode_0			(Working_Electrode__0__PC)
#define Working_Electrode_0_INTR	((uint16)((uint16)0x0001u << Working_Electrode__0__SHIFT))

#define Working_Electrode_INTR_ALL	 ((uint16)(Working_Electrode_0_INTR))

#endif /* End Pins Working_Electrode_ALIASES_H */


/* [] END OF FILE */
