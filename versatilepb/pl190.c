/* ambaIntrCtl.c - AMBA interrupt controller driver */

/*
modification history
--------------------
01b,15sep11,d_l  add SIC support.
01a,13aug11,d_l  created from ambaIntcrl.c
*/

#include "vxWorks.h"
#include "config.h"
#include "intLib.h"

IMPORT int ffsLsb (UINT32);

/* Defines from config.h, or <bsp>.h */

#define AMBA_INT_VEC_BASE	(0x0)

/* hardware access methods */

#ifndef AMBA_INT_REG_READ
#define AMBA_INT_REG_READ(reg,result) \
	((result) = *(volatile UINT32 *)(reg))
#endif /*AMBA_INT_REG_READ*/

#ifndef AMBA_INT_REG_WRITE
#define AMBA_INT_REG_WRITE(reg,data) \
	(*((volatile UINT32 *)(reg)) = (data))
#endif /*AMBA_INT_REG_WRITE*/


/* Convert level number to vector number */

#ifndef AMBA_INT_LVL_VEC_MAP
#define AMBA_INT_LVL_VEC_MAP(level, vector) \
	((vector) = ((level) + AMBA_INT_VEC_BASE))
#endif /* AMBA_INT_LVL_VEC_MAP */


#ifndef	AMBA_INT_PRIORITY_MAP
/* Convert pending register value, to a level number */

#ifndef AMBA_INT_PEND_LVL_MAP
#define AMBA_INT_PEND_LVL_MAP(pendReg, level) \
	((level) = (pendReg))
#endif /* AMBA_INT_PEND_LVL_MAP */
#endif /* AMBA_INT_PRIORITY_MAP */

/* driver constants */

#define AMBA_INT_ALL_ENABLED	(AMBA_INT_NUM_LEVELS)
#define AMBA_INT_ALL_DISABLED	0

/* registers */


/* Local data */

/* Current interrupt level setting (ambaIntLvlChg). */

LOCAL UINT32 ambaIntLvlCurrent = AMBA_INT_ALL_DISABLED; /* all levels disabled*/

/*
 * A mask word.  Bits are set in this word when a specific level
 * is enabled. It is used to mask off individual levels that have
 * not been explicitly enabled.
 */

LOCAL UINT32 ambaIntLvlEnabled;
LOCAL UINT32 ambaIntLvlEnabledSIC;

/* use this table for INT 0-31*/
LOCAL UINT32 ambaIntLvlMask [AMBA_INT_NUM_LEVELS + 1] = /* int lvl mask */
    {
    0x0000,                             /* level 0, all disabled */
    0x00000001, 0x00000003, 0x00000007, 0x0000000f,
    0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
    0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
    0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
    0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
    0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
    0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
    0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,
    /* level 1-32 all enabled */
    };


/* forward declarations */

STATUS	ambaIntLvlVecChk (int*, int*);
STATUS  ambaIntLvlVecAck (int, int);
int	ambaIntLvlChg (int);
STATUS	ambaIntLvlEnable (int);
STATUS	ambaIntLvlDisable (int);

/*******************************************************************************
*
* ambaIntDevInit - initialize the interrupt controller
*
* This routine will initialize the interrupt controller device, disabling all
* interrupt sources.  It will also connect the device driver specific routines
* into the architecture level hooks.  If the BSP needs to create a wrapper
* routine around any of the arhitecture level routines, it should install the
* pointer to the wrapper routine after calling this routine.
*
* If used with configurable priorities (#define AMBA_INT_PRIORITY_MAP),
* before this routine is called, ambaIntLvlPriMap should be initialised
* as a list of interrupt bits to poll in order of decreasing priority and
* terminated by an entry containing -1.  If ambaIntLvlPriMap is a null
* pointer (or an empty list), the priority scheme used will be
* least-significant bit first.  This is equivalent to the scheme used if
* AMBA_INT_PRIORITY_MAP is not defined but slightly less efficient.
*
* The return value ERROR indicates that the contents of
* ambaIntLvlPriMap (if used) were invalid.
*
* RETURNS: OK or ERROR if ambaIntLvlPriMap invalid.
*/

int ambaIntDevInit (void)
    {

    /* install the driver routines in the architecture hooks */

    sysIntLvlVecChkRtn	= ambaIntLvlVecChk;
    sysIntLvlVecAckRtn	= ambaIntLvlVecAck;
    sysIntLvlChgRtn	= ambaIntLvlChg;
    sysIntLvlEnableRtn	= ambaIntLvlEnable;
    sysIntLvlDisableRtn	= ambaIntLvlDisable;

    ambaIntLvlEnabled = 0; 	/* all sources disabled */
    ambaIntLvlEnabledSIC = 0; 	/* all sources disabled */
    ambaIntLvlChg (AMBA_INT_ALL_ENABLED); /* enable all levels */

    return OK;
    }

/*******************************************************************************
*
* ambaIntLvlVecChk - check for and return any pending interrupts
*
* This routine interrogates the hardware to determine the highest priority
* interrupt pending.  It returns the vector associated with that interrupt, and
* also the interrupt priority level prior to the interrupt (not the
* level of the interrupt).  The current interrupt priority level is then
* raised to the level of the current interrupt so that only higher priority
* interrupts will be accepted until this interrupt is finished.
*
* This routine must be called with CPU interrupts disabled.
*
* The return value ERROR indicates that no pending interrupt was found and
* that the level and vector values were not returned.
*
* RETURNS: OK or ERROR if no interrupt is pending.
*/

STATUS  ambaIntLvlVecChk
    (
    int* pLevel,  /* ptr to receive old interrupt level */
    int* pVector  /* ptr to receive current interrupt vector */
    )
    {
    int newLevel, newSicLevel;
    UINT32 isr, sicStatus;
    

    /* Read pending interrupt register and mask undefined bits */

    AMBA_INT_REG_READ (AMBA_INT_CSR_PEND, isr);

    /* If no interrupt is pending, return ERROR */

    if (isr == 0)
	return ERROR;
	
	/* only bit 31 is set */
	
	if ((isr & 0x7fffffff) == 0)
	    {
	    sicStatus = *(volatile UINT32 *) SIC_STATUS;
	    newSicLevel = ffsLsb (sicStatus);
	    if (newSicLevel == 0)
	        return ERROR;
        --newSicLevel;		/* ffsLsb returns numbers from 1, not 0 */	
        newLevel = newSicLevel + 32;  
	    }
    else
    {
    /* find first bit set in ISR, starting from lowest-numbered bit */

    if (newLevel = ffsLsb (isr), newLevel == 0)
	return ERROR;

    --newLevel;		/* ffsLsb returns numbers from 1, not 0 */
    }
    /* change to new interrupt level, returning previous level to caller */

    *pVector = newLevel;
    
    *pLevel = ambaIntLvlChg (newLevel);


    /* fetch, or compute the interrupt vector number */

    return OK;
    }

/*******************************************************************************
*
* ambaIntLvlVecAck - acknowledge the current interrupt
*
* Acknowledge the current interrupt cycle.  The level and vector values are
* those generated during the ambaIntLvlVecChk() routine for this interrupt
* cycle.  The basic action is to reset the current interrupt and return
* the interrupt level to its previous setting.  Note that the AMBA interrupt
* controller does not need an acknowledge cycle.
*
* RETURNS: OK or ERROR if a hardware fault is detected.
*/

STATUS  ambaIntLvlVecAck
    (
    int level,	/* old interrupt level to be restored */
    int vector	/* current interrupt vector, if needed */
    )
    {
    /* restore the previous interrupt level */

    ambaIntLvlChg (level);

    return OK;
    }

/*******************************************************************************
*
* ambaIntLvlChg - change the interrupt level value
*
* This routine implements the overall interrupt setting.  All levels
* up to and including the specifed level are disabled.  All levels above
* the specified level will be enabled, but only if they were specifically
* enabled by the ambaIntLvlEnable() routine.
*
* The specific priority level AMBA_INT_NUM_LEVELS is valid and represents
* all levels enabled.
*
* RETURNS: Previous interrupt level.
*/

int  ambaIntLvlChg
    (
    int level	/* new interrupt level */
    )
    {
    int oldLevel;

    oldLevel = ambaIntLvlCurrent;

    if (level >= 0 &&
        level <= AMBA_INT_NUM_LEVELS)
	{
	/* change current interrupt level */

	ambaIntLvlCurrent = level;
	}

    if (ambaIntLvlCurrent <32)
    {
    /* Switch off all interrupts */
    
    AMBA_INT_REG_WRITE (AMBA_INT_CSR_DIS, (UINT32)-1);

    /* Activate the enabled interrupts */

    AMBA_INT_REG_WRITE (AMBA_INT_CSR_ENB,
		(ambaIntLvlMask[ambaIntLvlCurrent] & ambaIntLvlEnabled));
    }
    else
    {
    *(volatile UINT32 *)SIC_ENCLR = 0xffffffff;
    *(volatile UINT32 *)SIC_ENSET = (ambaIntLvlMask[ambaIntLvlCurrent - 32] & ambaIntLvlEnabledSIC);
    }
    return oldLevel;
    }

/*******************************************************************************
*
* ambaIntLvlEnable - enable a single interrupt level
*
* Enable a specific interrupt level.  The enabled level will be allowed
* to generate an interrupt when the overall interrupt level is set to
* enable interrupts of this priority (as configured by ambaIntLvlPriMap,
* if appropriate).  Without being enabled, the interrupt is blocked
* regardless of the overall interrupt level setting.
*
* RETURNS: OK or ERROR if the specified level cannot be enabled.
*/

STATUS  ambaIntLvlEnable
    (
    int level  /* level to be enabled */
    )
    {
    int key;

    if (level < 0 ||
	level >= AMBA_INT_NUM_LEVELS)
	return ERROR;


    /* set bit in enable mask */

    key = intLock ();
    if (level < 32)
        {
        ambaIntLvlEnabled |= (1 << level);
        }
    else
        {
        ambaIntLvlEnabledSIC |= (1 << (level - 32));    
        }
    intUnlock (key);

    ambaIntLvlChg (-1);	/* reset current mask */

    return OK;
    }

/*******************************************************************************
*
* ambaIntLvlDisable - disable a single interrupt level
*
* Disable a specific interrupt level.  The disabled level is prevented
* from generating an interrupt even if the overall interrupt level is
* set to enable interrupts of this priority (as configured by
* ambaIntLvlPriMap, if appropriate).
*
* RETURNS: OK or ERROR, if the specified interrupt level cannot be disabled.
*/

STATUS  ambaIntLvlDisable
    (
    int level  /* level to be disabled */
    )
    {
    int key;

    if (level < 0 ||
	level >= AMBA_INT_NUM_LEVELS)
	return ERROR;


    /* clear bit in enable mask */

    key = intLock ();
    if (level < 32)
        {
        ambaIntLvlEnabled &= ~(1 << level);
        }
    else
        {
        ambaIntLvlEnabledSIC &= ~(1 << (level - 32));    
        }
    intUnlock (key);

    ambaIntLvlChg (-1);	/* reset current mask */

    return OK;
    }
