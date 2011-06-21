/**
 * @file syscall.h
 * System call interface and handlers for user to kernel switches.
 *
 * $Id: syscall.h 2072 2009-09-21 22:26:50Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <stddef.h>
#include <mailbox.h>

exchandler syscall_entry(void);
int syscall_dispatch(int, int *);

struct syscall_info
{
    int nargs;                     /**< Number of arguments required    */
    syscall (*handler) (int *);    /**< Function handler                */
};

#define SYSCALL_NONE        0 /**< Perform no operation             */
#define SYSCALL_YIELD       1 /**< Yield processor                  */
#define SYSCALL_SLEEP       2 /**< Sleep for number of microseconds */
#define SYSCALL_KILL        3 /**< Kill a process                   */
#define SYSCALL_OPEN        4 /**< Open a device                    */
#define SYSCALL_CLOSE       5 /**< Close a device                   */
#define SYSCALL_READ        6 /**< Read from a device               */
#define SYSCALL_WRITE       7 /**< Write to a device                */
#define SYSCALL_GETC        8 /**< Get character from device        */
#define SYSCALL_PUTC        9 /**< Put character to device          */
#define SYSCALL_SEEK       10 /**< Seek to position on device       */
#define SYSCALL_CONTROL    11 /**< Control a device                 */
#define SYSCALL_GETDEV     12 /**< Get a device number from string  */
#define SYSCALL_MBOXALLOC  13 /**< Allocate a mailbox               */
#define SYSCALL_MBOXFREE   14 /**< Free a mailbox                   */
#define SYSCALL_MBOXRECV   15 /**< Receive message from mailbox     */
#define SYSCALL_MBOXSEND   16 /**< Send message to mailbox          */

extern const struct syscall_info syscall_table[];
extern int nsyscalls;

/* Prototypes from user mode system calls */
syscall user_none(void);
syscall user_yield(void);
syscall user_sleep(uint ms);
syscall user_kill(tid_typ thread_id);
syscall user_open(int descrp, int arg);
syscall user_close(int descrp);
syscall user_read(int descrp, void *buffer, uint length);
syscall user_write(int descrp, void *buffer, uint length);
syscall user_getc(int descrp);
syscall user_putc(int descrp, char character);
syscall user_seek(int descrp, uint offset);
syscall user_control(int descrp, int function, long arg1, long arg2);
syscall user_getdev(char *device_name);
syscall user_mboxalloc(uint nmsgs);
syscall user_mboxfree(mailbox mbox_id);
syscall user_mboxrecv(mailbox mbox_id);
syscall user_mboxsend(mailbox mbox_id, int msg);

#endif                          /* __SYSCALL_H__ */
