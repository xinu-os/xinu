/**
 * @file rand.c
 * @provides srand, rand.
 *
 * $Id: rand.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

static unsigned long randx = 1;

/**
 * Sets the random seed.
 * @param x random seed
 */
void srand(unsigned long x)
{
    randx = x;
}

/**
 * Generates a random long.
 * @return random long
 */
unsigned long rand(void)
{
    return (((randx = randx * 1103515245 + 12345) >> 16) & 077777);
}
