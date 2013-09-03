/**
 * @file tcpSetup.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <clock.h>
#include <network.h>
#include <semaphore.h>
#include <tcp.h>

static uint tcpIss(void);

/**
 * @ingroup tcp
 *
 * Intializes a transmission control block.
 * @prarm tcbptr TCB for connection
 * @return OK if TCB is initialized properly, otherwise SYSERR
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpSetup(struct tcb *tcbptr)
{
    /* Error check parameters */
    if (NULL == tcbptr)
    {
        return SYSERR;
    }

    /* Intialize connection semaphore */
    tcbptr->openclose = semcreate(0);

    /* Initialize input buffer */
    tcbptr->istart = 0;
    tcbptr->inxt = 0;
    tcbptr->icount = 0;
    tcbptr->ibytes = 0;
    tcbptr->readers = semcreate(0);

    /* Initialize output buffer */
    tcbptr->ostart = 0;
    tcbptr->ocount = 0;
    tcbptr->obytes = 0;
    tcbptr->writers = semcreate(1);

    /* Initialize send fields */
    tcbptr->iss = tcpIss();
    tcbptr->snduna = tcbptr->iss;
    tcbptr->sndnxt = tcbptr->iss;
    tcbptr->sndwl2 = tcbptr->iss;
    tcbptr->sndmss = TCP_INIT_MSS;
    tcbptr->sndflg = NULL;
    tcbptr->sndcwn = tcbptr->sndmss;
    tcbptr->sndsst = TCP_MAX_WND;
    tcbptr->rxttime = TCP_RXT_INITTIME;
    tcbptr->rxtcount = 0;
    tcbptr->psttime = TCP_PST_INITTIME;

    /* Initialize receive fields */
    tcbptr->rcvmss = TCP_INIT_MSS - TCP_HDR_LEN;
    tcbptr->rcvflg = NULL;

    /* Verify creation of semaphores */
    if ((SYSERR == (int)tcbptr->openclose)
        || (SYSERR == (int)tcbptr->readers)
        || (SYSERR == (int)tcbptr->writers))
    {
        return SYSERR;
    }

    return OK;
}

/*
 * Provides an initial send sequence number.
 * @return initial send sequence number
 */
static uint tcpIss(void)
{
    static int nextseq = 0;

    if (0 == nextseq)
    {
        nextseq = clktime;
    }
    nextseq += TCP_SEQINCR;
    return nextseq;
}
