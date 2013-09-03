/**
 * @file drawChars.c
 *
 * Allows characters to be rendered onscreen.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <framebuffer.h>

/* font.c includes a 12 byte representation of each of the first 128 ASCII
 * characters.  each byte is one row of 0's and 1's, where 1's represent that a
 * pixel should be rendered in that location. */
void drawChar(char c, int x, int y, ulong color)
{
    int i, j;
    uchar line;

    /* stupid proof error checking: make sure we have a representation for this
     * char.  */
    if (c < 0 || c > 127)
    {
        return;
    }

    for (i = 0; i < CHAR_HEIGHT; i++)
    {
        line = FONT[c * CHAR_HEIGHT + i];
        for (j = 0; j < CHAR_WIDTH; j++)
        {
            if (line & 0x1)
            {
                drawPixel(x + j, y, color);
            }
            line >>= 1;
        }
        y++;
    }
}

