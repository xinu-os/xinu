/**
 * @file sscanf.c
 * @provides sscanf, sgetch, sungetch.
 *
 * $Id: sscanf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#define EOF   (-2)

static int sgetch(int, char **);
static int sungetch(int, char **);
extern int _doscan(register char *, register int **,
                   int (*getc) (int, char **),
                   int (*ungetc) (int, char **), int, int);

/**
 * Read from a string according to a format.
 * @param *str string to read from
 * @param *fmt format string
 * @param args number of arguments in format string
 * @return result of _doscan
 */
int sscanf(char *str, char *fmt, int args)
{
    char *s;

    s = str;
    return (_doscan(fmt, (int **)&args, sgetch, sungetch, 0, (int)&s));
}

/**
 * Get the next character from a string.
 * @param dummy unused variable
 * @param cpp string to read next character from
 */
static int sgetch(int dummy, char **cpp)
{
    return (*(*cpp) == '\0' ? EOF : *(*cpp)++);
}

/**
 * Pushback a character in a string.
 * @param dummy unused variable
 * @param cpp string to pushback character to
 */
static int sungetch(int dummy, char **cpp)
{
    return (*(*cpp)--);
}
