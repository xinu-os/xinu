/*
 * @file     tcpStat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Transmission Control Protocol status command.
 * @param tcbptr pointer to transmission control block
 */
void tcpStat(struct tcb *tcbptr)
{
    device *pdev;
    uchar state, devstate, opentype;
    struct netaddr localip, remoteip;
    ushort localpt, remotept;
    tcpseq rcvnxt, rcvwnd;
    tcpseq snduna, sndnxt;
    uint sndwnd;
    uint istart, icount, ibytes;
    uint ostart, ocount, obytes;
    char strA[20];
    char strB[20];

    if (NULL == tcbptr)
    {
        return;
    }

    /* Grab useful fields out of TCB -- atomically */
    wait(tcbptr->mutex);

    devstate = tcbptr->devstate;
    pdev = (device *)&devtab[tcbptr->dev];
    state = tcbptr->state;
    opentype = tcbptr->opentype;

    localip = tcbptr->localip;
    remoteip = tcbptr->remoteip;
    localpt = tcbptr->localpt;
    remotept = tcbptr->remotept;

    rcvnxt = tcbptr->rcvnxt;
    rcvwnd = tcbptr->rcvwnd;
    snduna = tcbptr->snduna;
    sndnxt = tcbptr->sndnxt;
    sndwnd = tcbptr->sndwnd;

    istart = tcbptr->istart;
    icount = tcbptr->icount;
    ibytes = tcbptr->ibytes;
    ostart = tcbptr->ostart;
    ocount = tcbptr->ocount;
    obytes = tcbptr->obytes;

    signal(tcbptr->mutex);

    /* Skip interface if not allocated */
    if (devstate != TCP_ALLOC)
    {
        printf("BLOCK%-3d   Inactive\n", tcbptr - tcptab);
        return;
    }

    /* print out device name */
    printf("%-10s ", pdev->name);

    switch (state)
    {
    case TCP_CLOSED:
        sprintf(strA, "Closed");
        break;
    case TCP_LISTEN:
        sprintf(strA, "Listen");
        break;
    case TCP_SYNSENT:
        sprintf(strA, "Syn Sent");
        break;
    case TCP_SYNRECV:
        sprintf(strA, "Syn Recv'd");
        break;
    case TCP_ESTAB:
        sprintf(strA, "Established");
        break;
    case TCP_FINWT1:
        sprintf(strA, "Fin Wait 1");
        break;
    case TCP_FINWT2:
        sprintf(strA, "Fin Wait 2");
        break;
    case TCP_CLOSEWT:
        sprintf(strA, "Close Wait");
        break;
    case TCP_CLOSING:
        sprintf(strA, "Closing");
        break;
    case TCP_LASTACK:
        sprintf(strA, "Last Ack");
        break;
    case TCP_TIMEWT:
        sprintf(strA, "Time Wait");
        break;
    default:
        sprintf(strA, "Unknown");
        break;
    }
    switch (opentype)
    {
    case TCP_PASSIVE:
        sprintf(strB, "Passive");
        break;
    case TCP_ACTIVE:
        sprintf(strB, "Active");
        break;
    default:
        sprintf(strB, "Unknown");
        break;
    }
    printf("State: %-11s    Open Type: %-7s\n", strA, strB);
    printf("           ");

    /* Connection details */
    netaddrsprintf(strA, &localip);
    printf("Local  Port: %-5d    IP: %-15s\n", localpt, strA);
    printf("           ");
    netaddrsprintf(strA, &remoteip);
    printf("Remote Port: %-5d    IP: %-15s\n", remotept, strA);

    /* Sequence numbers */
    printf("           ");
    printf("Rcv Nxt: %-10u   Wnd: %-10u\n", rcvnxt,
           tcpSeqdiff(rcvwnd, rcvnxt));
    printf("           ");
    printf("Snd Una: %-10u   Nxt: %-10u   Wnd: %-10u\n",
           snduna, sndnxt, sndwnd);

    /* Buffers */
    printf("           ");
    printf("In  Start: %-10u Count: %-10u Read %-10u\n",
           istart, icount, ibytes);
    printf("           ");
    printf("Out Start: %-10u Count: %-10u Read %-10u\n",
           ostart, ocount, obytes);
    printf("\n");

    return;
}
