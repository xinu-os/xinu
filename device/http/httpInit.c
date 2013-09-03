/**
 * @file httpInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>

#include <http.h>
#include <semaphore.h>

struct http httptab[NHTTP];
semaphore maxhttp = -1;
semaphore activeXWeb = -1;

/* Shell command and its length */
const struct httpcmd httpcmdtab[] = {
    {NULL, "index.html", "HOME", FALSE},
    {"config", "config.html", "configure router", FALSE},
    {"nvram list", "nvram.html", "nvram variable list", TRUE},
    {"flashstat", "flashstat.html", "flash device info", TRUE},
    {"netstat", "netstat.html", "net device info", TRUE},
    {"ps", "ps.html", "process stats", TRUE},
    {"gpiostat", "gpiostat.html", "gpio stats", TRUE},
    {"memstat", "memstat.html", "memory stats", TRUE},
    {"ethstat", "ethstat.html", "ethernet stats", TRUE},
    {"vlanstat", "vlanstat.html", "virtual lan stats", TRUE},
    {"uartstat", "uartstat.html", "serial device stats", TRUE},
    {"arp", "arp.html", "arp table", TRUE},
    {"route", "route.html", "route table", TRUE}
};

/* Number of entries in above tables */
ulong nhttpcmd = sizeof(httpcmdtab) / sizeof(struct httpcmd);

/**
 * Initialize HTTP structures.
 * @param devptr HTTP device table entry
 * @return OK if device is initialized
 */
devcall httpInit(device *devptr)
{
    struct http *webptr;

    webptr = &httptab[devptr->minor];

    if (isbadsem(maxhttp))
    {
        if (NHTTP < NTCP)
        {
            maxhttp = semcreate(NHTTP);
        }
        else
        {
            maxhttp = semcreate(NTCP);
        }
    }
    if (isbadsem(activeXWeb))
    {
        activeXWeb = semcreate(1);
    }

    bzero(webptr, sizeof(struct http));
    webptr->state = HTTP_STATE_FREE;
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_AWAITINGRQST, NULL);

    return OK;
}
