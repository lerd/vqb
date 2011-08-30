/* config.h - ARM Integrator configuration header */

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
01a,12aug11,d_l  created.
*/

/*
This module contains the configuration parameters for the ARM VersatilePB BSP.
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
extern "C" {
#endif

/* BSP version/revision identification, before configAll.h */

#define BSP_VER_1_1     1       /* 1.2 is backwards compatible with 1.1 */
#define BSP_VER_1_2     1
#define BSP_VERSION	"2.0"
#define BSP_REV		"/0"	/* 0 for first revision */

#include <configAll.h>

/*
 * STANDALONE_NET must be defined for network debug with
 * standalone vxWorks
 */

#define STANDALONE_NET

/* SYS_MODEL define
*
* NOTE
* This define does not include all of the possible variants, and the
* inclusion of a variant in here does not mean that it is supported.
*
*/
#define SYS_MODEL "ARM VersatilePB - ARM926EJS (ARM)"

/*
 * Support network devices.
 */

#undef  INCLUDE_NETWORK

#define DEFAULT_BOOT_LINE1 \
    "fei(0,0) host:/tor2/target/config/integrator926ejs/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"

#define DEFAULT_BOOT_LINE \
    "sme(0,0) host:/home/lerd/binary/vxWorks " \
    "h=192.168.1.99 e=192.168.1.91:ffffff00 g=192.168.1.1 u=target pw=vxTarget tn=versatilepb"
/* Memory configuration */

#undef	LOCAL_MEM_AUTOSIZE			/* run-time memory sizing */
#define USER_RESERVED_MEM	0		/* see sysMemTop() */

/*
 * Local-to-Bus memory address constants:
 * the local memory address always appears at 0 locally;
 * it is not dual ported.
 */

#undef INTEGRATOR_EARLY_I_CACHE_ENABLE         /* Enable Early I-Cache */
#define INTEGRATOR_CONSERVE_VIRTUAL_SPACE       /* The speeds up boot significantly */
#define LOCAL_MEM_LOCAL_ADRS    0x00000000
#define LOCAL_MEM_BUS_ADRS      0x00000000
#define LOCAL_MEM_SIZE          0x07C00000
#undef  LOCAL_MEM_AUTOSIZE                      /* Enable AUTOSIZE */
#define LOCAL_MEM_END_ADRS	(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE)

/*
 * Boot ROM is an image written into Flash. Part of the Flash can be
 * reserved for boot parameters etc. (see the Flash section below).
 *
 * The following parameters are defined here and in the Makefile.
 * They must be kept synchronized; effectively config.h depends on Makefile.
 * Any changes made here must be made in the Makefile and vice versa.
 *
 * ROM_BASE_ADRS is the base of the Flash ROM/EPROM.
 * ROM_TEXT_ADRS is the entry point of the VxWorks image
 * ROM_SIZE is the size of the part of the Flash ROM/EPROM allocated to
 *		the VxWorks image (block size - size of headers)
 *
 * Two other constants are used:
 * ROM_COPY_SIZE is the size of the part of the ROM to be copied into RAM
 * 		 (e.g. in uncompressed boot ROM)
 * ROM_SIZE_TOTAL is the size of the entire Flash ROM (used in sysPhysMemDesc)
 *
 * The values are given as literals here to make it easier to ensure
 * that they are the same as those in the Makefile.
 */

#define ROM_BASE_ADRS       0x07c00000     /* last 4MB of ram */

#define ROM_TEXT_ADRS       ROM_BASE_ADRS  /* code start addr in ROM */
#define ROM_SIZE            0x00200000     /* size of ROM holding VxWorks*/

#define ROM_COPY_SIZE       ROM_SIZE

#  define RAM_LOW_ADRS		 0x00100000	/* VxWorks image entry point */
#  define RAM_HIGH_ADRS		0x00600000	/* RAM address for ROM boot */

/*
 * Flash/NVRAM memory configuration
 *
 * A block of the Flash memory (FLASH_SIZE bytes at FLASH_ADRS) is
 * reserved for non-volatile storage of data.
 *
 * See also integrator.h
 */

#undef INCLUDE_FLASH

#ifdef INCLUDE_FLASH
#define FLASH_SIZE		0x00020000	/* one 128kbyte block of Flash*/
#define NV_RAM_SIZE		0x100		/* how much we use as NVRAM */
#undef	NV_BOOT_OFFSET
#define NV_BOOT_OFFSET		0		/* bootline at start of NVRAM */
#undef FLASH_NO_OVERLAY				/* read-modify-write can be done for
									 * all of Flash */
#define INCLUDE_FLASH_SIB_FOOTER		/* add a SIB footer to block */
#else	/* INCLUDE_FLASH */
#define NV_RAM_SIZE		NONE

#endif	/* INCLUDE_FLASH */

/* Serial port configuration */

#undef	NUM_TTY
#define NUM_TTY			N_SIO_CHANNELS

#define DEFAULT_BAUD      9600

/*
 * Define SERIAL_DEBUG to enable debugging
 * via the serial ports
 */

#undef SERIAL_DEBUG

#undef INCLUDE_WDB

/* INCLUDE_WDB is defined by default in configAll.h.
 * INCLUDE_BOOT_WDB can be defined to add WDB support to the bootroms.
 */
#if defined(INCLUDE_WDB) || defined (INCLUDE_BOOT_WDB)
      #undef WDB_COMM_TYPE
      #undef WDB_TTY_BAUD
      #undef WDB_TTY_CHANNEL
      #undef WDB_TTY_DEV_NAME

     #ifdef SERIAL_DEBUG
      #define WDB_NO_BAUD_AUTO_CONFIG
      #define WDB_COMM_TYPE       WDB_COMM_SERIAL /* WDB in Serial mode */
      #define WDB_TTY_BAUD        115200        /* Baud rate for WDB Connection */
      #define WDB_TTY_CHANNEL     1           /* COM PORT #2 */
      #define WDB_TTY_DEV_NAME    "/tyCo/1"   /* default TYCODRV_5_2 device name */
     #else /* SERIAL_DEBUG */
      #define WDB_COMM_TYPE       WDB_COMM_END
      #define WDB_TTY_BAUD        DEFAULT_BAUD    /* Baud rate for WDB Connection */
      #define WDB_TTY_CHANNEL     0               /* COM PORT #1 */
      #define WDB_TTY_DEV_NAME    "/tyCo/0"       /* default TYCODRV_5_2 device name */
     #endif /* SERIAL_DEBUG */
#endif /* INCLUDE_WDB || INCLUDE_BOOT_WDB */

/*
 * Cache/MMU configuration
 *
 * Note that when MMU is enabled, cache modes are controlled by
 * the MMU table entries in sysPhysMemDesc[], not the cache mode
 * macros defined here.
 */

/*
 * We use the generic architecture libraries, with caches/MMUs present. A
 * call to sysHwInit0() is needed from within usrInit before
 * cacheLibInit() is called.
 */

#ifndef _ASMLANGUAGE
IMPORT void sysHwInit0 (void);
#endif
#define INCLUDE_SYS_HW_INIT_0
#define SYS_HW_INIT_0()         sysHwInit0 ()

#undef  USER_I_CACHE_MODE
#define USER_I_CACHE_MODE       CACHE_WRITETHROUGH

/* 920T/926E/940T/946ES has to be this. */

#undef  USER_D_CACHE_MODE
#define USER_D_CACHE_MODE       CACHE_COPYBACK

/*
 * All ARM 940T and 926ES BSPs must define a variable sysCacheUncachedAdrs: a
 * pointer to a word that is uncached and is safe to read (i.e. has no
 * side effects).  This is used by the cacheLib code to perform a read
 * (only) to drain the write-buffer. Clearly this address must be present
 * within one of the regions created within sysPhysMemDesc, where it must
 * be marked as non-cacheable. There are many such addresses we could use
 * on the board, but we choose to use an address here that will be
 * mapped in on just about all configurations: a safe address within the
 * interrupt controller: the IRQ Enabled status register. This saves us
 * from having to define a region just for this pointer. This constant
 * defined here is used to initialise sysCacheUncachedAdrs in sysLib.c
 * and is also used by the startup code in sysALib.s and romInit.s in
 * draining the write-buffer.
 */

#define SYS_CACHE_UNCACHED_ADRS               0x10140000

/*
 * Include MMU BASIC and CACHE support for command line and project builds
 */
#define	INCLUDE_MMU_BASIC
#define INCLUDE_BOOT_MMU_BASIC
#define INCLUDE_CACHE_SUPPORT


/*
 * Vector Floating Point Support
 */

#undef INCLUDE_VFP


/*
 * Network driver configuration.
 *
 * De-select unused (default) network drivers selected in configAll.h.
 */

#undef	INCLUDE_ENP		/* include CMC Ethernet interface*/
#undef	INCLUDE_EX		/* include Excelan Ethernet interface */
#undef	INCLUDE_SM_NET		/* include backplane net interface */
#undef	INCLUDE_SM_SEQ_ADDR	/* shared memory network auto address setup */

/* Enhanced Network Driver (END) Support */

#undef INCLUDE_END

#ifdef	INCLUDE_END
#undef INCLUDE_DEC21X40END	/* include PCI-based DEC 21X4X END Ethernet */
#define INCLUDE_FEI82557END	/* include PCI-based Intel END Ethernet */


#ifdef INCLUDE_DEC21X40END
#define INCLUDE_MIILIB
#endif

#endif  /* INCLUDE_END */


/* PCI configuration */

#undef INCLUDE_PCI


/*
 * Interrupt mode - interrupts can be in either preemptive or non-preemptive
 * mode.  For non-preemptive mode, change INT_MODE to INT_NON_PREEMPT_MODEL
 */

#define INT_MODE	INT_PREEMPT_MODEL


/*
 * miscellaneous definitions
 * Note: ISR_STACK_SIZE is defined here rather than in ../all/configAll.h
 * (as is more usual) because the stack size depends on the interrupt
 * structure of the BSP.
 */

#define ISR_STACK_SIZE	0x2000	/* size of ISR stack, in bytes */


/* Optional timestamp support */

#undef	INCLUDE_TIMESTAMP	/* define to include timestamp driver */


/* Optional TrueFFS support */

#undef	INCLUDE_TFFS		/* define to include TrueFFS driver */

#ifdef INCLUDE_TFFS
#define INCLUDE_SHOW_ROUTINES
#endif /* INCLUDE_TFFS */

/* ------- new features independent of layer ------- */

#if 0
#define INCLUDE_SIO_POLL
#define SIO_POLL_CONSOLE	0	/* 0 = PrimeCell UART1
					 * 1 = PrimeCell UART2
					 */
#endif

#include "versatilepb.h"

#undef INCLUDE_WINDML          /* define to include windML support */


#ifdef __cplusplus
}
#endif
#endif  /* INCconfigh */

#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif
