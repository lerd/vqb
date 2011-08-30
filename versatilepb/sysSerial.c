/* sysSerial.c - ARM Integrator board serial device initialization */

/* Copyright 1999-2000 ARM Limited */
/*
 * Copyright (c) 1999, 2000, 2005, 2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
*/


#include <vxWorks.h>
#include <iv.h>
#include <intLib.h>
#include "config.h"
#include <sysLib.h>
#include "pl011.h"

/* device initialization structure */

typedef struct
    {
    UINT	vector;
    UINT32 baseAdrs;
    UINT	intLevel;
    } SYS_AMBA_CHAN_PARAS;


/* Local data structures */

LOCAL SYS_AMBA_CHAN_PARAS devParas[] =
    {
      {INT_LVL_UART_0,UART_0_BASE_ADR,INT_LVL_UART_0},
      {INT_LVL_UART_1,UART_1_BASE_ADR,INT_LVL_UART_1}
    };

LOCAL AMBA_CHAN ambaChan[N_AMBA_UART_CHANNELS];

/*
 * Array of pointers to all serial channels configured in system.
 * See sioChanGet(). It is this array that maps channel pointers
 * to standard device names.  The first entry will become "/tyCo/0",
 * the second "/tyCo/1", and so forth.
 */

LOCAL SIO_CHAN * sysSioChans [] =
    {
    &ambaChan[0].sio, /* /tyCo/0 */
    &ambaChan[1].sio, /* /tyCo/1 */
    };


/* forward declarations */

#ifdef	INCLUDE_SIO_POLL
LOCAL int sysSerialPollConsoleOut (int arg, char *buf, int len);
#endif	/* INCLUDE_SIO_POLL */

/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiescent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiescent state.  It is called from sysHwInit() with
* interrupts locked.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit()
*/

void sysSerialHwInit (void)
    {
    int i;

    for (i = 0; i < N_AMBA_UART_CHANNELS; i++)
	{
	ambaChan[i].regs = devParas[i].baseAdrs;
	ambaChan[i].baudRate = CONSOLE_BAUD_RATE;
	ambaChan[i].xtal = UART_XTAL_FREQ;

	ambaChan[i].levelRx = devParas[i].intLevel;
	ambaChan[i].levelTx = devParas[i].intLevel;

	/*
	 * Initialize driver functions, getTxChar, putRcvChar and channelMode
	 * and initialize UART
	 */

	primeCellSioDevInit(&ambaChan[i]);
	}

#ifdef	INCLUDE_SIO_POLL
    sioIoctl (sysSioChans[SIO_POLL_CONSOLE], SIO_MODE_SET,
	      (void *) SIO_MODE_POLL);
    _func_consoleOut = sysSerialPollConsoleOut;
#endif	/* INCLUDE_SIO_POLL */
    }

#ifdef	INCLUDE_TTY_DEV
/******************************************************************************
*
* sysSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  Serial device interrupts could not be connected in
* sysSerialHwInit() because the kernel memory allocator was not initialized
* at that point, and intConnect() may call malloc().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/

void sysSerialHwInit2 (void)
    {
    int i;

    for (i = 0; i < N_AMBA_UART_CHANNELS; i++)
	{
	/*
	 * Connect and enable the interrupt.
	 * We would like to check the return value from this and log a message
	 * if it failed. However, logLib has not been initialized yet, so we
	 * cannot log a message, so there's little point in checking it.
	 */

	(void) intConnect (INUM_TO_IVEC(devParas[i].vector),
			   primeCellSioInt, (int) &ambaChan[i] );
	intEnable (devParas[i].intLevel);
	}
    }
#endif	/* INCLUDE_TTY_DEV */

/******************************************************************************
*
* sysSerialChanGet - get the SIO_CHAN device associated with a serial channel
*
* This routine returns a pointer to the SIO_CHAN device associated with
* a specified serial channel.  It is called by usrRoot() to obtain
* pointers when creating the system serial devices '/tyCo/x'.  It is also
* used by the WDB agent to locate its serial channel.
*
* RETURNS: A pointer to the SIO_CHAN structure for the channel, or ERROR
* if the channel is invalid.
*/

SIO_CHAN * sysSerialChanGet
    (
    int channel         /* serial channel */
    )
    {
    if (channel < 0 || channel >= (int)(NELEMENTS(sysSioChans)))
	return (SIO_CHAN *)ERROR;

    return sysSioChans[channel];
    }

#ifdef	INCLUDE_TTY_DEV
/******************************************************************************
*
* sysSerialReset - reset the sio devices to a quiet state
*
* Reset all devices to prevent them from generating interrupts.
*
* This is called from sysToMonitor to shutdown the system gracefully before
* transferring to the boot ROMs.
*
* RETURNS: N/A
*/

void sysSerialReset (void)
    {
    int i;

    for (i = 0; i < N_AMBA_UART_CHANNELS; i++)
	{
	/* disable serial interrupts */

	intDisable (devParas[i].intLevel);
	}
    }
#endif	/* INCLUDE_TTY_DEV */

#ifdef	INCLUDE_SIO_POLL
/******************************************************************************
*
* sysSerialPollConsoleOut - poll out routine
*
* This function prints by polling.
*
* RETURNS: bytes sent to console
*/

LOCAL int sysSerialPollConsoleOut
    (
    int    arg,
    char * buf,
    int    len
    )
    {
    char c;
    int  bytesOut = 0;

    if ((len <= 0) || (buf == NULL))
	return (0);

    while ((bytesOut < len) && ((c = *buf++) != EOS))
	{
	while (sioPollOutput (sysSioChans[SIO_POLL_CONSOLE], c) == EAGAIN);
	bytesOut++;

	if (c == '\n')
	    while (sioPollOutput (sysSioChans[SIO_POLL_CONSOLE], '\r') == EAGAIN);
	}

    return (bytesOut);
    }
#endif	/* INCLUDE_SIO_POLL */
