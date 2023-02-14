/*******************************************************************************
* File Name: VDAC_source_PM.c  
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

#include "VDAC_source.h"

static VDAC_source_backupStruct VDAC_source_backup;


/*******************************************************************************
* Function Name: VDAC_source_SaveConfig
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
void VDAC_source_SaveConfig(void) 
{
    if (!((VDAC_source_CR1 & VDAC_source_SRC_MASK) == VDAC_source_SRC_UDB))
    {
        VDAC_source_backup.data_value = VDAC_source_Data;
    }
}


/*******************************************************************************
* Function Name: VDAC_source_RestoreConfig
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
void VDAC_source_RestoreConfig(void) 
{
    if (!((VDAC_source_CR1 & VDAC_source_SRC_MASK) == VDAC_source_SRC_UDB))
    {
        if((VDAC_source_Strobe & VDAC_source_STRB_MASK) == VDAC_source_STRB_EN)
        {
            VDAC_source_Strobe &= (uint8)(~VDAC_source_STRB_MASK);
            VDAC_source_Data = VDAC_source_backup.data_value;
            VDAC_source_Strobe |= VDAC_source_STRB_EN;
        }
        else
        {
            VDAC_source_Data = VDAC_source_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: VDAC_source_Sleep
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
*  VDAC_source_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void VDAC_source_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(VDAC_source_ACT_PWR_EN == (VDAC_source_PWRMGR & VDAC_source_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        VDAC_source_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        VDAC_source_backup.enableState = 0u;
    }
    
    VDAC_source_Stop();
    VDAC_source_SaveConfig();
}


/*******************************************************************************
* Function Name: VDAC_source_Wakeup
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
*  VDAC_source_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void VDAC_source_Wakeup(void) 
{
    VDAC_source_RestoreConfig();
    
    if(VDAC_source_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        VDAC_source_Enable();

        /* Restore the data register */
        VDAC_source_SetValue(VDAC_source_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
