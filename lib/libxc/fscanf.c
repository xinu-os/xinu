/**
 * @file fscanf.c
 * @provides fscanf, getch, ungetch.
 *
 * $Id: fscanf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#define EMPTY (-1)
#define EOF   (-2)

extern int getc(int);

static int getch(int, int);
static int ungetch(int, int);
extern int _doscan(register char *, register int **,
                   int (*getc) (int, int), int (*ungetc) (int, int), int,
                   int);

/**
 * Read from a device (file) according to a format.
 * @param dev device to read from
 * @param *fmt format string
 * @param args number of arguments in format string
 * @return result of _doscan
 */
int fscanf(int dev, char *fmt, int args)
{
    int buf;

    buf = EMPTY;
    return (_doscan
            (fmt, (int **)&args, getch, ungetch, dev, (int)(int)&buf));
}

/** 
 * Get a character from a device with pushback.
 * @param dev device to read from
 * @param abuf buffer for reading into
 */
static int getch(int dev, int abuf)
{
    int *buf = (int *)abuf;

    if (*buf != EOF && *buf != EMPTY)
    {
        *buf = getc(dev);
    }
/* 	if( *buf != EOF ) */
/* 	{ *buf = control(dev, TTY_IOC_NEXTC, 0, 0); } */
    return (*buf);
}

/**
 * Pushback a character for getch.
 * @param dev device to push back to
 * @param abuf buffer for pushing back from
 */
static int ungetch(int dev, int abuf)
{
    int *buf = (int *)abuf;

    *buf = EMPTY;
    return (*buf);
}
