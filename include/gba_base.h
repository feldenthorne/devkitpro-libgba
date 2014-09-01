//**********************************************************************************
/** \file gba_base.h
 *  Header for GBA base macros
 * 
 *  This is included by all libgba files because it contains most of the useful
 *  macro defines, such as those for syscalls, memory locations, and aligns.
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

//---------------------------------------------------------------------------------
#ifndef _gba_base_h_
#define _gba_base_h_
//---------------------------------------------------------------------------------

#include "gba_types.h"

/** \def VRAM
 *  \brief Base address of GBA video RAM.
 */
#define	VRAM		0x06000000

/** \def IWRAM
 *  \brief Base address of GBA internal work RAM.
 */
#define	IWRAM		0x03000000

/** \def EWRAM
 *  \brief Base address of GBA external work RAM.
 */
#define	EWRAM		0x02000000

/** \def EWRAM_END
 *  \brief End address of GBA external work RAM.
 */
#define	EWRAM_END	0x02040000

/** \def SRAM
 *  \brief Base address of GBA cart save RAM.
 */
#define	SRAM		0x0E000000

/** \def REG_BASE
 *  \brief Base address of GBA hardware registers.
*/
#define	REG_BASE	0x04000000

#ifndef	NULL
#define	NULL	0
#endif

//---------------------------------------------------------------------------------
/** \def SystemCall(Number)
 *  \brief Helper macro to insert a BIOS call.
 *  \details Inserts a SWI of the correct format for arm or thumb code.
 *  @param Number SWI number to call
 */
#if	defined	( __thumb__ )
#define	SystemCall(Number)	 __asm ("SWI	  "#Number"\n" :::  "r0", "r1", "r2", "r3")
#else
#define	SystemCall(Number)	 __asm ("SWI	  "#Number"	<< 16\n" :::"r0", "r1", "r2", "r3")
#endif

/** \def BIT(number)
 *  \brief Macro for bit settings.
 *  @param number Bit number to set
 */
#define BIT(number) (1<<(number))

/** \def IWRAM_CODE
 *  \brief Location of the internal work RAM code
 */
#define IWRAM_CODE	__attribute__((section(".iwram"), long_call))

/** \def EWRAM_CODE
 *  \brief Location of the external work RAM code
 */
#define EWRAM_CODE	__attribute__((section(".ewram"), long_call))

/** \def IWRAM_DATA
 *  \brief Location of the internal work RAM data
 */
#define IWRAM_DATA	__attribute__((section(".iwram")))

/** \def EWRAM_DATA
 *  \brief Location of the external work RAM data
 */
#define EWRAM_DATA	__attribute__((section(".ewram")))

/** \def EWRAM_BSS
 *  \brief Location of the internal work RAM code
 */
#define EWRAM_BSS	__attribute__((section(".sbss")))

/** \def ALIGN(m)
 *  \brief Aligns your data to 32- or 16-bits.
 *  \details 2 = 16-bit; 4 = 32-bit
 */
#define ALIGN(m)	__attribute__((aligned (m)))

//---------------------------------------------------------------------------------
/** \brief Modes for DMA and CPU(Fast)Set
 *  \details Fills use address as inputs, not values! COPY16 and COPY32 will copy
 *  half-words and words, respectively, from the source address!
 *  \warning DMA modes use addresses as inputs, rather than values.
 */
enum DMA_MODES {
	FILL	= (1<<24),
	COPY16	= (0<<26),
	COPY32	= (1<<26)
};

//---------------------------------------------------------------------------------
#endif //_gba_base_h
//---------------------------------------------------------------------------------
