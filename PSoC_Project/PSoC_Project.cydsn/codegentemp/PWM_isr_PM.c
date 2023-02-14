/*******************************************************************************
* File Name: PWM_isr_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PWM_isr.h"

static PWM_isr_backupStruct PWM_isr_backup;


/*******************************************************************************
* Function Name: PWM_isr_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_isr_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_isr_SaveConfig(void) 
{

    #if(!PWM_isr_UsingFixedFunction)
        #if(!PWM_isr_PWMModeIsCenterAligned)
            PWM_isr_backup.PWMPeriod = PWM_isr_ReadPeriod();
        #endif /* (!PWM_isr_PWMModeIsCenterAligned) */
        PWM_isr_backup.PWMUdb = PWM_isr_ReadCounter();
        #if (PWM_isr_UseStatus)
            PWM_isr_backup.InterruptMaskValue = PWM_isr_STATUS_MASK;
        #endif /* (PWM_isr_UseStatus) */

        #if(PWM_isr_DeadBandMode == PWM_isr__B_PWM__DBM_256_CLOCKS || \
            PWM_isr_DeadBandMode == PWM_isr__B_PWM__DBM_2_4_CLOCKS)
            PWM_isr_backup.PWMdeadBandValue = PWM_isr_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PWM_isr_KillModeMinTime)
             PWM_isr_backup.PWMKillCounterPeriod = PWM_isr_ReadKillTime();
        #endif /* (PWM_isr_KillModeMinTime) */

        #if(PWM_isr_UseControl)
            PWM_isr_backup.PWMControlRegister = PWM_isr_ReadControlRegister();
        #endif /* (PWM_isr_UseControl) */
    #endif  /* (!PWM_isr_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PWM_isr_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_isr_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_isr_RestoreConfig(void) 
{
        #if(!PWM_isr_UsingFixedFunction)
            #if(!PWM_isr_PWMModeIsCenterAligned)
                PWM_isr_WritePeriod(PWM_isr_backup.PWMPeriod);
            #endif /* (!PWM_isr_PWMModeIsCenterAligned) */

            PWM_isr_WriteCounter(PWM_isr_backup.PWMUdb);

            #if (PWM_isr_UseStatus)
                PWM_isr_STATUS_MASK = PWM_isr_backup.InterruptMaskValue;
            #endif /* (PWM_isr_UseStatus) */

            #if(PWM_isr_DeadBandMode == PWM_isr__B_PWM__DBM_256_CLOCKS || \
                PWM_isr_DeadBandMode == PWM_isr__B_PWM__DBM_2_4_CLOCKS)
                PWM_isr_WriteDeadTime(PWM_isr_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PWM_isr_KillModeMinTime)
                PWM_isr_WriteKillTime(PWM_isr_backup.PWMKillCounterPeriod);
            #endif /* (PWM_isr_KillModeMinTime) */

            #if(PWM_isr_UseControl)
                PWM_isr_WriteControlRegister(PWM_isr_backup.PWMControlRegister);
            #endif /* (PWM_isr_UseControl) */
        #endif  /* (!PWM_isr_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PWM_isr_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_isr_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_isr_Sleep(void) 
{
    #if(PWM_isr_UseControl)
        if(PWM_isr_CTRL_ENABLE == (PWM_isr_CONTROL & PWM_isr_CTRL_ENABLE))
        {
            /*Component is enabled */
            PWM_isr_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PWM_isr_backup.PWMEnableState = 0u;
        }
    #endif /* (PWM_isr_UseControl) */

    /* Stop component */
    PWM_isr_Stop();

    /* Save registers configuration */
    PWM_isr_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_isr_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_isr_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_isr_Wakeup(void) 
{
     /* Restore registers values */
    PWM_isr_RestoreConfig();

    if(PWM_isr_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PWM_isr_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
