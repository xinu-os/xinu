/**
 * @file     xsh_user.c
 * @provides xsh_user.
 * Shell command demonstrating use of userland functions/syscall interface.
 *
 * NOTE (applies to xsh_user.c, syscall_entry.S, and syscall_dispatch.c):
 * This file is only intended to demonstrate proof-of-concept that a system
 * call interface in Xinu can work.  At this time this file is running in
 * kernel mode and using these functions will NOT result in user->kernel
 * mode escalation.  However, calling a user_* function will create the
 * following trace:
 *
 *  -> user_*: loads the system call number and generates a syscall exception
 *   -> dispatch: jumps to sc_* handler for user_* and passed pointer to args
 *    -> sc_*: re-musters args from pointer and calls proper kernel function
 *    <- sc_*: gets return value of kernel function, or OK/SYSERR
 *   <- dispatch: gets return value of sc_*, SYSERR if no syscall executed
 *  <- user_*: gets return status from handler, returns status to caller
 *
 * The change from user->kernel mode would occur implicitly when the
 * syscall exception occurs, so (in theory) the interface works.
 *
 * Currently, no method exists for building user-address/user-mode
 * binaries, so this cannot be tested until such a method exists.
 *
 * Also, note that the parameters passed to the user_* functions in
 * xsh_user, are NOT safe or known runtime values.  They are simply
 * arbitrary values used by the programmer to test the interface.
 *                                                        -mjs (2009-Sep-24)
 *
 * $Id: xsh_user.c 2075 2009-09-24 21:34:24Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

#define BUF_LENGTH 512

#ifndef LOOP
#define LOOP (-1)
#endif
/**
 * Take a system call number and run that system call.  This is only for
 * demonstration.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_user(int nargs, char **args)
{
    static mailbox mybox;
    int call_num;
    char buffer[BUF_LENGTH];

    if (nargs != 2)
    {
        printf("Insufficient arguments.\n");
        printf("Try again later.\n");
        return 1;
    }

    call_num = atoi(args[1]);

    switch (call_num)
    {
    case 0:
        user_none();
        break;
    case 1:
        user_yield();
        break;
    case 2:
        user_sleep(4000);
        break;
    case 3:
        user_kill(5);           // pick a better number
        break;
    case 4:
        user_open(LOOP, 0);
        break;
    case 5:
        user_control(LOOP, 0, 1, 2);
        break;
    case 6:
        sprintf(buffer, "Process %d\n", gettid());
        user_write(LOOP, buffer, BUF_LENGTH);
        break;
    case 7:
        user_read(LOOP, buffer, BUF_LENGTH);
        printf("%s\n", buffer);
        break;
    case 8:
        user_putc(LOOP, 'm');
        break;
    case 9:
        printf("%c\n", user_getc(LOOP));
        break;
    case 10:
        user_seek(LOOP, 5);
        break;
    case 11:
        user_close(LOOP);
        break;
    case 12:
        printf("%d\n", user_getdev("LOOP"));
        break;
    case 13:
        mybox = user_mboxalloc(50);
        printf("Mailbox %d assigned.\n", mybox);
        break;
    case 14:
        user_mboxfree(mybox);
        break;
    case 15:
        user_mboxsend(mybox, 0xa5a5a5a5);
        break;
    case 16:
        printf("0x%08x\n", user_mboxrecv(mybox));
        break;
    default:
        printf("No corresponding call.\n");
    }

    return 0;
}
