/**
 * @file telnetRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <semaphore.h>
#include <string.h>
#include <device.h>
#include <telnet.h>
#include <thread.h>

static void telnetEchoNegotiate(struct telnet *, int);
static void telnetEcho(device *, int);
static void telnetSendOption(device *, uchar, uchar);

/**
 * @ingroup telnet
 *
 * Read characters from a telnet connection
 * @param devptr pointer to a telnet device
 * @param buf buffer for reading characters
 * @param len size of the buffer
 * @return number of characters read, EOF if end of file was reached
 */
devcall telnetRead(device *devptr, void *buf, uint len)
{
    struct telnet *tntptr;
    device *phw;
    int ch = 0;
    int count = 0;
    int index = 0;
    bool willSga = FALSE;
    bool recvSga = FALSE;

    uchar *buffer = buf;
    uchar cmdbuf[3] = { 0, 0, 0 };

    char amthere[] = "\r\n*****connections still open*****\r\n";

    tntptr = &telnettab[devptr->minor];
    phw = tntptr->phw;
    if (NULL == phw)
    {
        TELNET_TRACE("Null phw");
        return SYSERR;
    }

    /* If the EOF flag is set, clear the flag and return the EOF */
    if (tntptr->ieof)
    {
        TELNET_TRACE("EOF");
        tntptr->ieof = FALSE;
        return EOF;
    }

    /* Is the input buffer being modified already by something else? */
    wait(tntptr->isem);

    TELNET_TRACE("Have input semaphore");
    /* Check if there is any data in the input buffer */
    if (0 == tntptr->icount)
    {
        while ((tntptr->icount < TELNET_IBLEN) && !(tntptr->idelim))
        {
            /* Set index value to icount + istart values of input buffer */
            index = tntptr->icount + tntptr->istart;

            /* Read character */
            ch = (*phw->getc) (phw);
            if (SYSERR == ch)
            {
                TELNET_TRACE("Read error");
                return SYSERR;
            }

            /* Handle special characters and put others in the in buffer */
            switch (ch)
            {
            case '\r':
                TELNET_TRACE("Recv Carriage return");
                tntptr->in[index % TELNET_IBLEN] = ch;
                tntptr->icount++;
                index = tntptr->icount + tntptr->istart;
                telnetEcho(devptr, ch);
                telnetFlush(devptr);
                /* Get the next char to determine if idelim should be set */
                ch = (*phw->getc) (phw);
                if (SYSERR == ch)
                {
                    TELNET_TRACE("Read error");
                    return SYSERR;
                }
                switch (ch)
                {
                    /* Must be followed by either NUL or LF */
                case '\0':
                case '\n':
                    /* Put char in input buffer and set idelim to TRUE */
                    tntptr->in[index % TELNET_IBLEN] = ch;
                    tntptr->icount++;
                    tntptr->idelim = TRUE;
                    telnetEcho(devptr, ch);
                    break;
                default:
                    /* Default to NUL, since apparently only the carriage
                     * return is desired */
                    tntptr->in[index % TELNET_IBLEN] = '\0';
                    tntptr->icount++;
                    index = tntptr->icount + tntptr->istart;
                    tntptr->in[index % TELNET_IBLEN] = ch;
                    tntptr->icount++;
                    break;
                }
                //if (TELNET_ECHO_SELF_ECHOES == tntptr->echoState)
                if (tntptr->flags & TELNET_FLAG_ECHO);
                {
                    telnetEcho(devptr, ch);
                }
                break;
            case TELNET_CHR_DEL:
            case '\b':
                TELNET_TRACE("Recv Backspace");
                /* Move left one character position if possible */
                if (tntptr->icount >= 1)
                {
                    tntptr->icount--;

                    if (tntptr->flags & TELNET_FLAG_ECHO);
                    {
                        telnetEcho(devptr, ch);
                    }
                }
                break;

            case '\n':
                TELNET_TRACE("Recv Newline");
                /* Put char in input buffer and set idelim to TRUE */
                tntptr->in[index % TELNET_IBLEN] = ch;
                tntptr->icount++;
                tntptr->idelim = TRUE;
                //if (TELNET_ECHO_SELF_ECHOES == tntptr->echoState)
                if (tntptr->flags & TELNET_FLAG_ECHO);
                {
                    telnetEcho(devptr, ch);
                }
                break;
            case 0x04:
                TELNET_TRACE("Recv EOF");
                /* Handle EOF */
                tntptr->ieof = TRUE;
                tntptr->idelim = TRUE;
                break;
            case TELNET_IAC:
                /* Get another char and check which command it is */
                ch = (*phw->getc) (phw);
                if (SYSERR == ch)
                {
                    TELNET_TRACE("Recv Command read error");
                    return SYSERR;
                }
                switch (ch)
                {
                case TELNET_WILL:
                    /* Get another char and check which option it is */
                    ch = (*phw->getc) (phw);
                    if (SYSERR == ch)
                    {
                        TELNET_TRACE("Recv WILL Read error");
                        return SYSERR;
                    }
                    cmdbuf[2] = ch;

                    /* Request to enable SUPPRESS GA on server end */
                    if (TELNET_SUPPRESS_GA == ch)
                    {
                        TELNET_TRACE("Recv WILL Suppress Go-Ahead");
                        recvSga = TRUE;
                        cmdbuf[1] = TELNET_WILL;
                        if (FALSE == willSga)
                        {
                            TELNET_TRACE("Send WILL Suppress Go-Ahead");
                            willSga = TRUE;
                            telnetSendOption(phw, cmdbuf[1], cmdbuf[2]);
                        }
                    }
                    else if (TELNET_ECHO == ch)
                    {
                        TELNET_TRACE("Recv WILL Echo");
                        telnetEchoNegotiate(tntptr, TELNET_WILL);
                    }
                    else
                    {
                        TELNET_TRACE("Recv WILL %d (unsupported)", ch);
                        cmdbuf[1] = TELNET_DONT;
                        TELNET_TRACE("Send DONT %d (unsupported)", ch);
                        telnetSendOption(phw, cmdbuf[1], cmdbuf[2]);
                    }

                    break;
                case TELNET_WONT:
                    /* If client won't echo then server should */
                    ch = (*phw->getc) (phw);
                    if (SYSERR == ch)
                    {
                        TELNET_TRACE("Recv WONT Read error");
                        return SYSERR;
                    }

                    cmdbuf[0] = TELNET_IAC;
                    if (TELNET_ECHO == ch)
                    {
                        TELNET_TRACE("Recv WONT Echo");
                        telnetEchoNegotiate(tntptr, TELNET_WONT);
                    }
                    else
                    {
                        TELNET_TRACE("Recv WONT %d (unsupported)", ch);
                    }
                    /* Otherwise just ignore the won't request */
                    break;
                case TELNET_DO:
                    /* Get another char and check which option it is */
                    ch = (*phw->getc) (phw);
                    if (SYSERR == ch)
                    {
                        TELNET_TRACE("Recv DO   Read error");
                        return SYSERR;
                    }

                    /* Check which option it is and if it applies here */
                    cmdbuf[0] = TELNET_IAC;
                    if (TELNET_SUPPRESS_GA == ch)
                    {
                        TELNET_TRACE("Recv DO   Suppress Go-Ahead");
                        TELNET_TRACE("Send WILL Suppress Go-Ahead");
                        telnetSendOption(phw, TELNET_WILL, ch);
                        tntptr->flags |= TELNET_FLAG_SUPPRESS_GA;
                    }
                    else if (TELNET_ECHO == ch)
                    {
                        TELNET_TRACE("Recv DO   Echo");
                        telnetEchoNegotiate(tntptr, TELNET_DO);
                    }
                    else
                    {
                        TELNET_TRACE("Recv DO   %d (unsupported)", ch);
                        TELNET_TRACE("Send WONT %d (unsupported)", ch);
                        telnetSendOption(phw, TELNET_WONT, ch);
                    }
                    break;
                case TELNET_DONT:
                    /* Get another char and check which option it is */
                    ch = (*phw->getc) (phw);
                    if (SYSERR == ch)
                    {
                        TELNET_TRACE("Recv DONT Read error");
                        return SYSERR;
                    }
                    if (TELNET_ECHO == ch)
                    {
                        TELNET_TRACE("Recv DONT Echo");
                        telnetEchoNegotiate(tntptr, TELNET_DONT);
                    }
                    break;

                case TELNET_SE:
                case TELNET_NOP:
                    /* Don't do anything, since this is a null operation */
                case TELNET_DM:
                    /* Useless, since urgent mode is not implemented */
                case TELNET_BRK:
                    TELNET_TRACE("Recv Command %d (unsupported)", ch);
                    break;
                case TELNET_IP:
                    TELNET_TRACE("Recv Interrupt");
                    /* Terminate the currently running thread... */
                    /* unless the current thread name begins with "SHELL" */
                    if (strncmp(thrtab[thrcurrent].name, "SHELL", 5) != 0)
                    {
                        kill(thrcurrent);
                    }
                    break;
                case TELNET_AO:
                    TELNET_TRACE("Recv Abort Output");
                    /* Flush the output buffer */
                    telnetFlush(devptr);
                    break;
                case TELNET_AYT:
                    TELNET_TRACE("Recv Are You There");
                    /* Send "am there" message */
                    (*phw->write) (phw, (void *)amthere, 36);
                    break;
                case TELNET_EC:
                    TELNET_TRACE("Recv Erase character");
                    if (tntptr->icount >= 1)
                    {
                        tntptr->icount--;
                    }
                    break;
                case TELNET_EL:
                    TELNET_TRACE("Recv Erase Line");
                    while (tntptr->icount >= 1 &&
                           tntptr->in[index % TELNET_IBLEN] != '\n')
                    {
                        tntptr->icount--;
                        index = tntptr->icount + tntptr->istart;
                        tntptr->in[index % TELNET_IBLEN] = 0;
                    }
                    break;
                case TELNET_GA:
                    TELNET_TRACE("Recv Go-Ahead");
                    /* We want the client to suppress GAs, so this should
                     * not be sent. But just in case, just ignore it if it
                     * does. */
                    break;
                case TELNET_SB:
                    break;
                }
                break;
            default:
                /* Put byte in TELNET input buffer if it is printable */
                if (isprint(ch))
                {
                    tntptr->in[index % TELNET_IBLEN] = ch;
                    tntptr->icount++;
                }
                //if (TELNET_ECHO_SELF_ECHOES == tntptr->echoState)
                if (tntptr->flags & TELNET_FLAG_ECHO);
                {
                    telnetEcho(devptr, ch);
                }
                break;
            }
        }
    }

    /* Signal the input semaphore; done modifying the input buffer */
    signal(tntptr->isem);

    /* Fill user buffer from input buffer */
    while ((0 < tntptr->icount) && (count < len))
    {
        *buffer++ = tntptr->in[tntptr->istart];
        tntptr->icount--;
        tntptr->istart = (tntptr->istart + 1) % TELNET_IBLEN;
        count++;
    }

    if (0 == tntptr->icount)
    {
        tntptr->idelim = FALSE;
    }

    return count;
}

static void telnetEchoNegotiate(struct telnet *tntptr, int command)
{
    device *phw;
    phw = tntptr->phw;
    switch (tntptr->echoState)
    {
    case TELNET_ECHO_SENT_DO:
        if (TELNET_WILL == command)
        {
            TELNET_TRACE("Recv WILL Echo");
            tntptr->echoState = TELNET_ECHO_OTHER_ECHOES;
            TELNET_TRACE("Echo state OTHER_ECHOES");
            tntptr->flags &= ~TELNET_FLAG_ECHO;
        }
        else if (TELNET_WONT == command)
        {
            TELNET_TRACE("Recv WONT Echo");
            telnetSendOption(phw, TELNET_WILL, TELNET_ECHO);
            TELNET_TRACE("Send WILL Echo");
            tntptr->echoState = TELNET_ECHO_SENT_WILL;
            TELNET_TRACE("Echo state SENT_WILL");
        }
        else if (TELNET_DO == command)
        {
            TELNET_TRACE("Recv DO   Echo");
            telnetSendOption(phw, TELNET_WONT, TELNET_ECHO);
            TELNET_TRACE("Send WONT Echo");
        }
        break;
    case TELNET_ECHO_SENT_WILL:
        if (TELNET_DO == command)
        {
            TELNET_TRACE("Recv DO   Echo");
            tntptr->echoState = TELNET_ECHO_SELF_ECHOES;
            TELNET_TRACE("Echo state SELF_ECHOES");
            tntptr->flags |= TELNET_FLAG_ECHO;
        }
        else if (TELNET_DONT == command)
        {
            TELNET_TRACE("Recv DONT Echo");
            tntptr->echoState = TELNET_ECHO_NO_ECHO;
            TELNET_TRACE("Echo state NO_ECHO");
            tntptr->flags &= ~TELNET_FLAG_ECHO;
        }
        else if (TELNET_WILL == command)
        {
            TELNET_TRACE("Recv WILL Echo");
            telnetSendOption(phw, TELNET_DONT, TELNET_ECHO);
            TELNET_TRACE("Send DONT Echo");
        }
        break;
    case TELNET_ECHO_SELF_ECHOES:
        if (TELNET_DONT == command)
        {
            TELNET_TRACE("Recv DONT Echo");
            telnetSendOption(phw, TELNET_WONT, TELNET_ECHO);
            TELNET_TRACE("Send WONT Echo");
            tntptr->echoState = TELNET_ECHO_NO_ECHO;
            TELNET_TRACE("Echo state NO_ECHO");
            tntptr->flags &= ~TELNET_FLAG_ECHO;
        }
        else if (TELNET_WILL == command)
        {
            TELNET_TRACE("Recv WILL Echo");
            telnetSendOption(phw, TELNET_DONT, TELNET_ECHO);
            TELNET_TRACE("Send DONT Echo");
        }
        break;
    case TELNET_ECHO_OTHER_ECHOES:
        if (TELNET_WONT == command)
        {
            TELNET_TRACE("Recv WONT Echo");
            telnetSendOption(phw, TELNET_DONT, TELNET_ECHO);
            TELNET_TRACE("Send DONT Echo");
            telnetSendOption(phw, TELNET_WILL, TELNET_ECHO);
            TELNET_TRACE("Send WILL Echo");
            tntptr->echoState = TELNET_ECHO_SENT_WILL;
            TELNET_TRACE("Echo state SENT_WILL");
        }
        else if (TELNET_DO == command)
        {
            TELNET_TRACE("Recv DO   Echo");
            telnetSendOption(phw, TELNET_WONT, TELNET_ECHO);
        }
        break;
    case TELNET_ECHO_NO_ECHO:
        if (TELNET_DO == command)
        {
            TELNET_TRACE("Recv DO   Echo");
            telnetSendOption(phw, TELNET_WILL, TELNET_ECHO);
            TELNET_TRACE("Send WILL Echo");
            tntptr->echoState = TELNET_ECHO_SELF_ECHOES;
            TELNET_TRACE("Echo state SELF_ECHOES");
            tntptr->flags |= TELNET_FLAG_ECHO;
        }
        else if (TELNET_WILL == command)
        {
            TELNET_TRACE("Recv WILL Echo");
            telnetSendOption(phw, TELNET_DONT, TELNET_ECHO);
            TELNET_TRACE("Send DONT Echo");
        }
        break;
    default:
        TELNET_TRACE("Echo state unknown %d!", tntptr->echoState);
        //Error, unknown echoState.
        break;
    }
}

static void telnetEcho(device *devptr, int ch)
{
    if (('\b' == ch) || (TELNET_EC == ch) || (TELNET_CHR_DEL == ch))
    {
        telnetPutc(devptr, '\b');
        telnetPutc(devptr, ' ');
        telnetPutc(devptr, '\b');
        telnetFlush(devptr);
        return;
    }

    if ('\r' == ch)
    {
        telnetPutc(devptr, '\r');
        telnetPutc(devptr, '\n');
        telnetFlush(devptr);
        return;
    }

    if ('\n' == ch)
    {
        return;
    }

    if (!isprint(ch))
    {
        return;
    }

    telnetPutc(devptr, (char)ch);
    telnetFlush(devptr);
}

static void telnetSendOption(device *phw, uchar command, uchar option)
{
    uchar cmdbuf[3];
    cmdbuf[0] = TELNET_IAC;
    cmdbuf[1] = command;
    cmdbuf[2] = option;
    (*phw->write) (phw, (void *)cmdbuf, 3);
}
