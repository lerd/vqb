/* ambaTimer.c - Advanced RISC Machines AMBA timer library */

/*
modification history
--------------------
01a,18aug11,d_l  created.
*/


/*
SEE ALSO:
.I "ARM Reference Peripheral Specification, ARM DDI 0062D,"
.I "AMBA Timer Data Sheet, ARM DDI 0049B,"
.I "ARM Target Development System User Guide, ARM DUI 0061A,"
.I "Digital Semiconductor 21285 Core Logic for SA-110 Microprocessor Data
Sheet."
*/


/* includes */

#include "drv/timer/timestampDev.h"

/* defines */

#ifndef AMBA_TIMER_READ
#define AMBA_TIMER_READ(reg, result) \
	((result) = *((volatile UINT32 *)(reg)))
#endif /* AMBA_TIMER_READ */

#ifndef AMBA_TIMER_WRITE
#define AMBA_TIMER_WRITE(reg, data) \
	(*((volatile UINT32 *)(reg)) = (data))
#endif /* AMBA_TIMER_WRITE */

#ifndef AMBA_TIMER_INT_ENABLE
#define AMBA_TIMER_INT_ENABLE(level) intEnable (level)
#endif

#ifndef AMBA_TIMER_INT_DISABLE
#define AMBA_TIMER_INT_DISABLE(level) intDisable (level)
#endif

#define SYS_T1LOAD     (SYS_TIMER_BASE + 0x000)
#define SYS_T1VALUE    (SYS_TIMER_BASE + 0x004)
#define SYS_T1CTL      (SYS_TIMER_BASE + 0x008)
#define SYS_T1INTCLEAR (SYS_TIMER_BASE + 0x00C)
#define SYS_T1RIS      (SYS_TIMER_BASE + 0x010)
#define SYS_T1MIS      (SYS_TIMER_BASE + 0x014)
#define SYS_T1BGLOAD   (SYS_TIMER_BASE + 0x018)
#define SYS_T2LOAD     (SYS_TIMER_BASE + 0x020)
#define SYS_T2VALUE    (SYS_TIMER_BASE + 0x024)
#define SYS_T2CTL      (SYS_TIMER_BASE + 0x028)
#define SYS_T2INTCLEAR (SYS_TIMER_BASE + 0x02C)
#define SYS_T2RIS      (SYS_TIMER_BASE + 0x030)
#define SYS_T2MIS      (SYS_TIMER_BASE + 0x034)
#define SYS_T2BGLOAD   (SYS_TIMER_BASE + 0x038)


#define AUX_T1LOAD     (AUX_TIMER_BASE + 0x000)
#define AUX_T1VALUE    (AUX_TIMER_BASE + 0x004)
#define AUX_T1CTL      (AUX_TIMER_BASE + 0x008)
#define AUX_T1INTCLEAR (AUX_TIMER_BASE + 0x00C)
#define AUX_T1RIS      (AUX_TIMER_BASE + 0x010)
#define AUX_T1MIS      (AUX_TIMER_BASE + 0x014)
#define AUX_T1BGLOAD   (AUX_TIMER_BASE + 0x018)
#define AUX_T2LOAD     (AUX_TIMER_BASE + 0x020)
#define AUX_T2VALUE    (AUX_TIMER_BASE + 0x024)
#define AUX_T2CTL      (AUX_TIMER_BASE + 0x028)
#define AUX_T2INTCLEAR (AUX_TIMER_BASE + 0x02C)
#define AUX_T2RIS      (AUX_TIMER_BASE + 0x030)
#define AUX_T2MIS      (AUX_TIMER_BASE + 0x034)
#define AUX_T2BGLOAD   (AUX_TIMER_BASE + 0x038)

/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= 0;    /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;

#ifdef INCLUDE_AUX_CLK
LOCAL FUNCPTR sysAuxClkRoutine	= NULL;
LOCAL int sysAuxClkArg		= 0;
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkTicksPerSecond = 100;
#endif

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL	sysTimestampRunning  	= FALSE;   /* timestamp running flag */
#endif /* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*
* RETURNS: N/A.
*/

LOCAL void sysClkInt (void)
    {

    /* acknowledge interrupt: any write to Clear Register clears interrupt */

    AMBA_TIMER_WRITE (SYS_T1INTCLEAR, 0);


    /* If any routine attached via sysClkConnect(), call it */

    if (sysClkRoutine != NULL)
	(* sysClkRoutine) (sysClkArg);

    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  It does not enable system clock interrupts.
* Normally it is called from usrRoot() in usrConfig.c to connect
* usrClock() to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {
    if (sysClkConnected == FALSE)
    	{
    	sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */
    	sysClkConnected = TRUE;
    	}

    sysClkRoutine = NULL; /* ensure routine not called with wrong arg */
    sysClkArg	  = arg;

    if (routine == NULL)
	return OK;

    sysClkRoutine = routine;

    return OK;
    }

/*******************************************************************************
*
* sysClkDisable - turn off system clock interrupts
*
* This routine disables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkEnable()
*/

void sysClkDisable (void)
    {
    if (sysClkRunning)
	{
	/*
	 * Disable timer itself. Might as well leave it configured for
	 * Periodic mode, divided by 16.
	 */

	AMBA_TIMER_WRITE(SYS_T1CTL, ((1 << 6) | (1 << 2) | (1 << 1)) );


	/* Disable the timer interrupt in the Interrupt Controller */

	AMBA_TIMER_INT_DISABLE (SYS_TIMER_INT_LVL);

	sysClkRunning = FALSE;
	}
    }

/*******************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)
    {
    UINT32 tc;

    if (!sysClkRunning)
	{
	/* Set up in periodic mode, divide clock input by 16, enable timer */
	tc = (SYS_TIMER_CLK / sysClkTicksPerSecond) ;
	AMBA_TIMER_WRITE (SYS_T1LOAD, tc);

	/* enable clock interrupt in interrupt controller */

	AMBA_TIMER_INT_ENABLE (SYS_TIMER_INT_LVL);
	AMBA_TIMER_WRITE( SYS_T1CTL,
	       ( (1 << 7) |  /* EN */
	         (1 << 6) |  /* PERIODIC */
		       (1 << 5) |  /* INTEN */
		/*     (1 << 2) | */ /* 16DIV*/
		       (1 << 1)    /* 32BIT*/
		       ) );


	sysClkRunning = TRUE;
	}
    }

/*******************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the interrupt rate of the system clock.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkRateSet(), sysClkEnable()
*/

int sysClkRateGet (void)
    {
    return sysClkTicksPerSecond;
    }

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts unilaterally, but if the system clock is
* currently enabled, the clock is disabled and then re-enabled with the new
* rate.  Normally it is called by usrRoot() in usrConfig.c.
*
* RETURNS:
* OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkRateGet(), sysClkEnable()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	return ERROR;

    sysClkTicksPerSecond = ticksPerSecond;

    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}

    return OK;
    }

#ifdef	INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.
*
* RETURNS: ERROR, always.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
    )
    {
    return ERROR;
    }

/*******************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable (void)
   {
   if (sysTimestampRunning)
      {
      return OK;
      }

   if (!sysClkRunning)          /* timestamp timer is derived from the sysClk */
      return ERROR;

   sysTimestampRunning = TRUE;

   return OK;
   }

/*******************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables timestamp timer interrupts.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
        sysTimestampRunning = FALSE;

    return OK;
    }

/*******************************************************************************
*
* sysTimestampPeriod - get the period of a timestamp timer
*
* This routine gets the period of the timestamp timer, in ticks.  The
* period, or terminal count, is the number of ticks to which the timestamp
* timer counts before rolling over and restarting the counting process.
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 sysTimestampPeriod (void)
    {
    /*
     * The period of the timestamp depends on the clock rate of the system
     * clock.
     */

    return ((UINT32)(SYS_TIMER_CLK / sysClkTicksPerSecond));
    }

/*******************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per
* second.  The rate of the timestamp timer is set explicitly by the
* hardware and typically cannot be altered.
*
* NOTE: Because the system clock serves as the timestamp timer,
* the system clock frequency is also the timestamp timer frequency.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return ((UINT32)SYS_TIMER_CLK);
    }

/*******************************************************************************
*
* sysTimestamp - get a timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* This routine should be called with interrupts locked.  If interrupts are
* not locked, sysTimestampLock() should be used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    UINT32 t;

    AMBA_TIMER_READ (SYS_T1VALUE, t);

    return (sysTimestampPeriod () - t);
    }

/*******************************************************************************
*
* sysTimestampLock - lock interrupts and get the timestamp timer tick count
*
* This routine locks interrupts when the tick counter must be stopped
* in order to read it or when two independent counters must be read.
* It then returns the current value of the timestamp timer tick
* counter.
*
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestamp()
*/

UINT32 sysTimestampLock (void)
    {
    UINT32 t;

    AMBA_TIMER_READ (SYS_T1VALUE, t);

    return (sysTimestampPeriod () - t);
    }

#endif  /* INCLUDE_TIMESTAMP */

