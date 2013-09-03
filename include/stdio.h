/**
 * @file stdio.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>

/* For thrtab and thrcurrent. */
#include <thread.h>

/*
 * Standard in/out/err
 * Note: The C99 specification states that they are macro expansions to a
 * pointer to FILE.  I say, close enough for Xinu.
 * C99 doc: http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1256.pdf
 */

/** @ingroup libxc
 * Standard input  */
#define stdin ((thrtab[thrcurrent]).fdesc[0])

/** @ingroup libxc
 * Standard output  */
#define stdout ((thrtab[thrcurrent]).fdesc[1])

/** @ingroup libxc
 * Standard error  */
#define stderr ((thrtab[thrcurrent]).fdesc[2])

/**
 * Formatted input
 */
int _doscan(const char *fmt, va_list ap,
            int (*getch) (int, int), int (*ungetch) (int, int),
            int arg1, int arg2);

int fscanf(int dev, const char *format, ...);

/**
 * @ingroup libxc
 */
#define scanf(fmt, ...)      fscanf(stdin, fmt, __VA_ARGS__)

int sscanf(const char *str, const char *format, ...);

/* Formatted output */
int _doprnt(const char *format, va_list,
	    int (*putc_func)(int, int), int putc_arg);

int fprintf(int dev, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);

/* Character input and output  */
int fgetc(int dev);
char *fgets(char *s, int n, int dev);
int fputc(int c, int dev);
int fputs(const char *s, int dev);

/** @ingroup libxc */
#define putchar(c) fputc((c), stdout)

/** @ingroup libxc */
#define getchar() fgetc(stdin)

#endif  /* _STDIO_H_ */
