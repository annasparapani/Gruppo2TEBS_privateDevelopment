/*******************************************************************************
* File Name: Reference_Electrode.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Reference_Electrode_H) /* Pins Reference_Electrode_H */
#define CY_PINS_Reference_Electrode_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Reference_Electrode_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Reference_Electrode__PORT == 15 && ((Reference_Electrode__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Reference_Electrode_Write(uint8 value);
void    Reference_Electrode_SetDriveMode(uint8 mode);
uint8   Reference_Electrode_ReadDataReg(void);
uint8   Reference_Electrode_Read(void);
void    Reference_Electrode_SetInterruptMode(uint16 position, uint16 mode);
uint8   Reference_Electrode_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Reference_Electrode_SetDriveMode() function.
     *  @{
     */
        #define Reference_Electrode_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Reference_Electrode_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Reference_Electrode_DM_RES_UP          PIN_DM_RES_UP
        #define Reference_Electrode_DM_RES_DWN         PIN_DM_RES_DWN
        #define Reference_Electrode_DM_OD_LO           PIN_DM_OD_LO
        #define Reference_Electrode_DM_OD_HI           PIN_DM_OD_HI
        #define Reference_Electrode_DM_STRONG          PIN_DM_STRONG
        #define Reference_Electrode_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Reference_Electrode_MASK               Reference_Electrode__MASK
#define Reference_Electrode_SHIFT              Reference_Electrode__SHIFT
#define Reference_Electrode_WIDTH              1u

/* Interrupt constants */
#if defined(Reference_Electrode__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Reference_Electrode_SetInterruptMode() function.
     *  @{
     */
        #define Reference_Electrode_INTR_NONE      (uint16)(0x0000u)
        #define Reference_Electrode_INTR_RISING    (uint16)(0x0001u)
        #define Reference_Electrode_INTR_FALLING   (uint16)(0x0002u)
        #define Reference_Electrode_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Reference_Electrode_INTR_MASK      (0x01u) 
#endif /* (Reference_Electrode__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Reference_Electrode_PS                     (* (reg8 *) Reference_Electrode__PS)
/* Data Register */
#define Reference_Electrode_DR                     (* (reg8 *) Reference_Electrode__DR)
/* Port Number */
#define Reference_Electrode_PRT_NUM                (* (reg8 *) Reference_Electrode__PRT) 
/* Connect to Analog Globals */                                                  
#define Reference_Electrode_AG                     (* (reg8 *) Reference_Electrode__AG)                       
/* Analog MUX bux enable */
#define Reference_Electrode_AMUX                   (* (reg8 *) Reference_Electrode__AMUX) 
/* Bidirectional Enable */                                                        
#define Reference_Electrode_BIE                    (* (reg8 *) Reference_Electrode__BIE)
/* Bit-mask for Aliased Register Access */
#define Reference_Electrode_BIT_MASK               (* (reg8 *) Reference_Electrode__BIT_MASK)
/* Bypass Enable */
#define Reference_Electrode_BYP                    (* (reg8 *) Reference_Electrode__BYP)
/* Port wide control signals */                                                   
#define Reference_Electrode_CTL                    (* (reg8 *) Reference_Electrode__CTL)
/* Drive Modes */
#define Reference_Electrode_DM0                    (* (reg8 *) Reference_Electrode__DM0) 
#define Reference_Electrode_DM1                    (* (reg8 *) Reference_Electrode__DM1)
#define Reference_Electrode_DM2                    (* (reg8 *) Reference_Electrode__DM2) 
/* Input Buffer Disable Override */
#define Reference_Electrode_INP_DIS                (* (reg8 *) Reference_Electrode__INP_DIS)
/* LCD Common or Segment Drive */
#define Reference_Electrode_LCD_COM_SEG            (* (reg8 *) Reference_Electrode__LCD_COM_SEG)
/* Enable Segment LCD */
#define Reference_Electrode_LCD_EN                 (* (reg8 *) Reference_Electrode__LCD_EN)
/* Slew Rate Control */
#define Reference_Electrode_SLW                    (* (reg8 *) Reference_Electrode__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Reference_Electrode_PRTDSI__CAPS_SEL       (* (reg8 *) Reference_Electrode__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Reference_Electrode_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Reference_Electrode__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Reference_Electrode_PRTDSI__OE_SEL0        (* (reg8 *) Reference_Electrode__PRTDSI__OE_SEL0) 
#define Reference_Electrode_PRTDSI__OE_SEL1        (* (reg8 *) Reference_Electrode__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Reference_Electrode_PRTDSI__OUT_SEL0       (* (reg8 *) Reference_Electrode__PRTDSI__OUT_SEL0) 
#define Reference_Electrode_PRTDSI__OUT_SEL1       (* (reg8 *) Reference_Electrode__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Reference_Electrode_PRTDSI__SYNC_OUT       (* (reg8 *) Reference_Electrode__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Reference_Electrode__SIO_CFG)
    #define Reference_Electrode_SIO_HYST_EN        (* (reg8 *) Reference_Electrode__SIO_HYST_EN)
    #define Reference_Electrode_SIO_REG_HIFREQ     (* (reg8 *) Reference_Electrode__SIO_REG_HIFREQ)
    #define Reference_Electrode_SIO_CFG            (* (reg8 *) Reference_Electrode__SIO_CFG)
    #define Reference_Electrode_SIO_DIFF           (* (reg8 *) Reference_Electrode__SIO_DIFF)
#endif /* (Reference_Electrode__SIO_CFG) */

/* Interrupt Registers */
#if defined(Reference_Electrode__INTSTAT)
    #define Reference_Electrode_INTSTAT            (* (reg8 *) Reference_Electrode__INTSTAT)
    #define Reference_Electrode_SNAP               (* (reg8 *) Reference_Electrode__SNAP)
    
	#define Reference_Electrode_0_INTTYPE_REG 		(* (reg8 *) Reference_Electrode__0__INTTYPE)
#endif /* (Reference_Electrode__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Reference_Electrode_H */


/* [] END OF FILE */
