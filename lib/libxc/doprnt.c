/**
 * @file doprnt.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdio.h>
#include <string.h>

/* Number of bits in an 'unsigned long'.  */
#define LONG_BITS (8 * sizeof(unsigned long))

static void ulong_to_string(unsigned long num, char *str,
                            unsigned int base, bool alt_digits);

/**
 * @ingroup libxc
 *
 * Write formatted output.
 *
 * This is a minimal implementation and not all standard features are supported.
 * The supported conversion specifications include:
 *
 * - @c \%d to print an <code>int</code> in decimal.
 * - @c \%b to print an <code>unsigned int</code> in binary.
 * - @c \%o to print an <code>unsigned int</code> in octal.
 * - @c \%u to print an <code>unsigned int</code> in decimal.
 * - @c \%x to print an <code>unsigned int</code> in lower case hex.
 * - @c \%X to print an <code>unsigned int</code> in upper case hex.
 * - @c \%c to print a single character.
 * - @c \%s to print a null-terminated string, or "(null)" for a @c NULL pointer.
 * - @c \%\% to print a literal percent sign.
 * - @c '-' flag to specify left-justification.
 * - @c '0' flag to specify zero padding.
 * - Minimum field width specification.
 * - @c '.PREC' optional precision to set maximum field length.
 * - @c '*' flag to specify that the minimum field width or precision is specified
 *   as an @c int argument rather than literally in the format string.
 *
 * If a feature is not mentioned above, assume it is not supported.
 *
 * @param fmt
 *      Format string.
 * @param ap
 *      Variable-length list of values that will be formatted.
 * @param putc_func
 *      Character output function.  It is passed two arguments; the first will
 *      be the character to output, and the second will be @p putc_arg.  It is
 *      expected to return @c EOF on failure.
 * @param putc_arg
 *      Second argument to @p putc_func.
 *
 * @return
 *      number of characters written on success, or @c EOF on failure
 */
int _doprnt(const char *fmt, va_list ap,
            int (*putc_func) (int, int), int putc_arg)
{
    int i;
    char *str;                  /* Running pointer in string            */
    char string[LONG_BITS + 1]; /* The string str points to this output */

    /*  from number conversion              */
    int length;                 /* Length of string "str"               */
    char fill;                  /* Fill character (' ' or '0')          */
    int leftjust;               /* 0 = right-justified, else left-just  */
    int fmax, fmin;             /* Field specifications % MIN . MAX s   */
    int leading;                /* No. of leading/trailing fill chars   */
    char sign;                  /* Set to '-' for negative decimals     */
    long larg;
    int chars_written = 0;      /* Number of characters written so far  */
    const char *spec_start;     /* Start of this format specifier.      */
    bool alt_digits;            /* Use alternate digits?                */
    unsigned int base;          /* Base to use for printing.            */

    while (*fmt != '\0')
    {
        if (*fmt == '%' && *++fmt != '%')
        {
            /* Conversion specification.  */
            spec_start = fmt - 1;

            /* Check for "%-..." == Left-justified output */
            leftjust = 0;
            if (*fmt == '-')
            {
                leftjust = 1;
                fmt++;
            }

            /* Allow for zero-filled numeric outputs ("%0...") */
            fill = ' ';
            if (*fmt == '0')
            {
                /* '-' overrides '0' */
                if (!leftjust)
                {
                    fill = '0';
                }
                fmt++;
            }

            /* Allow for minimum field width specifier for %d,u,x,o,c,s */
            /* Also allow %* for variable width (%0* as well)       */
            fmin = 0;
            if (*fmt == '*')
            {
                fmin = va_arg(ap, int);
                fmt++;
            }
            else
            {
                while ('0' <= *fmt && *fmt <= '9')
                {
                    fmin *= 10;
                    fmin += (*fmt - '0');
                    fmt++;
                }
            }

            /* Allow optional precision (e.g. maximum string width for %s) */
            fmax = 0;
            if (*fmt == '.')
            {
                fmt++;
                if (*fmt == '*')
                {
                    fmax = va_arg(ap, int);
                    fmt++;
                }
                else
                {
                    while ('0' <= *fmt && *fmt <= '9')
                    {
                        fmax *= 10;
                        fmax += (*fmt - '0');
                        fmt++;
                    }
                }
            }

            str = string;
            sign = '+';    /* sign == '-' for negative decimal */
            alt_digits = FALSE;
            base = 0;
            switch (*fmt++) /* Switch on the format specifier character. */
            {
            case 'c':
                string[0] = va_arg(ap, int);
                string[1] = '\0';
                fmax = 0;
                fill = ' ';
                break;

            case 's':
                str = va_arg(ap, char *);
                if (NULL == str)
                {
                    str = "(null)";
                }
                fill = ' ';
                break;

            case 'd':
                larg = va_arg(ap, int);
                if (larg < 0)
                {
                    sign = '-';
                    larg = -larg;
                }
                base = 10;
                break;

            case 'u':
                larg = va_arg(ap, unsigned int);
                base = 10;
                break;

            case 'o':
                larg = va_arg(ap, unsigned int);
                base = 8;
                break;

            case 'X':
                larg = va_arg(ap, unsigned int);
                base = 16;
                alt_digits = TRUE;
                break;

            case 'x':
                larg = va_arg(ap, unsigned int);
                base = 16;
                break;

            case 'b':
                larg = va_arg(ap, unsigned int);
                base = 2;
                break;

            default:
                /* Unknown format specifier; this also includes the case where
                 * we encounted the end of the format string prematurely.  Write
                 * the '%' literally and continue parsing from the next
                 * character.  */
                fmt = spec_start;
                goto literal;
            }

            if (base != 0)
            {
                /* Print a number to the temporary buffer.  */
                ulong_to_string(larg, str, base, alt_digits);
                fmax = 0;
            }

            length = strlen(str);

            leading = 0;
            if (fmax != 0 || fmin != 0)
            {
                if (fmax != 0)
                {
                    if (length > fmax)
                    {
                        length = fmax;
                    }
                }
                if (fmin != 0)
                {
                    leading = fmin - length;
                }
                if (sign == '-')
                {
                    --leading;
                }
            }
            if (sign == '-' && fill == '0')
            {
                if ((*putc_func) (sign, putc_arg) == EOF)
                {
                    return EOF;
                }
                chars_written++;
            }
            if (leftjust == 0)
            {
                for (i = 0; i < leading; i++)
                {
                    if ((*putc_func) (fill, putc_arg) == EOF)
                    {
                        return EOF;
                    }
                    chars_written++;
                }
            }
            if (sign == '-' && fill == ' ')
            {
                if ((*putc_func) (sign, putc_arg) == EOF)
                {
                    return EOF;
                }
                chars_written++;
            }
            for (i = 0; i < length; i++)
            {
                if ((*putc_func) (str[i], putc_arg) == EOF)
                {
                    return EOF;
                }
                chars_written++;
            }
            if (leftjust != 0)
            {
                for (i = 0; i < leading; i++)
                {
                    if ((*putc_func) (fill, putc_arg) == EOF)
                    {
                        return EOF;
                    }
                    chars_written++;
                }
            }
        }
        else
        {
literal:
            /* Literal character.  */
            if ((*putc_func) (*fmt, putc_arg) == EOF)
            {
                return EOF;
            }
            chars_written++;
            fmt++;
        }
    }
    return chars_written;
}

static const char digits_lc[16] = "0123456789abcdef";
static const char digits_uc[16] = "0123456789ABCDEF";
static const unsigned char base_to_nbits[17] = {
    [2]  = 1,
    [4]  = 2,
    [8]  = 3,
    [16] = 4,
};

/* When this is not defined, special code is used to speed up
 * converting numbers to a a string with power-of-two base.  */
/*#define ALWAYS_USE_DIVISION*/

/**
 * Convert an unsigned long integer to a string.
 *
 * @param num
 *      Number to convert.
 * @param str
 *      Buffer into which to write the string.
 * @param base
 *      Base to use; must be less than or equal to 16.
 * @param alt_digits
 *      TRUE if hex digits should be upper case rather than lowercase.
 */
static void ulong_to_string(unsigned long num, char *str,
                            unsigned int base, bool alt_digits)
{
    const char *digits = digits_lc;
    char temp[LONG_BITS + 1];
    int i;

    /* Print the string to a temporary buffer in
     * reverse order before copying it to @str.  */
    digits = (alt_digits) ? digits_uc : digits_lc;
    temp[0] = '\0';
#ifdef ALWAYS_USE_DIVISION
    if (TRUE)
#else
    if (base_to_nbits[base] == 0)
#endif
    {
        /* Use modulo operation and integral division.  */
        for (i = 1; num != 0; i++)
        {
            temp[i] = digits[num % base];
            num /= base;
        }
    }
    else
    {
        /* Use masking and shifting (works when base is a power of 2) */
        unsigned char shift = base_to_nbits[base];
        unsigned long mask = (1 << shift) - 1;
        for (i = 1; num != 0; i++)
        {
            temp[i] = digits[num & mask];
            num >>= shift;
        }
    }

    /* Reverse string and copy it to @str.  */

    i--;
    if (i == 0)
    {
        /* If no digits were printed, then the number was 0 and we should at
         * least print a 0.  */
        temp[++i] = '0';
    }

    while (i >= 0)
    {
        *str++ = temp[i--];
    }
}
