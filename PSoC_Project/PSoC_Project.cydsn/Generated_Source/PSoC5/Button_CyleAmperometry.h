/*******************************************************************************
* File Name: Button_CyleAmperometry.h  
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

#if !defined(CY_PINS_Button_CyleAmperometry_H) /* Pins Button_CyleAmperometry_H */
#define CY_PINS_Button_CyleAmperometry_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Button_CyleAmperometry_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Button_CyleAmperometry__PORT == 15 && ((Button_CyleAmperometry__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Button_CyleAmperometry_Write(uint8 value);
void    Button_CyleAmperometry_SetDriveMode(uint8 mode);
uint8   Button_CyleAmperometry_ReadDataReg(void);
uint8   Button_CyleAmperometry_Read(void);
void    Button_CyleAmperometry_SetInterruptMode(uint16 position, uint16 mode);
uint8   Button_CyleAmperometry_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Button_CyleAmperometry_SetDriveMode() function.
     *  @{
     */
        #define Button_CyleAmperometry_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Button_CyleAmperometry_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Button_CyleAmperometry_DM_RES_UP          PIN_DM_RES_UP
        #define Button_CyleAmperometry_DM_RES_DWN         PIN_DM_RES_DWN
        #define Button_CyleAmperometry_DM_OD_LO           PIN_DM_OD_LO
        #define Button_CyleAmperometry_DM_OD_HI           PIN_DM_OD_HI
        #define Button_CyleAmperometry_DM_STRONG          PIN_DM_STRONG
        #define Button_CyleAmperometry_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Button_CyleAmperometry_MASK               Button_CyleAmperometry__MASK
#define Button_CyleAmperometry_SHIFT              Button_CyleAmperometry__SHIFT
#define Button_CyleAmperometry_WIDTH              1u

/* Interrupt constants */
#if defined(Button_CyleAmperometry__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Button_CyleAmperometry_SetInterruptMode() function.
     *  @{
     */
        #define Button_CyleAmperometry_INTR_NONE      (uint16)(0x0000u)
        #define Button_CyleAmperometry_INTR_RISING    (uint16)(0x0001u)
        #define Button_CyleAmperometry_INTR_FALLING   (uint16)(0x0002u)
        #define Button_CyleAmperometry_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Button_CyleAmperometry_INTR_MASK      (0x01u) 
#endif /* (Button_CyleAmperometry__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Button_CyleAmperometry_PS                     (* (reg8 *) Button_CyleAmperometry__PS)
/* Data Register */
#define Button_CyleAmperometry_DR                     (* (reg8 *) Button_CyleAmperometry__DR)
/* Port Number */
#define Button_CyleAmperometry_PRT_NUM                (* (reg8 *) Button_CyleAmperometry__PRT) 
/* Connect to Analog Globals */                                                  
#define Button_CyleAmperometry_AG                     (* (reg8 *) Button_CyleAmperometry__AG)                       
/* Analog MUX bux enable */
#define Button_CyleAmperometry_AMUX                   (* (reg8 *) Button_CyleAmperometry__AMUX) 
/* Bidirectional Enable */                                                        
#define Button_CyleAmperometry_BIE                    (* (reg8 *) Button_CyleAmperometry__BIE)
/* Bit-mask for Aliased Register Access */
#define Button_CyleAmperometry_BIT_MASK               (* (reg8 *) Button_CyleAmperometry__BIT_MASK)
/* Bypass Enable */
#define Button_CyleAmperometry_BYP                    (* (reg8 *) Button_CyleAmperometry__BYP)
/* Port wide control signals */                                                   
#define Button_CyleAmperometry_CTL                    (* (reg8 *) Button_CyleAmperometry__CTL)
/* Drive Modes */
#define Button_CyleAmperometry_DM0                    (* (reg8 *) Button_CyleAmperometry__DM0) 
#define Button_CyleAmperometry_DM1                    (* (reg8 *) Button_CyleAmperometry__DM1)
#define Button_CyleAmperometry_DM2                    (* (reg8 *) Button_CyleAmperometry__DM2) 
/* Input Buffer Disable Override */
#define Button_CyleAmperometry_INP_DIS                (* (reg8 *) Button_CyleAmperometry__INP_DIS)
/* LCD Common or Segment Drive */
#define Button_CyleAmperometry_LCD_COM_SEG            (* (reg8 *) Button_CyleAmperometry__LCD_COM_SEG)
/* Enable Segment LCD */
#define Button_CyleAmperometry_LCD_EN                 (* (reg8 *) Button_CyleAmperometry__LCD_EN)
/* Slew Rate Control */
#define Button_CyleAmperometry_SLW                    (* (reg8 *) Button_CyleAmperometry__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Button_CyleAmperometry_PRTDSI__CAPS_SEL       (* (reg8 *) Button_CyleAmperometry__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Button_CyleAmperometry_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Button_CyleAmperometry__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Button_CyleAmperometry_PRTDSI__OE_SEL0        (* (reg8 *) Button_CyleAmperometry__PRTDSI__OE_SEL0) 
#define Button_CyleAmperometry_PRTDSI__OE_SEL1        (* (reg8 *) Button_CyleAmperometry__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Button_CyleAmperometry_PRTDSI__OUT_SEL0       (* (reg8 *) Button_CyleAmperometry__PRTDSI__OUT_SEL0) 
#define Button_CyleAmperometry_PRTDSI__OUT_SEL1       (* (reg8 *) Button_CyleAmperometry__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Button_CyleAmperometry_PRTDSI__SYNC_OUT       (* (reg8 *) Button_CyleAmperometry__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Button_CyleAmperometry__SIO_CFG)
    #define Button_CyleAmperometry_SIO_HYST_EN        (* (reg8 *) Button_CyleAmperometry__SIO_HYST_EN)
    #define Button_CyleAmperometry_SIO_REG_HIFREQ     (* (reg8 *) Button_CyleAmperometry__SIO_REG_HIFREQ)
    #define Button_CyleAmperometry_SIO_CFG            (* (reg8 *) Button_CyleAmperometry__SIO_CFG)
    #define Button_CyleAmperometry_SIO_DIFF           (* (reg8 *) Button_CyleAmperometry__SIO_DIFF)
#endif /* (Button_CyleAmperometry__SIO_CFG) */

/* Interrupt Registers */
#if defined(Button_CyleAmperometry__INTSTAT)
    #define Button_CyleAmperometry_INTSTAT            (* (reg8 *) Button_CyleAmperometry__INTSTAT)
    #define Button_CyleAmperometry_SNAP               (* (reg8 *) Button_CyleAmperometry__SNAP)
    
	#define Button_CyleAmperometry_0_INTTYPE_REG 		(* (reg8 *) Button_CyleAmperometry__0__INTTYPE)
#endif /* (Button_CyleAmperometry__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Button_CyleAmperometry_H */


/* [] END OF FILE */
