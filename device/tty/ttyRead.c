/**
 * @file ttyRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ctype.h>
#include <device.h>
#include <tty.h>

static void ttyEcho(device *, char);

/**
 * @ingroup tty
 *
 * Read characters from a tty.
 * @param devptr pointer to tty device
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters read, EOF if end of file was reached
 */
devcall ttyRead(device *devptr, void *buf, uint len)
{
    struct tty *ttyptr;
    device *phw;
    int ch = 0;
    int count = 0;
    char *buffer = buf;

    /* Setup and error check pointers to structures */
    ttyptr = &ttytab[devptr->minor];
    phw = ttyptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* If the eof flag is set, clear the flag and return EOF */
    if (ttyptr->ieof)
    {
        ttyptr->ieof = FALSE;
        return EOF;
    }

    /* In raw mode, no line buffering and no cooking */
    if (ttyptr->iflags & TTY_IRAW)
    {
        /* Fill user buffer first from input buffer */
        while ((0 < ttyptr->icount) && (count < len))
        {
            *buffer++ = ttyptr->in[ttyptr->istart];
            ttyptr->icount--;
            ttyptr->istart = (ttyptr->istart + 1) % TTY_IBLEN;
            count++;
        }

        /* Fill rest of user buffer by reading input */
        while (count < len)
        {
            ch = (*phw->getc) (phw);
            if (SYSERR == ch)
            {
                return SYSERR;
            }
            *buffer++ = ch;
            count++;

            /* Echo character if TTY_ECHO flag is set */
            if (ttyptr->iflags & TTY_ECHO)
            {
                ttyEcho(devptr, ch);
            }
        }

        return count;
    }

    /* In non-raw mode, read characters into the TTY input buffer     */
    /* until a line delimiter is read or the TTY input buffer is full */
    while ((ttyptr->icount < TTY_IBLEN) && !ttyptr->idelim)
    {
        /* Read character */
        ch = (*phw->getc) (phw);
        if (SYSERR == ch)
        {
            return SYSERR;
        }

        /* Cook special characters */
        switch (ch)
        {
            /* Backspace or delete */
        case '\b':
        case 0x7F:
            if (ttyptr->icount < 1)
            {
                continue;
            }
            ttyptr->icount--;
            break;
            /* Newline */
        case '\n':
            /* Translate NL to CR if INLCR flag is set */
            if (ttyptr->iflags & TTY_INLCR)
            {
                ch = '\r';
            }
            /* Place character in TTY input buffer */
            ttyptr->in[(ttyptr->istart + ttyptr->icount) % TTY_IBLEN] =
                ch;
            ttyptr->icount++;
            ttyptr->idelim = TRUE;
            break;
            /* Carriage return */
        case '\r':
            /* Ignore carriage return if IGNCR flag is set */
            if (ttyptr->iflags & TTY_IGNCR)
            {
                continue;
            }
            /* Translate CR to NL if ICRNL flag is set */
            if (ttyptr->iflags & TTY_ICRNL)
            {
                ch = '\n';
            }
            /* Place character in TTY input buffer */
            ttyptr->in[(ttyptr->istart + ttyptr->icount) % TTY_IBLEN] =
                ch;
            ttyptr->icount++;
            ttyptr->idelim = TRUE;
            break;
            /* End of file */
        case 0x04:
            ttyptr->ieof = TRUE;
            ttyptr->idelim = TRUE;
            break;
            /* All other characters */
        default:
            /* Ignore unprintable characters */
            if (!isprint(ch))
            {
                continue;
            }
            /* Place character in TTY input buffer */
            ttyptr->in[(ttyptr->istart + ttyptr->icount) % TTY_IBLEN] =
                ch;
            ttyptr->icount++;
            break;
        }

        /* Echo character if TTY_ECHO flag is set */
        if (ttyptr->iflags & TTY_ECHO)
        {
            ttyEcho(devptr, ch);
        }

        if (ttyptr->iflags & TTY_IRAW)
        {
            break;
        }
    }

    /* Fill user buffer from input buffer */
    while ((0 < ttyptr->icount) && (count < len))
    {
        *buffer++ = ttyptr->in[ttyptr->istart];
        ttyptr->icount--;
        ttyptr->istart = (ttyptr->istart + 1) % TTY_IBLEN;
        count++;
    }

    /* If TTY input buffer is empty, clear idelimiter flag */
    if (0 == ttyptr->icount)
    {
        ttyptr->idelim = FALSE;
    }

    /* If nothing was read, and the eof flag was set, return EOF */
    if ((0 == count) && (ttyptr->ieof))
    {
        ttyptr->ieof = FALSE;
        return EOF;
    }

    return count;
}

/**
 * Echo a single character on a TTY.
 * @param devptr TTY device table entry
 * @param ch character to echo
 */
static void ttyEcho(device *devptr, char ch)
{
    /* Backspace or delete */
    if (('\b' == ch) || (0x7F == ch))
    {
        ttyPutc(devptr, '\b');
        ttyPutc(devptr, ' ');
        ttyPutc(devptr, '\b');
        return;
    }

    /* Do not echo unprintable characters */
    if (!isprint(ch))
    {
        return;
    }

    /* Echo character */
    ttyPutc(devptr, ch);
}
