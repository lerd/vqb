/* ambaSio.h - ARM AMBA UART header file */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,21feb00,jpd  renamed primecell... symbols to primeCell...
01a,10nov99,ajb  Modified from ambaSio.h, version 01b.  Added support for
                 enabling interrupts in UART_CR.
*/

#ifndef __INCprimeCellSioh 
#define __INCprimeCellSioh 

#ifdef __cplusplus
extern "C" {
#endif

/* Register description OF ARM AMBA UART */

#ifndef _ASMLANGUAGE


/* Pl011 Register offsets from base address */

#define	UARTDR		0x00		/* UART data register (R/W) */
#define	RXSTAT		0x04		/* Rx data status register (R/O) */
#define	UMSEOI		0x04		/* Clr modem status changed int (W/O) */
#if 0
#define	H_UBRLCR	0x08		/* } High middle and low bytes (R/W) */
#define	M_UBRLCR	0x0C		/* } of bit rate and line (W/O) */
#define	L_UBRLCR	0x10		/* } register (W/O) */
#define	UARTCON		0x14		/* control register (R/W) */
#endif
#define	UARTFLG		0x18		/* FIFO status register (R/O) */
#if 0
#define UARTIIR		0x1C		/* Int identification reg (R/O) */
#define UARTICR		0x1C		/* Int clear register (W/O) */
#endif

#define UARTILPR		0x20
#define UARTIBRD		0x24
#define UARTFBRD		0x28
#define UARTLCR_H		0x2C
#define UARTCON		0x30
#define UARTIFLS		0x34
#define UARTIMSC		0x38
#define UARTRIS		0x3C
#define UARTMIS		0x40
#define UARTICR		0x44
#define UARTDMACR	0x48

/* bit definitions within H_UBRLCR register */

#define PARITY_NONE	0x00		/* set no parity */
#define ONE_STOP	0x00		/* set one stop bit */
#define FIFO_ENABLE	0x10		/* Enable both FIFOs */
#define WORD_LEN_5	(0x00 << 5)	/* Set UART word lengths */
#define WORD_LEN_6	(0x01 << 5)
#define WORD_LEN_7	(0x02 << 5)
#define WORD_LEN_8	(0x03 << 5)


/* bit definitions within UARTCR register */

#define UART_LBE	0x80		/* Loop Back Enable */
#define UART_RTIE 	0x40		/* Receive Timeout Int Enable */
#define UART_TIE	0x20		/* Transmit Int Enable */
#define UART_RIE	0x10		/* Receive Int Enable */
#define UART_MSIE	0x08		/* Modem Status Int Enable */
#define UART_ENABLE	0x01		/* Enable the UART */


/* bit definitions within UARTFR register */

#define FLG_UTXFE	(0x01 << 7)	/* UART Tx FIFO Empty */
#define FLG_URXFF	(0x01 << 6)	/* UART Rx FIFO Full */
#define FLG_UTXFF	(0x01 << 5)	/* UART Tx FIFO Full */
#define FLG_URXFE	(0x01 << 4)	/* UART Rx FIFO Empty */

/* bit definitions within UARTIIR/ICR register */

#define UART_RTIS	0x40		/* Receive Timeout Int Status */
#define UART_TIS	0x20		/* Transmit Int Status */
#define UART_RIS	0x10		/* Receive Int Status */
#define UART_MIS	0x01		/* Modem Int Status */


typedef struct AMBA_CHAN
    {
    /* must be first */

    SIO_CHAN	sio;		/* standard SIO_CHAN element */

    /* callbacks */

    STATUS	(*getTxChar) ();  /* installed Tx callback routine */
    STATUS	(*putRcvChar) (); /* installed Rx callback routine */
    void *	getTxArg;	/* argument to Tx callback routine */
    void *	putRcvArg;	/* argument to Rx callback routine */

    UINT32 	regs;		/* AMBA registers */
    UINT8 	levelRx;	/* Rx Interrupt level for this device */
    UINT8 	levelTx;	/* Tx Interrupt level for this device */

    UINT32	channelMode;	/* such as INT, POLL modes */
    int		baudRate;	/* the current baud rate */
    UINT32	xtal;		/* UART clock frequency */     

    } AMBA_CHAN;


/* function declarations */

extern void primeCellSioInt (AMBA_CHAN *pChan);
extern void primeCellSioIntTx (AMBA_CHAN *pChan);
extern void primeCellSioIntRx (AMBA_CHAN *pChan);
extern void primeCellSioDevInit (AMBA_CHAN *pChan);

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif
 
#endif /* __INCprimeCellSioh */
