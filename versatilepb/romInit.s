/* romInit.s - ARM Integrator ROM initialization module */

/* Copyright 1999-2001 ARM Limited */

/*
 * Copyright (c) 1999-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01a,20aug11,d_l created.
*/

/*
SEE ALSO:
\tb "ARM Architecture Reference Manual,"
*/

#define	_ASMLANGUAGE
#include <vxWorks.h>
#include <sysLib.h>
#include <asm.h>
#include <regs.h>
#include <config.h>
#include <arch/arm/mmuArmLib.h>

/* internals */

	.globl	FUNC(romInit)		/* start of system code */
	.globl	VAR(sdata)		/* start of data */
        .globl  _sdata
	.globl	VAR(integratorMemSize)	/* actual memory size */


/* externals */

	.extern	FUNC(romStart)	/* system initialization routine */

	.data
_sdata:
VAR_LABEL(sdata)
	.asciz	"start of data"
	.balign	4

/* variables */

VAR_LABEL(integratorMemSize)
	.long	0

	.text
	.balign 4

/*******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*
* SYNOPSIS
* \ss
* romInit
*     (
*     int startType	/@ only used by 2nd entry point @/
*     )
* \se
*
* RETURNS: N/A
*
* INTERNAL
* sysToMonitor examines the ROM for the first instruction and the string
* "Copy" in the third word so if this changes, sysToMonitor must be updated.
*/

_ARM_FUNCTION(romInit)
_romInit:
cold:
	MOV	r0, #BOOT_COLD	/* fall through to warm boot entry */
warm:
	B	start

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1999-2001 ARM Limited"
	.ascii   "\nCopyright 1999-2007 Wind River Systems, Inc."
	.balign 4

start:

	/*
	 * Set processor and MMU to known state as follows (we may have not
	 * been entered from a reset). We must do this before setting the CPU
	 * mode as we must set PROG32/DATA32.
	 *
	 * MMU Control Register layout.
	 *
	 * bit
	 *  0 M 0 MMU disabled
	 *  1 A 0 Address alignment fault disabled, initially
	 *  2 C 0 Data cache disabled
	 *  3 W 0 Write Buffer disabled
	 *  4 P 1 PROG32
	 *  5 D 1 DATA32
	 *  6 L 1 Should Be One (Late abort on earlier CPUs)
	 *  7 B ? Endianness (1 => big)
	 *  8 S 0 System bit to zero } Modifies MMU protections, not really
	 *  9 R 1 ROM bit to one     } relevant until MMU switched on later.
	 * 10 F 0 Should Be Zero
	 * 11 Z 0 Should Be Zero (Branch prediction control on 810)
	 * 12 I 0 Instruction cache control
	 */

	/* Setup MMU Control Register */

	LDR	r1, =MMU_INIT_VALUE		/* Defined in mmuArmLib.h */

	MCR	CP_MMU, 0, r1, c1, c0, 0	/* Write to MMU CR */

	/*
	 * If MMU was on before this, then we'd better hope it was set
	 * up for flat translation or there will be problems. The next
	 * 2/3 instructions will be fetched "translated" (number depends
	 * on CPU).
	 *
	 * We would like to discard the contents of the Write-Buffer
	 * altogether, but there is no facility to do this. Failing that,
	 * we do not want any pending writes to happen at a later stage,
	 * so drain the Write-Buffer, i.e. force any pending writes to
	 * happen now.
	 */


	MOV	r1, #0				/* data SBZ */
	MCR	CP_MMU, 0, r1, c7, c10, 4	/* drain write-buffer */

	/* Flush (invalidate) both I and D caches */

	MCR	CP_MMU, 0, r1, c7, c7, 0	/* R1 = 0 from above, data SBZ*/
#if defined(INTEGRATOR_EARLY_I_CACHE_ENABLE)
        MRC     CP_MMU, 0, r1, c1, c0, 0
	ORR	r1, r1, #MMUCR_I_ENABLE		/* conditionally enable Icache*/
        MCR     CP_MMU, 0, r1, c1, c0, 0        /* Write to MMU CR */
#endif /* defined(INTEGRATOR_EARLY_I_CACHE_ENABLE) */

       /*
	 * Set Process ID Register to zero, this effectively disables
	 * the process ID remapping feature.
	 */

	MOV	r1, #0
	MCR	CP_MMU, 0, r1, c13, c0, 0


            /* Set Context ID Register to zero, including Address Space ID */

            MCR     CP_MMU, 0, r1, c13, c0, 1



	/* disable interrupts in CPU and switch to SVC32 mode */

	MRS	r1, cpsr
	BIC	r1, r1, #MASK_MODE
	ORR	r1, r1, #MODE_SVC32 | I_BIT | F_BIT
	MSR	cpsr, r1

	/*
	 * CPU INTERRUPTS DISABLED
	 *
	 * disable individual interrupts in the interrupt controller
	 */

	/* MOV	r2, #IC_BASE */
/* R2->interrupt controller */
	LDR R2, =IC_BASE
	MVN	r1, #0				/* &FFFFFFFF */
	STR	r1, [r2, #0X14]	/* disable all PIC sources */
	LDR R2, =0x10003000
	STR	r1, [r2, #0x0c]	/* disable all SIC sources */

#if 0
ldmloop:
    ldr r1,=0x101f1000
    mov r2, #0x33
    str r2, [r1]
    b ldmloop
#endif

	LDR	sp, L$_STACK_ADDR
	MOV	fp, #0			/* zero frame pointer */

#if	(ARM_THUMB)
	LDR	r12, L$_rStrtInRom
	ORR	r12, r12, #1		/* force Thumb state */
	BX	r12
#else
	LDR	pc, L$_rStrtInRom
#endif	/* (ARM_THUMB) */

/******************************************************************************/

/*
 * PC-relative-addressable pointers - LDR Rn,=sym is broken
 * note "_" after "$" to stop preprocessor performing substitution
 */

	.balign	4

/*
L$_HiPosn:
	.long	ROM_TEXT_ADRS + HiPosn - FUNC(romInit)
*/

L$_rStrtInRom:
	.long	ROM_TEXT_ADRS + FUNC(romStart) - FUNC(romInit)

L$_STACK_ADDR:
	.long	STACK_ADRS

L$_memSize:
	.long	VAR(integratorMemSize)

#if defined(CPU_940T) || defined (CPU_940T_T)
L$_sysCacheUncachedAdrs:
	.long	SYS_CACHE_UNCACHED_ADRS
#endif /* defined(940T,940T_T) */
