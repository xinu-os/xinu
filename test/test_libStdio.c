#include <interrupt.h>
#include <limits.h>
#include <loopback.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <testsuite.h>

#define TEST_STR "75 113 4b K ABC"
#define TEST_STR_LEN (sizeof(TEST_STR) - 1)

static bool do_detailed_fprintf_tests(bool verbose, bool passed);
static bool do_detailed_fscanf_tests(bool verbose, bool passed);

/**
 * Tests the stdio.h header in the Xinu Standard Library.
 * @return OK when testing is complete
 */
thread test_libStdio(bool verbose)
{
#if defined(LOOP0)
    char str[50];
    int stdsav;
    int d, o, x;
    char c;
    char s[50];
    bool passed = TRUE;
    int ret;
    char *pret;

    ret = open(LOOP0);
    failif(ret == SYSERR, "failed to open loopback device");

    ret = control(LOOP0, LOOP_CTRL_SET_FLAG, LOOP_NONBLOCK, 0);
    failif(ret == SYSERR, "failed to set loopback device to nonblocking");

    /* fputc, fgetc.  Note: return value of fgetc() must be stored in 'int', not
     * 'char', because it can be 257 possible values (all characters, plus EOF).
     * */
    testPrint(verbose, "fgetc, fputc: basic functionality");
    ret = fputc('a', LOOP0);
    failif(ret != 'a', "fputc failed to return put character");
    ret = fputc('\n', LOOP0);
    failif(ret != '\n', "fputc failed to return put character");
    ret = fputc(4, LOOP0);
    failif(ret != 4, "fputc failed to return put character");
    ret = fgetc(LOOP0);
    failif(ret != 'a', "fgetc failed to return character");
    ret = fgetc(LOOP0);
    failif(ret != '\n', "fgetc failed to return character");
    ret = fgetc(LOOP0);
    failif(ret != 4, "fgetc failed to return character");

    /* fgets */

    /* fgets - test basic functionality */
    testPrint(verbose, "fgets: basic functionality");
    ret = write(LOOP0, "Test sentence.\n", 15);
    failif(ret != 15, "failed to write data to loopback device");
    memset(str, 'X', 20);
    pret = fgets(str, 20, LOOP0);
    failif(pret != str, "fgets failed to return output buffer");
    failif((0 != memcmp(str, "Test sentence.\n\0XXXX", 20)),
           "fgets failed to correctly return written data");

    /* fgets - Test partial reads */
    testPrint(verbose, "fgets: partial reads");
    ret = write(LOOP0, "Test sentence.\n", 15);
    failif(ret != 15, "failed to write data to loopback device");
    memset(str, 'X', 20);
    pret = fgets(str, 8, LOOP0);
    failif(pret != str, "fgets failed to return output buffer");
    failif((0 != memcmp(str, "Test se\0XX", 10)),
           "fgets failed to correctly return written data");
    memset(str, 'X', 20);
    pret = fgets(str, 9, LOOP0);
    failif(pret != str, "fgets failed to return output buffer");
    failif((0 != memcmp(str, "ntence.\n\0X", 10)),
           "fgets failed to correctly return written data");

    /* fgets - check returns NULL at end-of-file */
    testPrint(verbose, "fgets: return NULL on end-of-file");
    pret = fgets(str, 9, LOOP0);
    failif(pret != NULL, "fgets failed to return NULL on end-of-file");

    /* fgets - Test reading stops at newline */
    testPrint(verbose, "fgets: only read until newline");
    ret = write(LOOP0, "line1\nline2\n", 12);
    failif(ret != 12, "failed to write data to loopback device");
    memset(str, 'X', 20);
    pret = fgets(str, 16, LOOP0);
    failif(pret != str, "fgets failed to return output buffer");
    failif(0 != memcmp(str, "line1\n\0XXX", 10),
           "fgets failed to read only partial line");

    memset(str, 'X', 20);
    pret = fgets(str, 16, LOOP0);
    failif(pret != str, "fgets failed to return output buffer");
    failif(0 != memcmp(str, "line2\n\0XXX", 10),
           "fgets failed to read only partial line");

    /* fputs */
    testPrint(verbose, "fputs: basic functionality");
    ret = fputs("Put test.", LOOP0);
    failif(ret < 0, "fputs failed to return nonnegative value on success");
    ret = read(LOOP0, str, 9);
    failif(ret != 9, "failed to read data put with fputs");
    failif((0 != strncmp(str, "Put test.", 9)),
           "data read back with fputs was not the same as written");

    /* putchar, getchar */
    testPrint(verbose, "putchar, getchar: basic functionality");
    stdsav = stdout;
    stdout = LOOP0;
    ret = putchar('a');
    stdout = stdsav;
    failif(ret != 'a', "putchar failed to return character written");
    stdsav = stdin;
    stdin = LOOP0;
    ret = getchar();
    stdin = stdsav;
    failif(ret != 'a', "getchar failed to return character previously written");

    testPrint(verbose, "getchar: return EOF on end-of-file");
    stdsav = stdin;
    stdin = LOOP0;
    ret = getchar();
    stdin = stdsav;
    failif(ret != EOF, "getchar failed to return EOF on end-of-file");

    /* fprintf */
    testPrint(verbose, "fprintf: basic functionality");
    ret = fprintf(LOOP0, "%d %o %x %c %s", 75, 75, 75, 75, "ABC");

    failif(ret != TEST_STR_LEN,
           "fprintf() did not correctly return number of characters written");

    ret = read(LOOP0, str, TEST_STR_LEN);
    failif(ret != TEST_STR_LEN,
           "failed to read data back from loop device");

    failif((0 != strncmp(str, TEST_STR, TEST_STR_LEN)),
           "fprintf() failed to print the data correctly");

    /* printf */
    testPrint(verbose, "printf: basic functionality");
    stdsav = stdout;
    stdout = LOOP0;
    ret = printf("%d %o %x %c %s", 75, 75, 75, 75, "ABC");
    stdout = stdsav;
    failif(ret != TEST_STR_LEN,
           "printf() did not correctly return number of characters written");
    ret = read(LOOP0, str, TEST_STR_LEN);
    failif(ret != TEST_STR_LEN,
           "failed to read data back from loop device");
    failif((0 != strncmp(str, TEST_STR, TEST_STR_LEN)),
           "printf() failed to print the data correctly");

    /* sprintf */
    testPrint(verbose, "sprintf: basic functionality");
    ret = sprintf(str, "%d %o %x %c %s", 75, 75, 75, 75, "ABC");
    failif(ret != TEST_STR_LEN,
           "sprintf() did not correctly return number of characters written");
    failif((0 != strncmp(str, TEST_STR, TEST_STR_LEN)),
           "sprintf() failed to print the data correctly");

    /* sscanf */
    testPrint(verbose, "sscanf: basic functionality");
    strncpy(str, TEST_STR, TEST_STR_LEN + 1);
    d = o = x = c = 0;
    ret = sscanf(str, "%d %o %x %c %s", &d, &o, &x, &c, s);
    failif(ret != 5,
           "sscanf did not correctly return number of matches");

    failif(75 != d || 75 != o || 75 != x || 75 != c ||
            0 != strncmp(s, "ABC", 3),
            "sscanf did not scan data correctly");

    /* fscanf */
    testPrint(verbose, "fscanf: basic functionality");
    d = o = x = c = 0;
    ret = write(LOOP0, TEST_STR, TEST_STR_LEN);
    failif(ret != TEST_STR_LEN,
           "failed to write data to loopback device");
    ret = fscanf(LOOP0, "%d %o %x %c %s", &d, &o, &x, &c, s);
    failif(ret != 5,
           "fscanf did not correctly return number of matches");
    failif(75 != d || 75 != o || 75 != x || 75 != c ||
            0 != strncmp(s, "ABC", 3),
            "fscanf did not scan data correctly");

    /* scanf */

    testPrint(verbose, "scanf: basic functionality");
    d = o = x = c = 0;
    ret = write(LOOP0, TEST_STR, TEST_STR_LEN);
    failif(ret != TEST_STR_LEN,
           "failed to write data to loopback device");
    stdsav = stdin;
    stdin = LOOP0;
    ret = scanf("%d %o %x %c %s", &d, &o, &x, &c, s);
    stdin = stdsav;
    failif(ret != 5, "scanf did not correctly return number of matches");
    failif(75 != d || 75 != o || 75 != x || 75 != c ||
            0 != strncmp(s, "ABC", 3), "");

    /* More detailed fprintf tests */
    passed = do_detailed_fprintf_tests(verbose, passed);

    /* More detailed fscanf tests */
    passed = do_detailed_fscanf_tests(verbose, passed);
    
    control(LOOP0, LOOP_CTRL_CLR_FLAG, LOOP_NONBLOCK, 0);

    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    close(LOOP0);
#else  /* defined(LOOP0) */
    testSkip(TRUE, "");
#endif  /* !defined(LOOP0) */
    return OK;
}

#if defined(LOOP0)

/* Declarative format for formatted printing tests.  */
static const struct {
    const char *format;
    const char *expected_output;
    uint nargs;
    uintptr_t args[4]; /* Assumes all args are 4 bytes. */
} fprintf_specs[] = {
    { /* Empty string */
        .format = "",
        .expected_output = "",
        .nargs = 0,
        .args = {},
    },
    { /* Empty string (as arg) */
        .format = "%s",
        .expected_output = "",
        .nargs = 1,
        .args = {(uintptr_t)""},
    },
    { /* Binary number */
        .format = "%b",
        .expected_output = "10001110011",
        .nargs = 1,
        .args = {0x473},
    },
    { /* Single character */
        .format = "%c",
        .expected_output = "A",
        .nargs = 1,
        .args = {'A'},
    },
    { /* Hex number (uppercase) */
        .format = "0x%X",
        .expected_output = "0xFFFFFFFF",
        .nargs = 1,
        .args = {0xffffffff},
    },
    { /* Hex number (uppercase) */
        .format = "0x%X",
        .expected_output = "0xABCDEF12",
        .nargs = 1,
        .args = {0xabcdef12},
    },
    { /* Hex number (lowercase) */
        .format = "0x%x",
        .expected_output = "0xffffffff",
        .nargs = 1,
        .args = {0xffffffff},
    },
    { /* Hex number (lowercase) */
        .format = "0x%x",
        .expected_output = "0xabcdef12",
        .nargs = 1,
        .args = {0xabcdef12},
    },
    { /* Literal percent sign */
        .format = "%%",
        .expected_output = "%",
        .nargs = 0,
        .args = {},
    },
    { /* String */
        .format = "%s",
        .expected_output = "aoeu",
        .nargs = 1,
        .args = {(uintptr_t)"aoeu"},
    },
    { /* Null string */
        .format = "%s",
        .expected_output = "(null)",
        .nargs = 1,
        .args = {NULL},
    },
    { /* Invalid/incomplete format specifier */
        .format = "%\xff", /* invalid */
        .expected_output = "%\xff",
        .nargs = 0,
        .args = {},
    },
    { /* Incomplete format specifier */
        .format = "%",
        .expected_output = "%",
        .nargs = 0,
        .args = {},
    },
    { /* Incomplete format specifier */
        .format = "%-10",
        .expected_output = "%-10",
        .nargs = 0,
        .args = {},
    },
    { /* Truncated string */
        .format = "%.5s",
        .expected_output = "trunc",
        .nargs = 1,
        .args = {(uintptr_t)"truncate me"},
    },
    { /* Truncated string, max-width specified as vararg */
        .format = "%.*s",
        .expected_output = "trunc",
        .nargs = 2,
        .args = {5, (uintptr_t)"truncate me"},
    },
    { /* Non-truncated string, negative max-width has no effect  */
        .format = "%.*s",
        .expected_output = "truncate me",
        .nargs = 2,
        .args = {-1, (uintptr_t)"truncate me"},
    },
    { /* Truncated string, implicit zero max-width   */
        .format = "%.s",
        .expected_output = "",
        .nargs = 1,
        .args = {(uintptr_t)"truncate me"},
    },
    { /* Right justified string */
        .format = "%10s",
        .expected_output = "     right",
        .nargs = 1,
        .args = {(uintptr_t)"right"},
    },
    { /* Right justified string, min-width specified as vararg */
        .format = "%*s",
        .expected_output = "     right",
        .nargs = 2,
        .args = {10, (uintptr_t)"right"},
    },
    { /* Left justified string */
        .format = "%-8s",
        .expected_output = "left    ",
        .nargs = 1,
        .args = {(uintptr_t)"left"},
    },
    { /* Left justified string, min-width specified as vararg */
        .format = "%-*s",
        .expected_output = "left    ",
        .nargs = 2,
        .args = {8, (uintptr_t)"left"},
    },
    { /* Left justified string via negative min-width specified as vararg */
        .format = "%*s",
        .expected_output = "left    ",
        .nargs = 2,
        .args = {-8, (uintptr_t)"left"},
    },
    { /* Negative integer */
        .format = "%d",
        .expected_output = "-88",
        .nargs = 1,
        .args = {-88},
    },
    { /* INT_MIN */
        .format = "%d",
        .expected_output = "-2147483648", /* INT_MIN */
        .nargs = 1,
        .args = {INT_MIN},
    },
    { /* INT_MAX */
        .format = "%d",
        .expected_output = "2147483647", /* INT_MAX */
        .nargs = 1,
        .args = {INT_MAX},
    },
    { /* Large unsigned integer, > INT_MAX  */
        .format = "%u",
        .expected_output = "2147483648",
        .nargs = 1,
        .args = {(uintptr_t)INT_MAX + 1},
    },
    { /* min width (left justified, overriding zero padding) */
        .format = "%-06d", /* '-' overrides '0' */
        .expected_output = "123   ",
        .nargs = 1,
        .args = {123},
    },
    { /* zero padding */
        .format = "%06d",
        .expected_output = "000123",
        .nargs = 1,
        .args = {123},
    },
    { /* zero padding, with sign */
        .format = "%06d",
        .expected_output = "-00123",
        .nargs = 1,
        .args = {-123},
    },
    { /* min width (right justified), with sign */
        .format = "%6d",
        .expected_output = "  -123",
        .nargs = 1,
        .args = {-123},
    },
    { /* min width (left justified), with sign */
        .format = "%-6d",
        .expected_output = "-123  ",
        .nargs = 1,
        .args = {-123},
    },
    { /* Large min width */
        .format = "%23s",
        .expected_output = "     ""     ""     ""     ""123",
        .nargs = 1,
        .args = {(uintptr_t)"123"},
    },
    { /* Large max width */
        .format = "%.23s",
        .expected_output = "abcdefghijklmnopqrstuvw",
        .nargs = 1,
        .args = {(uintptr_t)"abcdefghijklmnopqrstuvwxyz"},
    },
    { /* Zero */
        .format = "%d",
        .expected_output = "0",
        .nargs = 1,
        .args = {0},
    },
    { /* Zero */
        .format = "%x",
        .expected_output = "0",
        .nargs = 1,
        .args = {0},
    },
    { /* Zero */
        .format = "%0x",
        .expected_output = "0",
        .nargs = 1,
        .args = {0},
    },
    { /* Zero */
        .format = "%02x",
        .expected_output = "00",
        .nargs = 1,
        .args = {0},
    },
    { /* Integer precision */
        .format = "%.3d",
        .expected_output = "003",
        .nargs = 1,
        .args = {3},
    },
    { /* Integer precision + padding  */
        .format = "%6.3d",
        .expected_output = "   003",
        .nargs = 1,
        .args = {3},
    },
    { /* Integer precision + padding (zero flag ignored) */
        .format = "%06.3d",
        .expected_output = "   003",
        .nargs = 1,
        .args = {3},
    },
    { /* Integer precision + padding (left) */
        .format = "%-6.3d",
        .expected_output = "003   ",
        .nargs = 1,
        .args = {3},
    },
    { /* Integer precision + padding (negative) */
        .format = "%6.3d",
        .expected_output = "  -003",
        .nargs = 1,
        .args = {-3},
    },
    { /* Integer precision + padding (left, negative) */
        .format = "%-6.3d",
        .expected_output = "-003  ",
        .nargs = 1,
        .args = {-3},
    },
    { /* Literal chars */
        .format = "literal",
        .expected_output = "literal",
        .nargs = 0,
        .args = {},
    },
    { /* Weird characters; EOF may be incorrectly considered to be one of these. */
        .format = "%c %c %c %c",
        .expected_output = "\xfc \xfd \xfe \xff",
        .nargs = 4,
        .args = {0xfc, 0xfd, 0xfe, 0xff},
    },
    { /* Combination of different conversion specifiers */
        .format = "literal %08d\t%-8s\t%c%cliteral",
        .expected_output = "literal 00004000\tfoobar  \t\xfeXliteral",
        .nargs = 4,
        .args = {4000, (uintptr_t)"foobar", 0xfe, 'X'},
    },
};


static bool do_detailed_fprintf_tests(bool verbose, bool passed)
{
    size_t i;
    size_t j;

    for (i = 0; i < sizeof(fprintf_specs) / sizeof(fprintf_specs[0]); i++)
    {
        const char *format = fprintf_specs[i].format;
        const char *expected_output = fprintf_specs[i].expected_output;
        uint nargs = fprintf_specs[i].nargs;
        const uintptr_t *args = fprintf_specs[i].args;
        int ret;
        int len = strlen(expected_output);
        uchar obuf[len + 1];
        char test_strbuf[50 + strlen(format)];
        sprintf(test_strbuf, "fprintf: test format string \"%s\"", format);

        testPrint(verbose, test_strbuf);

        /* Print a formatted string to the loopback device.  */
        switch (nargs)
        {
            case 0:
                ret = fprintf(LOOP0, format);
                break;
            case 1:
                ret = fprintf(LOOP0, format, args[0]);
                break;
            case 2:
                ret = fprintf(LOOP0, format, args[0], args[1]);
                break;
            case 3:
                ret = fprintf(LOOP0, format, args[0], args[1], args[2]);
                break;
            case 4:
                ret = fprintf(LOOP0, format, args[0], args[1], args[2], args[3]);
                break;
            default:
                failif(1, "invalid number of arguments");
                ret = -1;
                break;
        }

        /* Read the string back and compare with expected output.  */

        sprintf(test_strbuf, "wrote %d chars, expected %d chars", ret, len);
        failif(ret != len, test_strbuf);

        memset(obuf, 'X', len + 1);
        ret = read(LOOP0, obuf, len + 1);
        failif(ret != len, "failed to read all data printed");

        failif(0 != memcmp(obuf, expected_output, len),
               "fprintf did not print data correctly");

        /* Flush loopback device.  */
        for (j = 0; j < 10000; j++)
        {
            if (fgetc(LOOP0) == EOF)
            {
                break;
            }
        }
    }
    return passed;
}

/* Declarative format for scanning tests.  */

#define MAX_FSCANF_ARGS 4
struct fscanf_arg {
    ulong expected_result; /* Assumes a pointer can fit in here */
    enum {
        NONE_SCAN = 0,
        LONG_SCAN,
        INT_SCAN,
        SHORT_SCAN,
        STRING_SCAN,
    } type;
};

#define LONG_ARG(n) { (ulong)(ulong)(n), LONG_SCAN }
#define INT_ARG(n) { (ulong)(int)(n), INT_SCAN }
#define SHORT_ARG(n) { (ulong)(short)(n), SHORT_SCAN }
#define STRING_ARG(n) { (ulong)(char*)(n), STRING_SCAN }

/* Declarative format for formatted scanning tests.  */
static struct {
    const char *input;
    const char *format;
    int expected_nmatches;
    struct fscanf_arg expected_results[4];
} fscanf_specs[] = {
    { /* Decimal number */
        .input = "11",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(11), }
    },
    { /* Multiple matches */
        .input = "1 2 3",
        .format = "%d %d %d",
        .expected_nmatches = 3,
        .expected_results = { INT_ARG(1), INT_ARG(2), INT_ARG(3), },
    },
    { /* whitespace delimited strings */
        .input = "   hello\tworld  ",
        .format = "%s %s",
        .expected_nmatches = 2,
        .expected_results = { STRING_ARG("hello"), STRING_ARG("world"), },
    },
    { /* Number with leading zeroes */
        .input = "000123",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(123), },
    },
    { /* Negative number */
        .input = "-123",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(-123), },
    },
    { /* Negative number with leading zeroes */
        .input = "-000123",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(-123), },
    },
    { /* Numbers with max width specified  */
        .input="123456",
        .format = "%3d%3d",
        .expected_nmatches = 2,
        .expected_results = { INT_ARG(123), INT_ARG(456), },
    },
    { /* Numbers with max width specified  */
        .input="-123456",
        .format = "%3d%3d%c%c",
        .expected_nmatches = 3,
        .expected_results = { INT_ARG(-12), INT_ARG(345), STRING_ARG("6"), },
    },
    { /* Negative number with leading zeroes and leading whitespace */
        .input = "     -000123",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(-123), },
    },
    { /* INT_MAX */
        .input = "2147483647",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(INT_MAX), },
    },
    { /* INT_MIN */
        .input = "-2147483648",
        .format = "%d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(INT_MIN), },
    },
    { /* Literal characters */
        .input = "literal 9000",
        .format = "literal %d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(9000), },
    },
    { /* Literal percent sign */
        .input = "% 9000",
        .format = "%% %d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(9000), },
    },
    { /* Character set */
        .input = "abababc",
        .format = "%[ab]",
        .expected_nmatches = 1,
        .expected_results = { STRING_ARG("ababab") },
    },
    { /* Character set (inverted) */
        .input = "abababc",
        .format = "%[^c]",
        .expected_nmatches = 1,
        .expected_results = { STRING_ARG("ababab") },
    },
    { /* Character set (with right bracket) */
        .input = "aba]bab]c",
        .format = "%[]ab]",
        .expected_nmatches = 1,
        .expected_results = { STRING_ARG("aba]bab]") },
    },
    { /* Whitespace correctly not skipped with character class match */
        .input = "  ababab",
        .format = "%[ab]",
        .expected_nmatches = 0,
        .expected_results = {STRING_ARG(""), },
    },
    { /* Match character sequence (default length = 1) */
        .input = "hello world",
        .format = "%c",
        .expected_nmatches = 1,
        .expected_results = {STRING_ARG("h"), },
    },
    { /* Match character sequence (specified length = 1) */
        .input = "hello world",
        .format = "%5c",
        .expected_nmatches = 1,
        .expected_results = {STRING_ARG("hello"), },
    },
    { /* Match character sequence correctly does not skip whitespace */
        .input = "  hello world",
        .format = "%5c",
        .expected_nmatches = 1,
        .expected_results = {STRING_ARG("  hel"), },
    },
    { /* Short int */
        .input = " 88 ",
        .format = "%hd",
        .expected_nmatches = 1,
        .expected_results = { SHORT_ARG(88), },
    },
    { /* Long int */
        .input = " 88 ",
        .format = "%ld",
        .expected_nmatches = 1,
        .expected_results = { LONG_ARG(88), },
    },
    { /* Maximum length */
        .input = "    1234 ",
        .format = "%3d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(123), },
    },
    { /* Tokens with no separating whitespace */
        .input = "123hello",
        .format = "%d %s",
        .expected_nmatches = 2,
        .expected_results = { INT_ARG(123), STRING_ARG("hello") },
    },
    { /* Hex with lower case and upper case */
        .input = "dEaDbEeF",
        .format = "%x",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(0xdeadbeef), },
    },
    { /* Not valid octal digit */
        .input = "8",
        .format = "%o",
        .expected_nmatches = 0,
        .expected_results = { INT_ARG(0), },
    },
    { /* Char sequence, then string */
        .input = "12345678",
        .format = "%4c%s",
        .expected_nmatches = 2,
        .expected_results = { STRING_ARG("1234"), STRING_ARG("5678"), },
    },
    { /* Characters that might be mistaken for EOF */
        .input = "\xff\xfe\xfd\xfc",
        .format = "%4c",
        .expected_nmatches = 1,
        .expected_results = { STRING_ARG("\xff\xfe\xfd\xfc"), },
    },
    { /* Explicitly positive number directly followed by a character,
         then another explicitly positive number in base 1. */
        .input = "+000123@+fefe",
        .format = "%d %c %x",
        .expected_nmatches = 3,
        .expected_results = { INT_ARG(123), STRING_ARG("@"), INT_ARG(0xfefe),},
    },
    { /* Weird whitespace and characters */
        .input = "\xff    \xff\xfe\xfd \t\t\t  777777777",
        .format = "\t \xff \t %3c \t    %8o",
        .expected_nmatches = 2,
        .expected_results = { STRING_ARG("\xff\xfe\xfd"), INT_ARG(077777777) },
    },
    { /* Only one match */
        .input = "888 abcd",
        .format = "%d %d",
        .expected_nmatches = 1,
        .expected_results = { INT_ARG(888), INT_ARG(0) },
    },
};

static bool do_detailed_fscanf_tests(bool verbose, bool passed)
{
    size_t i;
    size_t j;

    for (i = 0; i < sizeof(fscanf_specs) / sizeof(fscanf_specs[0]); i++)
    {
        const char *input = fscanf_specs[i].input;
        const char *format = fscanf_specs[i].format;
        size_t input_len = strlen(input);
        int expected_nmatches = fscanf_specs[i].expected_nmatches;
        uint nargs;
        
        for (nargs = 0; fscanf_specs[i].expected_results[nargs].type != NONE_SCAN; nargs++)
        {
        }

        void *scan_ptrs[nargs];
        long longs[nargs];
        int ints[nargs];
        short shorts[nargs];
        char strings[nargs][128];
        int ret;
        char test_strbuf[50 + strlen(format)];

        sprintf(test_strbuf, "fscanf: test format string \"%s\"", format);

        testPrint(verbose, test_strbuf);

        /* Set up scan pointers */
        for (j = 0; j < nargs; j++)
        {
            const struct fscanf_arg *arg = &fscanf_specs[i].expected_results[j];
            switch (arg->type)
            {
                case LONG_SCAN:
                    scan_ptrs[j] = &longs[j];
                    break;
                case INT_SCAN:
                    scan_ptrs[j] = &ints[j];
                    break;
                case SHORT_SCAN:
                    scan_ptrs[j] = &shorts[j];
                    break;
                case STRING_SCAN:
                    memset(strings[j], 'X', sizeof(strings[j]));
                    scan_ptrs[j] = strings[j];
                    break;
                default:
                    failif(1, "invalid scan type");
                    scan_ptrs[j] = NULL;
                    break;
            }
        }

        /* Write data to be scanned to the loopback device  */

        ret = write(LOOP0, input, input_len);
        failif(ret != input_len, "failed to write data to loopback device");

        /* Scan the data  */
        switch (nargs)
        {
            case 0:
                ret = fscanf(LOOP0, format);
                break;
            case 1:
                ret = fscanf(LOOP0, format, scan_ptrs[0]);
                break;
            case 2:
                ret = fscanf(LOOP0, format, scan_ptrs[0], scan_ptrs[1]);
                break;
            case 3:
                ret = fscanf(LOOP0, format, scan_ptrs[0], scan_ptrs[1],
                             scan_ptrs[2]);
                break;
            case 4:
                ret = fscanf(LOOP0, format, scan_ptrs[0], scan_ptrs[1],
                             scan_ptrs[2], scan_ptrs[3]);
                break;
            default:
                failif(1, "invalid number of arguments");
                ret = -1;
                break;
        }

        /* Check number of matches returned  */
        failif(ret != expected_nmatches, "number of matches was incorrect");

        /* Compare with expected scan results  */
        for (j = 0; j < expected_nmatches; j++)
        {
            const struct fscanf_arg *arg = &fscanf_specs[i].expected_results[j];
            switch (arg->type)
            {
                case LONG_SCAN:
                    failif(longs[j] != (long)arg->expected_result,
                           "scan result incorrect");
                    break;
                case INT_SCAN:
                    failif(ints[j] != (int)arg->expected_result,
                           "scan result incorrect");
                    break;
                case SHORT_SCAN:
                    failif(shorts[j] != (short)arg->expected_result,
                           "scan result incorrect");
                    break;
                case STRING_SCAN:
                    {
                        size_t len = strlen((char*)arg->expected_result);
                        failif(0 != memcmp(strings[j],
                                           (char*)arg->expected_result,
                                           len),
                                "scan result incorrect");
                    }
                    break;
                default:
                    break;
            }
        }

        /* Flush loopback device.  */
        for (j = 0; j < 10000; j++)
        {
            if (fgetc(LOOP0) == EOF)
            {
                break;
            }
        }
    }
    return passed;
}

#endif /* defined(LOOP0) */
