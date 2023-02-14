/*******************************************************************************
* File Name: IDAC_calibrate.c
* Version 2.0
*
* Description:
*  This file provides the power management source code to API for the
*  IDAC8.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include "IDAC_calibrate.h"

static IDAC_calibrate_backupStruct IDAC_calibrate_backup;


/*******************************************************************************
* Function Name: IDAC_calibrate_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void IDAC_calibrate_SaveConfig(void) 
{
    if (!((IDAC_calibrate_CR1 & IDAC_calibrate_SRC_MASK) == IDAC_calibrate_SRC_UDB))
    {
        IDAC_calibrate_backup.data_value = IDAC_calibrate_Data;
    }
}


/*******************************************************************************
* Function Name: IDAC_calibrate_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void IDAC_calibrate_RestoreConfig(void) 
{
    if (!((IDAC_calibrate_CR1 & IDAC_calibrate_SRC_MASK) == IDAC_calibrate_SRC_UDB))
    {
        if((IDAC_calibrate_Strobe & IDAC_calibrate_STRB_MASK) == IDAC_calibrate_STRB_EN)
        {
            IDAC_calibrate_Strobe &= (uint8)(~IDAC_calibrate_STRB_MASK);
            IDAC_calibrate_Data = IDAC_calibrate_backup.data_value;
            IDAC_calibrate_Strobe |= IDAC_calibrate_STRB_EN;
        }
        else
        {
            IDAC_calibrate_Data = IDAC_calibrate_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: IDAC_calibrate_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
* Global variables:
*  IDAC_calibrate_backup.enableState: Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void IDAC_calibrate_Sleep(void) 
{
    if(IDAC_calibrate_ACT_PWR_EN == (IDAC_calibrate_PWRMGR & IDAC_calibrate_ACT_PWR_EN))
    {
        /* IDAC8 is enabled */
        IDAC_calibrate_backup.enableState = 1u;
    }
    else
    {
        /* IDAC8 is disabled */
        IDAC_calibrate_backup.enableState = 0u;
    }

    IDAC_calibrate_Stop();
    IDAC_calibrate_SaveConfig();
}


/*******************************************************************************
* Function Name: IDAC_calibrate_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  IDAC_calibrate_backup.enableState: Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void IDAC_calibrate_Wakeup(void) 
{
    IDAC_calibrate_RestoreConfig();
    
    if(IDAC_calibrate_backup.enableState == 1u)
    {
        /* Enable IDAC8's operation */
        IDAC_calibrate_Enable();
        
        /* Set the data register */
        IDAC_calibrate_SetValue(IDAC_calibrate_Data);
    } /* Do nothing if IDAC8 was disabled before */    
}


/* [] END OF FILE */
