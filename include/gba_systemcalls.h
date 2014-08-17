//**********************************************************************************
/** \file gba_systemcalls.h
 *  Header for GBA BIOS Support (system functions, SWI math, resets) 
 * 
 *  This includes useful enumerations and functions for manipulating the GBA 
 *  and its registers. Hardware division support and software interuppt handlers
 *  reside here.
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
#ifndef	_gba_systemcalls_h_
#define	_gba_systemcalls_h_
//---------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#include "gba_base.h"

//---------------------------------------------------------------------------------
// Reset Functions
//---------------------------------------------------------------------------------
/** \enum RESTART_FLAG
 *  \brief Flags for the SoftReset function
 */
typedef enum RESTART_FLAG {
	ROM_RESTART,	/**< Restart from RAM entry point. */
	RAM_RESTART		/**< Restart from ROM entry point */
} RESTART_FLAG;


/** \brief Reset the GBA and load the given return address.
 *  \details This clears the GBA stacks and interrupt flags, then reinitializes
 *  the system before entering system mode. It then loads the address passed in
 *  and restarts at RAM or ROM.
 *  @param RestartFlag Address to load upon reset, specified by the 
 *  \c RESTART_FLAG enum.
 */
void	SoftReset(RESTART_FLAG RestartFlag);

/** \enum RESET_FLAG
 *  \brief Flags controlling which parts of the system get reset.
 */
enum RESET_FLAG {
	RESET_EWRAM		=	(1<<0),	/**< Clear 256K on-board WRAM			*/
	RESET_IWRAM		=	(1<<1),	/**< Clear 32K in-chip WRAM				*/
	RESET_PALETTE	=	(1<<2),	/**< Clear Palette						*/
	RESET_VRAM		=	(1<<3),	/**< Clear VRAM							*/
	RESET_OAM		=	(1<<4),	/**< Clear OAM							*/
	RESET_SIO		=	(1<<5),	/**< Switches to general purpose mode	*/
	RESET_SOUND		=	(1<<6),	/**< Reset Sound registers				*/
	RESET_OTHER		=	(1<<7)	/**< Clear all other registers			*/
};

typedef enum RESET_FLAG RESET_FLAGS;

/** \brief reset the GBA registers and RAM.
 *  @param ResetFlags Flags for which registers to reset, specified by the
 *  \c RESET_FLAG enum.
 *  \note Bitwise OR ( | ) to combine them, bitwise AND ( & ) to mask.
 */
void RegisterRamReset(int ResetFlags);

//---------------------------------------------------------------------------------
// Interrupt functions
//---------------------------------------------------------------------------------

/** \brief Puts the CPU to sleep until an interrupt is called.
 *  \details This function puts the CPU into low-power mode and leaves the
 *  system peripherals (like sound and video) on until an interrupt is
 *  processed.
 */
static inline void Halt()	{ SystemCall(2); }

/** \brief Puts the system into deep sleep until specific interrupts are called.
 *  \details This function puts the CPU, clock, video, sound, and timers to 
 *  sleep. The processor can only be woken by the Game Pak, Joypad, and 
 *  GP-SIO interrupts.
 * 
 *  \warning If you don't disable video before implementing \c Stop(), it'll
 *  freeze the video, but you won't reap the benefits of reduced power
 *  consumption. Not disabling sound causes a similar problem. So, in general,
 *  turn off ALL THE THINGS! (And re-enable them on wakeup.)
 */
static inline void Stop()	{ SystemCall(3); }

/** \brief Calculates the checksum of the GBA BIOS ROM.
 *  \details This grabs data from the system BIOS (basically taking the header
 *  data in three parts and summing them) and returns the result as the
 *  checksum. A table of expected results is shown below:
 *  
 *  | System      | Value      |
 *  |-------------|------------|
 *  | GBA, GBA SP | 0xBAAE187F |
 *  | NDS         | 0xBAAE1880 |
 *  @return The BIOS Checksum, a 32-bit unsigned integer
 */
static inline u32 BiosCheckSum() {
//---------------------------------------------------------------------------------
	register u32 result;
	#if	defined	( __thumb__ )
		__asm ("SWI	0x0d\nmov %0,r0\n" :  "=r"(result) :: "r1", "r2", "r3");
	#else
		__asm ("SWI	0x0d<<16\nmov %0,r0\n" : "=r"(result) :: "r1", "r2", "r3");
	#endif
	return result;
}

//---------------------------------------------------------------------------------
// Math functions
//---------------------------------------------------------------------------------
/** \brief Divides two 32-bit signed integers (\c s32).
 *  \note This function is written in assembly and hooks into GBA SWI 6 (Div).
 *  It uses register "r1", which stores the division result, and moves the
 *  contents of "r1" into "r0".
 *  @param Number Number to be divided into
 *  @param Divisor Number to divide with
 *  @return Division result
 */
s32	Div(s32 Number, s32 Divisor);

/** \brief Modulo operation for two 32-bit signed integers (s32).
 *  \note This function is written in assembly and hooks into GBA SWI 6 (Div).
 *  It uses register "r1", which stores the division result, and moves the
 *  contents of "r1" into "r0".
 *  @param Number Number to be divided into
 *  @param Divisor Number to divide with
 *  @return Modulus (remainder of division) result
 */
s32	DivMod(s32 Number, s32 Divisor);

/** \brief Absolute value of the division of two 32-bit signed integers (s32).
 *  \note This function is written in assembly and hooks into GBA SWI 6 (Div).
 *  It uses register "r3", which performs the abs(number) function on the
 *  result of the division of the inputs. Essentially, this means it moves
 *  abs("r1") - see \c Div() - into "r0".
 *  @param Number Number to be divided into
 *  @param Divisor Number to divide with
 *  @return Absolute value of division result, UNSIGNED 32-bit integer (\c u32)
 */
u32	DivAbs(s32 Number, s32 Divisor);

/** \brief Divides two 32-bit signed integers (\c s32), ARM compatible.
 *  \details It functions like \c Div(), but swaps the register order.
 *  \note This function is written in assembly and hooks into GBA SWI 7 (DivArm).
 *  It uses register "r1", which stores the division result, and moves the
 *  contents of "r1" into "r0".
 *  \warning This function is slightly less efficient than its native GBA
 *  equivalent, \c Div(). SWI 6 is a clock cycle faster than SWI 7.
 *  @param Number Number to be divided into
 *  @param Divisor Number to divide with
 *  @return Division result
 */
s32	DivArm(s32 Divisor, s32 Number);

/** \brief Modulo operation for two 32-bit signed integers (s32), ARM compatible.
 *  \note This function is written in assembly and hooks into GBA SWI 7 (DivArm).
 *  It uses register "r1", which stores the division result, and moves the
 *  contents of "r1" into "r0".
 *  \warning This function is slightly less efficient than its native GBA
 *  equivalent, \c DivMod(). SWI 6 is a clock cycle faster than SWI 7.
 *  @param Number Number to be divided into
 *  @param Divisor Number to divide with
 *  @return Modulus (remainder of division) result
*/
s32	DivArmMod(s32 Divisor, s32 Number);

/** \brief Absolute value of the division of two 32-bit signed integers (s32), ARM compatible.
 *  \note This function is written in assembly and hooks into GBA SWI 7 (DivArm).
 *  It uses register "r3", which performs the abs(number) function on the
 *  result of the division of the inputs. Essentially, this means it moves
 *  abs("r1") - see \c Div() - into "r0".
 *  \warning This function is slightly less efficient than its native GBA
 *  equivalent, \c DivAbs(). SWI 6 is a clock cycle faster than SWI 7.
 *  @param Number Number to be divided into
 *  @param Divisor Number to divide with
 *  @return Absolute value of division result, UNSIGNED 32-bit integer (\c u32)
 */
u32	DivArmAbs(s32 Divisor, s32 Number);

/** \brief Calculates the square root of the input.
 *  \details  Because this returns an integer, you have re reposition your radix
 *  (decimal point) by shifting bits (multiplying the input by powers of two) to
 *  get a value beyond the decimal place.
 *  
 *  For example, say you want the square root of K to M/2 decimal places after
 *  You'd feed this function Sqrt( (2^M) * K ), which will return your result: a
 *  16-bit unsigned integer whose value is ( 2^(M/2)*sqrt(K) ). If you're
 *  comparing this result with a particular value, then multiplying the value
 *  for comparison by (2^(M/2)) before comparing it will allow you to use that
 *  precision for some good.
 *  \note In general, if you want accurate results, left-shift your input to 
 *  this function as much as possible. Do this with the \c shl instruction for
 *  assembly or by multiplying by 2^N.
 *  \warning 
 *  \parblock
 *  The result of this operation is a 16-bit unsigned integer! This
 *  means that if you want decimal precision, you need to left-shift your input
 *  number by TWICE the number of decimals you want to retain. This will leave
 *  you with a result whose value is left shifted by the number of decimals you
 *  want.
 * 
 *  As an example, say you want to take the square root of 57. If you simply
 *  use Sqrt(57), you'll get 0x07 back (7 decimal) when the actual result is 
 *  ~7.549. But if you want the result to have 2 decimal places of precision, 
 *  you'd multiply the input by 4: Sqrt(57*4) or, alternatively Sqrt(57 shl 4). 
 *  This makes the result 0x0F, or 15 decimal. Since we wanted
 *  2 decimal places of precision, note that if we divide 15/2, the result is
 *  7.50, which is much closer to the approximate root, ~7.549.
 *  \endparblock
 *  @param X The number whose square root will be taken
 *  @return Square root of the input, 16-bit unsigned integer (\c u16)
 */
u16 Sqrt(u32 X);

/** \brief Calculates the inverse tangent of the input number
 *  \warning As with most single-input inverse tangent functions, you need to
 *  keep track of what quadrant you're actually in, if it matters.
 *  @param Tan Input tangent value, a 16-bit signed integer (\c s16) whose form 
 *  is described in the table below:
 *  | Bit  | Use            |
 *  |------|----------------|
 *  | 15   | Sign bit       |
 *  | 14   | Integer Value  |
 *  | 13-0 | Decimal Values |
 *  @return The angle (\c s16) whose tangent is the input number (radians). 
 *  Range: -PI/2 < angle < PI/2 (0xC000 < angle < 0x4000h)
 */
s16 ArcTan(s16 Tan);

/** \brief Calculates the inverse tangent using the input X and Y coordinates.
 *  \note You should use this rather than \c ArcTan() wherever possible, as it
 *  gives you a result that correctly specifies your quadrant.
 *  @param X X-axis position, a 16-bit signed integer (\c s16), whose form 
 *  is described in the table below:
 *  | Bit  | Use            |
 *  |------|----------------|
 *  | 15   | Sign bit       |
 *  | 14   | Integer Value  |
 *  | 13-0 | Decimal Values |
 *  @param Y Y-axis position, a 16-bit signed integer (\c s16), see X table for
 *  form
 *  @return The angle (\c u16) whose tangent is Y/X, quadrant-specific.
 *  Range: 0 < angle < 2*PI (0x0000 < angle < 0xFFFF)
 */
u16 ArcTan2(s16 X, s16 Y);

/** \brief Copies or fills a memory location in units of words (4 bytes) 
 *  or halfwords (16 bytes) after it.
 *  \note If you're using this function to copy words, remember that length is
 *  specified in the number of words (bytes/4). Similarly, in halfword mode,
 *  length is (bytes/2). 
 *  @param source Address to begin copy from
 *  @param dest Address to copy to
 *  @param mode 32-bit integer specifying copy length and mode. Format is
 *  shown in the table below:
 *  | Bit  | Explanation                                                          |
 *  |------|----------------------------------------------------------------------|
 *  | 0-20 | Specifies number of <b>words or halfwords</b> to copy, based on mode | 
 *  | 24   | Fixed source address (0 = copy, 1 = fill in chunks based on Bit 26)  |
 *  | 26   | Data size (0 = 16-bit, 1 = 32-bit)                                   |
 */
void CpuSet( const void *source, void *dest, u32 mode);
/** \brief Copies or fills a memory location in units of 32-bytes.
 *  \note This function copies data in 32 byte chunks.
 *  @param source Address to begin copy from
 *  @param dest Address to copy to
 *  @param mode 32-bit integer specifying copy length and mode. Format is
 *  shown in the table below:
 *  | Bit  | Explanation                                                          |
 *  |------|----------------------------------------------------------------------|
 *  | 0-20 | Specifies number of <b>8 word chunks</b> to copy                     | 
 *  | 24   | Fixed source address (0 = copy, 1 = fill in word-size chunks)        |
 */
void CpuFastSet( const void *source, void *dest, u32 mode);

/** \brief Wait for an interrupt to occur.
 *  \details This acts a lot like \c Halt(), but it makes the system sleep
 *  until a specific interrupt occurs. The wake-up interrupt is specified based
 *  on the ordering of the Interrupt-Enable/Interrupt-Flag registers, shown
 *  in the table below:
 *  | Bit   | Explanation          |
 *  |-------|----------------------|
 *  | 0     | LCD V-Blank          |
 *  | 1     | LCD H-Blank          |
 *  | 2     | LCD V-Counter Match  |
 *  | 3     | Timer 0 Overflow     |
 *  | 4     | Timer 1 Overflow     |
 *  | 5     | Timer 2 Overflow     |
 *  | 6     | Timer 3 Overflow     |
 *  | 7     | Serial Communication |
 *  | 8     | DMA 0                |
 *  | 9     | DMA 1                |
 *  | 10    | DMA 2                |
 *  | 11    | DMA 3                |
 *  | 12    | Keypad               |
 *  | 13    | Game Pak             |
 *  | 14-15 | Unused               |
 *  \note This function hooks into GBA SWI 4 (IntrWait) and calling it 
 *  automatically sets IME to 1.
 *  @param ReturnFlag Flag specifying how to handle waking. The table below 
 *  describes acceptable values and their behaviors:
 *  | Value | Behavior                                                            |
 *  |-------|---------------------------------------------------------------------|
 *  | 0     | If there's already an interrupt queued, return. If not, wait.       |
 *  | 1     | Disregard any currently queued interrupts, return on next interrupt |
 *  @param IntFlag Flags specifying which interrupts to wake on. This input is in 
 *  the format of IE/IF registers.
 */
void IntrWait(u32 ReturnFlag, u32 IntFlag);

/** \brief Wait for a vertical blank interrupt to occur.
 *  \details This is a specific instance of \c IntrWait() that automatically
 *  sets it to ignore currently queued interrupts and only wake on the next
 *  VBlank interrupt.
 */
static inline void VBlankIntrWait()	{ SystemCall(5); }

//---------------------------------------------------------------------------------
#ifdef __cplusplus
}	   // extern "C"
#endif


//---------------------------------------------------------------------------------
#endif //_gba_systemcalls_h_
