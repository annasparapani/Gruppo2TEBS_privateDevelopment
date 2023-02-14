/*******************************************************************************
* File Name: TIA_resistor1.h  
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

#if !defined(CY_PINS_TIA_resistor1_H) /* Pins TIA_resistor1_H */
#define CY_PINS_TIA_resistor1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "TIA_resistor1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 TIA_resistor1__PORT == 15 && ((TIA_resistor1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    TIA_resistor1_Write(uint8 value);
void    TIA_resistor1_SetDriveMode(uint8 mode);
uint8   TIA_resistor1_ReadDataReg(void);
uint8   TIA_resistor1_Read(void);
void    TIA_resistor1_SetInterruptMode(uint16 position, uint16 mode);
uint8   TIA_resistor1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the TIA_resistor1_SetDriveMode() function.
     *  @{
     */
        #define TIA_resistor1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define TIA_resistor1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define TIA_resistor1_DM_RES_UP          PIN_DM_RES_UP
        #define TIA_resistor1_DM_RES_DWN         PIN_DM_RES_DWN
        #define TIA_resistor1_DM_OD_LO           PIN_DM_OD_LO
        #define TIA_resistor1_DM_OD_HI           PIN_DM_OD_HI
        #define TIA_resistor1_DM_STRONG          PIN_DM_STRONG
        #define TIA_resistor1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define TIA_resistor1_MASK               TIA_resistor1__MASK
#define TIA_resistor1_SHIFT              TIA_resistor1__SHIFT
#define TIA_resistor1_WIDTH              1u

/* Interrupt constants */
#if defined(TIA_resistor1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in TIA_resistor1_SetInterruptMode() function.
     *  @{
     */
        #define TIA_resistor1_INTR_NONE      (uint16)(0x0000u)
        #define TIA_resistor1_INTR_RISING    (uint16)(0x0001u)
        #define TIA_resistor1_INTR_FALLING   (uint16)(0x0002u)
        #define TIA_resistor1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define TIA_resistor1_INTR_MASK      (0x01u) 
#endif /* (TIA_resistor1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define TIA_resistor1_PS                     (* (reg8 *) TIA_resistor1__PS)
/* Data Register */
#define TIA_resistor1_DR                     (* (reg8 *) TIA_resistor1__DR)
/* Port Number */
#define TIA_resistor1_PRT_NUM                (* (reg8 *) TIA_resistor1__PRT) 
/* Connect to Analog Globals */                                                  
#define TIA_resistor1_AG                     (* (reg8 *) TIA_resistor1__AG)                       
/* Analog MUX bux enable */
#define TIA_resistor1_AMUX                   (* (reg8 *) TIA_resistor1__AMUX) 
/* Bidirectional Enable */                                                        
#define TIA_resistor1_BIE                    (* (reg8 *) TIA_resistor1__BIE)
/* Bit-mask for Aliased Register Access */
#define TIA_resistor1_BIT_MASK               (* (reg8 *) TIA_resistor1__BIT_MASK)
/* Bypass Enable */
#define TIA_resistor1_BYP                    (* (reg8 *) TIA_resistor1__BYP)
/* Port wide control signals */                                                   
#define TIA_resistor1_CTL                    (* (reg8 *) TIA_resistor1__CTL)
/* Drive Modes */
#define TIA_resistor1_DM0                    (* (reg8 *) TIA_resistor1__DM0) 
#define TIA_resistor1_DM1                    (* (reg8 *) TIA_resistor1__DM1)
#define TIA_resistor1_DM2                    (* (reg8 *) TIA_resistor1__DM2) 
/* Input Buffer Disable Override */
#define TIA_resistor1_INP_DIS                (* (reg8 *) TIA_resistor1__INP_DIS)
/* LCD Common or Segment Drive */
#define TIA_resistor1_LCD_COM_SEG            (* (reg8 *) TIA_resistor1__LCD_COM_SEG)
/* Enable Segment LCD */
#define TIA_resistor1_LCD_EN                 (* (reg8 *) TIA_resistor1__LCD_EN)
/* Slew Rate Control */
#define TIA_resistor1_SLW                    (* (reg8 *) TIA_resistor1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define TIA_resistor1_PRTDSI__CAPS_SEL       (* (reg8 *) TIA_resistor1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define TIA_resistor1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) TIA_resistor1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define TIA_resistor1_PRTDSI__OE_SEL0        (* (reg8 *) TIA_resistor1__PRTDSI__OE_SEL0) 
#define TIA_resistor1_PRTDSI__OE_SEL1        (* (reg8 *) TIA_resistor1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define TIA_resistor1_PRTDSI__OUT_SEL0       (* (reg8 *) TIA_resistor1__PRTDSI__OUT_SEL0) 
#define TIA_resistor1_PRTDSI__OUT_SEL1       (* (reg8 *) TIA_resistor1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define TIA_resistor1_PRTDSI__SYNC_OUT       (* (reg8 *) TIA_resistor1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(TIA_resistor1__SIO_CFG)
    #define TIA_resistor1_SIO_HYST_EN        (* (reg8 *) TIA_resistor1__SIO_HYST_EN)
    #define TIA_resistor1_SIO_REG_HIFREQ     (* (reg8 *) TIA_resistor1__SIO_REG_HIFREQ)
    #define TIA_resistor1_SIO_CFG            (* (reg8 *) TIA_resistor1__SIO_CFG)
    #define TIA_resistor1_SIO_DIFF           (* (reg8 *) TIA_resistor1__SIO_DIFF)
#endif /* (TIA_resistor1__SIO_CFG) */

/* Interrupt Registers */
#if defined(TIA_resistor1__INTSTAT)
    #define TIA_resistor1_INTSTAT            (* (reg8 *) TIA_resistor1__INTSTAT)
    #define TIA_resistor1_SNAP               (* (reg8 *) TIA_resistor1__SNAP)
    
	#define TIA_resistor1_0_INTTYPE_REG 		(* (reg8 *) TIA_resistor1__0__INTTYPE)
#endif /* (TIA_resistor1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_TIA_resistor1_H */


/* [] END OF FILE */
