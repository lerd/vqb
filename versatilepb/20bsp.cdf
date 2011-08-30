/* 20bsp.cdf - BSP component description file */

/*
 * Copyright (c) 2006-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01c,22feb07,jmt  Defect 88750 - Fix problem with network boot device CDF
                 defines
01b,16jan07,j_b  add bootApp support
01a,16jul06,pee  created

*/

Bsp integrator926ejs {
	NAME		board support package
	REQUIRES	INCLUDE_KERNEL
	CPU ARMARCH5
	ENDIAN little
}

Selection SELECT_POWER_MGMT {
        NAME            Power Management Operating Mode
        COUNT           1-1
	CHILDREN	INCLUDE_CPU_SLEEP_MODE_OFF	\
			INCLUDE_CPU_SLEEP_MODE_SHORT	\
			INCLUDE_CPU_SLEEP_MODE_LONG
	DEFAULTS	INCLUDE_CPU_SLEEP_MODE_LONG
}

Component INCLUDE_DEC21X40END {
	NAME		BSP dec21x40 End driver
	_CHILDREN	FOLDER_BSP_CONFIG
	LAYER		4
	REQUIRES	INCLUDE_PCI	\
			INCLUDE_END
	INCLUDE_WHEN	INCLUDE_END
}

Component INCLUDE_FEI82557END {
	NAME		BSP fei82557 End driver
	_CHILDREN	FOLDER_BSP_CONFIG
	LAYER		4
	REQUIRES	INCLUDE_PCI	\
			INCLUDE_END
	INCLUDE_WHEN	INCLUDE_END
}

Component INCLUDE_PCI {
	NAME		BSP PCI support
	LAYER		2
	_CHILDREN	FOLDER_BSP_CONFIG
	REQUIRES	INCLUDE_MEM_MGR_BASIC
	INCLUDE_WHEN	INCLUDE_USB
}

Component INCLUDE_FLASH {
	NAME		BSP specific Flash support
	_CHILDREN	FOLDER_BSP_CONFIG
	LAYER		2
	REQUIRES	INCLUDE_MEM_MGR_BASIC
}

Component INCLUDE_FLASH_SIB_FOOTER {
	NAME		BSP specific SIB footer support on flash
	_CHILDREN	FOLDER_BSP_CONFIG
	LAYER		2
	REQUIRES	INCLUDE_FLASH
	INCLUDE_WHEN	INCLUDE_FLASH
}

Component INCLUDE_TFFS_BSP {
	NAME		BSP specific portion of TrueFFS Flash File System
	LAYER		4
	REQUIRES	INCLUDE_SHOW_ROUTINES
	INCLUDE_WHEN	INCLUDE_TFFS
	_CHILDREN	FOLDER_NOT_VISIBLE
}

Component INCLUDE_SIO_POLL {
	/* always includ when project is created, deselect separately */
	INCLUDE_WHEN	+= INCLUDE_KERNEL
}

/* Network Boot Devices for a BSP
 * The REQUIRES line should be modified for a BSP.
 */
Component       INCLUDE_BOOT_NET_DEVICES {
       REQUIRES    INCLUDE_FEI82557END
}

Parameter RAM_HIGH_ADRS {
   NAME      Bootrom Copy region
   DEFAULT      (INCLUDE_BOOT_APP)::(0x00800000) \
                0x00600000
}

Parameter RAM_LOW_ADRS {
   NAME      Runtime kernel load address
   DEFAULT      (INCLUDE_BOOT_RAM_IMAGE)::(0x00400000) \
                (INCLUDE_BOOT_APP)::(0x00600000) \
                0x00004000
}

