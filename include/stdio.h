/**
 * @file stdio.h
 * @provides puts, printf, scanf.
 *
 * $Id: stdio.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>

/* For thrtab and thrcurrent. */
#include <thread.h>

/**
 * Formatted input
 */
int _doscan(register char *, register int **, int (*)(void),
            int (*)(char), int, int);
int fscanf(int, char *, int);

#define scanf(fmt, args)      fscanf(CONSOLE, fmt, args)
int sscanf(char *, char *, int);

/**
 * Standard in/out/err
 * The C99 specification states that they are macro expansions to a pointer
 * to FILE.  I say, close enough for Xinu.
 * C99 doc: http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1256.pdf
 */
#define stdin ((thrtab[thrcurrent]).fdesc[0])
#define stdout ((thrtab[thrcurrent]).fdesc[1])
#define stderr ((thrtab[thrcurrent]).fdesc[2])

/**
 * Formatted output
 */
void _doprnt(char *, va_list, int (*)(int, int), int);
int fprintf(int, char *, ...);
int printf(const char *, ...);
int sprintf(char *, char *, ...);

/**
 * Character input and output
 */
int fgetc(int);
char *fgets(char *, int, int);
int fputc(int, int);
int fputs(char *, int);
int putchar(int c);
int getchar(void);

#endif                          /* __STDIO_H__ */
