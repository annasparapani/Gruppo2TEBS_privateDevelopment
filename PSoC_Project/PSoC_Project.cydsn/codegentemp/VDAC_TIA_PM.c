/*******************************************************************************
* File Name: VDAC_TIA_PM.c  
* Version 1.90
*
* Description:
*  This file provides the power management source code to API for the
*  VDAC8.  
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

#include "VDAC_TIA.h"

static VDAC_TIA_backupStruct VDAC_TIA_backup;


/*******************************************************************************
* Function Name: VDAC_TIA_SaveConfig
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
void VDAC_TIA_SaveConfig(void) 
{
    if (!((VDAC_TIA_CR1 & VDAC_TIA_SRC_MASK) == VDAC_TIA_SRC_UDB))
    {
        VDAC_TIA_backup.data_value = VDAC_TIA_Data;
    }
}


/*******************************************************************************
* Function Name: VDAC_TIA_RestoreConfig
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
void VDAC_TIA_RestoreConfig(void) 
{
    if (!((VDAC_TIA_CR1 & VDAC_TIA_SRC_MASK) == VDAC_TIA_SRC_UDB))
    {
        if((VDAC_TIA_Strobe & VDAC_TIA_STRB_MASK) == VDAC_TIA_STRB_EN)
        {
            VDAC_TIA_Strobe &= (uint8)(~VDAC_TIA_STRB_MASK);
            VDAC_TIA_Data = VDAC_TIA_backup.data_value;
            VDAC_TIA_Strobe |= VDAC_TIA_STRB_EN;
        }
        else
        {
            VDAC_TIA_Data = VDAC_TIA_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: VDAC_TIA_Sleep
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
*  VDAC_TIA_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void VDAC_TIA_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(VDAC_TIA_ACT_PWR_EN == (VDAC_TIA_PWRMGR & VDAC_TIA_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        VDAC_TIA_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        VDAC_TIA_backup.enableState = 0u;
    }
    
    VDAC_TIA_Stop();
    VDAC_TIA_SaveConfig();
}


/*******************************************************************************
* Function Name: VDAC_TIA_Wakeup
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
*  VDAC_TIA_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void VDAC_TIA_Wakeup(void) 
{
    VDAC_TIA_RestoreConfig();
    
    if(VDAC_TIA_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        VDAC_TIA_Enable();

        /* Restore the data register */
        VDAC_TIA_SetValue(VDAC_TIA_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
