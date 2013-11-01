Billionrandom
=============

.. code:: c

    #include <stdio.h>
    #include <stdlib.h>
    #include <limits.h>

    void printNumber(unsigned long num, int base)
    {
      unsigned long i = 0UL;

      switch (base)
        {
        case 2:
          printf("0b");
          for (i = INT_MAX; i >= num; i = i >> 1)
        ;
          for (i = (i << 1) & ~i; i > 0; i = i >> 1)
        { printf("%c", (i & num) ? '1' : '0'); }
          printf("\t");
          break;
        case 8:
          printf("0%o\t", num);
          break;
        case 10:
          printf("%u\t", num);
          break;
        case 16:
          printf("0x%X\t", num);
          break;
        }
    }

    int main(int argc, char **argv)
    {
      unsigned long a = 1664525UL, 
        b = 1013904223UL, 
        c = 0UL, 
        d = 0UL, 
        e = 1000000000UL;
      
      char *val = 0;

      if (2 == argc)
        {
          if (('-' == argv[1][0])
          && ('v' == argv[1][1]))
        {
          printf("COSC 125, Spring 2006\n"
             "Linear Congruential Generator, v1.0\n");
          exit(0);
        }
          val = argv[1];
          e = 0UL;
          while (0 != *val)
        {
          e = e * 10 + *val - '0';
          val++;
        }
        }

      for (c = 0UL; c < e; c++)
        {
          d = a * d + b;
          if (0 == (d % 1000))
        { printf("\n"); }
          switch (c % 4)
        {
        case 0: printNumber(d, 2);  break;
        case 1: printNumber(d, 8);  break;
        case 2: printNumber(d, 10); break;
        case 3: printNumber(d, 16); break;
        }
        }

      printf("\n");
      return c;
    }

