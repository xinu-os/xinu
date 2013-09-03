/**
 * @file syscall_dispatch.c
 * Translate user_* functions to system calls, dispatch to proper sc_*
 * functions, and complete.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <syscall.h>
#include <interrupt.h>
#include <device.h>
#include <thread.h>
#include <memory.h>
#include <mailbox.h>

/* for argument parsing */
#define SCARG(type, args)  (type)(*args++)

/* quick and dirty macro to make user->kernel syscalls */
#define SYSCALL(num) int status; \
	asm("li $2, %0" : : "i"(SYSCALL_##num)); \
	asm("syscall"); \
	asm("move %0, $2" : "=r"(status)); \
	return status;

/* syscall wrapper prototypes */
syscall sc_none(int *);
syscall sc_yield(int *);
syscall sc_sleep(int *);
syscall sc_kill(int *);
syscall sc_open(int *);
syscall sc_close(int *);
syscall sc_read(int *);
syscall sc_write(int *);
syscall sc_getc(int *);
syscall sc_putc(int *);
syscall sc_seek(int *);
syscall sc_control(int *);
syscall sc_getdev(int *);
syscall sc_mboxalloc(int *);
syscall sc_mboxfree(int *);
syscall sc_mboxrecv(int *);
syscall sc_mboxsend(int *);

/* table for determining how to call syscalls */
const struct syscall_info syscall_table[] = {
    {5, (void *)sc_none},       /* SYSCALL_NONE      = 0  */
    {0, (void *)sc_yield},      /* SYSCALL_YIELD     = 1  */
    {1, (void *)sc_sleep},      /* SYSCALL_SLEEP     = 2  */
    {1, (void *)sc_kill},       /* SYSCALL_KILL      = 3  */
    {2, (void *)sc_open},       /* SYSCALL_OPEN      = 4  */
    {1, (void *)sc_close},      /* SYSCALL_CLOSE     = 5  */
    {3, (void *)sc_read},       /* SYSCALL_READ      = 6  */
    {3, (void *)sc_write},      /* SYSCALL_WRITE     = 7  */
    {1, (void *)sc_getc},       /* SYSCALL_GETC      = 8  */
    {2, (void *)sc_putc},       /* SYSCALL_PUTC      = 9  */
    {2, (void *)sc_seek},       /* SYSCALL_SEEK      = 10 */
    {4, (void *)sc_control},    /* SYSCALL_CONTROL   = 11 */
    {1, (void *)sc_getdev},     /* SYSCALL_GETDEV    = 12 */
    {1, (void *)sc_mboxalloc},  /* SYSCALL_MBOXALLOC = 13 */
    {1, (void *)sc_mboxfree},   /* SYSCALL_MBOXFREE  = 14 */
    {1, (void *)sc_mboxrecv},   /* SYSCALL_MBOXRECV  = 15 */
    {2, (void *)sc_mboxsend},   /* SYSCALL_MBOXSEND  = 16 */
};

int nsyscall = sizeof(syscall_table) / sizeof(struct syscall_info);

/**
 * Syscall dispatch routine.  Given a syscall code and pointer to
 * arguments, change execution to function.  Otherwise, generate error
 * saying no such syscall.
 * @param code syscall code to execute
 * @param args pointer to arguments for syscall
 */
int syscall_dispatch(int code, int *args)
{
    if (0 <= code && code < nsyscall)
    {
        return (*syscall_table[code].handler) (args);
    }

    return SYSERR;
}

/**
 * syscall wrapper for *no* function, just a test/demo.
 * @param args expands to: none.
 */
syscall sc_none(int *args)
{
    return OK;
}

syscall user_none(void)
{
    SYSCALL(NONE);
}

/**
 * syscall wrapper for yield().
 * @param args expands to: none.
 */
syscall sc_yield(int *args)
{
    /* this may change thread of execution, allow exceptions */
    exlreset();

    return yield();
}

syscall user_yield(void)
{
    SYSCALL(YIELD);
}

/**
 * syscall wrapper for sleep().
 * @param args expands to: uint ms (milliseconds to sleep)
 */
syscall sc_sleep(int *args)
{
    uint ms = SCARG(int, args);

    /* this may change thread of execution, allow exceptions */
    exlreset();

    return sleep(ms);
}

syscall user_sleep(uint ms)
{
    SYSCALL(SLEEP);
}

/**
 * syscall wrapper for kill().
 * @param args expands to: tid_typ thread_id
 */
syscall sc_kill(int *args)
{
    tid_typ thread_id = SCARG(tid_typ, args);

    /* this may change thread of execution, allow exceptions */
    exlreset();

    return kill(thread_id);
}

syscall user_kill(tid_typ thread_id)
{
    SYSCALL(KILL);
}

/**
 * syscall wrapper for open().
 * @param args expands to: int descrp, int arg
 */
syscall sc_open(int *args)
{
    int descrp = SCARG(int, args);
    int arg = SCARG(int, args);

    return open(descrp, arg);
}

syscall user_open(int descrp, int arg)
{
    SYSCALL(OPEN);
}

/**
 * syscall wrapper for close().
 * @param args expands to: int descrp
 */
syscall sc_close(int *args)
{
    int descrp = SCARG(int, args);

    return close(descrp);
}

syscall user_close(int descrp)
{
    SYSCALL(CLOSE);
}

/**
 * syscall wrapper for read().
 * @param args expands to: int descrp, void *buffer, uint length
 */
syscall sc_read(int *args)
{
    int descrp = SCARG(int, args);
    void *buffer = SCARG(void *, args);
    uint length = SCARG(uint, args);

    return read(descrp, buffer, length);
}

syscall user_read(int descrp, void *buffer, uint length)
{
    SYSCALL(READ);
}

/**
 * syscall wrapper for write().
 * @param args expands to: int descrp, void *buffer, uint length
 */
syscall sc_write(int *args)
{
    int descrp = SCARG(int, args);
    void *buffer = SCARG(void *, args);
    uint length = SCARG(uint, args);

    return write(descrp, buffer, length);
}

syscall user_write(int descrp, void *buffer, uint length)
{
    SYSCALL(WRITE);
}

/**
 * syscall wrapper for getc().
 * @param args expands to: int descrp
 */
syscall sc_getc(int *args)
{
    int descrp = SCARG(int, args);

    return getc(descrp);
}

syscall user_getc(int descrp)
{
    SYSCALL(GETC);
}

/**
 * syscall wrapper for putc().
 * @param args expands to: int descrp, char character
 */
syscall sc_putc(int *args)
{
    int descrp = SCARG(int, args);
    char character = SCARG(char, args);

    return putc(descrp, character);
}

syscall user_putc(int descrp, char character)
{
    SYSCALL(PUTC);
}

/**
 * syscall wrapper for seek().
 * @param args expands to: int descrp, uint offset
 */
syscall sc_seek(int *args)
{
    int descrp = SCARG(int, args);
    uint offset = SCARG(uint, args);

    return seek(descrp, offset);
}

syscall user_seek(int descrp, uint offset)
{
    SYSCALL(SEEK);
}

/**
 * syscall wrapper for control().
 * @param args expands to: int descrp, int function, long arg1, long arg2
 */
syscall sc_control(int *args)
{
    int descrp = SCARG(int, args);
    int function = SCARG(int, args);
    long arg1 = SCARG(long, args);
    long arg2 = SCARG(long, args);

    return control(descrp, function, arg1, arg2);
}

syscall user_control(int descrp, int function, long arg1, long arg2)
{
    SYSCALL(CONTROL);
}

/**
 * syscall wrapper for getdev().
 * @param args expands to: char *device_name
 */
syscall sc_getdev(int *args)
{
    char *device_name = SCARG(char *, args);

    return getdev(device_name);
}

syscall user_getdev(char *device_name)
{
    SYSCALL(GETDEV);
}

/**
 * syscall wrapper for mailboxAlloc().
 * @param args expands to: uint nmsgs
 */
syscall sc_mboxalloc(int *args)
{
    uint nmsgs = SCARG(uint, args);

    return mailboxAlloc(nmsgs);
}

syscall user_mboxalloc(uint nmsgs)
{
    SYSCALL(MBOXALLOC);
}

/**
 * syscall wrapper for mailboxFree().
 * @param args expands to: mailbox mbox_id
 */
syscall sc_mboxfree(int *args)
{
    mailbox mbox_id = SCARG(mailbox, args);

    return mailboxFree(mbox_id);
}

syscall user_mboxfree(mailbox mbox_id)
{
    SYSCALL(MBOXFREE);
}

/**
 * syscall wrapper for mailboxReceive().
 * @param args expands to: mailbox mbox_id
 */
syscall sc_mboxrecv(int *args)
{
    mailbox mbox_id = SCARG(mailbox, args);

    return mailboxReceive(mbox_id);
}

syscall user_mboxrecv(mailbox mbox_id)
{
    SYSCALL(MBOXRECV);
}

/**
 * syscall wrapper for mailboxSend().
 * @param args expands to: mailbox mbox_id, int msg
 */
syscall sc_mboxsend(int *args)
{
    mailbox mbox_id = SCARG(mailbox, args);
    int msg = SCARG(int, args);

    return mailboxSend(mbox_id, msg);
}

syscall user_mboxsend(mailbox mbox_id, int msg)
{
    SYSCALL(MBOXSEND);
}
