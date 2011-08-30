/* sysLib.c - ARM Integrator system-dependent routines */

/* Copyright 1999-2003 ARM Limited */
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
01a,15aug11,d_l  copied from integrator926ejs.
*/

/*
DESCRIPTION
This library provides board-specific routines for the ARM VersatilePB 
Development Board BSP.

It #includes the following chip drivers:
    nullVme.c -         dummy VMEbus routines
    ambaTimer.c -       AMBA timer driver
    ambaIntrCtl.c -     AMBA interrupt controller driver
    primeCellSio.c -	PrimeCell UART driver
    pciIomapLib.c -	PCI I/O mapping support
if INCLUDE_FLASH is defined, it #includes:
    flashMem.c -        Flash memory driver
    nvRamToFlash.c -    driver to use some Flash like NVRAM
else it #includes:
    nullNvRam.c -	dummy NVRAM routines

It #includes the following BSP files:
    sysSerial.c -	serial device initialization routines
    sysEnd.c -		END network driver support routines.
    pciIomapShow.c -	PCI Show routines

INCLUDE FILES: sysLib.h string.h intLib.h taskLib.h vxLib.h muxLib.h
	       pciIomapLib.h

SEE ALSO:
\tb "ARM Architecture Reference Manual,"
\tb "ARM 926E-S Technical Reference Manual",
\tb "ARM Reference Peripherals Specification,"
\tb "ARM VersatilePB User Guide",
\tb "ARM Integrator/CM926EJ-S User Guide",
*/

/* includes */

#include <vxWorks.h>
#include "config.h"

#if !defined(INCLUDE_MMU) && \
    (defined(INCLUDE_CACHE_SUPPORT) || defined(INCLUDE_MMU_BASIC) || \
     defined(INCLUDE_MMU_FULL) || defined(INCLUDE_MMU_MPU) || \
     defined(INCLUDE_MMU_GLOBAL_MAP))
#define INCLUDE_MMU
#endif

#include <sysLib.h>
#include <string.h>
#include <intLib.h>
#include <taskLib.h>
#include <vxLib.h>
#include <muxLib.h>
#include <cacheLib.h>

#ifdef	INCLUDE_MMU
#include <arch/arm/mmuArmLib.h>
#include <private/vmLibP.h>
#endif	/* INCLUDE_MMU */
#include <dllLib.h>
#ifdef INCLUDE_PCI
#include "pciIomapLib.h"
#endif

#ifdef INCLUDE_WINDML
#define INCLUDE_AMBAKEYBOARD
#define INCLUDE_AMBAMOUSE
#endif

/* imports */

extern void bspDbg(const char *fmt, ...);

IMPORT char end [];			    /* end of system, created by ld */
#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
IMPORT VOIDFUNCPTR _func_armIntStackSplit;  /* ptr to fn to split stack */
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */

#if defined(INCLUDE_CACHE_SUPPORT)
UINT32 * sysCacheUncachedAdrs = (UINT32 *)SYS_CACHE_UNCACHED_ADRS;
#endif /* defined(INCLUDE_CACHE_SUPPORT) */

/* globals */

#if defined(INCLUDE_MMU)

/*
 * The following structure describes the various different parts of the
 * memory map to be used only during initialization by
 * vm(Base)GlobalMapInit() when INCLUDE_MMU_BASIC/FULL/GLOBAL_MAP are
 * defined.
 *
 * Clearly, this structure is only needed if the CPU has an MMU!
 *
 * The following are not the smallest areas that could be allocated for a
 * working system. If the amount of memory used by the page tables is
 * critical, they could be reduced.
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {

    /* DRAM must always be the first entry */
    /* adrs and length parameters must be page-aligned (multiples of 0x1000) */
    /* DRAM - Always the first entry */
    {
    LOCAL_MEM_LOCAL_ADRS,	/* virtual address */
    LOCAL_MEM_LOCAL_ADRS,	/* physical address */
    ROUND_UP (LOCAL_MEM_SIZE, PAGE_SIZE), /* length, then initial state: */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE
    },

    /*
     * ROM is normally marked as uncacheable by VxWorks. We leave it like that
     * for the time being, even though this has a severe impact on execution
     * speed from ROM.
     */
    {
    ROM_BASE_ADRS,
    ROM_BASE_ADRS,
    ROUND_UP (ROM_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
#ifdef INCLUDE_FLASH
    /* needs to be writable */

    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
#else
    VM_STATE_VALID	| VM_STATE_WRITABLE_NOT  | VM_STATE_CACHEABLE_NOT
#endif
    },

    /*
     * I/O space:
     * Do not map in all I/O space, only that which has something there.
     * Otherwise we will use all of RAM allocating page tables!
     */
    {
    INTEGRATOR_HDR_BASE,	/* Core Module Header regs */
    INTEGRATOR_HDR_BASE,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
    {
    INTEGRATOR_SC_BASE,	/* System controller */
    INTEGRATOR_SC_BASE,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
    {
    INTEGRATOR_EBI_BASE,	/* EBI controller */
    INTEGRATOR_EBI_BASE,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#if defined(INCLUDE_TTY_DEV) || defined(INCLUDE_SIO_POLL)
    {
    UART_0_BASE_ADR,		/* PrimeCell UART */
    UART_0_BASE_ADR,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
    {
    UART_1_BASE_ADR,		/* PrimeCell UART */
    UART_1_BASE_ADR,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#endif /* INCLUDE_TTY_DEV || INCLUDE_SIO_POLL */
#ifdef INCLUDE_AMBAKEYBOARD
    {
    KBD_BASE_ADR,		/* PrimeCell Keyboard */
    KBD_BASE_ADR,
    ROUND_UP (KBD_REGS_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#endif /* INCLUDE_AMBAKEYBOARD */
#ifdef INCLUDE_AMBAMOUSE
    {
    MOUSE_BASE_ADR,		/* PrimeCell Mouse */
    MOUSE_BASE_ADR,
    ROUND_UP (MOUSE_REGS_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#endif /* INCLUDE_AMBAMOUSE */
    {
    IC_BASE,		/* Interrupt controller */
    IC_BASE,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
    {
    AMBA_TIMER_BASE,	/* Counter/Timer */
    AMBA_TIMER_BASE,
    PAGE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#ifdef INCLUDE_PCI
    {
    CPU_PCI_IO_ADRS,	/* PCI I/O space */
    CPU_PCI_IO_ADRS,
    ROUND_UP (CPU_PCI_IO_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
    {
    CPU_PCI_CNFG_ADRS,	/* PCI Configuration space */
    CPU_PCI_CNFG_ADRS,
    ROUND_UP (CPU_PCI_CNFG_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
#ifdef INTEGRATOR_CONSERVE_VIRTUAL_SPACE
    /* Conserve mmu table space and overhead */
    {
    CPU_PCI_MEM_ADRS,	/* PCI Mem space */
    CPU_PCI_MEM_ADRS,
    ROUND_UP (SZ_8M, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#else
    {
    CPU_PCI_MEM_ADRS,	/* PCI Mem space */
    CPU_PCI_MEM_ADRS,
    ROUND_UP (CPU_PCI_MEM_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID	| VM_STATE_WRITABLE	 | VM_STATE_CACHEABLE_NOT
    },
#endif /* INTEGRATOR_CONSERVE_VIRTUAL_SPACE */
    {
    V3_BASE,		/* V360EPC register */
    V3_BASE,
    ROUND_UP (V3_REGS_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
#endif /* INCLUDE_PCI */
    {
    INTEGRATOR_MBRD_SSRAM_BASE,
    INTEGRATOR_MBRD_SSRAM_BASE,
    ROUND_UP (INTEGRATOR_MBRD_SSRAM_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
    /*
     * The following entry is for the "real" address of the Core Module
     * SDRAM (as opposed to the alias in low memory). The size of the area
     * mapped here is for the amount that has been "occluded" by the Core
     * Module SSRAM from zero upwards.
     */
    {
    INTEGRATOR_HDR0_SDRAM_BASE,
    INTEGRATOR_HDR0_SDRAM_BASE,
    ROUND_UP (INTEGRATOR_HDR_SSRAM_SIZE, PAGE_SIZE),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    }
    };

int sysPhysMemDescNumEnt = NELEMENTS (sysPhysMemDesc);

#endif /* defined(INCLUDE_MMU) */

int	sysBus	    = BUS;		/* system bus type (VME_BUS, etc) */
int	sysCpu	    = CPU;		/* system CPU type (e.g. ARMARCH4/4_T)*/
char *	sysBootLine = BOOT_LINE_ADRS; 	/* address of boot line */
char *	sysExcMsg   = EXC_MSG_ADRS;	/* catastrophic message area */
int	sysProcNum;			/* processor number of this CPU */


/* locals */

/*
 * List of interrupts to be serviced in order of decreasing priority.
 * Interrupts not in this list will be serviced least-significant bit
 * first at a lower priority than those in the list.
 *
 * To use lowest-bit = highest-priority, reverse the sense of the
 * condition below so that ambaIntLvlPriMap is a zero pointer.
 */


/* defines */

/* externals */

IMPORT int  ambaIntDevInit (void);
#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
IMPORT void sysIntStackSplit (char *, long);
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */


/* globals */


/* forward LOCAL functions declarations */


/* forward declarations */

char *	sysPhysMemTop (void);


#if defined(INCLUDE_PCI) && defined(INCLUDE_USB)
void sysUsbPciInit(void);
#endif

/* included source files */

#ifdef INCLUDE_FLASH
#include <mem/nvRamToFlash.c>
#include "flashMem.c"
#else
#include <mem/nullNvRam.c>
#endif
#include <vme/nullVme.c>
#if defined(INCLUDE_TTY_DEV) || defined(INCLUDE_SIO_POLL)
#include "sysSerial.c"
#include "pl011.c"
#endif	/* INCLUDE_TTY_DEV || INCLUDE_SIO_POLL */

#include "sp804.c"
#include "pl190.c"
#ifdef	INCLUDE_PCI
#include "pciIomapLib.c"
#include "pciIomapShow.c"
#endif	/* INCLUDE_PCI */

#ifdef	INCLUDE_END
#include "sysEnd.c"
#endif

#ifdef INCLUDE_WINDML
#include "ambaKbd.c"
#include "ambaMse.c"
#include "sysWindML.c"
#endif /* INCLUDE_WINDML */

/*******************************************************************************
*
* sysModel - return the model name of the CPU board
*
* This routine returns the model name of the CPU board.
*
* RETURNS: A pointer to a string identifying the board and CPU.
*/

char *sysModel (void)
    {
    return SYS_MODEL;
    }

/*******************************************************************************
*
* sysBspRev - return the BSP version with the revision eg 1.2/<x>
*
* This function returns a pointer to a BSP version with the revision.
* e.g. 1.2/<x>. BSP_REV is concatenated to BSP_VERSION to form the
* BSP identification string.
*
* RETURNS: A pointer to the BSP version/revision string.
*/

char * sysBspRev (void)
    {
    return (BSP_VERSION BSP_REV);
    }

/*******************************************************************************
*
* sysHwInit0 - perform early BSP-specific initialization
*
* This routine performs such BSP-specific initialization as is necessary before
* the architecture-independent cacheLibInit can be called. It is called
* from usrInit() before cacheLibInit(), before sysHwInit() and before BSS
* has been cleared.
*
* RETURNS: N/A
*/

void sysHwInit0 (void)
    {

#ifdef INCLUDE_CACHE_SUPPORT
    /*
     * Install the appropriate cache library, no address translation
     * routines are required for this BSP, as the default memory map has
     * virtual and physical addresses the same.
     */

    cacheArm926eLibInstall (mmuPhysToVirt, mmuVirtToPhys);
#endif /* INCLUDE_CACHE_SUPPORT */


#if defined(INCLUDE_MMU)

   /* Install the appropriate MMU library and translation routines */

    mmuArm926eLibInstall (mmuPhysToVirt, mmuVirtToPhys);
#endif /* defined(INCLUDE_MMU) */

    return;
    }

#if defined(INCLUDE_PCI)
/*******************************************************************************
*
* sysV3Init - initialize the V3 PCI bridge controller
*
* This function initializes the V3 chip which controls the PCI bridge.
*
* RETURNS: N/A
*/

void sysV3Init (void)
    {
    volatile UINT32 i, j;

    /* setting this register will take the V3 out of reset */

    *(UINT32 *)(INTEGRATOR_SC_PCIENABLE) = 1;

    /* wait a few usecs to settle the device and the PCI bus (no timers yet) */
#if defined(CPU_1020E) || defined(CPU_1022E)

    /* Use the 10200 CM's 24 Mhz Reference Clock */
    i = *(volatile int *)(0x10000028);
    j = i + 240000;
    if( j < i)
        {
        /* Overflow */
	while( (i = *(volatile int *)(0x10000028)) > j)
            ;
	while( (i = *(volatile int *)(0x10000028)) < j)
            ;
        }
    else
        {
        while ( (i = *(volatile int *)(0x10000028)) < j )
            ;
        }
#else

    for (i = 0; i < 100 ; i++)
	   j = i + 1;

#endif /* defined(CPU_1020E, 1022E) */

    /* Now write the Base I/O Address Word to V3_BASE + 0x6C */

    *(UINT16 *)(V3_BASE + V3_LB_IO_BASE) = (UINT16)(V3_BASE >> 16);

    do
	{
        *(UINT8 *)(V3_BASE + V3_MAIL_DATA) = 0xAA;
	*(UINT8 *)(V3_BASE + V3_MAIL_DATA + 4) = 0x55;
	}
    while (*(UINT8 *)(V3_BASE + V3_MAIL_DATA) != 0xAA ||
	    *(UINT8 *)(V3_BASE + V3_MAIL_DATA + 4) != 0x55);

    /* Make sure that V3 register access is not locked, if it is, unlock it */

    if ((*(UINT16 *)(V3_BASE + V3_SYSTEM) & V3_SYSTEM_M_LOCK) ==
							    V3_SYSTEM_M_LOCK)
	*(UINT16 *)(V3_BASE + V3_SYSTEM) = 0xA05F;

    /*
     * Ensure that the slave accesses from PCI are disabled while we
     * setup windows
     */

    *(UINT16 *)(V3_BASE + V3_PCI_CMD) &=
    				~(V3_COMMAND_M_MEM_EN | V3_COMMAND_M_IO_EN);


    /* Clear RST_OUT to 0; keep the PCI bus in reset until we've finished */

    *(UINT16 *)(V3_BASE + V3_SYSTEM) &= ~V3_SYSTEM_M_RST_OUT;

    /* Make all accesses from PCI space retry until we're ready for them */

    *(UINT16 *)(V3_BASE + V3_PCI_CFG) |= V3_PCI_CFG_M_RETRY_EN;

    /*
     * Set up any V3 PCI Configuration Registers that we absolutely have to
     * LB_CFG controls Local Bus protocol.
     * Enable LocalBus byte strobes for READ accesses too.
     * set bit 7 BE_IMODE and bit 6 BE_OMODE
     */

    *(UINT16 *)(V3_BASE + V3_LB_CFG) |= 0x0C0;

    /*
     * PCI_CMD controls overall PCI operation.
     * Enable PCI bus master.
     */

    *(UINT16 *)(V3_BASE + V3_PCI_CMD) |= 0x04;

    /*
     * PCI_MAP0 controls where the PCI to CPU memory window is on the Local Bus
     */

    *(UINT32 *)(V3_BASE + V3_PCI_MAP0) = (INTEGRATOR_BOOT_ROM_BASE) |
					(V3_PCI_MAP_M_ADR_SIZE_512M |
					V3_PCI_MAP_M_REG_EN |
					V3_PCI_MAP_M_ENABLE);

    /* PCI_BASE0 is the PCI address of the start of the window */

    *(UINT32 *)(V3_BASE + V3_PCI_BASE0) = INTEGRATOR_BOOT_ROM_BASE;

    /* PCI_MAP1 is LOCAL address of the start of the window */

    *(UINT32 *)(V3_BASE + V3_PCI_MAP1) = (INTEGRATOR_HDR0_SDRAM_BASE) |
			(V3_PCI_MAP_M_ADR_SIZE_1024M | V3_PCI_MAP_M_REG_EN |
			 V3_PCI_MAP_M_ENABLE);

    /* PCI_BASE1 is the PCI address of the start of the window */

    *(UINT32 *)(V3_BASE + V3_PCI_BASE1) = INTEGRATOR_HDR0_SDRAM_BASE;

    /*
     * Set up the windows from local bus memory into PCI configuration,
     * I/O and Memory.
     * PCI I/O, LB_BASE2 and LB_MAP2 are used exclusively for this.
     */

    *(UINT16 *)(V3_BASE +V3_LB_BASE2) =
    			((CPU_PCI_IO_ADRS >> 24) << 8) | V3_LB_BASE_M_ENABLE;
    *(UINT16 *)(V3_BASE + V3_LB_MAP2) = 0;

    /*
     * PCI Configuration, use LB_BASE1/LB_MAP1.
     *
     * PCI Memory use LB_BASE0/LB_MAP0 and LB_BASE1/LB_MAP1
     * Map first 256Mbytes as non-prefetchable via BASE0/MAP0
     * (INTEGRATOR_PCI_BASE == PCI_MEM_BASE)
     */

    *(UINT32 *)(V3_BASE + V3_LB_BASE0) =
    			INTEGRATOR_PCI_BASE | (0x80 | V3_LB_BASE_M_ENABLE);

    *(UINT16 *)(V3_BASE + V3_LB_MAP0) =
    				((INTEGRATOR_PCI_BASE >> 20) << 0x4) | 0x0006;

    /* Map second 256 Mbytes as prefetchable via BASE1/MAP1 */

    *(UINT32 *)(V3_BASE + V3_LB_BASE1) =
    			INTEGRATOR_PCI_BASE | (0x84 | V3_LB_BASE_M_ENABLE);

    *(UINT16 *)(V3_BASE + V3_LB_MAP1) =
    			(((INTEGRATOR_PCI_BASE + SZ_256M) >> 20) << 4) | 0x0006;

    /*
     * Allow accesses to PCI Configuration space
     * and set up A1, A0 for type 1 config cycles
     */

    *(UINT16 *)(V3_BASE + V3_PCI_CFG) = ((*(UINT16 *)(V3_BASE + V3_PCI_CFG)) &
			   ~(V3_PCI_CFG_M_RETRY_EN | V3_PCI_CFG_M_AD_LOW1) ) |
			   V3_PCI_CFG_M_AD_LOW0;

    /* now we can allow in PCI MEMORY accesses */

    *(UINT16 *)(V3_BASE + V3_PCI_CMD) =
		    (*(UINT16 *)(V3_BASE + V3_PCI_CMD)) | V3_COMMAND_M_MEM_EN;

    /*
     * Set RST_OUT to take the PCI bus is out of reset, PCI devices can
     * initialize and lock the V3 system register so that no one else
     * can play with it
     */

   *(UINT16 *)(V3_BASE + V3_SYSTEM) =
		    (*(UINT16 *)(V3_BASE + V3_SYSTEM)) | V3_SYSTEM_M_RST_OUT;

   *(UINT16 *)(V3_BASE + V3_SYSTEM) =
   			(*(UINT16 *)(V3_BASE + V3_SYSTEM)) | V3_SYSTEM_M_LOCK;

    return;
    }
#endif /* defined(INCLUDE_PCI) */

/*******************************************************************************
*
* sysHwInit - initialize the CPU board hardware
*
* This routine initializes various features of the hardware.
* Normally, it is called from usrInit() in usrConfig.c.
*
* NOTE: This routine should not be called directly by the user.
*
* RETURNS: N/A
*/

void sysHwInit (void)
    {
    /* install the IRQ/SVC interrupt stack splitting routine */

#ifdef POWER_MGT_INSTRUMENT
    UINT16 leds;

    AMBA_TIMER_READ (LED_LIGHTS,leds);
    leds = leds & ~LED_2;
    AMBA_TIMER_WRITE (LED_LIGHTS, leds);
#endif

#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
   _func_armIntStackSplit = sysIntStackSplit;
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */

#if defined(INCLUDE_PCI)

    /* Initialize the V3 PCI bridge controller */

    sysV3Init();

    /*  Initialize PCI driver library. */

    if (pciIomapLibInit (PCI_MECHANISM_3, CPU_PCI_CNFG_ADRS,
    			 CPU_PCI_CNFG_ADRS, 0) != OK)
	sysToMonitor (BOOT_NO_AUTOBOOT);

#endif  /* INCLUDE_PCI */

#if defined(INCLUDE_TTY_DEV) || defined(INCLUDE_SIO_POLL)

    /* initialize the serial devices */

    sysSerialHwInit ();      /* initialize serial data structure */
#endif /* INCLUDE_TTY_DEV || INCLUDE_SIO_POLL */

#ifdef POWER_MGT_INSTRUMENT
    AMBA_TIMER_READ (LED_LIGHTS,leds);
    leds = leds | LED_2;
    AMBA_TIMER_WRITE (LED_LIGHTS, leds);
#endif
#ifdef INCLUDE_POWER_MGMT_CPU_BSP_SUPPORT
    sysCpuPwrInit ();
#endif /* INCLUDE_POWER_MGMT_CPU_BSP_SUPPORT */
#ifdef POWER_MGT_INSTRUMENT
    AMBA_TIMER_READ (LED_LIGHTS,leds);
    leds = leds & ~LED_2;
    AMBA_TIMER_WRITE (LED_LIGHTS, leds);
#endif

    }

/*******************************************************************************
*
* sysHwInit2 - additional system configuration and initialization
*
* This routine connects system interrupts and does any additional
* configuration necessary.  Note that this is called from
* sysClkConnect() in the timer driver.
*
* RETURNS: N/A
*/

void sysHwInit2 (void)
    {
    static BOOL initialised = FALSE;

    if (initialised)
    	return;

    /* initialize the interrupt library and interrupt driver */

    intLibInit (AMBA_INT_NUM_LEVELS, AMBA_INT_NUM_LEVELS, INT_MODE);
    ambaIntDevInit();

    /* connect sys clock interrupt and auxiliary clock interrupt */

    (void)intConnect (INUM_TO_IVEC (SYS_TIMER_INT_VEC), sysClkInt, 0);
#ifdef INCLUDE_AUX_CLK
    (void)intConnect (INUM_TO_IVEC (AUX_TIMER_INT_VEC), sysAuxClkInt, 0);
#endif

#ifdef INCLUDE_POWER_MGMT_CPU_BSP_SUPPORT

    /* connect sleep clock interrupt */

    (void)intConnect (INUM_TO_IVEC (INT_LVL_TIMER_2), sysOneShotInt, 0);
#endif /* INCLUDE_POWER_MGMT_CPU_BSP_SUPPORT */

#ifdef	INCLUDE_TTY_DEV

    /* connect serial interrupt */

    sysSerialHwInit2 ();
#endif /* INCLUDE_TTY_DEV */

#if defined (INCLUDE_PCI)
#if defined (INCLUDE_DEC21X40END) || defined (INCLUDE_FEI82557END)

    /* map all appropriate Ethernet PCI device memory and I/O addresses */

    sysLanPciInit ();

#endif  /* INCLUDE_DEC21X40END/FEI82557END */
#if defined (INCLUDE_USB)

    /* Low level init for usb */

    sysUsbPciInit ();

#endif
#endif  /* INCLUDE_PCI */

    initialised = TRUE;

    }

/*******************************************************************************
*
* sysPhysMemTop - get the address of the top of physical memory
*
* This routine returns the address of the first missing byte of memory,
* which indicates the top of memory.
*
* Normally, the user specifies the amount of physical memory with the
* macro LOCAL_MEM_SIZE in config.h.  BSPs that support run-time
* memory sizing do so only if the macro LOCAL_MEM_AUTOSIZE is defined.
* If not defined, then LOCAL_MEM_SIZE is assumed to be, and must be, the
* true size of physical memory.
*
* NOTE: Do no adjust LOCAL_MEM_SIZE to reserve memory for application
* use.  See sysMemTop() for more information on reserving memory.
*
* RETURNS: The address of the top of physical memory.
*
* SEE ALSO: sysMemTop()
*/

char * sysPhysMemTop (void)
    {
    static char * physTop = NULL;

    if (physTop == NULL)
	{
	/* Don't do autosizing, if size is given */

	physTop = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);

	}

    return physTop;
    }

/*******************************************************************************
*
* sysMemTop - get the address of the top of VxWorks memory
*
* This routine returns a pointer to the first byte of memory not
* controlled or used by VxWorks.
*
* The user can reserve memory space by defining the macro USER_RESERVED_MEM
* in config.h.  This routine returns the address of the reserved memory
* area.  The value of USER_RESERVED_MEM is in bytes.
*
* RETURNS: The address of the top of VxWorks memory.
*/

char * sysMemTop (void)
    {
    static char * memTop = NULL;

    if (memTop == NULL)
	{
#ifdef INCLUDE_EDR_PM
	memTop = (char *)sysPhysMemTop () - USER_RESERVED_MEM - PM_RESERVED_MEM;
#else
	memTop = (char *)sysPhysMemTop () - USER_RESERVED_MEM;
#endif
	}

    return (memTop);
    }

/*******************************************************************************
*
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  It is usually called
* only by reboot() -- which services ^X -- and bus errors at interrupt
* level.  However, in some circumstances, the user may wish to introduce a
* new <startType> to enable special boot ROM facilities.
*
* RETURNS: Does not return
*/

STATUS sysToMonitor
    (
    int startType	/* passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR	pRom;
    UINT32 *	p = (UINT32 *)ROM_TEXT_ADRS;

#ifdef	INCLUDE_TTY_DEV
    sysSerialReset ();	/* put serial devices into quiet state */
#endif	/* INCLUDE_TTY_DEV */

    sysClkDisable();

    intIFLock ();

    /*
     * Examine ROM - if it's a VxWorks boot ROM, jump to the warm boot entry
     * point; otherwise jump to the start of the ROM.
     * A VxWorks boot ROM begins
     *    MOV	R0,#BOOT_COLD
     *    B	...
     *    DCB	"Copyright"
     * We check the first and third words only. This could be tightened up
     * if required (see romInit.s).
     */

    if (p[0] == 0xE3A00002 && p[2] == 0x79706F43)
	pRom = (FUNCPTR)(ROM_TEXT_ADRS + 4);	/* warm boot address */
    else
	pRom = (FUNCPTR)ROM_TEXT_ADRS;		/* start of ROM */

#ifdef	INCLUDE_MMU
    VM_ENABLE(FALSE);	/* disable the MMU, cache(s) and write-buffer */
#endif	/* INCLUDE_MMU */

#ifdef	INCLUDE_CACHE_SUPPORT
    cacheDisable (INSTRUCTION_CACHE);
#endif	/* INCLUDE_CACHE_SUPPORT */

    (*pRom)(startType);	/* jump to boot ROM */

    return OK;		/* in case we ever continue from ROM monitor */
    }

/****************************************************************************
*
* sysProcNumGet - get the processor number
*
* This routine returns the processor number for the CPU board, which is
* set with sysProcNumSet().
*
* RETURNS: The processor number for the CPU board.
*
* SEE ALSO: sysProcNumSet()
*/

int sysProcNumGet (void)
    {
    return 0;
    }

/****************************************************************************
*
* sysProcNumSet - set the processor number
*
* Set the processor number for the CPU board.  Processor numbers should be
* unique on a single backplane.
*
* NOTE
* By convention, only processor 0 should dual-port its memory.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*/

void sysProcNumSet
    (
    int procNum		/* processor number */
    )
    {
    sysProcNum = procNum;
    }

/******************************************************************************
*
* sysLedsReadWrite - read/write the state of the LEDs on the board
*
* This routine can be used to read and write the state of the four LEDs
* on the board. The current state is ANDed and EORed with the supplied
* parameters. Bits 0..3 control LEDs 0..3. A 1 switches the LED on; a 0 off.
*
* RETURNS: previous state of LEDs.
*/

int sysLedsReadWrite
    (
    int and,
    int eor
    )
    {
    UINT32	 	current, previous;
    int			oldLevel;

    oldLevel = intLock();

    /* read current value from hardware and mask off undefined bits */

    current = *((volatile UINT32 *)LED_LIGHTS) & LED_LEDS_MASK;

    previous = current;
    current = (current & and) ^ eor;

    /* wait for display status to be idle */

    while (*((volatile UINT32 *)LED_ALPHA) & LED_STATUS)
	;

    /* set new value */

    *((volatile UINT32 *)LED_LIGHTS) = current;

    intUnlock (oldLevel);
    return previous;
    }
#include "bspDbg.c"

