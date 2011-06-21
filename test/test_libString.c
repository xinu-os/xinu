#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <testsuite.h>

#define LEN_STR 7

/**
 * Tests the string.h header in the Xinu Standard Library.
 * @return OK when testing is complete
 */
thread test_libString(bool verbose)
{
    char *s1 = NULL;
    char *s2 = NULL;
    char str[LEN_STR] = "ZYXWYZ";

    bool passed = TRUE;

    /* strncmp */
    testPrint(verbose, "String n comparison");
    failif(((0 > strncmp("ZYX", "WVU", 2))
            || (0 < strncmp("wvu", "zyx", 2))
            || (0 < strncmp("wvu", "zyx", 50))
            || (0 > strncmp("zyx", "wvu", 50))
            || (0 != strncmp("abc", "abc", 50))
            || (0 != strncmp("987", "987", 2))
            || (0 < strncmp("NOP", "NOQ", 3))
            || (0 > strncmp("noq", "nop", 3))
            || (0 != strncmp("450", "456", 2))), "");

    /* strncpy */
    testPrint(verbose, "String n copy");
    char sB[6] = "abcde";

    s1 = strncpy(sB, "fghij", 3);
    failif(((0 != strncmp(sB, "fghde", 5)) || (s1 != sB)), "");

    /* strncat */
    testPrint(verbose, "String n concantenate");
    char sD[6] = "zyx";

    s1 = strncat(sD, "wv", 1);
    failif(((0 != strncmp(sD, "zyxw", 4))
            || (0 != strncmp(s1, "zyxw", 4))), "");

    /* * * strchr * * */
    /* fail to find char */
    testPrint(verbose, "Fail to find; strchr()");
    s1 = strchr(str, 'a');
    failif(NULL != s1, "");

    /* find a uniq char */
    testPrint(verbose, "Find unique char; strchr()");
    s1 = strchr(str, 'X');
    failif('W' != *(s1 + 1), "");

    /* find a non-uniq char */
    testPrint(verbose, "Find non-unique char; strchr()");
    s1 = strchr(str, 'Y');
    failif('X' != *(s1 + 1), "");

    /* find null char */
    testPrint(verbose, "Find null char; strchr()");
    s1 = strchr(str, '\0');
    failif(str + (LEN_STR - 1) != s1 || '\0' != *s1, "");

    /* * * strrchr * * */
    /* fail to find char */
    testPrint(verbose, "Fail to find; strrchr()");
    s1 = strrchr(str, 'a');
    failif(NULL != s1, "");

    /* find a uniq char */
    testPrint(verbose, "Find unique char; strrchr()");
    s1 = strrchr(str, 'X');
    failif('W' != *(s1 + 1), "");

    /* find a non-uniq char */
    testPrint(verbose, "Find non-unique char; strrchr()");
    s1 = strrchr(str, 'Y');
    failif('Z' != *(s1 + 1), "");

    /* find null char */
    testPrint(verbose, "Find null char; strrchr()");
    s1 = strrchr(str, '\0');
    failif(str + (LEN_STR - 1) != s1 || '\0' != *s1, "");

    /* strstr */
    testPrint(verbose, "String search");
    char sG[9] = "ABBCDEBA";

    s1 = strstr(sG, "BCD");
    s1 += 3;
    failif(('E' != *s1), "");

    /* strnlen */
    testPrint(verbose, "String n length");
    failif(((5 != strnlen("12345", 100))
            || (0 != strnlen("", 5))
            || (3 != strnlen("12345", 3))
            || (0 != strnlen("12345", 0))), "");

    /* memcmp */
    testPrint(verbose, "Memory comparsion");
    failif(((0 < memcmp("ABC", "DEF", 3))
            || (0 > memcmp("def", "abc", 3))
            || (0 != memcmp("123", "123", 3))), "");

    /* memcpy */
    testPrint(verbose, "Memory copy");
    char sH[6] = "ABCDE";

    s1 = memcpy(sH, "FGHIJ", 5);
    failif(((0 != memcmp(sH, "FGHIJ", 5))
            || (0 != memcmp(s1, "FGHIJ", 5))), "");

    /* memchr */
    testPrint(verbose, "Memory character search");
    char sI[7] = "abcdba";

    s1 = memchr(sI, 'b', 6) + 1;
    s2 = memchr(sI, 'c', 6) + 1;
    failif((('c' != *s1) || ('d' != *s2)), "");

    /* memset */
    testPrint(verbose, "Memory set");
    char sJ[6] = "ABCDE";

    s1 = memset(sJ, 'F', 3);
    failif(((0 != memcmp(sJ, "FFFDE", 5)) || (s1 != sJ)), "");

    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
