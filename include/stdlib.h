/**
 * @file stdlib.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>

int abs(int j);
long labs(long j);
int atoi(const char *nptr);
long atol(const char *nptr);
void bzero(void *s, size_t n);
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void*));
int rand(void);
void srand(unsigned int seed);
void *malloc(size_t size) __attribute__((deprecated));
void free(void *ptr) __attribute__((deprecated));

#define RAND_MAX 32767

#endif /* _STDLIB_H_ */
