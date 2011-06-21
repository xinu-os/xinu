/*
 * @file     tcpStat.c
 * @provides tcpStat
 *
 * $Id: tcpStat.c 2135 2009-11-20 07:23:03Z svn $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <tcp.h>

/**
 * Transmission Control Protocol status command.
 * @param tcbptr pointer to transmission control block
 */
void tcpStat(struct tcb *tcbptr)
{
    device *pdev;
    struct tcb copy;
    char strA[20];
    char strB[20];

    if (NULL == tcbptr)
    {
        return;
    }

    wait(tcbptr->mutex);
    /* Take atomic snapshot of control block */
    memcpy(&copy, tcbptr, sizeof(struct tcb));
    signal(tcbptr->mutex);

    /* Skip interface if not allocated */
    if (copy.devstate != TCP_ALLOC)
    {
        printf("BLOCK%-3d   Inactive\n", tcbptr - tcptab);
        return;
    }

    /* Setup pointer to underlying device */
    pdev = (device *)&devtab[copy.dev];
    printf("%-10s ", pdev->name);

    switch (copy.state)
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
    switch (copy.opentype)
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
    netaddrsprintf(strA, &copy.localip);
    printf("Local  Port: %-5d    IP: %-15s\n", copy.localpt, strA);
    printf("           ");
    netaddrsprintf(strA, &copy.remoteip);
    printf("Remote Port: %-5d    IP: %-15s\n", copy.remotept, strA);

    /* Sequence numbers */
    printf("           ");
    printf("Rcv Nxt: %-10u   Wnd: %-10u\n", copy.rcvnxt,
           tcpSeqdiff(copy.rcvwnd, copy.rcvnxt));
    printf("           ");
    printf("Snd Una: %-10u   Nxt: %-10u   Wnd: %-10u\n",
           copy.snduna, copy.sndnxt, copy.sndwnd);

    /* Buffers */
    printf("           ");
    printf("In  Start: %-10u Count: %-10u Read %-10u\n",
           copy.istart, copy.icount, copy.ibytes);
    printf("           ");
    printf("Out Start: %-10u Count: %-10u Read %-10u\n",
           copy.ostart, copy.ocount, copy.obytes);
    printf("\n");

    return;
}
