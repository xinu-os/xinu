/**
 * @file doscan.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

/* Character look-ahead to be implemented as: ch=control(unit,TTY_IOC_NEXTC) */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

enum integer_size {
    SHORT_SIZE = 0,
    REGULAR_SIZE,
    LONG_SIZE
};

/* Default maximum length when none specified.  */
#define UNLIMITED_LENGTH (~(uint)0)

static int scan_string(char *ptr, int type, uint maxlen,
                       const uchar *stopchar_tab,
                       int (*getch) (int, uintptr_t), int (*ungetch) (int, uintptr_t),
                       int arg1, uintptr_t arg2, bool *eofptr);

static int scan_number_or_string(void *ptr, uchar type, uint maxlen,
                                 enum integer_size size,
                                 const uchar *stopchar_tab,
                                 int (*getch) (int, uintptr_t),
                                 int (*ungetch) (int, uintptr_t),
                                 int arg1, uintptr_t arg2, bool *eofptr);

static const uchar *build_stopchar_tab(const uchar *ufmt, uchar *stopchar_tab);

/**
 * @ingroup libxc
 *
 * Scan and recognize input according to a format.
 *
 * This is a minimal implementation and not all standard features are supported.
 * The supported conversion specifications include:
 *
 * - @c \%s to scan a whitespace-delimited string
 * - @c \%o to scan an octal number (@c \%O for long)
 * - @c \%x to scan a hexideminal number (@c \%X for long)
 * - @c \%c to scan a sequence of characters
 * - @c \%[ to scan a sequence of characters from the specified set
 * - @c \%\% to match a literal percent sign
 * - All other conversion specifiers, including @c \%d and @c \%u, are
 *   interpreted as specifying a decimal number.
 * - Whitespace to match any amount of whitespace in the input.
 * - Maximum length modifier
 * - @c 'l' type modifier to parse a long int rather than a normal int
 * - @c 'h' type modifier to parse a short int rather than a normal int
 * - @c '*' character to suppress assignment
 *
 * If a feature is not mentioned above, assume it is not supported.
 *
 * @param fmt
 *      format string for the scan
 * @param ap
 *      variable-length list of locations to store the scanned data
 * @param getch
 *      Function to get a character.  It is expected to return the character as
 *      an <code>unsigned char</code> cast to an @c int on success, or @c EOF on
 *      end-of-file or read error.
 * @param ungetch
 *      Function to unget a character.  Unlike the standard ungetc(), this is
 *      not passed in an explicit character but instead uses the last read
 *      character, which will then be expected to be returned on the next
 *      getch() call.  Only one character of putback needs to be supported.
 * @param arg1
 *      First argument to @p getch and @p ungetch.
 * @param arg2
 *      Second argument to @p getch and @p ungetch.
 *
 * @return
 *      The number of items successfully matched and assigned, which may be less
 *      than the number of format specifiers in the case of a matching failure;
 *      alternatively, if EOF or a read error occurs before any matches, EOF is
 *      returned.
 */
int _doscan(const char *fmt, va_list ap,
            int (*getch) (int, uintptr_t), int (*ungetch) (int, uintptr_t),
            int arg1, uintptr_t arg2)
{
    int nmatch;
    uint maxlen;
    bool eof;
    enum integer_size size;
    void *ptr;
    int c;
    uchar stopchar_tab[256];

    /* Interpret the format string as unsigned to avoid bugs with comparisons.
     * */
    const uchar *ufmt = (const uchar*)fmt;

    nmatch = 0;
    eof = FALSE;
    while (*ufmt != '\0')
    {
        if (*ufmt == '%' && *++ufmt != '%')
        {
            /* Conversion specification. */

            if (*ufmt == '*')
            {
                /* The '*' character indicates that no actual assignment is to
                 * occur.  */
                ptr = NULL;
                ufmt++;
            }
            else
            {
                /* Get the user's pointer as a void *.  We cast it to the proper
                 * pointer once we know the format of the data.  */
                ptr = va_arg(ap, void *);
            }

            /* Parse optional maximum length of this field.  */
            maxlen = 0;
            while (isdigit(*ufmt))
            {
                maxlen *= 10;
                maxlen += (*ufmt - '0');
                ufmt++;
            }
            if (maxlen == 0)
            {
                maxlen = UNLIMITED_LENGTH;
            }

            /* Parse number size modifier. */
            size = REGULAR_SIZE;
            if (*ufmt == 'l')
            {
                size = LONG_SIZE;
                ufmt++;
            }
            else if (*ufmt == 'h')
            {
                size = SHORT_SIZE;
                ufmt++;
            }

            /* Parse and process the actual conversion specifier. */
            c = *ufmt++;
            if (c == '[')
            {
                /* Parse the specification of a nonempty set of characters that
                 * will be accepted or rejected.  */
                ufmt = build_stopchar_tab(ufmt, stopchar_tab);
            }
            else
            {
                if (c == '\0')
                {
                    /*  Format string ended in the middle of a conversion
                     *  specifier.  End early. */
                    return nmatch;
                }
                /* Something else; treat it as a number.  If uppercase, treat it
                 * as specifying a long size.  */
                if (isupper(c))
                {
                    c = tolower(c);
                    size = LONG_SIZE;
                }
            }

            /* Scan the data.  */
            if (scan_number_or_string(ptr, c, maxlen, size, stopchar_tab,
                                      getch, ungetch, arg1, arg2, &eof))
            {
                /* Successfully scanned the data.  */
                if (ptr != NULL)
                {
                    /* Count this match towards the total only if '*' was not
                     * specified.  */
                    nmatch++;
                }
                /* Return early if EOF occurred. */
                if (eof)
                {
                    return (nmatch == 0) ? EOF : nmatch;
                }
            }
            else
            {
                /* Matching failure, or no assignment requested.  */
                if (ptr != NULL || eof)
                {
                    return (eof && nmatch == 0) ? EOF : nmatch;
                }
            }
        }
        else if (isspace(*ufmt))
        {
            /* One or more whitespace characters in format string match any
             * amount of whitespace, including none, in the input.  */
            do         
            {
                c = (*getch)(arg1, arg2);
                if (c == EOF)
                {
                    return (nmatch == 0) ? EOF : nmatch; 
                }
            } while (isspace(c));

            (*ungetch) (arg1, arg2);

            do
            {
                ufmt++;
            } while (isspace(*ufmt));
        }
        else
        {
            /* Literal character; must match the input exactly.  */
            c = (*getch)(arg1, arg2);
            if (c == EOF)
            {
                return (nmatch == 0) ? EOF : nmatch; 
            }
            if (c != *ufmt)
            {
                /* Mismatch; end early. */
                return nmatch;
            }
            ufmt++;
        }
    }
    return nmatch;
}

static int scan_string(char *ptr, int type, uint maxlen, const uchar *stopchar_tab,
                       int (*getch) (int, uintptr_t), int (*ungetch) (int, uintptr_t),
                       int arg1, uintptr_t arg2, bool *eofptr)
{
    uint len;
    int c;

    *eofptr = FALSE;
    
    /* %c defaults to length 1 (a single character) */
    if (type == 'c' && maxlen == UNLIMITED_LENGTH)
    {
        maxlen = 1;
    }

    /* scan characters until a mismatch occurs, the maximum length is reached,
     * or EOF or a read error occurs.  */
    for (len = 0; len < maxlen; len++)
    {
        c = (*getch)(arg1, arg2);
        if (c == EOF)
        {
            *eofptr = TRUE;
            break;
        }
        if ((type == '[' && stopchar_tab[c]) ||
            (type == 's' && isspace(c)))
        {
            (*ungetch)(arg1, arg2);
            break;
        }
        if (ptr != NULL)
        {
            *ptr++ = c;
        }
    }

    /* %s and %[ result is null terminated; %c result is not. */
    if (ptr != NULL && type != 'c')
    {
        *ptr = '\0';
    }

    /* Return 1 if the match met the minimum required length; otherwise 0. */
    return (len != 0 && !(type == 'c' && len < maxlen));
}


/* This function handles actually scanning the data after the conversion
 * specification has been parsed.  It is expected to return 1 if the match was
 * successfull, and 0 otherwise.  It is also expected to set *eofptr to a
 * boolean that indicates whether end-of-file or a read error was encountered or
 * not. */
static int scan_number_or_string(void *ptr, uchar type, uint maxlen,
                                 enum integer_size size,
                                 const uchar *stopchar_tab,
                                 int (*getch) (int, uintptr_t),
                                 int (*ungetch) (int,uintptr_t),
                                 int arg1, uintptr_t arg2, bool *eofptr)
{
    int c = EOF;
    ulong n;
    bool negative;
    uint base;
    uint len;
    bool had_sign;
    uint nmatch = 0;

    *eofptr = FALSE;

    /* Skip leading whitespace if required by the conversion specifier. */
    if (type != '[' && type != 'c')
    {
        do
        {
            c = (*getch)(arg1, arg2);
            if (c == EOF)
            {
                *eofptr = TRUE;
                return nmatch;
            }
        } while (isspace(c));
        if (type == 's')
        {
            (*ungetch) (arg1, arg2);
        }
    }

    /* If parsing a string or sequence of characters, pass control to
     * scan_string().  Otherwise set the numeric base and parse as a number.  */
    switch (type)
    {
    case 'c':
    case 's':
    case '[':
        return scan_string(ptr, type, maxlen, stopchar_tab,
                           getch, ungetch, arg1, arg2, eofptr);
    case 'o':
        base = 8;
        break;
    case 'x':
        base = 16;
        break;
    default:
        base = 10;
        break;
    }

    /* Parsing a number.  */

    /* Parse optional sign.  */
    len = 0;
    negative = (c == '-');
    if (c == '-' || c == '+')
    {
        c = (*getch) (arg1, arg2);
        had_sign = TRUE;
        len++;
    }
    else
    {
        had_sign = FALSE;
    }

    /* Parse digits.  n is the number we are building up.  */
    n = 0;
    for (;;)
    {
        if (c == EOF)
        {
            break;
        }

        c = tolower(c);
        switch (base)
        {
        case 16:
            if (c >= 'a' && c <= 'f')
            {
                n *= base;
                n += 0xA + (c - 'a');
                len++;
                break;
            }
            /* Fall through */
        case 10:
            if (c >= '8' && c <= '9')
            {
                n *= base;
                n += (c - '0');
                len++;
                break;
            }
            /* Fall through */
        case 8:
            if (c >= '0' && c <= '7')
            {
                n *= base;
                n += (c - '0');
                len++;
                break;
            }
            /* Fall through */
        default:
            /* Not a digit in this base. */
            ungetch(arg1, arg2);
            goto num_scanned;
        }

        if (len >= maxlen)
        {
            break;
        }

        c = getch(arg1, arg2);
    }

num_scanned:

    if (c == EOF)
    {
        *eofptr = TRUE;
    }

    /* Calculate the final number and store it in the pointer, but only if the
     * format string did not have '*' to suppress assignment, and at least one
     * digit was actually parsed.   */
    if (ptr != NULL && len != 0 && !(had_sign && len == 1))
    {
        if (negative)
        {
            n = -n;
        }
        switch (size)
        {
        case SHORT_SIZE:
            *(short *)ptr = n;
            break;

        case REGULAR_SIZE:
            *(int *)ptr = n;
            break;

        case LONG_SIZE:
            *(long *)ptr = n;
            break;
        }
        nmatch = 1;
    }
    return nmatch;
}

/* Given a scanf character class specification (in square brackets), build a
 * table that maps characters to whether they are allowed or not in the
 * following nonempty sequence of characters in the input.
 *
 * We support inversion with '^' and specifying ']' in the set by providing it
 * as the first character, but character ranges are unsupported.  */
static const uchar *build_stopchar_tab(const uchar *ufmt, uchar *stopchar_tab)
{
    bool first;
    uchar c;
    uchar fill_char;

    if (*ufmt == '^')
    {
        ufmt++;
        fill_char = 0; /* Inverting, so no characters are stop characters by default. */
    }
    else
    {
        fill_char = 1; /* Not inverting, so all characters are stop characters by default. */
    }

    memset(stopchar_tab, fill_char, 256);

    /* Invert the fill character. */
    fill_char ^= 1;

    first = TRUE;
    for (;;)
    {
        c = *ufmt++;

        if (c == ']' && !first)
        {
            break;
        }
        first = FALSE;

        if (c == '\0')
        {
            ufmt--;
            break;
        }

        stopchar_tab[c] = fill_char;
    }
    return ufmt;
}
