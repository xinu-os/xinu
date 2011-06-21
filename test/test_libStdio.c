#include <stddef.h>
#include <testsuite.h>
#include <interrupt.h>
#include <stdio.h>
#include <string.h>
#include <loopback.h>

/**
 * Tests the stdio.h header in the Xinu Standard Library.
 * @return OK when testing is complete
 */
thread test_libStdio(bool verbose)
{
#if LOOP
    char str[50];
    int stdsav;
    ulong ul;
    bool passed = TRUE;

    enable();

    open(LOOP);

    /* fprintf */
    testPrint(verbose, "fprintf");
    fprintf(LOOP, "%d %o %x %c %s", 75, 75, 75, 75, "ABC");
    read(LOOP, str, 15);
    failif((0 != strncmp(str, "75 113 4b K ABC", 15)), "");

    /* printf */
    testPrint(verbose, "printf");
    stdsav = stdout;
    stdout = LOOP;
    printf("%d %o %x %c %s", 75, 75, 75, 75, "ABC");
    stdout = stdsav;
    read(LOOP, str, 15);
    failif((0 != strncmp(str, "75 113 4b K ABC", 15)), "");

    /* sprintf */
    testPrint(verbose, "sprintf");
    sprintf(str, "%d %o %x %c %s", 75, 75, 75, 75, "ABC");
    failif((0 != strncmp(str, "75 113 4b K ABC", 15)), "");

    /* fscanf */

    /* scanf */

    /* sscanf */

    /* fgetc */
    testPrint(verbose, "fgetc");
    putc(LOOP, 'a');
    putc(LOOP, '\n');
    putc(LOOP, 4);
    str[0] = fgetc(LOOP);
    str[1] = fgetc(LOOP);
    str[2] = fgetc(LOOP);

    failif((('a' != str[0]) || ('\n' != str[1]) || (4 != str[2])), "");

    /* fgets */
    testPrint(verbose, "fgets");
    write(LOOP, "Test sentence.\n", 15);
    fgets(str, 20, LOOP);
    failif((0 != strncmp(str, "Test sentence.\n", 15)), "");

    /* fputc */
    testPrint(verbose, "fputc");
    ul = fputc('a', LOOP);
    failif((((ulong)'a' != ul) || ((ulong)'a' != getc(LOOP))), "");

    /* fputs */
    testPrint(verbose, "fputs");
    fputs("Put test.", LOOP);
    read(LOOP, str, 9);
    failif((0 != strncmp(str, "Put test.", 9)), "");

    /* getchar */
    testPrint(verbose, "getchar");
    putc(LOOP, 'a');
    putc(LOOP, '\n');
    putc(LOOP, 4);
    stdsav = stdin;
    stdin = LOOP;
    str[0] = getchar();
    str[1] = getchar();
    str[2] = getchar();
    stdin = stdsav;;
    failif((('a' != str[0]) || ('\n' != str[1]) || (4 != str[2])), "");

    /* putchar */
    testPrint(verbose, "putchar");
    stdsav = stdout;
    stdout = LOOP;
    ul = putchar('a');
    stdout = stdsav;
    failif(((ulong)'a' != ul) || ((int)'a' != getc(LOOP)), "");

    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    close(LOOP);
#endif                          /* LOOP */
    return OK;
}
