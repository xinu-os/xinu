/**
 * @file qsort.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdlib.h>

static size_t partition(void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void*));

static void swap_elements(void *p1, void *p2, size_t size);

/**
 * @ingroup libxc
 *
 * Sorts an array of data using quicksort.  The average-case running time is O(n
 * log n) but the worst case running time is O(n^2).
 *
 * @param base
 *      Pointer to the array of data to sort.
 * @param nmemb
 *      Number of elements in the array.
 * @param size
 *      Size of each element in the array, in bytes.
 * @param compar
 *      Comparison callback function that is passed pointers to two elements in
 *      the array.  It must return a negative value, 0, or a positive value if
 *      the first element is less than, equal to, or greater than the second
 *      element, respectively.
 */
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *))
{
    if (nmemb > 1)
    {
        /* Partition the array around a pivot element, then recurse on parts of
         * the array before and after the pivot.  */
        size_t pivot_index = partition(base, nmemb, size, compar);
        qsort(base, pivot_index, size, compar);
        qsort(base + (pivot_index + 1) * size, nmemb - (pivot_index + 1),
              size, compar);
    }
}

/*
 * Does quicksort partitioning on an array of length 2 or greater.  The first
 * element is taken to be the pivot.  The array is re-arranged so that all
 * elements before the pivot compare less than or equal to it and all elements
 * after the pivot compare greater than or equal to it.  The return value is the
 * resulting 0-based index of the pivot.
 */
static size_t partition(void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void*))
{
    void *p1, *p2;
    size_t p1_index = 1;

    /* Pivot is at @base.  */
    p1 = base + size;
    p2 = base + (nmemb * size);
    do
    {
        if ((*compar)(p1, base) <= 0)
        {
            /* Element at @p1 can go before pivot.  Continue onto next element.
             * */
            p1 += size;
            p1_index++;
        }
        else
        {
            /* Element at @p1 must go after pivot.  Swap it with the element at
             * @p2, after decrementing @p2 so it points to the next element on
             * the right end of the array.  Then, @p1 will be left unmodified so
             * it points to the element that was previously at @p2.  */
            p2 -= size;
            swap_elements(p1, p2, size);
        }
    } while (p1 != p2);

    /* Now, p1 == p2, and all elements left of @p1 compare less than or equal to
     * the pivot, while all elements right of or at @p1 compare greater than or
     * equal to the pivot.  Finish by swapping the pivot into its final position
     * and returning its index.  */
    p1 -= size;
    p1_index--;
    swap_elements(p1, base, size);
    return p1_index;
}

/* Swaps the two elements of the specified size, pointed to by @p1 and @p2.  */
static void swap_elements(void *_p1, void *_p2, size_t size)
{
    unsigned char *p1 = _p1;
    unsigned char *p2 = _p2;
    size_t i;
    unsigned char tmp;

    for (i = 0; i < size; i++)
    {
        tmp = p1[i];
        p1[i] = p2[i];
        p2[i] = tmp;
    }
}
