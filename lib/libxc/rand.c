/**
 * @file rand.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdlib.h>

static unsigned int seed = 1;

/**
 * @ingroup libxc
 *
 * Sets the random seed that will be used in future calls to rand().
 *
 * @param x
 *      the random seed to set
 */
void srand(unsigned int x)
{
    seed = x;
}

/**
 * @ingroup libxc
 *
 * Generates a pseudorandom integer in the range [0, RAND_MAX].
 * Beware: this is not re-entrant.
 *
 * @return the random integer
 */
int rand(void)
{
    /* This uses a linear congruential generator.  The seed retains a 32-bit
     * state, but for the actual random numbers returned we take just some of
     * the upper bits, which are more random than the lower bits.  */
    seed = (seed * 1103515245) + 12345;
    return (seed >> 16) % (RAND_MAX + 1);
}
