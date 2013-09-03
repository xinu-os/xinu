/**
 * @file     lexan.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <shell.h>

/**
 * @ingroup shell
 *
 * Ad hoc lexical analyzer to divide command line into tokens
 * @param *line   pointer to line to parse
 * @param linelen length of line to parse
 * @param *tokbuf buffer for tokens 
 * @param *tok[]  array of pointers into token buffer
 * @return number of tokens created
 */
short lexan(char *line, ushort linelen, char *tokbuf, char *tok[])
{
    char quote;                 /* character for quoted string  */
    ushort ntok = 0;            /* number of tokens parsed      */
    ushort i = 0;               /* temp variable                */

    while ((i < linelen) && (ntok < SHELL_MAXTOK))
    {
        /* Skip whitespace in line of input */
        while (isWhitespace(line[i]) && (i < linelen))
        {
            i++;
        }

        /* Stop parsing at end of line */
        if (isEndOfLine(line[i]) || (i >= linelen))
        {
            return ntok;
        }

        /* Set token to point to value in token buffer */
        tok[ntok] = tokbuf;

        /* Handle quoted string */
        if (isQuote(line[i]))
        {
            quote = *tokbuf++ = line[i++];

            while ((quote != line[i])
                   && (!isEndOfLine(line[i])) && (i < linelen))
            {
                *tokbuf++ = line[i++];
            }

            if (quote == line[i])
            {
                *tokbuf++ = line[i++];
            }
            else
            {
                return SYSERR;
            }
        }
        else
        {
            *tokbuf++ = line[i++];

            /* Handle standard alphanumeric token */
            if (!isOtherSpecial(line[i - 1]))
            {
                while ((!isEndOfLine(line[i])) && (!isQuote(line[i]))
                       && (!isOtherSpecial(line[i]))
                       && (!isWhitespace(line[i])) && (i < linelen))
                {
                    *tokbuf++ = line[i++];
                }

                if (i >= linelen)
                    return SYSERR;
            }
        }

        /* Finish current token */
        *tokbuf++ = '\0';
        ntok++;
    }

    return ntok;
}
