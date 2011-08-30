/* primeCellSio.c - ARM AMBA UART tty driver */

/*
 * Copyright (c) 1997-2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */


/*
modification history
--------------------
*/
/*
The following example shows the first parts of the initialisation:

\cs
#include "primeCellSio.h"

LOCAL AMBA_CHAN ambaChan[N_AMBA_UART_CHANS];

void sysSerialHwInit (void)
    {
    int i;

    for (i = 0; i < N_AMBA_UART_CHANS; i++)
	{
	ambaChan[i].regs = devParas[i].baseAdrs;
	ambaChan[i].baudRate = CONSOLE_BAUD_RATE;
	ambaChan[i].xtal = UART_XTAL_FREQ; 

	ambaChan[i].levelRx = devParas[i].intLevelRx;
	ambaChan[i].levelTx = devParas[i].intLevelTx;

	/@
	 * Initialise driver functions, getTxChar, putRcvChar and
	 * channelMode, then initialise UART
	 @/

	ambaDevInit(&ambaChan[i]);
	}
    }
\ce

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(),
which connects the chips interrupts via intConnect() (the two
interrupts `ambaIntTx' and `ambaIntRx') and enables those interrupts,
as shown in the following example:

\cs

void sysSerialHwInit2 (void)
    {
    /@ connect and enable Rx interrupt @/

    (void) intConnect (INUM_TO_IVEC(devParas[0].vectorRx),
		       ambaIntRx, (int) &ambaChan[0]);
    intEnable (devParas[0].intLevelRx);


    /@ connect Tx interrupt @/

    (void) intConnect (INUM_TO_IVEC(devParas[0].vectorTx),
		       ambaIntTx, (int) &ambaChan[0]);
    /@
     * There is no point in enabling the Tx interrupt, as it will
     * interrupt immediately and then be disabled.
     @/

    }
\ce

*/

#include <vxWorks.h>
#include <intLib.h>
#include <errnoLib.h>
#include <errno.h>
#include <sioLib.h>
#include "pl011.h"

/* local defines  */

#ifndef AMBA_UART_REG
#define AMBA_UART_REG(pChan, reg) \
	(*(volatile UINT32 *)((UINT32)(pChan)->regs + (reg)))
#endif

#ifndef AMBA_UART_REG_READ
#define AMBA_UART_REG_READ(pChan, reg, result) \
	(result) = (AMBA_UART_REG(pChan, reg))
#endif

#ifndef AMBA_UART_REG_WRITE
#define AMBA_UART_REG_WRITE(pChan, reg, data) \
	(AMBA_UART_REG(pChan, reg)) = (data)
#endif

#ifndef AMBA_UART_REG_BIT_SET
#define AMBA_UART_REG_BIT_SET(pChan, reg, data) \
	(AMBA_UART_REG(pChan, reg)) |= (data)
#endif

#ifndef AMBA_UART_REG_BIT_CLR
#define AMBA_UART_REG_BIT_CLR(pChan, reg, data) \
	(AMBA_UART_REG(pChan, reg)) &= ~(data)
#endif

/* locals */

/* function prototypes */

LOCAL STATUS ambaDummyCallback (void);
LOCAL void   ambaInitChannel (AMBA_CHAN * pChan);
LOCAL STATUS ambaIoctl (SIO_CHAN * pSioChan, int request, int arg);
#ifdef	INCLUDE_TTY_DEV
LOCAL int    ambaTxStartup (SIO_CHAN * pSioChan);
LOCAL int    ambaCallbackInstall (SIO_CHAN * pSioChan, int callbackType,
			       STATUS (*callback)(), void * callbackArg);
#endif	/* INCLUDE_TTY_DEV */
LOCAL int    ambaPollInput (SIO_CHAN * pSioChan, char *);
LOCAL int    ambaPollOutput (SIO_CHAN * pSioChan, char);

/* driver functions */

LOCAL SIO_DRV_FUNCS ambaSioDrvFuncs =
    {
    (int (*)())                 ambaIoctl,
#ifdef	INCLUDE_TTY_DEV
    (int (*)())                 ambaTxStartup,
    (int (*)())                 ambaCallbackInstall,
#else	/* INCLUDE_TTY_DEV */
    (int (*)())                 NULL,
    (int (*)())                 NULL,
#endif	/* INCLUDE_TTY_DEV */
    (int (*)())                 ambaPollInput,
    (int (*)(SIO_CHAN *,char))  ambaPollOutput
    };

/*******************************************************************************
*
* ambaDummyCallback - dummy callback routine.
*
* This routine does nothing.
*
* RETURNS: ERROR, always.
*
* ERRNO
*/

LOCAL STATUS ambaDummyCallback (void)
    {
    return ERROR;
    }

/*******************************************************************************
*
* primeCellSioDevInit - initialise an AMBA channel
*
* This routine initialises some SIO_CHAN function pointers and then resets
* the chip to a quiescent state.  Before this routine is called, the BSP
* must already have initialised all the device addresses, etc. in the
* AMBA_CHAN structure.
*
* RETURNS: N/A
*
* ERRNO
*/

void primeCellSioDevInit
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    int oldlevel = intLock();


    /* initialise the driver function pointers in the SIO_CHAN */

    pChan->sio.pDrvFuncs = &ambaSioDrvFuncs;


    /* set the non BSP-specific constants */

    pChan->getTxChar = ambaDummyCallback;
    pChan->putRcvChar = ambaDummyCallback;

    pChan->channelMode = 0;    /* undefined */


    /* initialise the chip */

    ambaInitChannel (pChan);


    intUnlock (oldlevel);

    }

/*******************************************************************************
*
* ambaInitChannel - initialise UART
*
* This routine performs hardware initialisation of the UART channel.
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void ambaInitChannel
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    int		i;
    UINT32	discard;
    

    /* Fill Tx FIFO with nulls (data sheet specifies this) */

    for (i = 0; i < 16; i++)
        AMBA_UART_REG_WRITE(pChan, UARTDR, 0);


    /* Program UART control register */

    AMBA_UART_REG_WRITE(pChan, UARTCON, 0);
    AMBA_UART_REG_WRITE(pChan, UARTCON, (3<<8)|UART_ENABLE);

    /* Ensure that only Receive ints are generated. */
    AMBA_UART_REG_WRITE(pChan, UARTIMSC, UART_RIE | UART_RTIE);
    AMBA_UART_REG_WRITE(pChan, UARTIFLS, 0x10);


    /* Set baud rate divisor */
#if 0
    AMBA_UART_REG_WRITE(pChan, L_UBRLCR,
		    (pChan->xtal / (16 * pChan->baudRate) - 1) & 0xFF);
    AMBA_UART_REG_WRITE(pChan, M_UBRLCR,
		    ((pChan->xtal / (16 * pChan->baudRate) - 1) >> 8) & 0xF);
#endif
    /*
     * Set word format, enable FIFOs: set 8 bits, 1 stop bit, no parity.
     * This also latches the writes to the two (sub)registers above.
     */

    AMBA_UART_REG_WRITE(pChan, UARTLCR_H,
	(UINT8)(WORD_LEN_8 | ONE_STOP | PARITY_NONE | FIFO_ENABLE));
 

    /* Clear Rx FIFO (data sheet specifies this) */

    for (i = 0; i < 16; i++)
        AMBA_UART_REG_READ(pChan, RXSTAT, discard);

    }

/*******************************************************************************
*
* ambaIoctl - special device control
*
* This routine handles the IOCTL messages from the user.
*
* RETURNS: OK on success
*
* ERRNO:  ENOSYS on unsupported request, EIO on failed request.
*/

LOCAL STATUS ambaIoctl
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		request,	/* request code */
    int		arg		/* some argument */
    )
    {
    int		oldlevel;	/* current interrupt level mask */
    STATUS	status;		/* status to return */
    UINT32	brd;		/* baud rate divisor */
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;

    status = OK;	/* preset to return OK */

    switch (request)
        {
        case SIO_BAUD_SET:
            /*
             * Set the baud rate. Return EIO for an invalid baud rate, or
             * OK on success.
             */

            /*
             * baudrate divisor must be non-zero and must fit in a 12-bit
             * register.
             */

            brd = (pChan->xtal/(16*arg)) - 1;   /* calculate baudrate divisor */

            if ((brd < 1) || (brd > 0xFFF))
                {
                status = EIO;       /* baud rate out of range */
                break;
                }

            /* disable interrupts during chip access */

            oldlevel = intLock ();


            /* Set baud rate divisor in UART */
#if 0
            AMBA_UART_REG_WRITE(pChan, L_UBRLCR, brd & 0xFF);
            AMBA_UART_REG_WRITE(pChan, M_UBRLCR, (brd >> 8) & 0xF);
#endif
            /*
             * Set word format, enable FIFOs: set 8 bits, 1 stop bit, no parity.
             * This also latches the writes to the two (sub)registers above.
             */

            AMBA_UART_REG_WRITE(pChan, UARTLCR_H,
                                (UINT8)(WORD_LEN_8 | ONE_STOP | PARITY_NONE | FIFO_ENABLE));

            pChan->baudRate = arg;

            intUnlock (oldlevel);

            break;

#ifdef	INCLUDE_TTY_DEV
        case SIO_BAUD_GET:

            /* Get the baud rate and return OK */

            *(int *)arg = pChan->baudRate;
            break; 


        case SIO_MODE_SET:

            /*
             * Set the mode (e.g., to interrupt or polled). Return OK
             * or EIO for an unknown or unsupported mode.
             */

            if ((arg != SIO_MODE_POLL) && (arg != SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            oldlevel = intLock ();

            if (arg == SIO_MODE_INT)
                {
                /* Ensure that only Receive ints are generated. */
                AMBA_UART_REG_WRITE(pChan, UARTIMSC, UART_RIE | UART_RTIE);

                /* Enable appropriate interrupts. */
                intEnable (pChan->levelRx);

                /*
                 * There is no point in enabling the Tx interrupt, as it
                 * will interrupt immediately and be disabled.
                 */  
                }
            else
                {
                /* Disable all interrupts for this UART. */ 

                intDisable (pChan->levelRx);
                if (pChan->levelTx != pChan->levelRx)
                    {
                    intDisable (pChan->levelTx);
                    }

                AMBA_UART_REG_WRITE(pChan, UARTIMSC, 0);
                }

            pChan->channelMode = arg;

            intUnlock (oldlevel);
            break;      


        case SIO_MODE_GET:

            /* Get the current mode and return OK */

            *(int *)arg = pChan->channelMode;
            break;


        case SIO_AVAIL_MODES_GET:

            /* Get the available modes and return OK */

            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            break;


        case SIO_HW_OPTS_SET:

            /*
             * Optional command to set the hardware options (as defined
             * in sioLib.h).
             * Return OK, or ENOSYS if this command is not implemented.
             * Note: several hardware options are specified at once.
             * This routine should set as many as it can and then return
             * OK. The SIO_HW_OPTS_GET is used to find out which options
             * were actually set.
             */

        case SIO_HW_OPTS_GET:

            /*
             * Optional command to get the hardware options (as defined
             * in sioLib.h). Return OK or ENOSYS if this command is not
             * implemented.  Note: if this command is not implemented, it
             * will be assumed that the driver options are CREAD | CS8
             * (e.g., eight data bits, one stop bit, no parity, ints enabled).
             */
#endif	/* INCLUDE_TTY_DEV */

        default:
            status = ENOSYS;
        }

    return status;

    }

#ifdef	INCLUDE_TTY_DEV
/*******************************************************************************
*
* primeCellSioIntTx - handle a transmitter interrupt 
*
* This routine handles write interrupts from the UART.
*
* RETURNS: N/A
*
* ERRNO
*/

void primeCellSioIntTx 
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    char outChar;
    int i;

    for(i = 8; i>0; i--)
    {

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)

        /* write char. to Transmit Holding Reg. */

        AMBA_UART_REG_WRITE(pChan, UARTDR, outChar);
    else
        {
        /* Disable transmit interrupts. Leave receive interrupts enabled. */
        if (pChan->levelTx != pChan->levelRx)
            intDisable (pChan->levelTx);

        AMBA_UART_REG_BIT_CLR(pChan, UARTIMSC, UART_TIE);
        break;
        }
	   } 
    }

/*****************************************************************************
*
* primeCellSioIntRx - handle a receiver interrupt 
*
* This routine handles read interrupts from the UART.
*
* RETURNS: N/A
*
* ERRNO
*/

void primeCellSioIntRx
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    char inchar;
    char flags;
    BOOL more_data = FALSE;

    /* read characters from Receive Holding Reg. */
    do
        {
        /* While RX FIFO isn't empty, we have more data to read */
        AMBA_UART_REG_READ(pChan, UARTFLG, flags);
        more_data = ( (flags & FLG_URXFE) == 0);

        if (more_data)
            {
            /* Read from data register. */
            AMBA_UART_REG_READ(pChan, UARTDR, inchar);
            (*pChan->putRcvChar) (pChan->putRcvArg, inchar);
            }

        } while (more_data);

    }

/******************************************************************************
*
* primeCellSioInt - handle any UART interrupt
*
* This routine handles interrupts from the UART and determines whether
* the source is a transmit interrupt or receive/receive-timeout interrupt.
*
* The Prime Cell UART generates a receive interrupt when the RX FIFO is
* half-full, and a receive-timeout interrupt after 32 bit-clocks have
* elapsed with no incoming data.
*
* RETURNS: N/A
*
* ERRNO
*/

void primeCellSioInt
    (
    AMBA_CHAN * pChan   /* ptr to AMBA_CHAN describing this channel */
    )
    {
    char intId;

    AMBA_UART_REG_READ(pChan, UARTMIS, intId);
    

    if (intId & UART_TIS)
        {
        primeCellSioIntTx (pChan);
        }

    if ((intId & UART_RIS) || (intId & UART_RTIS))
        {
        primeCellSioIntRx (pChan);
        }


    } 


/*******************************************************************************
*
* ambaTxStartup - transmitter startup routine
*
* Enable interrupt so that interrupt-level char output routine will be called.
*
* RETURNS: OK on success
*
* ERRNO: ENOSYS if the device is polled-only, or EIO on hardware error.
*/

LOCAL int ambaTxStartup
    (
    SIO_CHAN *	pSioChan	/* ptr to SIO_CHAN describing this channel */
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;


    if (pChan->channelMode == SIO_MODE_INT)
        {
        AMBA_UART_REG_BIT_SET(pChan, UARTIMSC, UART_TIE);

        return OK;
        }
    else
        return ENOSYS;
    }
#endif	/* INCLUDE_TTY_DEV */

/******************************************************************************
*
* ambaPollOutput - output a character in polled mode.
*
* This routine sends a character in polled mode.
*
* RETURNS: OK if a character arrived
*
* ERRNO: EIO on device error, EAGAIN if the output buffer is full, 
* ENOSYS if the device is interrupt-only.
*/

LOCAL int ambaPollOutput
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    char	outChar 	/* char to output */
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;
    FAST UINT32 pollStatus;

    AMBA_UART_REG_READ(pChan, UARTFLG, pollStatus);

    /* is the transmitter ready to accept a character? */

    if ((pollStatus & FLG_UTXFF) != 0x00)
        return EAGAIN;


    /* write out the character */

    AMBA_UART_REG_WRITE(pChan, UARTDR, outChar);    /* transmit character */

    return OK;
    }

/******************************************************************************
*
* ambaPollInput - poll the device for input.
*
* This routines polls for an input character.
*
* RETURNS: OK if a character arrived
*
* ERRNO: EIO on device error, EAGAIN if the input buffer is empty, 
* ENOSYS if the device is interrupt-only.
*/

LOCAL int ambaPollInput
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    char *	thisChar	/* pointer to where to return character */
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;
    FAST UINT32 pollStatus;

    AMBA_UART_REG_READ(pChan, UARTFLG, pollStatus);

    if ((pollStatus & FLG_URXFE) != 0x00)
        return EAGAIN;


    /* got a character */

    AMBA_UART_REG_READ(pChan, UARTDR, *thisChar);

    return OK;

    }

#ifdef	INCLUDE_TTY_DEV
/******************************************************************************
*
* ambaCallbackInstall - install ISR callbacks to get/put chars.
*
* This routine installs interrupt callbacks for transmitting characters
* and receiving characters.
*
* RETURNS: OK on success
*
* ERRNO: ENOSYS for an unsupported callback type.
*/

LOCAL int ambaCallbackInstall
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		callbackType,	/* type of callback */
    STATUS	(*callback)(),	/* callback */
    void *	callbackArg	/* parameter to callback */
		 
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg = callbackArg;
            return OK;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return OK;

        default:
            return ENOSYS;
        }

    }
#endif	/* INCLUDE_TTY_DEV */
