/**
 * @file telnet.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009. All rights reserved. */

#ifndef _TELNET_H_
#define _TELNET_H_

#include <device.h>
#include <stdarg.h>
#include <stddef.h>
#include <semaphore.h>
#include <thread.h>
#include <network.h>

/* Testing macros */
//#define TRACE_TELNET CONSOLE
#ifdef TRACE_TELNET
#include <stdio.h>
#define TELNET_TRACE(...)     { \
		fprintf(TRACE_TELNET, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_TELNET, __VA_ARGS__); \
		fprintf(TRACE_TELNET, "\n"); }
#else
#define TELNET_TRACE(...)
#endif

#define TELNET_PORT     23      /**< default telnet port                    */
#define TELNET_IBLEN    80    /**< input buffer length                    */
#define TELNET_OBLEN    80    /**< output buffer length                   */

/* Telnet Codes */
#define TELNET_EOR      239 /**< end of record command                      */
#define TELNET_SE       240 /**< end of subnegotiations command             */
#define TELNET_NOP      241 /**< no operation command                       */
#define TELNET_DM       242 /**< data mark command                          */
#define TELNET_BRK      243 /**< break NVT charater                         */
#define TELNET_IP       244 /**< interupt process command                   */
#define TELNET_AO       245 /**< abort output command                       */
#define TELNET_AYT      246 /**< are you there command                      */
#define TELNET_EC       247 /**< erase character command                    */
#define TELNET_EL       248 /**< erase line command                         */
#define TELNET_GA       249 /**< go ahead command                           */
#define TELNET_SB       250 /**< begin option subnegotiations command       */
#define TELNET_WILL     251 /**< will enable option                         */
#define TELNET_WONT     252 /**< won't enable option                        */
#define TELNET_DO       253 /**< request other party enables option         */
#define TELNET_DONT     254 /**< request other party doesn't enable option  */
#define TELNET_IAC      255 /**< interpret as command                       */
/* special characters */
#define TELNET_CHR_CLOSE            29
#define TELNET_CHR_EL               21
#define TELNET_CHR_DEL              127


/* TELNET options */
#define TELNET_ECHO             1 /**< echo option code                     */
#define TELNET_SUPPRESS_GA      3 /**< suppress go ahead option code        */
#define TELNET_TRANSMIT_BINARY  0 /**< transmit binary option code          */

/* TELNET flags */
#define TELNET_FLAG_ECHO            0x01    /**< echo flag                  */
#define TELNET_FLAG_SUPPRESS_GA     0x02    /**< suppress go ahead flag     */
#define TELNET_FLAG_TRANSMIT_BINARY 0x04    /**< transmit binary flag       */

/* Control funcitons */
#define TELNET_CTRL_FLUSH       1 /**< flush output buffer control function */
#define TELNET_CTRL_CLRFLAG     2 /**< clear a flag                         */
#define TELNET_CTRL_SETFLAG     3 /**< set a flag                           */

/* TELNET device states */
#define TELNET_STATE_FREE       0
#define TELNET_STATE_ALLOC      1
#define TELNET_STATE_OPEN       2

/* TELNET echo negotation states */
#define TELNET_ECHO_START           0
#define TELNET_ECHO_SENT_DO         1
#define TELNET_ECHO_SENT_WILL       2
#define TELNET_ECHO_SELF_ECHOES     3
#define TELNET_ECHO_OTHER_ECHOES    4
#define TELNET_ECHO_NO_ECHO         5

struct telnet
{
    /* Pointers to associated structures */
    device *phw;                /**< hardware device structure          */

    /* TELNET fields */
    uchar state;                /**< TELNET_STATE_* above               */
    uchar flags;                /**< Flags for Telnet options           */
    uchar echoState;            /**< State of echo option negotation    */
    semaphore killswitch;           /**< Semaphore for killing server thread*/

    /* TELNET input fields */
    bool ieof;                  /**< EOF is in the input buffer         */
    bool idelim;                /**< Partial line in buffer             */
    char in[TELNET_IBLEN];      /**< Input buffer                       */
    uint icount;                /**< Number of chars in input buffer    */
    uint istart;                /**< Index of first char in "in" buffer */
    semaphore isem;             /**< Semaphore for input buffer         */

    /* TELNET output fields */
    char out[TELNET_OBLEN];     /**< Output buffer                      */
    uint ocount;                /**< Number of characters in out buffer */
    uint ostart;                /**< Index of first char in out buffer  */
    semaphore osem;             /**< Semaphore for output buffer        */
};

extern struct telnet telnettab[];

/* Driver functions */
int telnetAlloc(void);
devcall telnetInit(device *);
devcall telnetOpen(device *, va_list);
devcall telnetClose(device *);
devcall telnetRead(device *, void *, uint);
devcall telnetWrite(device *, void *, uint);
devcall telnetGetc(device *);
devcall telnetPutc(device *, char);
devcall telnetControl(device *, int, long, long);
devcall telnetFlush(device *);
thread telnetServer(int, int, ushort, char *);

#endif                          /* _TELNET_H_ */
