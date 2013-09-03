/**
 * @file fbPutc.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <framebuffer.h>
#include <device.h>

extern int rows;
extern int cols;
extern int cursor_col;
extern int cursor_row;
extern ulong foreground;
extern ulong background;
extern bool screen_initialized;

/**
 * @ingroup framebuffer
 *
 * Write a single character to the framebuffer
 * @param  devptr  pointer to framebuffer device
 * @param  ch    character to write
 */
devcall fbPutc(device *devptr, char ch)
{
    if (screen_initialized)
    {
        if (ch == '\n')
        {
            cursor_row++;
            cursor_col = 0;
        }
        else if (ch == '\t')
        {
            cursor_col += 4;
        }
        drawChar(ch, cursor_col * CHAR_WIDTH,
                 cursor_row * CHAR_HEIGHT, foreground);
        cursor_col++;
        if (cursor_col == cols)
        {
            cursor_col = 0;
            cursor_row += 1;
        }
        if ( (minishell == TRUE) && (cursor_row == rows) )
        {
            minishellClear(background);
            cursor_row = rows - MINISHELLMINROW;
        }
        else if (cursor_row == rows)
        {
            screenClear(background);
            cursor_row = 0;
        }
        /* TODO: hack to echo characters to the console as well.  A better
         * solution is needed.  */
#ifdef CONSOLE
        putc(CONSOLE, ch);
#endif
        return (uchar)ch;
    }
    else
    {
        /* TODO: hack to echo characters to the console as well.  A better
         * solution is needed.  */
#ifdef CONSOLE
        return putc(CONSOLE, ch);
#endif
    }
    return SYSERR;
}
