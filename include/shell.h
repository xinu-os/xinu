/**
 * @file shell.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _SHELL_H_
#define _SHELL_H_

#include <stddef.h>
#include <conf.h>

/* Size contstants */
#define SHELL_BUFLEN  160       /**< length of general buffer           */
#define SHELL_MAXTOK  32        /**< maximum tokens per line            */
#define SHELL_CMDSTK  8192      /**< size of command proc. stack        */
#define SHELL_CMDPRIO 20        /**< command process priority           */

/* Message constants */
#define SHELL_BANNER_DEFAULT "\n\033[1;31m--------------------------------------\n      ____  ___.__                    \n      \\   \\/  /|__| ____  __ __       \n       \\     / |  |/    \\|  |  \\      \n       /     \\ |  |   |  \\  |  /      \n      /___/\\  \\|__|___|  /____/       \n            \\_/        \\/       v2.0  \n--------------------------------------\n\033[0;39m\n"

#define SHELL_BANNER_DEFAULT_NONVT100 "--------------------------------------\n      ____  ___.__                    \n      \\   \\/  /|__| ____  __ __       \n       \\     / |  |/    \\|  |  \\      \n       /     \\ |  |   |  \\  |  /      \n      /___/\\  \\|__|___|  /____/       \n            \\_/        \\/       v3.14 \n--------------------------------------\n\n"

#define SHELL_BANNER_PI "\n\033[1;31m-----------------------------------------------------\n      ____  ___\033[1;32m.__\033[1;31m                 .___   \033[1;32m.__\033[1;31m\n      \\   \\/  /\033[1;32m|__|\033[1;31m ____  __ __    |  _ \\ \033[1;32m|__|\033[1;31m\n       \\     / |  |/    \\|  |  \\   | |_| ||  |\n       /     \\ |  |   |  \\  |  /   |  __/ |  |\n      /___/\\  \\|__|___|  /____/    | |    |__|\n            \\_/        \\/          |/          v3.14\n-----------------------------------------------------\n\033[0;39m\n"

#define SHELL_BANNER_PI_NONVT100 "-----------------------------------------------------\n      ____  ___.__                 .___   .__\n      \\   \\/  /|__| ____  __ __    |  _ \\ |__|\n       \\     / |  |/    \\|  |  \\   | |_| ||  |\n       /     \\ |  |   |  \\  |  /   |  __/ |  |\n      /___/\\  \\|__|___|  /____/    | |    |__|\n            \\_/        \\/          |/          v3.14\n-----------------------------------------------------\n\n"

#ifdef _XINU_PLATFORM_ARM_RPI_
#  define SHELL_BANNER            SHELL_BANNER_PI
#  define SHELL_BANNER_NONVT100   SHELL_BANNER_PI_NONVT100
#else
#  define SHELL_BANNER            SHELL_BANNER_DEFAULT
#  define SHELL_BANNER_NONVT100   SHELL_BANNER_DEFAULT_NONVT100
#endif

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

/* helpers for parse command line arguments */
struct getopt
{
    int argc;
    char **argv;
    char *optstring;
    char *optarg;
    int optind;
    int optopt;
    int opterr;
    int optreset;
};
int getopt(int, char **, char *, struct getopt *);

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
shellcmd xsh_kexec(int, char *[]);
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
shellcmd xsh_pktgen(int, char *[]);
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
shellcmd xsh_timeserver(int, char *[]);
shellcmd xsh_turtle(int, char *[]);
shellcmd xsh_uartstat(int, char *[]);
shellcmd xsh_udpstat(int, char *[]);
shellcmd xsh_usbinfo(int, char *[]);
shellcmd xsh_user(int, char *[]);
shellcmd xsh_vlanstat(int, char *[]);
shellcmd xsh_voip(int, char *[]);
shellcmd xsh_xweb(int, char *[]);

#endif                          /* _SHELL_H_ */
