/* sysEnd.c - System Enhanced network interface support library */

/* Copyright (c) 1999-2002, 2004, 2007 Wind River Systems, Inc. */

/*
modification history
--------------------
01m,26sep07,mdo  Clear out apigen errors/warnings
01l,16aug04,scm  correct diab warnings...
01k,16jul02,m_h  dc driver load string changes (78869)
01j,25apr02,to   fix PCI to CPU offset address
01i,03dec01,rec  fix compiler warnings
01h,09jul01,jpd  fixed compilation warning.
01g,09apr01,rec  fix 559 initialization problem
01f,22jan01,jmb  add support for Intel InBusiness 10/100 PCI Network Adapter
01e,20nov00,jpd  added Intel Ethernet driver (FEI) support; added use of
		 DEC_USR_SF to do 100 Mbits/s; modified DEC driver support.
01d,17feb00,jpd  minor comments corrections.
01c,07feb00,jpd  tidied.
01b,07dec99,pr   support for PCI/DEC21x4x
01a,10nov99,ajb  created based on version 01a from pid7t.
*/

/*
DESCRIPTION
This file contains the board-specific routines for Ethernet adapter
initialization of Intel (formerly DEC) 21040, 21140 and 21143-based
adapters and Intel Pro 100+ based adapters.

NOTE
At the time of writing, this module has only been tested with the following
Ethernet cards:
    21040 controller: Digital DE435.
    21140 controller: D-Link DFE-500TX
                      Kingston KNE-100TX
    21143 controller: Intel (formerly DEC) EB143 evaluation card.
                      Kingston KNE-100TX
                      Longshine 8038 TXD
    82559 controller: Intel PRO/100+ Management Adapter
                      Intel InBusiness 10/100 PCI Network Adapter

Refer to the BSP reference entry for any eventual limitations or
problems related to the BSP.

INCLUDE FILES: drv/end/dec21x40End.h  drv/end/fei82557End.h


SEE ALSO: ifLib,
\tb "Digital Semiconductor 21143 PCI/CardBus Fast Ethernet LAN Controller,"
\tb "Digital Semiconductor 21143 10/100Base-TX Evaluation Board User's Guide,"
\tb "Intel 10/100 MBit Ethernet Family Software technical Reference Manual."
*/

#include <vxWorks.h>
#include "config.h"
#include <cacheLib.h>

#include <stdio.h>

#ifdef INCLUDE_END

#include <drv/end/dec21x40End.h>
#include <drv/end/fei82557End.h>


/* defines */

#undef  SYS_FEI_DEBUG

#define CSR_BASE_MSK	0x7f		/* Mask Base Address Register */
#define	END_LD_STR_SIZE	80

/* PCI memory base address register configuration mode */

#define FORCE		0x00	/* overwrite membase address register */
#define AUTO		0x01	/* read membase address register */

/*
 * Need to use automatic configuration mode using the resource assigning in
 * pciAssignResources().
 */

#define PCI_REG_MOD	AUTO	/* define the register configuration mode */

#define TYPE_ALLOC	1000

/*
 * DEC cards in range 1 -> 1000
 */

#define DEC_START	1

/* DEC 21X4X 10/100Base-TX Board type */

#define	EB143		DEC_START + 1	/* DEC 21143 10/100Base-TX */
#define	DC140		DEC_START + 2	/* DEC 21140 10/100Base-TX */
#define	DC040		DEC_START + 3	/* DEC 21040 10/100Base-TX */

/* Untested types are > 100 */

#define LC82C168	DEC_START + 104	/* Lite-On PNIC */
#define MX98713		DEC_START + 105	/* Macronix 98713 PMAC */
#define MX98715		DEC_START + 106	/* Macronix 98715 PMAC */
#define AX88140		DEC_START + 107	/* ASIX AX88140 */
#define PNIC2		DEC_START + 108	/* Lite-On PNIC-II */
#define COMET		DEC_START + 109	/* Comet family */
#define COMPEX9881	DEC_START + 110	/* Compex 9881 */
#define I21145		DEC_START + 111	/* Intel 21145 */

/*
 * FEI cards in range 1001 -> 2000
 */

#define FEI_START	TYPE_ALLOC + 1

#define PRO100B		FEI_START	/* Intel EtherExpress PRO-100B PCI */
#define INBUSINESS	FEI_START + 1	/* Intel InBusiness 10/100 PCI */

#define XX82559ER 	FEI_START + 100	/* Arbitrary card with 82559ER */

/*
 * DEC driver user flags
 *
 * Use of the SF (Store and Forward) bit is necessary at the time of
 * writing on current boards to support 100 Mbits/s operation, though
 * it is not necessary for 10 Mbits/s.
 */

#define DEC_USR_FLAGS_143 	(DEC_USR_21143 | DEC_USR_SF)
#define DEC_USR_FLAGS_140 	(DEC_USR_BAR_RX	| \
                                 DEC_USR_RML	| \
                                 DEC_USR_CAL_08 | \
				 DEC_USR_PBL_04 | \
				 DEC_USR_21140	| \
				 DEC_USR_SF)
#define DEC_USR_FLAGS_040 	(DEC_USR_BAR_RX | \
                                 DEC_USR_CAL_08 | \
				 DEC_USR_PBL_04)

/* These are untested */

#define PNIC_USR_FLAGS	  	(DEC_USR_21143)
#define PNIC2_USR_FLAGS	  	(DEC_USR_PNIC2)
#define MX98713_USR_FLAGS	(0)
#define MX98715_USR_FLAGS	(0)
#define AX88140_USR_FLAGS	(0)
#define COMET_USR_FLAGS		(0)
#define COMET_USR_FLAGS		(0)
#define COMET_USR_FLAGS		(0)
#define COMPEX9881_USR_FLAGS	(0)
#define I21145_USR_FLAGS	(0)

/* END table information */

#define DEC_BUFF_LOAN   1                       /* enable buffer loaning */

/* DEC 21X4X PCI/CardBus specific definitions */

#define DEC21X4X_PCI_VENDOR_ID		0x1011	/* PCI vendor ID */
#define DEC21143_PCI_DEVICE_ID		0x0019	/* PCI device ID */
#define DEC21140_PCI_DEVICE_ID		0x0009	/* PCI device ID */
#define DEC21040_PCI_DEVICE_ID		0x0002	/* PCI device ID */

#define PNIC_PCI_VENDOR_ID		0x11AD	/* PCI vendor ID */
#define PNIC_PCI_DEVICE_ID		0x0002
#define PNIC2_PCI_DEVICE_ID		0xc115

#define MACRONIX_PCI_VENDOR_ID		0x10d9	/* PCI vendor ID */
#define MX98713_PCI_DEVICE_ID		0x0512
#define MX98715_PCI_DEVICE_ID		0x0531

#define ASIX_PCI_VENDOR_ID		0x125B	/* PCI vendor ID */
#define AX88140_PCI_DEVICE_ID		0x1400

#define COMET_PCI_VENDOR_ID			0x1317	/* PCI vendor ID */
#define COMET1_PCI_DEVICE_ID		0x0981
#define COMET2_PCI_DEVICE_ID		0x0985
#define COMET3_PCI_DEVICE_ID		0x1985

#define COMPEX_PCI_VENDOR_ID		0x11F6	/* PCI vendor ID */
#define COMPEX9881_PCI_DEVICE_ID	0x9881

#define INTEL_PCI_VENDOR_ID		0x8086	/* PCI vendor ID */
#define I21145_PCI_DEVICE_ID		0x0039

#define DAVICOM_PCI_VENDOR_ID		0x1282	/* PCI vendor ID */
#define DAVICOM9100_PCI_DEVICE_ID	0x9100
#define DAVICOM9102_PCI_DEVICE_ID	0x9102

#define ACCTON_PCI_VENDOR_ID		0x1113	/* PCI vendor ID */
#define EN1217_PCI_DEVICE_ID		0x1217

#define	BOARD_TYPE_NB			(NELEMENTS(boardResources))


#ifdef INCLUDE_FEI82557END

/* EEPROM control bits */

#define EE_SK           0x01            /* shift clock */
#define EE_CS           0x02            /* chip select */
#define EE_DI           0x04            /* chip data in */
#define EE_DO           0x08            /* chip data out */

/* EEPROM opcode */

#define EE_CMD_WRITE    0x05            /* WRITE opcode, 101 */
#define EE_CMD_READ     0x06            /* READ  opcode, 110 */
#define EE_CMD_ERASE    0x07            /* ERASE opcode, 111 */

/* EEPROM misc. defines */

#define EE_CMD_BITS     3               /* number of opcode bits */
#define EE_ADDR_BITS    6               /* number of address bits */
#define EE_DATA_BITS    16              /* number of data bits */
#define EE_SIZE         0x40            /* 0x40 words */
#define EE_CHECKSUM     0xbaba          /* checksum */

/* PC compatibility macros */

#define sysOutWord(addr,data)   (*((UINT16 *) (addr)) = ((UINT16) (data)))
#define sysInWord(addr)         (*((UINT16 *) (addr)))
#define sysOutLong(addr,data)   (*((UINT32 *) (addr)) = ((UINT32) (data)))
#define sysInLong(addr)         (*((UINT32 *) (addr)))

#define FEI_MEMSIZE0            0x00001000
#define FEI_INIT_STATE_MASK     (-1)    /* unused */
#define FEI_INIT_STATE          (-1)    /* unused */
#define UNKNOWN                 (-1)

/* 82557/9 PCI specific definitions */

/* Intel PRO-100B PCI specific definitions */
#ifndef PRO100B_PCI_VENDOR_ID
#define PRO100B_PCI_VENDOR_ID   0x8086  /* PCI vendor ID */
#define PRO100B_PCI_DEVICE_ID   0x1229  /* PCI device ID */
#endif    /* PRO100B_PCI_VENDOR_ID */

/* Intel InBusiness 10/100 PCI specific definitions */
#ifndef INBUSINESS_PCI_VENDOR_ID
#define INBUSINESS_PCI_VENDOR_ID   PRO100B_PCI_VENDOR_ID  /* PCI vendor ID */
#define INBUSINESS_PCI_DEVICE_ID   0x1030                 /* PCI device ID */
#endif    /* INBUSINESS_PCI_VENDOR_ID */

#endif /* INCLUDE_FEI82557END */

/* typedefs */

typedef struct pciResource              /* PCI_RESOURCES */
    {
    UINT32      iobaseCsr;              /* Base Address Register 0 */
    UINT32      membaseCsr;             /* Base Address Register 1 */
    char        irq;                    /* Interrupt Request Level */
    UINT32	irqvec;			/* Interrupt Request vector */
    UINT32      configType;             /* type of configuration */
    void *	buf;			/* any allocated buffer space */
    UINT32	cpuToPciOffset;		/* Any offset from CPU to PCI address */
    } PCI_RESOURCES;

typedef struct boardResource		/* BOARD_RESOURCES */
    {
    UINT32	type;			/* type of the board */
    UINT32	vendorId;		/* Vendor ID */
    UINT32	deviceId;		/* Device ID */
    UINT32	decUsrFlags;		/* DEC driver user flags */
    } BOARD_RESOURCES;

#ifdef INCLUDE_FEI82557END

typedef struct feiResource              /* FEI_RESOURCE */
    {
    UINT32      membaseCsr;             /* Base Address Register 0 */
    UINT32      iobaseCsr;              /* Base Address Register 1 */
    UINT32      membaseFlash;           /* Base Address Register 2 */
    char        irq;                    /* Interrupt Request Level */
    UINT32      configType;             /* type of configuration - unused */
    UINT32      boardType;              /* type of LAN board this unit is */
    UINT16      eeprom[0x40];           /* Ethernet Address of this unit */
    INT32       timeout;                /* timeout for the self-test */
    INT32	str[2];			/* storage for the self-test result */
    volatile INT32 * pResults;		/* pointer to the self-test result */
    UINT        memLength;              /* required memory size */
    UINT        initialStateMask;       /* mask parameter to vmStateSet */
    UINT        initialState;           /* state parameter to vmStateSet */
    } FEI_RESOURCE;

#endif


/* locals */

/*
 * This array defines the board-specific PCI resources, the base address
 * register configuration mode and the Ethernet adapter type. It's indexed
 * using the device number returned from pciFindDevice().
 *
 * The mode is set as AUTO so this will be erased by the configuration read
 * from the card that is effectively set by pciAssignResources(). See
 * sysLanPciInit() for this.
 */

LOCAL PCI_RESOURCES pciResources [INTEGRATOR_MAX_END_DEVS] =
    {
    {PCI_IO_ADR0, PCI_MEM_ADR0, PCI_INT_LVL0, PCI_INT_VEC0, PCI_REG_MOD, 0, 0},
    {PCI_IO_ADR1, PCI_MEM_ADR1, PCI_INT_LVL1, PCI_INT_VEC1, PCI_REG_MOD, 0, 0},
    {PCI_IO_ADR2, PCI_MEM_ADR2, PCI_INT_LVL2, PCI_INT_VEC2, PCI_REG_MOD, 0, 0}
    };

/*
 * This array defines board-specific vendor and device ids, flags to pass to
 * the drive load routine and the function used to select the media.
 */

LOCAL BOARD_RESOURCES boardResources [] =
    {
#ifdef INCLUDE_DEC21X40END

    {EB143, DEC21X4X_PCI_VENDOR_ID, DEC21143_PCI_DEVICE_ID, DEC_USR_FLAGS_143},
    {DC140, DEC21X4X_PCI_VENDOR_ID, DEC21140_PCI_DEVICE_ID, DEC_USR_FLAGS_140},
    {DC040, DEC21X4X_PCI_VENDOR_ID, DEC21040_PCI_DEVICE_ID, DEC_USR_FLAGS_040},

#ifdef INCLUDE_DEC_UNTESTED

    /* Untested cards */

    {LC82C168, PNIC_PCI_VENDOR_ID, PNIC_PCI_DEVICE_ID, PNIC_USR_FLAGS},
    {PNIC2, PNIC_PCI_VENDOR_ID, PNIC2_PCI_DEVICE_ID, PNIC2_USR_FLAGS},
    {MX98713, MACRONIX_PCI_VENDOR_ID, MX98713_PCI_DEVICE_ID, MX98713_USR_FLAGS},
    {MX98715, MACRONIX_PCI_VENDOR_ID, MX98715_PCI_DEVICE_ID, MX98715_USR_FLAGS},
    {AX88140, ASIX_PCI_VENDOR_ID, AX88140_PCI_DEVICE_ID, AX88140_USR_FLAGS},
    {COMET, COMET_PCI_VENDOR_ID, COMET1_PCI_DEVICE_ID, COMET_USR_FLAGS},
    {COMET, COMET_PCI_VENDOR_ID, COMET2_PCI_DEVICE_ID, COMET_USR_FLAGS},
    {COMET, COMET_PCI_VENDOR_ID, COMET3_PCI_DEVICE_ID, COMET_USR_FLAGS},
    {COMPEX9881, COMPEX_PCI_VENDOR_ID, COMPEX9881_PCI_DEVICE_ID,\
     COMPEX9881_USR_FLAGS},
    {I21145, INTEL_PCI_VENDOR_ID, I21145_PCI_DEVICE_ID, I21145_USR_FLAGS},
    {DC140, DAVICOM_PCI_VENDOR_ID, DAVICOM9100_PCI_DEVICE_ID,\
     DEC_USR_FLAGS_140},
    {DC140, DAVICOM_PCI_VENDOR_ID, DAVICOM9102_PCI_DEVICE_ID,\
     DEC_USR_FLAGS_140},
    {MX98715, ACCTON_PCI_VENDOR_ID, EN1217_PCI_DEVICE_ID, MX98715_USR_FLAGS},

#endif /* INCLUDE_DEC_UNTESTED */

#endif /* INCLUDE_DEC21X40END */

#ifdef INCLUDE_FEI82557END

    {PRO100B, PRO100B_PCI_VENDOR_ID, PRO100B_PCI_DEVICE_ID, 0},
    {INBUSINESS, INBUSINESS_PCI_VENDOR_ID, INBUSINESS_PCI_DEVICE_ID, 0},

#ifdef INCLUDE_FEI_UNTESTED
    {XX82559ER, PRO100B_PCI_VENDOR_ID, I82559ER_PCI_DEVICE_ID, 0},
#endif

#endif /* INCLUDE_FEI82557END */
    };

/* END load strings */

LOCAL char	endLoadStr[INTEGRATOR_MAX_END_DEVS][END_LD_STR_SIZE];

/* Index of devices */

LOCAL int       currentEndDevice        = 0;

#ifdef INTEGRATOR_ENET_FIXED_BUF_ADRS
LOCAL UINT32 sysEnetBufAdrs[] =
    {
    INTEGRATOR_ENET_PRIMARY_BUF_ADRS,
    INTEGRATOR_ENET_SECONDARY_BUF_ADRS,
    INTEGRATOR_ENET_TERTIARY_BUF_ADRS
    };
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

#ifdef INCLUDE_FEI82557END

LOCAL FEI_RESOURCE feiResources [INTEGRATOR_MAX_END_DEVS] =
    {
    {(UINT32)UNKNOWN, (UINT32)UNKNOWN, (UINT32)UNKNOWN, (char)UNKNOWN,
     (UINT32)UNKNOWN, (UINT32)UNKNOWN, {(UINT16)UNKNOWN}, UNKNOWN,
     {UNKNOWN}, (INT32 *)NULL, (UINT)FEI_MEMSIZE0, (UINT)FEI_INIT_STATE_MASK,
     (UINT)FEI_INIT_STATE},

    {(UINT32)UNKNOWN, (UINT32)UNKNOWN, (UINT32)UNKNOWN, (char)UNKNOWN,
     (UINT32)UNKNOWN, (UINT32)UNKNOWN, {(UINT16)UNKNOWN}, UNKNOWN,
     {UNKNOWN}, (INT32 *)NULL, (UINT)FEI_MEMSIZE0, (UINT)FEI_INIT_STATE_MASK,
     (UINT)FEI_INIT_STATE},

    {(UINT32)UNKNOWN, (UINT32)UNKNOWN, (UINT32)UNKNOWN, (char)UNKNOWN,
     (UINT32)UNKNOWN, (UINT32)UNKNOWN, {(UINT16)UNKNOWN}, UNKNOWN,
     {UNKNOWN}, (INT32 *)NULL, (UINT)FEI_MEMSIZE0, (UINT)FEI_INIT_STATE_MASK,
     (UINT)FEI_INIT_STATE}
    };

LOCAL const char * phys[] =
    {
    "None", "i82553-A/B", "i82553-C", "i82503",
    "DP83840", "80c240", "80c24", "i82555",
    "unknown-8", "unknown-9", "DP83840A", "unknown-11",
    "unknown-12", "unknown-13", "unknown-14", "unknown-15"
    };

enum phy_chips
    {
    NonSuchPhy=0, I82553AB, I82553C, I82503,
    DP83840, S80C240, S80C24, I82555, DP83840A=10, UndefinedPhy
    };

LOCAL const char *connectors[] = {" RJ45", " BNC", " AUI", " MII"};

#endif /* INCLUDE_FEI82557END */

/* imports */

/* End device table - should be INTEGRATOR_MAX_END_DEVS+1 entries */

IMPORT END_TBL_ENTRY	endDevTbl[];

#ifdef INCLUDE_DEC21X40END
/* DEC specific imports */

IMPORT END_OBJ * 	dec21x40EndLoad (char *, void *);
IMPORT int		dec21x40Loops;
#endif

#ifdef INCLUDE_FEI82557END

/* FEI specific imports */

IMPORT FUNCPTR  feiEndIntConnect;
IMPORT FUNCPTR  feiEndIntDisconnect;
IMPORT END_OBJ* fei82557EndLoad (char *, void *);
IMPORT void	sysFlashBoardDelay (void);

/* forward declarations */

LOCAL UINT16    sys557eepromRead (int unit, int location);
LOCAL UINT32    sys557mdioRead   (int unit, int phyId, int location);
LOCAL UINT32    sys557mdioWrite  (int unit, int phyId, int location, int value);
LOCAL int       sys557IntEnable  (int unit);
LOCAL int       sys557IntDisable (int unit);
LOCAL int       sys557IntAck     (int unit);

#endif /* INCLUDE_FEI82557END */

/*******************************************************************************
*
* sysLanPciInit - prepare LAN adapter for initialization
*
* This routine find out the PCI device, and map its memory and I/O address.
* It will understand both DEC21x4x and FEI type cards.
*
* RETURNS: OK or ERROR
*/

STATUS sysLanPciInit (void)
    {
    PCI_RESOURCES * pRsrc;	/* dec resource */
    UINT32	    pciBus;	/* PCI Bus number */
    UINT32	    pciDevice;	/* PCI Device number */
    UINT32	    pciFunc;	/* PCI Function number */
    unsigned int    ix;		/* counter */
    int		    iy;		/* counter */
    int		    unit = 0;	/* unit numbers */
    UINT32	    boardType = (UINT32) NONE; /* board type detected */

#ifdef INCLUDE_FEI82557END

    FEI_RESOURCE *      pFeiRes;        /* FEI specific info */

#endif

#ifdef INCLUDE_DEC21X40END

    /*
     * Set the loops to be less per nano-second since this board is slower,
     * than say, EBSA-285.
     */

    dec21x40Loops = 1;

#endif

    /*
     * The following code tries to automatically detect and configure
     * all instances of supported Ethernet cards.
     */

    for (ix = 0; ix < BOARD_TYPE_NB; ix++)
	{
	for (iy = 0; iy < INTEGRATOR_MAX_END_DEVS; iy++)
	    {
	    if (pciFindDevice ((int) boardResources[ix].vendorId,
			       (int) boardResources[ix].deviceId,
			       iy, (int *)&pciBus, (int *)(&pciDevice),
			       (int *)(&pciFunc)) != OK)
		{
		break;	/* skip to next vendor/product pair */
		}

	    /* board detected */

	    boardType = boardResources[ix].type;

	    /*
	     * Update the END device table
	     *
	     * pciDevice for PCI cards plugged in is in the range 10 to 12.
	     */

	    pRsrc = &(pciResources[pciDevice - 10]);

	    if (pRsrc->configType == AUTO)
		{
		/* Assign resources (interrupt, I/O) */

		pciAssignResources (pciBus, pciDevice, pciFunc);

		/* get memory base address and I/O base address */

#ifdef INCLUDE_DEC21X40END

		if ((boardType >= DEC_START) &&
		    (boardType < (DEC_START + TYPE_ALLOC)))
		    {

		    pciConfigInLong ((int)pciBus, (int)pciDevice, (int)pciFunc,
				     PCI_CFG_BASE_ADDRESS_0,
				     (int *)&pRsrc->iobaseCsr);
		    pciConfigInLong ((int)pciBus, (int)pciDevice, (int)pciFunc,
				     PCI_CFG_BASE_ADDRESS_1,
				     (int *)&pRsrc->membaseCsr);
		    pciConfigInByte ((int)pciBus, (int)pciDevice, (int)pciFunc,
				     PCI_CFG_DEV_INT_LINE, (char *)&pRsrc->irq);

		    pRsrc->membaseCsr &= ~CSR_BASE_MSK;
		    pRsrc->iobaseCsr &= ~CSR_BASE_MSK;

		    /* overwrite the resource table with read value */

		    pRsrc->irqvec       = IVEC_TO_INUM(pRsrc->irq);
		    }

#endif /* INCLUDE_DEC21X40END */

#ifdef INCLUDE_FEI82557END

		if ((boardType >= FEI_START) &&
 		    (boardType < (FEI_START + TYPE_ALLOC)))
                    {
                    pFeiRes = &feiResources [unit];

                    pciConfigInLong ((int)pciBus, (int)pciDevice, (int)pciFunc,
                                     PCI_CFG_BASE_ADDRESS_0,
                                     (int *)&pFeiRes->membaseCsr);

                    /* Convert to CPU address */

                    pFeiRes->membaseCsr += PCI2CPU_MEM_OFFSET;

                    pciConfigInLong ((int)pciBus, (int)pciDevice, (int)pciFunc,
                                     PCI_CFG_BASE_ADDRESS_1,
                                     (int *)&pFeiRes->iobaseCsr);

                    pFeiRes->iobaseCsr &= ~PCI_BASE_IO;
                    pFeiRes->iobaseCsr += PCI2CPU_IO_OFFSET;

                    pciConfigInByte (pciBus, pciDevice, pciFunc,
                                     PCI_CFG_DEV_INT_LINE,
                                     &pFeiRes->irq);
                    }
#endif /* INCLUDE_FEI82557END */
		}
	    else		/* Force PCI configuration */
		{
#ifdef INCLUDE_DEC21X40END

		if ((boardType >= DEC_START) &&
		    (boardType < (DEC_START + TYPE_ALLOC)))
		    {

		    /* set memory base address and I/O base address */

		    pciConfigOutLong (pciBus, pciDevice, pciFunc,
				      PCI_CFG_BASE_ADDRESS_0,
				      pRsrc->iobaseCsr | PCI_BASE_IO);
		    pciConfigOutLong (pciBus, pciDevice, pciFunc,
				      PCI_CFG_BASE_ADDRESS_1,
				      pRsrc->membaseCsr);
		    pciConfigOutByte (pciBus, pciDevice, pciFunc,
				      PCI_CFG_DEV_INT_LINE, pRsrc->irq);
		    }

#endif /* INCLUDE_DEC21X40END */

#ifdef INCLUDE_FEI82557END
		if ((boardType >= FEI_START) &&
 		    (boardType < (FEI_START + TYPE_ALLOC)))
                    {
                    pFeiRes = &feiResources [unit];

                    pciConfigOutLong (pciBus, pciDevice, pciFunc,
                                      PCI_CFG_BASE_ADDRESS_0,
                                      pRsrc->membaseCsr);
                    pFeiRes->membaseCsr = pRsrc->membaseCsr + PCI2CPU_MEM_OFFSET;
                    pciConfigOutLong (pciBus, pciDevice, pciFunc,
                                      PCI_CFG_BASE_ADDRESS_1,
                                      pRsrc->iobaseCsr | PCI_BASE_IO);
                    pFeiRes->iobaseCsr = pRsrc->iobaseCsr + PCI2CPU_IO_OFFSET;
                    pciConfigOutByte (pciBus, pciDevice, pciFunc,
                                      PCI_CFG_DEV_INT_LINE, pRsrc->irq);
                    pFeiRes->irq = pRsrc->irq;
		    }
#endif /* INCLUDE_FEI82557END */

		}

	    /*
	     * Update the END device table & dynamically create the load
	     * string we need for this device
	     */

#ifdef INCLUDE_DEC21X40END
	    if ((boardType >= DEC_START) &&
		(boardType < (DEC_START + TYPE_ALLOC)))
		{
		/*
		 * END load string format:
		 *  "<deviceAddr>:<pciAddr>:<iVec>:<iLevel>:<numRds>:<numTds>:\
		 *  <memBase>:<memSize>:<userFlags>:<phyAddr>:<pPhyTbl>:\
		 *  <phyFlags>:<offset>"
		 */
#ifdef INTEGRATOR_ENET_FIXED_BUF_ADRS
		pRsrc->buf = (void *)sysEnetBufAdrs [unit];
		pRsrc->cpuToPciOffset = 0;
#else /* INTEGRATOR_ENET_FIXED_BUF_ADRS */
		pRsrc->cpuToPciOffset = PCI2DRAM_BASE_ADRS;

#ifdef INTEGRATOR_ENET_CHECK_BUFFERS
		if (pRsrc->buf = cacheDmaMalloc (INTEGRATOR_DEC_BUF_SIZE),
		    pRsrc->buf == NULL)
		    {
		    /* cannot log msg at this point in initialization timeline*/

		    return ERROR;
		    }

		if (((UINT32)pRsrc->buf) < INTEGRATOR_HDR_SSRAM_SIZE)
		    {
		    /* cannot log msg at this point in initialization timeline*/

		    return ERROR;
		    }
#else /* INTEGRATOR_ENET_CHECK_BUFFERS */
		pRsrc->buf = (void *)NONE;
#endif /* INTEGRATOR_ENET_CHECK_BUFFERS */
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

		sprintf (endLoadStr[currentEndDevice],
			 "%#x:%#x:%#x:%#x:-1:-1:%#x:%#x:%#x:%#x:%#x:%#x:2:-1:0:",
                         /* Note: unit is prepended by the mux driver */
			 pRsrc->iobaseCsr + PCI2CPU_IO_OFFSET,  /*devAdrs*/
			 pRsrc->cpuToPciOffset,                 /*pciMemBase*/
			 pRsrc->irq, pRsrc->irqvec,             /*ivec,ilevel*/
                         /* numRds=default, numTds=default */
			 (UINT32)pRsrc->buf,                   /*memBase*/
			 INTEGRATOR_DEC_BUF_SIZE,              /*memSize*/
			 boardResources[ix].decUsrFlags,        /*usrFlags*/
                         1, 0,                       /* phyAddr,pMiiPhyTbl,*/
                         DEC_USR_MII_10MB  |
                         DEC_USR_MII_HD    | DEC_USR_MII_100MB |
                         DEC_USR_MII_FD    | DEC_USR_MII_BUS_MON
                         );

		endDevTbl[currentEndDevice].unit = unit++;
		endDevTbl[currentEndDevice].endLoadFunc = dec21x40EndLoad;
		endDevTbl[currentEndDevice].endLoadString =
						endLoadStr[currentEndDevice];
		endDevTbl[currentEndDevice].endLoan = DEC_BUFF_LOAN;

		currentEndDevice++;

		/* enable mapped memory and I/O addresses */

		pciConfigOutWord (pciBus, pciDevice, pciFunc, PCI_CFG_COMMAND,
				  PCI_CMD_IO_ENABLE | PCI_CMD_MASTER_ENABLE);

		/* disable sleep mode */

		pciConfigOutByte (pciBus, pciDevice, pciFunc, PCI_CFG_MODE,
				  SLEEP_MODE_DIS);
	        }
#endif /* INCLUDE_DEC21X40END */

#ifdef INCLUDE_FEI82557END
            if ((boardType >= FEI_START) &&
                (boardType < (FEI_START + TYPE_ALLOC)))
                {
#ifdef INTEGRATOR_ENET_FIXED_BUF_ADRS
		pRsrc->buf = (void *)sysEnetBufAdrs [unit];
#else  /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

#ifdef INTEGRATOR_ENET_CHECK_BUFFERS
		if (pRsrc->buf = cacheDmaMalloc (INTEGRATOR_FEI_BUF_SIZE),
		    pRsrc->buf == NULL)
		    {
		    /* cannot log msg at this point in initialization timeline*/

		    return ERROR;
		    }

		if (((UINT32)pRsrc->buf) < INTEGRATOR_HDR_SSRAM_SIZE)
		    {
		    /* cannot log msg at this point in initialization timeline*/

		    return ERROR;
		    }

#else /* INTEGRATOR_ENET_CHECK_BUFFERS */
		pRsrc->buf = (void *)NONE;
#endif /* INTEGRATOR_ENET_CHECK_BUFFERS */
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

                sprintf (endLoadStr[currentEndDevice],
                        "0x%08X:0x%08X:0x%X:0x%X:0x00:2",
			(UINT32)pRsrc->buf, INTEGRATOR_FEI_BUF_SIZE,
			INTEGRATOR_FEI_NUM_CFDS, INTEGRATOR_FEI_NUM_RFDS);

                endDevTbl[currentEndDevice].unit = unit++;
                endDevTbl[currentEndDevice].endLoadFunc = fei82557EndLoad;
                endDevTbl[currentEndDevice].endLoadString =
                                                endLoadStr[currentEndDevice];
                endDevTbl[currentEndDevice].endLoan = 1;
                currentEndDevice++;

                /* enable mapped I/O addresses */

                pciConfigOutWord (pciBus, pciDevice, pciFunc,
                          PCI_CFG_COMMAND, PCI_CMD_IO_ENABLE |
                          PCI_CMD_MASTER_ENABLE);
		}
#endif /* INCLUDE_FEI82557END */

	    /* Configured the maximum number of adaptors? */

	    if (currentEndDevice == INTEGRATOR_MAX_END_DEVS)
		{
		return OK;
		}
	    }
	}

    if ((unit == 0) || (pciDevice > PCI_MAX_DEV))
	{
	return ERROR;
	}

    return OK;
    }

#ifdef INCLUDE_DEC21X40END

/*******************************************************************************
*
* sysLanIntEnable - enable dec21X4X interrupts
*
* This routine enables dec21X4X interrupts.  This may involve operations on
* interrupt control hardware.
*
* RETURNS: OK or ERROR for invalid arguments
*/

STATUS sysLanIntEnable
    (
    int	level		/* level number */
    )
    {
    return (intEnable(level));
    }

/*******************************************************************************
*
* sysLanIntDisable - disable dec21X4X interrupts
*
* This routine disables dec21X4X interrupts.  This may involve operations on
* interrupt control hardware.
*
* RETURNS: OK or ERROR for invalid arguments
*/

STATUS sysLanIntDisable
    (
    int	level		/* level number */
    )
    {
    return (intDisable(level));
    }

/*******************************************************************************
*
* sysDec21x40EnetAddrGet - get Ethernet address
*
* This routine provides a target-specific interface for accessing a
* device Ethernet address.
*
* RETURNS: ERROR, always
*/

STATUS sysDec21x40EnetAddrGet
    (
    int		unit,
    char *	enetAdrs
    )
    {
    /*
     * There isn't a target-specific interface for accessing a
     * device Ethernet address.
     */

    return (ERROR);
    }

#endif /* INCLUDE_DEC21X40END */

#ifdef INCLUDE_FEI82557END

#ifndef INTEGRATOR_ENET_FIXED_BUF_ADRS
/*******************************************************************************
*
* sys557CpuToPci - Convert a CPU memory address to a PCI address
*
* Must convert virtual address to a physical address, then convert the
* physical address to a PCI address (PCI2DRAM_BASE_ADRS is the offset of the
* CPU's memory on the PCI bus,
* i.e. PCI address = (CPU physical address + PCI2DRAM_BASE_ADRS)
*
* This routine works only for addresses in SDRAM.
*
* WARNING: No attempt is made to ensure that the address passed into this
* function is valid.
*
* RETURNS: PCI address corresponding to the CPU virtual address provided
*/

UINT32 sys557CpuToPci
    (
    int		unit,		/* ignored */
    UINT32	addr
    )
    {
#ifdef SYS_FEI_DEBUG
    logMsg ("sys557CpuToPci %#x -> %#x\n", addr, addr + PCI2DRAM_BASE_ADRS,
	    0,0,0,0);
#endif

    return (UINT32)(((UINT32)CACHE_DMA_VIRT_TO_PHYS(addr)) + PCI2DRAM_BASE_ADRS);
    }

/*******************************************************************************
*
* sys557PciToCpu - Convert a PCI memory address to a CPU address
*
* This is the reverse of the previous function. Here we convert from a PCI
* address to a physical address in the CPU's address space, then use the
* current translation tables to find the corresponding virtual address.
*
* This routine works only for addresses in SDRAM.
*
* WARNING: No attempt is made to ensure that the address passed into this
* function is valid.
*
* RETURNS: CPU virtual address for specified PCI address
*/

UINT32 sys557PciToCpu
    (
    int		unit,		/* ignored */
    UINT32	addr
    )
    {
#ifdef SYS_FEI_DEBUG
    logMsg ("sys557PciToCpu %#x -> %#x\n", addr, addr - PCI2DRAM_BASE_ADRS,
	    0,0,0,0);
#endif

    return (UINT32)(CACHE_DMA_PHYS_TO_VIRT(addr - PCI2DRAM_BASE_ADRS));
    }
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

/*******************************************************************************
*
* sysDelay - a small delay
*
* This routine provides a brief delay.
*
* RETURNS: N/A
*/

void sysDelay (void)
    {
    volatile UINT32 p;

    /* Read from ROM, a known timing */

    p = *(volatile UINT32 *)ROM_TEXT_ADRS;

    /*
     * Create a little more delay: some arithmetic operations that
     * involve immediate constants that cannot be performed in one
     * instruction.  Clearly it is possible that compiler changes will
     * affect this code and leave this no longer calibrated.
     */

    p &=0xffff;
    p +=0xff11;
    p +=0xff51;

    return;
    }

/*******************************************************************************
*
* sys557Init - prepare LAN adapter for 82557 initialization
*
* This routine is expected to perform any adapter-specific or target-specific
* initialization that must be done prior to initializing the 82557.
*
* The 82557 driver calls this routine from the driver attach routine before
* any other routines in this library.
*
* This routine returns the interrupt level the <pIntLvl> parameter.
*
* RETURNS: OK or ERROR if the adapter could not be prepared for initialization
*/

STATUS sys557Init
    (
    int	unit,			/* unit number */
    FEI_BOARD_INFO * pBoard     /* board information for the end driver */
    )
    {
    volatile FEI_RESOURCE * pReso = &feiResources [unit];
    UINT16	sum          = 0;
    int		ix;
    int		iy;
    UINT16	value;
#ifndef INTEGRATOR_ENET_FIXED_BUF_ADRS
    void *	testbuf = 0;	/* keep compiler quite */
#endif

    /*
     * Locate the 82557 based adapter - PRO100B, INBUSINESS and XXX.
     * Note that since the INBUSINESS adapter is based upon the PRO100B
     * board type, it is initialized and driven like one.
     */


#ifdef SYS_FEI_DEBUG
    printf ("fei%d: I/O %08X membase %08X irq %d\n", unit, pReso->iobaseCsr,
	    pReso->membaseCsr, pReso->irq);
#endif

    if (pReso->boardType == PRO100B)		/* only setup once */
	{
	}
    else
	{
	/* read the configuration in EEPROM */

	for (ix = 0; ix < EE_SIZE; ix++)
	    {
	    value = sys557eepromRead (unit, ix);
	    pReso->eeprom[ix] = value;
	    sum += value;
	    }
	if (sum != EE_CHECKSUM)
	    printf ("fei%d: Invalid EEPROM checksum %#4.4x\n", unit, sum);

	/* DP83840 specific setup */

	if (((pReso->eeprom[6]>>8) & 0x3f) == DP83840)
	    {
	    int reg23 = sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 23);
	    sys557mdioWrite (unit, pReso->eeprom[6] & 0x1f, 23, reg23 | 0x0420);
	    }

	/* perform a system self-test. */

        pReso->timeout = 16000;		/* Timeout for self-test. */

	/*
	 * We require either a cache-line sized buffer in order to be
	 * able to flush the cache, or use a buffer that is marked as
	 * non-cacheable. If the driver is configured to use a defined
	 * area of memory, we assume that it will be marked as
	 * non-cacheable. We will use that (temporarily).
	 */
#ifdef INTEGRATOR_ENET_FIXED_BUF_ADRS

	pReso->pResults = (volatile INT32 *)sysEnetBufAdrs [unit];
#else
	/*
	 * No specific area specified, so we assume that cacheDmaMalloc() will
	 * return a pointer to a suitable area. If the data cache is on,
	 * this will be page-aligned, but if the data cache is off, then we
	 * will just get whatever malloc returns.
	 */

	if (testbuf = cacheDmaMalloc (32), testbuf == 0)
	    {
	    printf("fei%d cacheDmaMalloc failed\n", unit);
	    return ERROR;
	    }
	pReso->pResults = (volatile INT32 *)testbuf;

#ifdef INTEGRATOR_ENET_CHECK_BUFFERS
	if (((UINT32)pReso->pResults) < INTEGRATOR_HDR_SSRAM_SIZE)
	    {
	    printf("fei%d: cacheDmaMalloc() returned address in \
		    header card SSRAM\n", unit);
	    return ERROR;
	    }
#endif /* INTEGRATOR_ENET_CHECK_BUFFERS */
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

	/* The chip requires the results buffer to be 16-byte aligned. */

        pReso->pResults    = (volatile INT32 *)
			     ((((int) pReso->pResults) + 0xf) & ~0xf);

	/* initialize results buffer */

	pReso->pResults[0] = 0;
	pReso->pResults[1] = -1;

	/* Issue the self-test command */

#ifdef INTEGRATOR_ENET_FIXED_BUF_ADRS
	/* If fixed address specified, no address conversion will be required */

	sysOutLong (pReso->iobaseCsr + SCB_PORT, (int)pReso->pResults | 1);
#else /* INTEGRATOR_ENET_FIXED_BUF_ADRS */
	/*
	 * If using cacheDmaMalloc() it will return a "low-alias" address in
	 * SDRAM, and this will need converting to a "high-alias"
	 * address, so it can be accessed from the PCI bus.
	 */

	sysOutLong (pReso->iobaseCsr + SCB_PORT,
		    sys557CpuToPci(unit, (int)pReso->pResults) | 1);
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */

	/* wait for results */

	do
	    {
	    sysDelay();		/* cause a delay of at least an I/O cycle */
	    }
	while ((pReso->pResults[1] == -1) && (--pReso->timeout >= 0));

        pReso->boardType = PRO100B;	/* note that it is now initialized */

        /* Save results so we can refer to them again later */

        pReso->str[0] = pReso->pResults[0];
        pReso->str[1] = pReso->pResults[1];

#ifndef INTEGRATOR_ENET_FIXED_BUF_ADRS
        cacheDmaFree (testbuf);
#endif

        pReso->pResults = pReso->str;

	}
    /* initialize the board information structure */

    pBoard->vector	  = IVEC_TO_INUM(pReso->irq);
    pBoard->baseAddr	  = pReso->iobaseCsr;
    for (ix = 0, iy = 0; ix < 3; ix++)
	{
	pBoard->enetAddr[iy++] = pReso->eeprom[ix] & 0xff;
	pBoard->enetAddr[iy++] = (pReso->eeprom[ix] >> 8) & 0xff;
	}
    pBoard->intEnable	  = sys557IntEnable;
    pBoard->intDisable	  = sys557IntDisable;
    pBoard->intAck	  = sys557IntAck;

    /* install address conversion routines for driver, if appropriate */

#ifndef INTEGRATOR_ENET_FIXED_BUF_ADRS
    pBoard->sysLocalToBus = sys557CpuToPci;
    pBoard->sysBusToLocal = sys557PciToCpu;
#else
    pBoard->sysLocalToBus = NULL;
    pBoard->sysBusToLocal = NULL;
#endif

#ifdef FEI_10MB
    pBoard->phySpeed	  = NULL;
    pBoard->phyDpx	  = NULL;
#endif

    return (OK);
    }

/*******************************************************************************
*
* sys557IntAck - acknowledge an 82557 interrupt
*
* This routine performs any 82557 interrupt acknowledge that may be
* required.  This typically involves an operation to some interrupt
* control hardware.
*
* This routine gets called from the 82557 driver's interrupt handler.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if the interrupt could not be acknowledged
*/

LOCAL STATUS sys557IntAck
    (
    int	unit		/* unit number */
    )
    {
    FEI_RESOURCE * pReso = &feiResources [unit];

    switch (pReso->boardType)
	{
	case PRO100B:		/* handle PRO100B LAN Adapter */
	    /* no addition work necessary for the PRO100B */
	    break;

	default:
	    return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* sys557IntEnable - enable 82557 interrupts
*
* This routine enables 82557 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82557 driver calls this routine throughout normal operation to terminate
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be enabled
*/

LOCAL STATUS sys557IntEnable
    (
    int	unit		/* unit number */
    )
    {
    FEI_RESOURCE * pReso = &feiResources [unit];

    switch (pReso->boardType)
	{
	case PRO100B:		/* handle PRO100B LAN Adapter */
	    intEnable (pReso->irq);
	    break;

	default:
	    return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* sys557IntDisable - disable 82557 interrupts
*
* This routine disables 82557 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82557 driver calls this routine throughout normal operation to enter
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be disabled
*/

LOCAL STATUS sys557IntDisable
    (
    int	unit		/* unit number */
    )
    {
    FEI_RESOURCE * pReso = &feiResources [unit];

    switch (pReso->boardType)
	{
	case PRO100B:		/* handle PRO100B LAN Adapter */
	    intDisable (pReso->irq);
	    break;

	default:
	    return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* sys557eepromRead - read a word from the 82557 EEPROM
*
* This routine reads a word from the 82557 EEPROM
*
* RETURNS: the EEPROM data word read in
*/

LOCAL UINT16 sys557eepromRead
    (
    int	unit,		/* unit number */
    int	location	/* address of word to be read */
    )
    {
    UINT32 iobase = feiResources[unit].iobaseCsr;
    UINT16 retval = 0;
    UINT16 dataval;
    volatile UINT16 dummy;
    int ix;

    sysOutWord (iobase + SCB_EEPROM, EE_CS);	/* enable EEPROM */

    /* write the READ opcode */

    for (ix = EE_CMD_BITS - 1; ix >= 0; ix--)
	{
        dataval = (EE_CMD_READ & (1 << ix)) ? EE_DI : 0;
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval);
        sysDelay ();	/* delay for one I/O READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval | EE_SK);
        sysDelay ();	/* delay for one I/O READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval);
        sysDelay ();	/* delay for one I/O READ cycle */
        }

    /* write the location */

    for (ix = EE_ADDR_BITS - 1; ix >= 0; ix--)
	{
        dataval = (location & (1 << ix)) ? EE_DI : 0;
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval);
        sysDelay ();	/* delay for one I/O READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval | EE_SK);
        sysDelay ();	/* delay for one I/O READ cycle */
        sysOutWord (iobase + SCB_EEPROM, EE_CS | dataval);
        sysDelay ();	/* delay for one I/O READ cycle */
	dummy = sysInWord (iobase + SCB_EEPROM);
        }

    if ((dummy & EE_DO) == 0)		/* dummy read */
	;

    /* read the data */

    for (ix = EE_DATA_BITS - 1; ix >= 0; ix--)
	{
        sysOutWord (iobase + SCB_EEPROM, EE_CS | EE_SK);
        sysDelay ();	/* delay for one I/O READ cycle */
        retval = (retval << 1) |
	         ((sysInWord (iobase + SCB_EEPROM) & EE_DO) ? 1 : 0);
        sysOutWord (iobase + SCB_EEPROM, EE_CS);
        sysDelay ();	/* delay for one I/O READ cycle */
        }

    sysOutWord (iobase + SCB_EEPROM, 0x00);	/* disable EEPROM */

    return (retval);
    }

/*******************************************************************************
*
* sys557mdioRead - read MDIO
*
* This routine reads a word from the MDIO.
*
* RETURNS: read value
*/

LOCAL UINT32 sys557mdioRead
    (
    int	unit,		/* unit number */
    int phyId,		/* PHY ID */
    int location	/* location to read */
    )
    {
    UINT32 iobase = feiResources[unit].iobaseCsr;
    int timeout   = 64*4;	/* <64 usec. to complete, typ 27 ticks */
    int val;

    sysOutLong (iobase + SCB_MDI, 0x08000000 | (location<<16) | (phyId<<21));
    do {
    	sysDelay ();	/* delay for one I/O READ cycle */
    	val = sysInLong (iobase + SCB_MDI);
    	if (--timeout < 0)
    	    printf ("sys557mdioRead() timed out with val = %8.8x.\n", val);
        } while (! (val & 0x10000000));

    return (val & 0xffff);
    }

/*******************************************************************************
*
* sys557mdioWrite - write MDIO
*
* This routine writes a word from the MDIO.
*
* RETURNS: write value
*/

LOCAL UINT32 sys557mdioWrite
    (
    int unit,		/* unit number */
    int phyId,		/* PHY ID */
    int location,	/* location to write */
    int value		/* value to write */
    )
    {
    UINT32 iobase = feiResources[unit].iobaseCsr;
    int timeout   = 64*4;	/* <64 usec. to complete, typ 27 ticks */
    int val;

    sysOutLong (iobase + SCB_MDI,
		0x04000000 | (location<<16) | (phyId<<21) | value);
    do {
    	sysDelay ();	/* delay for one I/O READ cycle */
    	val = sysInLong (iobase + SCB_MDI);
    	if (--timeout < 0)
    	    printf ("sys557mdioWrite() timed out with val = %8.8x.\n", val);
        } while (! (val & 0x10000000));

    return (val & 0xffff);
    }

/*******************************************************************************
*
* sys557Show - shows 82557 configuration
*
* This routine shows the (Intel Pro Express 100) configuration
*
* RETURNS: N/A
*/

void sys557Show
    (
    int	unit		/* unit number */
    )
    {
    FEI_RESOURCE * pReso = &feiResources [unit];
    UINT32 iobase       = pReso->iobaseCsr;
    UCHAR etheraddr[6];
    int ix;
    int iy;

    if (unit > INTEGRATOR_MAX_END_DEVS)
	{
	printf ("Illegal unit number %d\n", unit);
	return;
	}

    if (pReso->boardType != PRO100B)
	{
	printf ("Unit %d not an FEI device\n", unit);
	return;
	}

    for (ix = 0, iy = 0; ix < 3; ix++)
	{
        etheraddr[iy++] = pReso->eeprom[ix];
        etheraddr[iy++] = pReso->eeprom[ix] >> 8;
        }
    printf ("fei%d: Intel EtherExpress Pro 10/100 at %#8x ", unit, iobase);
    for (ix = 0; ix < 5; ix++)
    	printf ("%2.2X:", etheraddr[ix]);
    printf ("%2.2X\n", etheraddr[ix]);

    printf ("CSR mem base address = %x, Flash mem base address = %x\n",
	    pReso->membaseCsr, pReso->membaseFlash);

    if (pReso->eeprom[3] & 0x03)
        printf ("Receiver lock-up bug exists -- enabling work-around.\n");

    printf ("Board assembly %4.4x%2.2x-%3.3d, Physical connectors present:",
	pReso->eeprom[8], pReso->eeprom[9]>>8, pReso->eeprom[9] & 0xff);
    for (ix = 0; ix < 4; ix++)
	if (pReso->eeprom[5] & (1 << ix))
	    printf ("%s", connectors [ix]);

    printf ("\nPrimary interface chip %s PHY #%d.\n",
	phys[(pReso->eeprom[6]>>8)&15], pReso->eeprom[6] & 0x1f);
    if (pReso->eeprom[7] & 0x0700)
	printf ("Secondary interface chip %s.\n",
		phys[(pReso->eeprom[7]>>8)&7]);

#if	FALSE  /* we don't show PHY specific info at this time */
    /* ToDo: Read and set PHY registers through MDIO port. */
    for (ix = 0; ix < 2; ix++)
	printf ("MDIO register %d is %4.4x.\n",
		ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
    for (ix = 5; ix < 7; ix++)
	printf ("MDIO register %d is %4.4x.\n",
		ix, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, ix));
    printf ("MDIO register %d is %4.4x.\n",
	    25, sys557mdioRead (unit, pReso->eeprom[6] & 0x1f, 25));
#endif	/* FALSE */

    if (pReso->timeout < 0)
        {		/* Test optimized out. */
	printf ("Self test failed, status %8.8x:\n"
	        " Failure to initialize the 82557.\n"
	        " Verify that the card is a bus-master capable slot.\n",
	        pReso->pResults[1]);
	}
    else
        {
	printf ("General self-test: %s.\n"
	        " Serial sub-system self-test: %s.\n"
	        " Internal registers self-test: %s.\n"
	        " ROM checksum self-test: %s (%#8.8x).\n",
	        pReso->pResults[1] & 0x1000 ? "failed" : "passed",
	        pReso->pResults[1] & 0x0020 ? "failed" : "passed",
	        pReso->pResults[1] & 0x0008 ? "failed" : "passed",
	        pReso->pResults[1] & 0x0004 ? "failed" : "passed",
	        pReso->pResults[0]);
        }
    }

#endif /* INCLUDE_FEI82557END */

#endif /* INCLUDE_END */
