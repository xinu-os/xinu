XMMS
====

`thumb\|400px\|left\|A full M&M jug. <Image:Mms_full_02.jpg>`__
`thumb\|400px\|left\|The M&M jug from a more dramatic
angle. <Image:XMMS-02-13-2008b.jpg>`__ M&M's are small candy coated milk
chocolate delights that come in a rainbow of colors and several
variations. The XINU Lab maintains a supply of these colorful treats so
that members of the lab can make it through the hard days and long
nights of XINU development.

`thumb\|200px\|center\|Reference shot for 3-45 oz bags of M&Ms in the
jug. <Image:145ozmm.jpg>`__ `thumb\|300px\|center\|Reference shot for
4.5-56 oz bags of M&Ms in the jug. <Image:Mms_full_01.jpg>`__

Number of M&Ms in the jug
-------------------------

After filling the M&M jug about halfway full, I wondered about how many
M&Ms it held. A little investigation and I developed this program to
provide the number of M&Ms given :math:`n` ounces.

mm.c
~~~~

Perhaps someone should port this to work with the XINU code base.

.. code:: c

    #include <stdio.h>
    #include <stdlib.h>

    void usage(char *prog);

    int main(int argc, char **argv)
    {
        int oz, mm;

        if (argc != 2)
        {
            usage(argv[0]);
        }

        oz = atoi(argv[1]);

        /* empirical evidence shows 500 M&Ms per pound */
        mm = 500 * (oz / 16);

        /* catch the remaining oz */
        mm += (31 * (oz % 16));

        /* technically it is 31.25 M&Ms per ounce */
        /* so add 1 M&M for every 4 ounces */
        mm += ((oz % 16) / 4);

        printf("There are approximately %d M&Ms in %d ounces.\n", mm, oz);

        return 0;
    }

    void usage(char *prog)
    {
        fprintf(stderr, "Usage: %s oz\n", prog);
        fprintf(stderr, "  Calculate the number of M&Ms in oz ounces.\n");
        exit(1);
    }

