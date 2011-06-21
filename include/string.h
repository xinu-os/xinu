/**
 * @file string.h
 *
 * $Id: string.h 2033 2009-08-13 21:25:14Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _STRING_H_
#define _STRING_H_

char *strncpy(char *, const char *, int);
char *strncat(char *, const char *, int);
int strncmp(const char *, const char *, int);
char *strchr(const char *, int);
char *strrchr(const char *, int);
char *strstr(const char *, const char *);
int strnlen(const char *, uint);

int memcmp(const void *, const void *, int);
void *memcpy(void *, const void *, int);
void *memchr(const void *, int, int);
void *memset(void *, int, int);

#endif                          /* _STRING_H_ */
