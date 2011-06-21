/**
 * @file shell.h
 *
 * $Id: shell.h 2122 2009-11-11 01:06:21Z kthurow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _SHELL_H_
#define _SHELL_H_

#include <stddef.h>

/* Size contstants */
#define SHELL_BUFLEN  80        /**< length of general buffer           */
#define SHELL_MAXTOK  16        /**< maximum tokens per line            */
#define SHELL_CMDSTK  8192      /**< size of command proc. stack        */
#define SHELL_CMDPRIO 20        /**< command process priority           */

/* Message constants */
#define SHELL_BANNER "\033[1;31m--------------------------------------\n      ____  ___.__                    \n      \\   \\/  /|__| ____  __ __       \n       \\     / |  |/    \\|  |  \\      \n       /     \\ |  |   |  \\  |  /      \n      /___/\\  \\|__|___|  /____/       \n            \\_/        \\/       v2.0  \n--------------------------------------\n\033[0;39m\n"
#define SHELL_BANNER_NONVT100 "--------------------------------------\n      ____  ___.__                    \n      \\   \\/  /|__| ____  __ __       \n       \\     / |  |/    \\|  |  \\      \n       /     \\ |  |   |  \\  |  /      \n      /___/\\  \\|__|___|  /____/       \n            \\_/        \\/       v2.0  \n--------------------------------------\n\n"
/** start message */
#define SHELL_START     "Welcome to the wonderful world of Xinu!\n"
//#define SHELL_START     "\033[1;5;37;41mThis is NOT the kernel you are looking for!\033[0;39m\n"
#define SHELL_EXIT       "Shell closed.\n"  /**< exit message            */
#define SHELL_PROMPT     "xsh"              /**< prompt                  */
#define MAX_PROMPT_LEN   32                 /**< basic prompt max length */
#define SHELL_SYNTAXERR  "Syntax error.\n"  /**< syntax error            */
#define SHELL_CHILDERR   "Cannot create.\n" /**< command error           */

/* Shell return constants */
#define SHELL_OK    0
#define SHELL_ERROR 1

/**
 * Defines what an entry in the shell command table looks like.
 */
struct centry
{
    char *name;                 /**< name of command                    */
    bool builtin;               /**< built-in command?                  */
    shellcmd (*procedure) (int, char *[]);/**< procedure                */
};

/* Token parsing functions */
#define isEndOfLine(c)    ((c) == '\0' || (c) == '\r' || (c) == '\n')
#define isQuote(c)        ((c) == '"' || (c) == '\'')
#define isOtherSpecial(c) ((c) == '<' || (c) == '>' || (c) == '&')
#define isWhitespace(c)   ((c) == ' ' || (c) == '\t')

extern const struct centry commandtab[];
                                     /**< table of commands             */
extern ulong ncommand;               /**< number of commands in table   */

/* Function prototypes */
thread shell(int, int, int);
short lexan(char *, ushort, char *, char *[]);
shellcmd xsh_arp(int, char *[]);
shellcmd xsh_clear(int, char *[]);
shellcmd xsh_dumptlb(int, char *[]);
shellcmd xsh_date(int, char *[]);
shellcmd xsh_ethstat(int, char *[]);
shellcmd xsh_exit(int, char *[]);
shellcmd xsh_flashstat(int, char *[]);
shellcmd xsh_gpiostat(int, char *[]);
shellcmd xsh_help(int, char *[]);
shellcmd xsh_kill(int, char *[]);
shellcmd xsh_led(int, char *[]);
shellcmd xsh_memdump(int, char *[]);
shellcmd xsh_memstat(int, char *[]);
shellcmd xsh_nc(int, char *[]);
shellcmd xsh_netstat(int, char *[]);
shellcmd xsh_netup(int, char *[]);
shellcmd xsh_netemu(int, char *[]);
shellcmd xsh_netdown(int, char *[]);
shellcmd xsh_nvram(int, char *[]);
shellcmd xsh_ping(int, char *[]);
shellcmd xsh_ps(int, char *[]);
shellcmd xsh_rdate(int, char *[]);
shellcmd xsh_reset(int, char *[]);
shellcmd xsh_route(int, char *[]);
shellcmd xsh_sleep(int, char *[]);
shellcmd xsh_snoop(int, char *[]);
shellcmd xsh_tar(int, char *[]);
shellcmd xsh_tcpstat(int, char *[]);
shellcmd xsh_telnet(int, char *[]);
shellcmd xsh_telnetserver(int, char *[]);
shellcmd xsh_test(int, char *[]);
shellcmd xsh_testsuite(int, char *[]);
shellcmd xsh_uartstat(int, char *[]);
shellcmd xsh_udpstat(int, char *[]);
shellcmd xsh_user(int, char *[]);
shellcmd xsh_vlanstat(int, char *[]);
shellcmd xsh_voip(int, char *[]);
shellcmd xsh_xweb(int, char *[]);

#endif                          /* _SHELL_H_ */
