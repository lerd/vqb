/* versatilepb.h - ARM VersatilePb header file */

/*
modification history
--------------------
01b,15sep11,d_l  add SIC definitions.
01a,14aug11,d_l  modified from integrator926ejs.
*/

/*
This file contains I/O address and related constants for the ARM
Integrator board.
*/

#ifndef	INCversatilepbh
#define	INCversatilepbh

#ifdef __cplusplus
extern "C" {
#endif

#undef TARGET_INTEGRATOR

/* Flash definitions */

#define FLASH_BASE		0x34000000	/* Base address of Flash part */
#define FLASH_BLOCK_NUM		254		/* 128 kB block used as NVRAM */
#define FLASH_ADRS		(FLASH_BASE + (FLASH_BLOCK_NUM * 0x20000))
#define INTEGRATOR_FLASH_SIZE 	0x02000000 	/* Total Flash available. */
#define FLASH_WIDTH		4	  	/* Two 16-bit wide parts */
#define FLASH_CHIP_WIDTH        2
#define FLASH_WIDTH_SPECIAL_2			/* see flash28.h */
#define SYS_FLASH_WRITE				/* use enable/disable routines*/

/*
 * It is not necessary to define SYS_FLASH_TYPE as FLASH_28F320 as
 * auto-identification correctly identifies the Flash part.
 */


/* Integrator memory map */

#define INTEGRATOR_PERIPHERAL_BASE	0x10000000
#define INTEGRATOR_PERIPHERAL_SIZE	0x10000000

#define INTEGRATOR_BOOT_ROM_LO          0x00000000
#define INTEGRATOR_BOOT_ROM_HI          0x20000000
#define INTEGRATOR_BOOT_ROM_BASE        INTEGRATOR_BOOT_ROM_HI
#define INTEGRATOR_BOOT_ROM_SIZE	0x00080000

/*
 *  New Core Modules have different amounts of SSRAM,
 *  the amount of SSRAM fitted can be found in HDR_STAT.
 *
 *  The symbol INTEGRATOR_SSRAM_SIZE is kept, however this
 *  now refers to the minimum amount of SSRAM fitted on any
 *  core module.
 *
 *  New Core Modules also alias the SSRAM.
 */

#define INTEGRATOR_SSRAM_BASE           0x00000000
#define INTEGRATOR_SSRAM_ALIAS_BASE     0x10800000
#define INTEGRATOR_SSRAM_SIZE           SZ_256K

#define INTEGRATOR_HDR_SSRAM_SIZE	SZ_1M

#define INTEGRATOR_MBRD_SSRAM_BASE      0x28000000
#define INTEGRATOR_MBRD_SSRAM_SIZE      SZ_512K

/*  SDRAM is a DIMM therefore the size is not known. */

#define INTEGRATOR_SDRAM_BASE           0x00040000

#define INTEGRATOR_SDRAM_ALIAS_BASE     0x80000000
#define INTEGRATOR_HDR0_SDRAM_BASE      0x80000000
#define INTEGRATOR_HDR1_SDRAM_BASE      0x90000000
#define INTEGRATOR_HDR2_SDRAM_BASE      0xA0000000
#define INTEGRATOR_HDR3_SDRAM_BASE      0xB0000000

/* RAM base in reset memory map */

#define INTEGRATOR_RESET_RAM_BASE	0x28000000

/* PCI Base area */

#define INTEGRATOR_PCI_BASE		0x40000000
#define INTEGRATOR_PCI_SIZE		0x3FFFFFFF

#define BUS		BUS_TYPE_PCI


/* Signals generated from Integrator clock generator. */

#define INTEGRATOR_SYSCLK   20000000		/* System bus clock */
#define INTEGRATOR_P_CLK    33000000		/* PCI clock */
#define INTEGRATOR_UARTCLK  14745600		/* UART clock */
#define INTEGRATOR_CLK24MHZ 24000000		/* KMI/Timer clock */

/* Integrator 940T core module registers. */

#define INTEGRATOR_HDR_ID_OFFSET        0x00
#define INTEGRATOR_HDR_PROC_OFFSET      0x04
#define INTEGRATOR_HDR_OSC_OFFSET       0x08
#define INTEGRATOR_HDR_CTRL_OFFSET      0x0C
#define INTEGRATOR_HDR_STAT_OFFSET      0x10
#define INTEGRATOR_HDR_LOCK_OFFSET      0x14
#define INTEGRATOR_HDR_SDRAM_OFFSET     0x20
#define INTEGRATOR_HDR_INIT_OFFSET      0x24
#define INTEGRATOR_HDR_IC_OFFSET        0x40
#define INTEGRATOR_HDR_SPDBASE_OFFSET   0x100
#define INTEGRATOR_HDR_SPDTOP_OFFSET    0x200

#define INTEGRATOR_HDR_CTRL_FASTBUS	0x40

#define INTEGRATOR_HDR_BASE             0x10000000
#define INTEGRATOR_HDR_ID    (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_ID_OFFSET)
#define INTEGRATOR_HDR_PROC  (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_PROC_OFFSET)
#define INTEGRATOR_HDR_OSC   (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_OSC_OFFSET)
#define INTEGRATOR_HDR_CTRL  (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_CTRL_OFFSET)
#define INTEGRATOR_HDR_STAT  (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_STAT_OFFSET)
#define INTEGRATOR_HDR_LOCK  (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_LOCK_OFFSET)
#define INTEGRATOR_HDR_SDRAM (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_SDRAM_OFFSET)
#define INTEGRATOR_HDR_INIT  (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_INIT_OFFSET)
#define INTEGRATOR_HDR_IC    (INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_IC_OFFSET)
#define INTEGRATOR_HDR_SPDBASE \
			(INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_SPDBASE_OFFSET)
#define INTEGRATOR_HDR_SPDTOP \
			(INTEGRATOR_HDR_BASE + INTEGRATOR_HDR_SPDTOP_OFFSET)

#define INTEGRATOR_HDR_OSC_CORE_10MHz	(0x102)
#define INTEGRATOR_HDR_OSC_CORE_15MHz	(0x107)
#define INTEGRATOR_HDR_OSC_CORE_20MHz	(0x10C)
#define INTEGRATOR_HDR_OSC_CORE_25MHz	(0x111)
#define INTEGRATOR_HDR_OSC_CORE_30MHz	(0x116)
#define INTEGRATOR_HDR_OSC_CORE_35MHz	(0x11B)
#define INTEGRATOR_HDR_OSC_CORE_40MHz	(0x120)
#define INTEGRATOR_HDR_OSC_CORE_45MHz	(0x125)
#define INTEGRATOR_HDR_OSC_CORE_50MHz	(0x12A)
#define INTEGRATOR_HDR_OSC_CORE_55MHz	(0x12F)
#define INTEGRATOR_HDR_OSC_CORE_60MHz	(0x134)
#define INTEGRATOR_HDR_OSC_CORE_65MHz	(0x139)
#define INTEGRATOR_HDR_OSC_CORE_70MHz	(0x13E)
#define INTEGRATOR_HDR_OSC_CORE_75MHz	(0x143)
#define INTEGRATOR_HDR_OSC_CORE_80MHz	(0x148)
#define INTEGRATOR_HDR_OSC_CORE_85MHz	(0x14D)
#define INTEGRATOR_HDR_OSC_CORE_90MHz	(0x152)
#define INTEGRATOR_HDR_OSC_CORE_95MHz	(0x157)
#define INTEGRATOR_HDR_OSC_CORE_100MHz	(0x15C)
#define INTEGRATOR_HDR_OSC_CORE_105MHz	(0x161)
#define INTEGRATOR_HDR_OSC_CORE_110MHz	(0x166)
#define INTEGRATOR_HDR_OSC_CORE_115MHz	(0x16B)
#define INTEGRATOR_HDR_OSC_CORE_120MHz	(0x170)
#define INTEGRATOR_HDR_OSC_CORE_125MHz	(0x175)
#define INTEGRATOR_HDR_OSC_CORE_130MHz	(0x17A)
#define INTEGRATOR_HDR_OSC_CORE_135MHz	(0x17F)
#define INTEGRATOR_HDR_OSC_CORE_140MHz	(0x184)
#define INTEGRATOR_HDR_OSC_CORE_145MHz	(0x189)
#define INTEGRATOR_HDR_OSC_CORE_150MHz	(0x18E)
#define INTEGRATOR_HDR_OSC_CORE_155MHz	(0x193)
#define INTEGRATOR_HDR_OSC_CORE_160MHz	(0x198)
#define INTEGRATOR_HDR_OSC_CORE_MASK	(0x7FF)

#define INTEGRATOR_HDR_OSC_MEM_10MHz	(0x10C000)
#define INTEGRATOR_HDR_OSC_MEM_15MHz	(0x116000)
#define INTEGRATOR_HDR_OSC_MEM_20MHz	(0x120000)
#define INTEGRATOR_HDR_OSC_MEM_25MHz	(0x12A000)
#define INTEGRATOR_HDR_OSC_MEM_30MHz	(0x134000)
#define INTEGRATOR_HDR_OSC_MEM_33MHz	(0x13A000)
#define INTEGRATOR_HDR_OSC_MEM_40MHz	(0x148000)
#define INTEGRATOR_HDR_OSC_MEM_50MHz	(0x15C000)
#define INTEGRATOR_HDR_OSC_MEM_60MHz	(0x170000)
#define INTEGRATOR_HDR_OSC_MEM_66MHz	(0x17C000)
#define INTEGRATOR_HDR_OSC_MEM_MASK	(0x7FF000)

/*
 * Values we use for core and memory clocks on different header cards. Later
 * versions of cards and/or upgraded FPGAs on the boards may enable the cards
 * to run at faster speeds.
 *
 * These are:
 *
 * Processor    Core    Memory Bus   System Bus   PCI Bus
 * =========    ====    ==========   ==========   =======
 * Unknown      40MHz      20MHz        20MHz      33MHz
 * ARM720T      50MHz      40MHz        20MHz      33MHz
 * ARM740T      50MHz      40MHz        20MHz      33MHz
 * ARM920T     140MHz      25MHz        20MHz      33MHz
 * ARM940T     100MHz      25MHz        20MHz      33MHz
 * ARM946ES    100MHz      25MHz        20MHz      33MHz
 * ARM926ES    100MHz      25MHz        20MHz      33MHz
 * ARM966ES    120MHz      40MHz        20MHz      33MHz
 * ARM1136JF   100MHz      25MHz        20MHz      33MHz
 *
 * NOTE
 * ====
 * The memory bus frequency of 25 MHz for ARM 9 cores is a very
 * conservative figure. Depending on silicon manufacturer and version, it may
 * be possible to select a higher frequency by experimentation.
 */

#define INTEGRATOR_HDR_OSC_DFLT_VAL \
		(INTEGRATOR_HDR_OSC_CORE_40MHz | INTEGRATOR_HDR_OSC_MEM_20MHz)
#define INTEGRATOR_HDR_OSC_720T_VAL \
		(INTEGRATOR_HDR_OSC_CORE_50MHz | INTEGRATOR_HDR_OSC_MEM_40MHz)
#define INTEGRATOR_HDR_OSC_740T_VAL \
		(INTEGRATOR_HDR_OSC_CORE_50MHz | INTEGRATOR_HDR_OSC_MEM_25MHz)
#define INTEGRATOR_HDR_OSC_920T_VAL \
		(INTEGRATOR_HDR_OSC_CORE_140MHz | INTEGRATOR_HDR_OSC_MEM_25MHz)
#define INTEGRATOR_HDR_OSC_940T_VAL \
		(INTEGRATOR_HDR_OSC_CORE_100MHz | INTEGRATOR_HDR_OSC_MEM_25MHz)
#define INTEGRATOR_HDR_OSC_946ES_VAL \
		(INTEGRATOR_HDR_OSC_CORE_80MHz | INTEGRATOR_HDR_OSC_MEM_25MHz)
#define INTEGRATOR_HDR_OSC_926ES_VAL \
	        (INTEGRATOR_HDR_OSC_CORE_80MHz | INTEGRATOR_HDR_OSC_MEM_25MHz)
#define INTEGRATOR_HDR_OSC_966ES_VAL	INTEGRATOR_HDR_OSC_CORE_120MHz
#define INTEGRATOR_HDR_OSC_1020E_VAL	0xC
#define INTEGRATOR_HDR_OSC_1022E_VAL	0x16
#define INTEGRATOR_HDR_OSC_1136JF_VAL \
	        (INTEGRATOR_HDR_OSC_CORE_140MHz | INTEGRATOR_HDR_OSC_MEM_25MHz)


#define INTEGRATOR_HDR_REMAP		0x4

#define INTEGRATOR_HDR_SDRAM_SPD_OK	0x20

#define INTEGRATOR_HDR_PLLBYPASS_ON	0x3
#define INTEGRATOR_HDR_HCLKDIV_3	0x20
#define INTEGRATOR_HDR_CLKRATIO_2	0x100
#define INTEGRATOR_HDR_TCRAM_ENABLE	0x10000
#define INTEGRATOR_HDR_TCRAM_EMULATE	0x20000


/* Integrator EBI register definitions. */

#define INTEGRATOR_EBI_BASE 0x12000000

#define INTEGRATOR_EBI_CSR0_OFFSET      0x00
#define INTEGRATOR_EBI_CSR1_OFFSET      0x04
#define INTEGRATOR_EBI_CSR2_OFFSET      0x08
#define INTEGRATOR_EBI_CSR3_OFFSET      0x0C
#define INTEGRATOR_EBI_LOCK_OFFSET      0x20

#define INTEGRATOR_EBI_CSR0 (INTEGRATOR_EBI_BASE + INTEGRATOR_EBI_CSR0_OFFSET)
#define INTEGRATOR_EBI_CSR1 (INTEGRATOR_EBI_BASE + INTEGRATOR_EBI_CSR1_OFFSET)
#define INTEGRATOR_EBI_CSR2 (INTEGRATOR_EBI_BASE + INTEGRATOR_EBI_CSR2_OFFSET)
#define INTEGRATOR_EBI_CSR3 (INTEGRATOR_EBI_BASE + INTEGRATOR_EBI_CSR3_OFFSET)
#define INTEGRATOR_EBI_LOCK (INTEGRATOR_EBI_BASE + INTEGRATOR_EBI_LOCK_OFFSET)

#define INTEGRATOR_EBI_8_BIT            0x00
#define INTEGRATOR_EBI_16_BIT           0x01
#define INTEGRATOR_EBI_32_BIT           0x02
#define INTEGRATOR_EBI_WRITE_ENABLE     0x04
#define INTEGRATOR_EBI_SYNC             0x08
#define INTEGRATOR_EBI_WS_2             0x00
#define INTEGRATOR_EBI_WS_3             0x10
#define INTEGRATOR_EBI_WS_4             0x20
#define INTEGRATOR_EBI_WS_5             0x30
#define INTEGRATOR_EBI_WS_6             0x40
#define INTEGRATOR_EBI_WS_7             0x50
#define INTEGRATOR_EBI_WS_8             0x60
#define INTEGRATOR_EBI_WS_9             0x70
#define INTEGRATOR_EBI_WS_10            0x80
#define INTEGRATOR_EBI_WS_11            0x90
#define INTEGRATOR_EBI_WS_12            0xA0
#define INTEGRATOR_EBI_WS_13            0xB0
#define INTEGRATOR_EBI_WS_14            0xC0
#define INTEGRATOR_EBI_WS_15            0xD0
#define INTEGRATOR_EBI_WS_16            0xE0
#define INTEGRATOR_EBI_WS_17            0xF0


/* System Controller */

#define INTEGRATOR_SC_ID_OFFSET		(0x00)
#define INTEGRATOR_SC_OSC_OFFSET	(0x04)
#define INTEGRATOR_SC_CTRLS_OFFSET	(0x08)
#define INTEGRATOR_SC_CTRLC_OFFSET	(0x0C)
#define INTEGRATOR_SC_DEC_OFFSET	(0x10)
#define INTEGRATOR_SC_ARB_OFFSET	(0x14)
#define INTEGRATOR_SC_PCIENABLE_OFFSET	(0x18)
#define INTEGRATOR_SC_LOCK_OFFSET	(0x1C)

#define INTEGRATOR_SC_BASE		(0x11000000)
#define INTEGRATOR_SC_ID \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_ID_OFFSET)
#define INTEGRATOR_SC_OSC \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_OSC_OFFSET)
#define INTEGRATOR_SC_CTRLS \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_CTRLS_OFFSET)
#define INTEGRATOR_SC_CTRLC \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_CTRLC_OFFSET)
#define INTEGRATOR_SC_DEC \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_DEC_OFFSET)
#define INTEGRATOR_SC_ARB \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_ARB_OFFSET)
#define INTEGRATOR_SC_PCIENABLE \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_PCIENABLE_OFFSET)
#define INTEGRATOR_SC_LOCK \
			(INTEGRATOR_SC_BASE + INTEGRATOR_SC_LOCK_OFFSET)

#define INTEGRATOR_SC_OSC_SYS_10MHz	(0x20)
#define INTEGRATOR_SC_OSC_SYS_15MHz	(0x34)
#define INTEGRATOR_SC_OSC_SYS_20MHz	(0x48)
#define INTEGRATOR_SC_OSC_SYS_25MHz	(0x5C)
#define INTEGRATOR_SC_OSC_SYS_33MHz	(0x7C)
#define INTEGRATOR_SC_OSC_SYS_MASK	(0xFF)

#define INTEGRATOR_SC_OSC_PCI_25MHz	(0x100)
#define INTEGRATOR_SC_OSC_PCI_33MHz	(0x0)
#define INTEGRATOR_SC_OSC_PCI_MASK	(0x100)

#define FL_SC_CONTROL			0x06	/* Enable Flash Write and Vpp */


/* - Hard coded id of the core module we are compiling for. */

#define CORE_MODULE 0

/*
 * interrupt control stuff
 * Note: FIQ is not handled within VxWorks so this is just IRQ
 */

#define IC_BASE 	0x10140000
#define IRQ_IRQSTATUS	(IC_BASE+0x000)	/* Read */
#define IRQ_FIQSTATUS	(IC_BASE+0x004)
#define IRQ_RAW_STATUS	(IC_BASE+0x008)	/* Read */
#define IRQ_INTSELECT	(IC_BASE+0x00C)
#define IRQ_INTENABLE	(IC_BASE+0x010)
#define IRQ_INTENCLEAR	(IC_BASE+0x014)
#define IRQ_SOFTINT	(IC_BASE+0x018)
#define IRQ_SOFTINTCLEAR (IC_BASE+0x01C)
#define IRQ_PROTECTION	(IC_BASE+0x020)
#define IRQ_VECTADDR	(IC_BASE+0x030)
#define IRQ_DEFVECTADDR	(IC_BASE+0x034)

#define SIC_BASE        (0x10003000)
#define SIC_STATUS      (SIC_BASE + 0x00)
#define SIC_RAWSTAT     (SIC_BASE + 0x04)
#define SIC_ENABLE      (SIC_BASE + 0x08)
#define SIC_ENSET       (SIC_BASE + 0x08)
#define SIC_ENCLR       (SIC_BASE + 0x0c)
#define SIC_SOFTINTSET  (SIC_BASE + 0x10)
#define SIC_SOFTINTCLR  (SIC_BASE + 0x14)
#define SIC_PICENABLE   (SIC_BASE + 0x18)
#define SIC_PICENSET    (SIC_BASE + 0x20)
#define SIC_PICENCLR    (SIC_BASE + 0x24)

#define AMBA_INT_NUM_LEVELS	64
#define AMBA_INT_CSR_PEND	IRQ_IRQSTATUS
#define AMBA_INT_CSR_ENB	IRQ_INTENABLE
#define AMBA_INT_CSR_DIS	IRQ_INTENCLEAR
#define AMBA_INT_CSR_MASK	0xFFFFFFFF /* Mask out invalid status bits */


/* Interrupt levels */

#define INT_LVL_WD		0	
#define INT_LVL_SOFTINT  	1	
#define INT_LVL_COMMRX		2
#define INT_LVL_COMMTX		3
#define INT_LVL_TIMER01		4
#define INT_LVL_TIMER23		5
#define INT_LVL_GPIO0		6
#define INT_LVL_GPIO1		7
#define INT_LVL_GPIO2		8
#define INT_LVL_GPIO3		9
#define INT_LVL_RTC		10
#define INT_LVL_SSP		11
#define INT_LVL_UART_0		12
#define INT_LVL_UART_1		13
#define INT_LVL_UART_2		14
#define INT_LVL_SCI0		15
#define INT_LVL_CLCD		16
#define INT_LVL_DMA		17
#define INT_LVL_PWRFAIL		18
#define INT_LVL_MBX		19
#define INT_LVL_GND		20
#define INT_LVL_EXT21		21
#define INT_LVL_EXT22		22
#define INT_LVL_EXT23		23
#define INT_LVL_EXT24		24
#define INT_LVL_EXT25		25
#define INT_LVL_EXT26		26
#define INT_LVL_EXT27		27
#define INT_LVL_EXT28		28
#define INT_LVL_EXT29		29
#define INT_LVL_EXT30		30
#define INT_LVL_EXT31		31

#define INT_LVL_SIC_SOFTINT	0
#define INT_LVL_SIC_MMCI0B	1
#define INT_LVL_SIC_MMCI1B	2
#define INT_LVL_SIC_KMI0	3
#define INT_LVL_SIC_KMI1	4
#define INT_LVL_SIC_SCU1	5
#define INT_LVL_SIC_UART3	6
#define INT_LVL_SIC_CLCD	7
#define INT_LVL_SIC_TS		8
#define INT_LVL_SIC_KEYPAD	9
#define INT_LVL_SIC_DISKONCHIP	21
#define INT_LVL_SIC_MMCI0A	22
#define INT_LVL_SIC_MMCI1A	23
#define INT_LVL_SIC_AACI	24
#define INT_LVL_SIC_ETHER	25
#define INT_LVL_SIC_USB		26
#define INT_LVL_SIC_PCI0	27
#define INT_LVL_SIC_PCI1	28
#define INT_LVL_SIC_PCI2	29
#define INT_LVL_SIC_PCI3	30


/* interrupt vectors */

#define INT_VEC_SOFT		IVEC_TO_INUM(INT_LVL_SOFT)
#define INT_VEC_UART_0		IVEC_TO_INUM(INT_LVL_UART_0)
#define INT_VEC_UART_1		IVEC_TO_INUM(INT_LVL_UART_1)
#define INT_VEC_KEYBOARD	IVEC_TO_INUM(INT_LVL_KEYBOARD)
#define INT_VEC_MOUSE		IVEC_TO_INUM(INT_LVL_MOUSE)
#define INT_VEC_TIMER01		IVEC_TO_INUM(INT_LVL_TIMER01)
#define INT_VEC_TIMER23		IVEC_TO_INUM(INT_LVL_TIMER23)
#define INT_VEC_RTC		IVEC_TO_INUM(INT_LVL_RTC)
#define INT_VEC_EXP0		IVEC_TO_INUM(INT_LVL_EXP0)
#define INT_VEC_EXP1		IVEC_TO_INUM(INT_LVL_EXP1)
#define INT_VEC_EXP2		IVEC_TO_INUM(INT_LVL_EXP2)
#define INT_VEC_EXP3		IVEC_TO_INUM(INT_LVL_EXP3)
#define INT_VEC_PCI_0		IVEC_TO_INUM(INT_LVL_SIC_PCI0)
#define INT_VEC_PCI_1		IVEC_TO_INUM(INT_LVL_SIC_PCI1)
#define INT_VEC_PCI_2		IVEC_TO_INUM(INT_LVL_SIC_PCI2)
#define INT_VEC_PCI_3		IVEC_TO_INUM(INT_LVL_SIC_PCI3)
#define INT_VEC_PCIBRIDGE	IVEC_TO_INUM(INT_LVL_PCIBRIDGE)
#define INT_VEC_DEG		IVEC_TO_INUM(INT_LVL_DEG)
#define INT_VEC_ENUM		IVEC_TO_INUM(INT_LVL_ENUM)
#define INT_VEC_PCILB		IVEC_TO_INUM(INT_LVL_PCILB)
#define INT_VEC_APC		IVEC_TO_INUM(INT_LVL_APC)


/* definitions for the AMBA UART */

#define UART_XTAL_FREQ		INTEGRATOR_UARTCLK /* UART baud rate clk freq */
#define N_SIO_CHANNELS		N_AMBA_UART_CHANNELS
#define N_UART_CHANNELS		N_AMBA_UART_CHANNELS
#define N_AMBA_UART_CHANNELS	2		/* number of AMBA UART chans */

#define UART_0_BASE_ADR		0x101F1000	/* UART 0 base address */
#define UART_1_BASE_ADR		0x101F2000	/* UART 1 base address */

/* definitions for the keyboard */
#define KBD_BASE_ADR		0x10006000	/* Keyboard 0 base address */
#define MOUSE_BASE_ADR		0x10007000	/* Mouse 1 base address */

#define KBD_REGS_SIZE           0x14
#define MOUSE_REGS_SIZE         0x14

/* definitions for the AMBA Timer */

#define AMBA_TIMER_BASE		0x101e2000	/* Address of base of timer */
#define AMBA_BASE_OFFSET        0X1000


#define AMBA_TIMER01_BASE ((AMBA_TIMER_BASE))
#define AMBA_TIMER23_BASE ((AMBA_TIMER_BASE) + AMBA_BASE_OFFSET)

#define SYS_TIMER_BASE   AMBA_TIMER01_BASE
#define AUX_TIMER_BASE   AMBA_TIMER23_BASE

#define SYS_TIMER_INT_LVL   (INT_LVL_TIMER01)
#define AUX_TIMER_INT_LVL   (INT_LVL_TIMER23)

/* Add corresponding INT_VEC definitions for intConnect calls. */

#define SYS_TIMER_INT_VEC (INT_VEC_TIMER01)
#define AUX_TIMER_INT_VEC (INT_VEC_TIMER23)

/*
#define AMBA_TIMER_SYS_TC_DISABLE	(TC_DISABLE | TC_PERIODIC | TC_DIV16)
#define AMBA_TIMER_SYS_TC_ENABLE	(TC_ENABLE  | TC_PERIODIC | TC_DIV16)
#define AMBA_TIMER_AUX_TC_DISABLE	(TC_DISABLE | TC_PERIODIC | TC_DIV16)
#define AMBA_TIMER_AUX_TC_ENABLE	(TC_ENABLE  | TC_PERIODIC | TC_DIV16)
*/

/* Frequency of counter/timers */

#define SYS_TIMER_CLK	(INTEGRATOR_CLK24MHZ/16)
#define AUX_TIMER_CLK	(INTEGRATOR_SYSCLK/16)
#define SLEEP_TIMER_CLK (INTEGRATOR_CLK24MHZ/16)

#define AMBA_RELOAD_TICKS	0	/* No overhead */

/* Mask out unused bits from timer register. */

#define AMBA_TIMER_VALUE_MASK	0xFFFF


/*
 * Clock rates depend upon CPU power and work load of application.
 * The values below are minimum and maximum allowed by the hardware.
 * So:
 * min frequency = roundup(clock_rate/(max_counter_value))
 * max frequency = rounddown(clock_rate/(min_counter_value))
 * i.e. SYS_CLK_RATE_MAX = SYS_TIMER_CLK
 *      AUX_CLK_RATE_MAX = AUX_TIMER_CLK
 *
 * However, we must set maxima that are sustainable on a running
 * system determined by experimentation.
 *
 * The following values have been determined by experimentation.
 */

#define SYS_CLK_RATE_MIN ((SYS_TIMER_CLK+0xFFFF)/0x10000)
#define SYS_CLK_RATE_MAX 25000

#define AUX_CLK_RATE_MIN ((AUX_TIMER_CLK+0xFFFF)/0x10000)
#define AUX_CLK_RATE_MAX 25000

/* PCI definitions */

/* Interrupt number for PCI */

#define INT_NUM_IRQ0	INT_LVL_PCI_0

/* V3 PCI bridge controller */

#define V3_BASE		0x62000000    /* V360EPC registers */

/* Bit field definitions */

#define BIT0                            0x00000001
#define BIT1                            0x00000002
#define BIT2                            0x00000004
#define BIT3                            0x00000008
#define BIT4                            0x00000010
#define BIT5                            0x00000020
#define BIT6                            0x00000040
#define BIT7                            0x00000080
#define BIT8                            0x00000100
#define BIT9                            0x00000200
#define BIT10                           0x00000400
#define BIT11                           0x00000800
#define BIT12                           0x00001000
#define BIT13                           0x00002000
#define BIT14                           0x00004000
#define BIT15                           0x00008000
#define BIT16                           0x00010000
#define BIT17                           0x00020000
#define BIT18                           0x00040000
#define BIT19                           0x00080000
#define BIT20                           0x00100000

/* Handy sizes */

#define SZ_1K                           0x00000400
#define SZ_4K                           0x00001000
#define SZ_8K                           0x00002000
#define SZ_16K                          0x00004000
#define SZ_64K                          0x00010000
#define SZ_128K                         0x00020000
#define SZ_256K                         0x00040000
#define SZ_512K                         0x00080000

#define SZ_1M                           0x00100000
#define SZ_2M                           0x00200000
#define SZ_4M                           0x00400000
#define SZ_8M                           0x00800000
#define SZ_16M                          0x01000000
#define SZ_32M                          0x02000000
#define SZ_64M                          0x04000000
#define SZ_128M                         0x08000000
#define SZ_256M                         0x10000000
#define SZ_512M                         0x20000000

#define SZ_1G                           0x40000000
#define SZ_2G                           0x80000000


/* V3 Local Bus to PCI Bridge definitions */

#define V3_PCI_VENDOR                   0x00000000
#define V3_PCI_DEVICE                   0x00000002
#define V3_PCI_CMD                      0x00000004
#define V3_PCI_STAT                     0x00000006
#define V3_PCI_CC_REV                   0x00000008
#define V3_PCI_HDR_CFG                  0x0000000C
#define V3_PCI_IO_BASE                  0x00000010
#define V3_PCI_BASE0                    0x00000014
#define V3_PCI_BASE1                    0x00000018
#define V3_PCI_SUB_VENDOR               0x0000002C
#define V3_PCI_SUB_ID                   0x0000002E
#define V3_PCI_ROM                      0x00000030
#define V3_PCI_BPARAM                   0x0000003C
#define V3_PCI_MAP0                     0x00000040
#define V3_PCI_MAP1                     0x00000044
#define V3_PCI_INT_STAT                 0x00000048
#define V3_PCI_INT_CFG                  0x0000004C
#define V3_LB_BASE0                     0x00000054
#define V3_LB_BASE1                     0x00000058
#define V3_LB_MAP0                      0x0000005E
#define V3_LB_MAP1                      0x00000062
#define V3_LB_BASE2                     0x00000064
#define V3_LB_MAP2                      0x00000066
#define V3_LB_SIZE                      0x00000068
#define V3_LB_IO_BASE                   0x0000006E
#define V3_FIFO_CFG                     0x00000070
#define V3_FIFO_PRIORITY                0x00000072
#define V3_FIFO_STAT                    0x00000074
#define V3_LB_ISTAT                     0x00000076
#define V3_LB_IMASK                     0x00000077
#define V3_SYSTEM                       0x00000078
#define V3_LB_CFG                       0x0000007A
#define V3_PCI_CFG                      0x0000007C
#define V3_DMA_PCI_ADR0                 0x00000080
#define V3_DMA_PCI_ADR1                 0x00000090
#define V3_DMA_LOCAL_ADR0               0x00000084
#define V3_DMA_LOCAL_ADR1               0x00000094
#define V3_DMA_LENGTH0                  0x00000088
#define V3_DMA_LENGTH1                  0x00000098
#define V3_DMA_CSR0                     0x0000008B
#define V3_DMA_CSR1                     0x0000009B
#define V3_DMA_CTLB_ADR0                0x0000008C
#define V3_DMA_CTLB_ADR1                0x0000009C
#define V3_DMA_DELAY                    0x000000E0
#define V3_MAIL_DATA                    0x000000C0
#define V3_PCI_MAIL_IEWR                0x000000D0
#define V3_PCI_MAIL_IERD                0x000000D2
#define V3_LB_MAIL_IEWR                 0x000000D4
#define V3_LB_MAIL_IERD                 0x000000D6
#define V3_MAIL_WR_STAT                 0x000000D8
#define V3_MAIL_RD_STAT                 0x000000DA
#define V3_QBA_MAP                      0x000000DC

/* SYSTEM register bits */

#define V3_SYSTEM_M_RST_OUT             BIT15
#define V3_SYSTEM_M_LOCK                BIT14

/*  PCI_CFG bits */

#define V3_PCI_CFG_M_RETRY_EN           BIT10
#define V3_PCI_CFG_M_AD_LOW1            BIT9
#define V3_PCI_CFG_M_AD_LOW0            BIT8

/* PCI MAP register bits (PCI -> Local bus) */

#define V3_PCI_MAP_M_MAP_ADR            0xFFF00000
#define V3_PCI_MAP_M_RD_POST_INH        BIT15
#define V3_PCI_MAP_M_ROM_SIZE           BIT11+BIT10
#define V3_PCI_MAP_M_SWAP               BIT9+BIT8
#define V3_PCI_MAP_M_ADR_SIZE           0x000000F0
#define V3_PCI_MAP_M_REG_EN             BIT1
#define V3_PCI_MAP_M_ENABLE             BIT0

/* 9 => 512M window size */

#define V3_PCI_MAP_M_ADR_SIZE_512M      0x00000090

/* A => 1024M window size */

#define V3_PCI_MAP_M_ADR_SIZE_1024M     0x000000A0

/* LB_BASE register bits (Local bus -> PCI) */

#define V3_LB_BASE_M_MAP_ADR            0xFFF00000
#define V3_LB_BASE_M_SWAP               BIT9+BIT8
#define V3_LB_BASE_M_ADR_SIZE           0x000000F0
#define V3_LB_BASE_M_PREFETCH           BIT3
#define V3_LB_BASE_M_ENABLE             BIT0

/* LB_MAP register bits (Local bus -> PCI) */

#define V3_LB_MAP_M_MAP_ADR             0xFFF0
#define V3_LB_MAP_M_TYPE                0x000E
#define V3_LB_MAP_M_AD_LOW_EN           BIT0

/* PCI COMMAND REGISTER bits */

#define V3_COMMAND_M_FBB_EN             BIT9
#define V3_COMMAND_M_SERR_EN            BIT8
#define V3_COMMAND_M_PAR_EN             BIT6
#define V3_COMMAND_M_MASTER_EN          BIT2
#define V3_COMMAND_M_MEM_EN             BIT1
#define V3_COMMAND_M_IO_EN              BIT0

/* size of area defined in memory map for V3 registers */

#define V3_REGS_SIZE			0x10000


/* defines for generic pciIoMapLib.c code */

#define PCI_IN_BYTE(x)		*(volatile UINT8 *) (x)
#define PCI_OUT_BYTE(x,y)	*(volatile UINT8 *) (x) = (UINT8)  y
#define PCI_IN_WORD(x)		*(volatile UINT16 *)(x)
#define PCI_OUT_WORD(x,y)	*(volatile UINT16 *)(x) = (UINT16) y
#define PCI_IN_LONG(x)		*(volatile UINT32 *)(x)
#define PCI_OUT_LONG(x,y)	*(volatile UINT32 *)(x) = (UINT32) y

/* restrict use to the first bus, by default */

#define PCI_MAX_BUS 1

/* memory map as seen by the CPU on the local bus */

#define CPU_PCI_IO_ADRS		0x60000000 	/* PCI I/O space base */
#define CPU_PCI_IO_SIZE		0x10000

#define CPU_PCI_CNFG_ADRS	0x61000000	/* PCI config space */
#define CPU_PCI_CNFG_SIZE	0x1000000

#define CPU_PCI_MEM_ADRS	0x40000000
#define CPU_PCI_MEM_SIZE	0x10000000

/* PCI view of PCI space for PCI devices */

#define PCI_IO_ADRS             0x00000000      /* base of PCI I/O address */
#define PCI2DRAM_BASE_ADRS      0x80000000      /* memory seen from PCI bus */

/* PCI view of PCI memory space for PCI memory devices */

#define PCI_MEM_ADRS		0x40000000	/* base of PCI memory space */

/* PCI address to CPU address offset */

#define PCI2CPU_IO_OFFSET	(CPU_PCI_IO_ADRS - PCI_IO_ADRS)
#define PCI2CPU_MEM_OFFSET	(CPU_PCI_MEM_ADRS - PCI_MEM_ADRS)

/* PCI bus resources */

#define PCI_IO_SIZE		0x00004000	/* PCI I/O slot size */
#define PCI_MEM_SIZE		0x00004000	/* PCI memory slot size */

#define PCI_MEM_ADR0            PCI_MEM_ADRS
#define PCI_IO_ADR0             PCI_IO_ADRS     /* I/O base for CSR 32Bytes */
#define PCI_IO_SIZE0            PCI_IO_SIZE     /* I/O size for CSR */
#define PCI_INT_LVL0            INT_LVL_PCI_0	/* Interrupt level */
#define PCI_INT_VEC0            INT_VEC_PCI_0	/* Interrupt vector */

#define PCI_MEM_ADR1            PCI_MEM_ADR0 + PCI_MEM_SIZE
#define PCI_IO_ADR1             PCI_IO_ADR0 + PCI_IO_SIZE
#define PCI_IO_SIZE1            PCI_IO_SIZE
#define PCI_INT_LVL1            INT_LVL_PCI_1
#define PCI_INT_VEC1            INT_VEC_PCI_1

#define PCI_MEM_ADR2            PCI_MEM_ADR1 + PCI_MEM_SIZE
#define PCI_IO_ADR2             PCI_IO_ADR1 + PCI_IO_SIZE
#define PCI_IO_SIZE2            PCI_IO_SIZE
#define PCI_INT_LVL2            INT_LVL_PCI_2
#define PCI_INT_VEC2            INT_VEC_PCI_2


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
 * Definitions for Ethernet driver buffer use.
 *
 * The buffers to be shared across the PCI bus in must not have addresses
 * in low memory which refer to header card SSRAM (which is not
 * reachable from the PCI bus). We can do several things:
 *
 * 1. Simply allow the driver to use use cacheDmaMalloc() without
 * checking the addresses returned. Because of where the heap usually
 * is, cacheDmaMalloc() in default kernels will generally return
 * addresses which are "low-aliases" for SDRAM and not header card SSRAM.
 * To use this option, do not define INTEGRATOR_ENET_FIXED_BUF_ADRS and do
 * not define the parameter INTEGRATOR_ENET_CHECK_BUFFERS. Header cards for
 * later CPUs may have more local SSRAM, so this may become more of a problem
 * with those later cards.
 *
 * 2. Use cacheDmaMalloc() in the BSP initialisation code, but check that the
 * memory returned is not, in fact, SSRAM, but is actually SDRAM. To use this
 * option define the parameter INTEGRATOR_ENET_CHECK_BUFFERS. A disadvantage
 * of this is that the BSP then needs knowledge of the relationship between
 * buffer space required and the numbers of transmit and receive buffers to be
 * allocated, which would normally be kept within the driver. This is the
 * option configured by default.
 *
 * 3. Define the buffer address to be a specific address, which is
 * accessible from the PCI bus.
 * (a) One possibility is the unaliased SDRAM at its real (high) address.
 * To use this option, define INTEGRATOR_ENET_FIXED_BUF_ADRS and define
 * INTEGRATOR_XXX_BUF_ADRS to be INTEGRATOR_SDRAM_ALIAS_BASE.
 * All of the SDRAM is mapped at the high address, but the size of the
 * part that is not aliased down to low memory, will be the size of the
 * header card SSRAM. On 720T, there is 256 Kbytes and on 920T, we have 1
 * megabyte.
 * (b) Another possibility is the motherboard SSRAM, though early
 * motherboards have a fault that prevents this SSRAM from being accessed
 * from PCI unless the FPGAs on the board have been upgraded. To use this
 * option, define INTEGRATOR_ENET_FIXED_BUF_ADRS and define
 * INTEGRATOR_ENET_XXX_BUF_ADRS to be INTEGRATOR_MBRD_SSRAM_ALIAS_BASE
 * (+ offset if required).
 *
 * In the FEI driver, we use ((nCFDs + 5*nRFDs) * 1544) + 4 bytes.
 * On 720T, this is more than the size of the unaliased SDRAM (256 kbytes),
 * so if we use that area, we need to adjust the number of CFDs and
 * RFDs downwards slightly.
 *
 * Note that all of the fixed address options listed above will suffer a
 * performance penalty over using low-aliased-SDRAM.
 *
 * 4. Reserve an area of SDRAM (at the low alias, but above the end of
 * header card SSRAM) to use for the buffer memory.
 *
 * The drivers will check that the amount of memory given is sufficient
 * for the numbers of CFDs and RFDs and will fail to load if there is a
 * mismatch.
 *
 * Obviously, if only one of the FEI and DEC drivers is being used, then buffer
 * space need only be allocated for the one being used.
 */

#undef  INTEGRATOR_ENET_FIXED_BUF_ADRS	/* do not use fixed address bufs */
#undef INTEGRATOR_ENET_CHECK_BUFFERS	/* check cacheDmaMalloc() addresses */


#ifdef  INTEGRATOR_ENET_FIXED_BUF_ADRS

/*
 * Provide definitions for the buffer address for the three possible PCI cards.
 * The SDRAM alias address should be available on all boards.
 */

#define INTEGRATOR_ENET_PRIMARY_BUF_ADRS INTEGRATOR_SDRAM_ALIAS_BASE

/* next two areas require a later board, or the FPGAs to have been upgraded */

#define INTEGRATOR_ENET_SECONDARY_BUF_ADRS INTEGRATOR_MBRD_SSRAM_BASE
#define INTEGRATOR_ENET_TERTIARY_BUF_ADRS (INTEGRATOR_MBRD_SSRAM_BASE + 0x40000)
#endif /* INTEGRATOR_ENET_FIXED_BUF_ADRS */


#define INTEGRATOR_DEC_BUF_SIZE	0x30000

/*
 * Assuming we are on a 720T, and three drivers are being started, the
 * smallest slot is 256 kbytes, so reduce memory usage to fit into 256 Kbyte
 * slot. This can be overridden, if we know differently.
 */

#if defined (INTEGRATOR_ENET_FIXED_BUF_ADRS)
#define INTEGRATOR_FEI_BUF_SIZE	0x40000
#define INTEGRATOR_FEI_NUM_CFDS	28
#define INTEGRATOR_FEI_NUM_RFDS	28
#else
#define INTEGRATOR_FEI_BUF_SIZE	0x49000
#define INTEGRATOR_FEI_NUM_CFDS	32
#define INTEGRATOR_FEI_NUM_RFDS	32
#endif


/*
 * Max number of END devices we support - we currently set to three for
 * three PCI slots.
 */

#define INTEGRATOR_MAX_END_DEVS 3

/*
 * Max number of VGA devices we support - we currently set to three for
 * three PCI slots.
 */

#define INTEGRATOR_MAX_VGA_DEVS 3


/* Definitions for LED controller and sysLedsReadWrite */

#define LED_BASE        0x1A000000
#define LED_ALPHA 	(LED_BASE + 0x00)
#define LED_LIGHTS	(LED_BASE + 0x04)
#define LED_SWITCHES	(LED_BASE + 0x08)

#define LED_STATUS	0x01

#define LED_0		0x01
#define LED_1		0x02
#define LED_2		0x04
#define LED_4		0x08

#define LED_LEDS_MASK	0x0F


#define ARM966_I_SRAM_ENABLE    (1<<12)
#define ARM966_WBUFF_ENABLE     (1<<3)
#define ARM966_D_SRAM_ENABLE    (1<<2)

/* definition for power management instrumentation */
#ifdef POWER_MGT_INSTRUMENT
#define SYS_CLK_INT_ENABLE_ENTRY_INDICATOR     UINT16 leds; \
    AMBA_TIMER_READ(LED_LIGHTS,leds); \
    leds = leds ^ LED_1; \
    AMBA_TIMER_WRITE(LED_LIGHTS, leds);

#define SYS_ONE_SHOT_ENABLE_ENTRY_INDICATOR     UINT16 leds; \
    AMBA_TIMER_READ(LED_LIGHTS,leds); \
    leds = leds | LED_0; \
    AMBA_TIMER_WRITE(LED_LIGHTS, leds);
#define SYS_ONE_SHOT_DISABLE_EXIT_INDICATOR
    AMBA_TIMER_READ(LED_LIGHTS,leds); \
    leds = leds & ~LED_0; \
    AMBA_TIMER_WRITE(LED_LIGHTS, leds);
#endif

#ifndef _ASMLANGUAGE
#ifdef INCLUDE_POWER_MGMT_CPU_BSP_SUPPORT
IMPORT void sysCpuPwrEnable (void);
#endif


#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* INCversatilepbh */
