/**
 * @file icmpRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <icmp.h>
#include <clock.h>
#include <ethernet.h>
#include <interrupt.h>
#include <ipv4.h>
#include <mailbox.h>
#include <thread.h>
#include <network.h>
#include <string.h>

/**
 * @ingroup icmp
 *
 * Processes an incoming ICMP packet.
 * @param pkt pointer to the incoming packet
 * return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall icmpRecv(struct packet *pkt)
{
    struct icmpPkt *icmp = NULL;
    struct icmpEcho *echo = NULL;
    struct icmpEchoQueue *eq = NULL;
    int id = 0, i = 0;
    irqmask im;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    icmp = (struct icmpPkt *)pkt->curr;

    switch (icmp->type)
    {
    case ICMP_ECHOREPLY:
        ICMP_TRACE("Received Echo Reply");
        echo = (struct icmpEcho *)icmp->data;
        id = net2hs(echo->id);
        if ((id >= 0) && (id < NTHREAD))
        {
            im = disable();

            echo->arrivcyc = clkcount();
            echo->arrivtic = clkticks;
            echo->arrivsec = clktime;

            for (i = 0; i < NPINGQUEUE; i++)
            {
                eq = &echotab[i];
                if (id == eq->tid)
                {
                    ICMP_TRACE("Ping matches queue %d", i);
                    if (((eq->head + 1) % NPINGHOLD) == eq->tail)
                    {
                        ICMP_TRACE("Queue full, discarding");
                        restore(im);
                        netFreebuf(pkt);
                        return SYSERR;
                    }
                    eq->pkts[eq->head] = pkt;
                    eq->head = ((eq->head + 1) % NPINGHOLD);
                    send(id, (message)pkt);
                    restore(im);
                    return OK;
                }
            }
            restore(im);
        }
        ICMP_TRACE("Reply id %d does not correspond to ping queue", id);
        netFreebuf(pkt);
        return SYSERR;

    case ICMP_ECHO:
        ICMP_TRACE("Enqueued Echo Request for daemon to reply");
        mailboxSend(icmpqueue, (int)pkt);
        return OK;

    case ICMP_UNREACH:
    case ICMP_SRCQNCH:
    case ICMP_REDIRECT:
    case ICMP_TIMEEXCD:
    case ICMP_PARAMPROB:
    case ICMP_TMSTMP:
    case ICMP_TMSTMPREPLY:
    case ICMP_INFORQST:
    case ICMP_INFOREPLY:
    case ICMP_TRACEROUTE:
        ICMP_TRACE("ICMP message type %d not handled", icmp->type);
        break;

    default:
        ICMP_TRACE("ICMP message type %d unknown", icmp->type);
        break;
    }

    netFreebuf(pkt);
    return OK;
}
