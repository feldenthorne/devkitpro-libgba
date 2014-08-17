//**********************************************************************************
/** \file gba_interrupt.h
 *  Header for GBA interrupt handling
 * 
 *  It declares the functions required to initialize, set, enable, and disable
 *  system interrupts. It also enumerates the IE/IF registers for
 *  easily-interpreted code.
 */ 
 /* Copyright 2003-2005 by Dave Murphy.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 * 
 * Please report all bugs and problems through the bug tracker at
 * "http://sourceforge.net/tracker/?group_id=114505&atid=668551". */
//*********************************************************************************

#ifndef _gba_interrupt_h_
#define _gba_interrupt_h_
//---------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------------------------------

#include "gba_base.h"

/** \typedef void ( * IntFn)(void)
 *  \brief Type definition for an interrupt function pointer
 */
typedef void ( * IntFn)(void);

/** \struct IntTable
 *  \brief Structure that stores interrupts and their masks
 *  @param handler The handle/name of a particular interrupt
 *  @param mask The mask used for the interrupt
 */
struct IntTable{IntFn handler; u32 mask;};

/** \brief Defines the max number of interrupts that can be declared.
 */
#define MAX_INTS	15

/** \brief Defines the BIOS interrupt vector.
 */
#define INT_VECTOR	*(IntFn *)(0x03007ffc)		// BIOS Interrupt vector

/** \def REG_IME
 *  \brief Interrupt Master Enable Register.
 *  \details When bit 0 is clear, all interrupts are masked.  When it is 1,
 *  interrupts will occur if not masked out in REG_IE.
 */
#define	REG_IME		*(vu16 *)(REG_BASE + 0x208)	// Interrupt Master Enable

/** \def REG_IE
 *  \brief Interrupt Enable Register.
 *  \details This is the activation mask for the internal interrupts.  Unless
 * the corresponding bit is set, the IRQ will be masked out.
 */
#define	REG_IE		*(vu16 *)(REG_BASE + 0x200)	// Interrupt Enable

/** \def REG_IF
 *  \brief Interrupt Flag Register.
 *  \details Since there is only one hardware interrupt vector, the IF register
 *  contains flags to indicate when a particular of interrupt has occured.
 *  To acknowledge processing interrupts, set IF to the value of the
 *  interrupt handled.
 */
#define	REG_IF		*(vu16 *)(REG_BASE + 0x202)	// Interrupt Request

/** \typedef enum irqMASKS
 *  \brief Interrupt masks
 *  \details These masks are used in conjuction with REG_IE to enable specific 
 *  interrupts and with REG_IF to acknowledge interrupts have been serviced.
 *  \note Bitwise OR (|) and AND (&) are convenient setters/maskers here.
 */
typedef enum irqMASKS {
	IRQ_VBLANK	=	(1<<0),		/*!< vertical blank interrupt mask */
	IRQ_HBLANK	=	(1<<1),		/*!< horizontal blank interrupt mask */
	IRQ_VCOUNT	=	(1<<2),		/*!< vcount match interrupt mask */
	IRQ_TIMER0	=	(1<<3),		/*!< timer 0 interrupt mask */
	IRQ_TIMER1	=	(1<<4),		/*!< timer 1 interrupt mask */
	IRQ_TIMER2	=	(1<<5),		/*!< timer 2 interrupt mask */
	IRQ_TIMER3	=	(1<<6),		/*!< timer 3 interrupt mask */
	IRQ_SERIAL	=	(1<<7),		/*!< serial interrupt mask */
	IRQ_DMA0	=	(1<<8),		/*!< DMA 0 interrupt mask */
	IRQ_DMA1	=	(1<<9),		/*!< DMA 1 interrupt mask */
	IRQ_DMA2	=	(1<<10),	/*!< DMA 2 interrupt mask */
	IRQ_DMA3	=	(1<<11),	/*!< DMA 3 interrupt mask */
	IRQ_KEYPAD	=	(1<<12),	/*!< Keypad interrupt mask */
	IRQ_GAMEPAK	=	(1<<13)		/*!< horizontal blank interrupt mask */
} irqMASK;

extern struct IntTable IntrTable[];

/** \brief Initializes the GBA interrupt code.
 *  \details This function simply calls \c irqInit().
 *  \deprecated The following function has been deprecated, use \c irqInit()
 *  instead.
 */
void InitInterrupt(void) __attribute__ ((deprecated));

/** \brief Initializes the GBA interrupt code.
 *  \details Essentially, this sets all the interrupt pointers to dummy
 *  functions and their masks to 0.
 */
void irqInit();

/** \brief Sets the interrupt handler for a particular interrupt.
 *  \details This function simply points to the \c irqSet() pointer.
 *  \deprecated The following function has been deprecated, use \c irqSet() 
 *  instead.
 * 
 *  @param mask Mask to initialize the IRQ with
 *  @param function Function to use for the interrupt
 *  @return The function used as the interrupt
 */
IntFn *SetInterrupt(irqMASK mask, IntFn function) __attribute__ ((deprecated));

/** \brief Sets the interrupt handler for a particular interrupt.
 *  \details This should be used to replace the dummy function pointers and
 *  masks set by \c irqInit().
 * 
 *  @param mask Mask to initialize the IRQ with
 *  @param function Function to use for the interrupt
 *  @return The function used as the interrupt
 */
IntFn *irqSet(irqMASK mask, IntFn function);

/** \brief Allows an interrupt to occur.
 *  \details This function simply calls \c irqEnable().
 *  \deprecated The following function has been deprecated, use \c irqEnable()
 *  instead.
 * 
 *  @param mask The mask for the interrupt enable (IE) register
 */
void EnableInterrupt(irqMASK mask) __attribute__ ((deprecated));

/** \brief Allows an interrupt to occur.
 *  \details This function applies the supplied mask to the interrupt enable
 *  (IE) register, then sets IME to 1.
 * 
 *  @param mask The mask for the interrupt enable (IE) register
 */
void irqEnable(int mask);

/** \brief Prevents an interrupt occurring.
 *  \details This function simply calls \c irqDisable().
 *  \deprecated The following function has been deprecated, use \c irqDisable()
 *  instead.
 * 
 *  \param mask The mask for the interrupt enable (IE) register
 */
void DisableInterrupt(irqMASK mask) __attribute__ ((deprecated));

/** \brief Prevents an interrupt occurring.
 *  \details This function applies the supplied mask to the interrupt enable
 *  (IE) register, then sets IME to 1.
 * 
 *  \param mask The mask for the interrupt enable (IE) register
 */
void irqDisable(int mask);

/** \brief Interrupt dispatcher.
 *  \details This is an assembly function that manages how interrupts are
 *  called and handled.
 *  \note This function is written in assembly.
 */
void IntrMain();

//---------------------------------------------------------------------------------
#ifdef __cplusplus
}	   // extern "C"
#endif
//---------------------------------------------------------------------------------
#endif // _gba_interrupt_h_
