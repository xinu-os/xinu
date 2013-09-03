#include <stddef.h>
#include <interrupt.h>
#include <stdio.h>
#include <string.h>
#include <testsuite.h>
#include <nvram.h>
#include <memory.h>

/**
 * Tests the nvram abilities of Xinu.
 * @return OK when testing is complete
 */
thread test_nvram(bool verbose)
{
#if NVRAM
    char *str;
    uint value, temp;
    irqmask im;
    bool passed;

    passed = TRUE;

    /* make sure we are initialized properly */
    testPrint(verbose, "initialization");
    value = nvramInit();
    failif(((OK != value)
            || (nvram_header == NULL)
            || (nvram_header->magic != NVRAM_MAGIC)), "erroneous value");

    /* make sure we are not consuming memory beyond true init */
    testPrint(verbose, "memory consumption");

    im = disable();
    value = memlist.length;
    nvramInit();
    temp = memlist.length;
    restore(im);

    failif(temp != value, "lost memory");

    /* make sure nvramGet for a good var is correct */
    testPrint(verbose, "good variable get()");
    /* CFE makes sure this variable is set */
#define NVRAM_GOODVAR "et0macaddr"
    str = nvramGet(NVRAM_GOODVAR);
    /* backup since we're passed a pointer to the value */
    if (NULL != str)
    {
        str = str - strnlen(NVRAM_GOODVAR, NVRAM_STRMAX) - 1;
        temp = strnlen(NVRAM_GOODVAR, NVRAM_STRMAX);
    }

    failif((NULL == str)
           || (0 != strncmp(NVRAM_GOODVAR, str, temp)),
           "bad good string detected");

    /* make sure nvramGet for a bad var is NULL */
    testPrint(verbose, "bad variable get()");
    /* we'll just assume they don't have this variable */
#define NVRAM_BADVAR "nvram_bad_var_5"
    str = nvramGet(NVRAM_BADVAR);

    /* nvramGet should return NULL if not found */
    failif((NULL != str), "good bad string detected");

    /* make sure nvramGet for a NULL var is NULL */
    testPrint(verbose, "null variable get()");
    str = nvramGet(NULL);

    /* nvramGet should return NULL if not found */
    failif((NULL != str), "non-null value returned");

    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }
#else /* NVRAM */
    testSkip(TRUE, "");
#endif /* !NVRAM */
    return OK;
}
