Assignment: Translation
=======================

Overview
--------

In this project students implement a translator that takes the
Abstract Syntax Tree (AST) output from the semantic analysis pass in
:doc:`Project 4 <Semantic-Analysis>` and converts it into an
Intermediate Representation (IR) tree.

Notes
-----

The AST representations of the Xinu external calls must be mapped to
IR nodes that reference the corresponding underlying runtime
functions.  Care must be taken at this point to choose runtime
function names that will not conflict with legitimate source language
method names; we follow longstanding tradition and append an
underscore to the runtime function names. The mapping of I/O and
threading functions is self-explanatory, but also at this point we
need to add in a dynamic memory allocation function to support the
source language *new* operator for instantiating new objects and
arrays. In addition, if the compiler is to support Java-like runtime
checking for null pointers and array bounds, corresponding runtime
error handlers must be mapped. The figure below gives a brief
description of each part of the MiniJava to Xinu Compatability Layer,
but the `full C file <Xinu Compatability Layer>`_ is also available.

Changes must also be made to support the *synchronized* key word.
First :doc:`monitors </teaching/Adding-Monitors-To-Xinu>` must be
added to Xinu.  The *new* function in the compatibility layer must
acquire a monitor from the O/S and associate it with the new object.
Also, ``lock()`` and ``unlock()`` functions must be added to the
compatibility layer which map to the monitor ``lock()`` and
``unlock()`` functions added to Xinu.  To achieve Java-like thread
synchronization the compiler must wrap synchronized method bodies with
calls to the compatibility functions ``lock()`` and ``unlock()``. It
is important that the compiler ensures that the ``lock()`` action
precedes the evaluation of *any* part of the method body including
evaluation of the right hand side of local variable declarations.
Similarly, the ``unlock()`` action must come after *any* part of the
method body including the evaluation of the return expression.

MiniJava to Xinu Compatibility Layer

===================================   =============
 Xinu function                        Purpose
===================================   =============
 ``int _readint(void)``               Parse in integer input
 ``syscall _printint(int i)``         Print an integer
 ``syscall _print(char *s)``          Print a string literal
 ``syscall _println(void)``           Print a carriage return
 ``syscall _threadCreate(int *t)``    Spawn a new thread of execution, running t's ``run`` method
 ``syscall _yield(void)``             Yield the processor
 ``syscall _sleep(int time)``         Sleep ``time`` number of milliseconds
 ``syscall _lock(int *obj)``          Lock the monitor associated with object ``obj``
 ``syscall _unlock(int *obj)``        Unlock the monitor associated with object ``obj``
 ``int *_new(int n, int f)``          Allocate array or allocate object and associate a monitor with that object
 ``void _BADPTR(void)``               Null pointer exception
 ``void _BADSUB(void)``               Bounds exception
===================================   =============

Xinu Compatability Layer
------------------------

Here is the full C source code for the Xinu Compatability Layer.

::

    #include <stdio.h>
    #include <stdarg.h>
    #include <stdlib.h>
    #include <device.h>
    #include <tty.h>
    #include <memory.h>
    #include <thread.h>
    #include <monitor.h>

    syscall _readint(void)
    {
        int i = 0, c = 0;

        control(CONSOLE, TTY_CTRL_SET_IFLAG, TTY_ECHO, 0);

        c = getchar();
        while (('\n' != c) && ('\r' != c) && (EOF != c))
        {
            if (('0' <= c) && ('9' >= c))
            {
                i = i * 10 + c - '0';
            }

            c = getchar();
        }
        kprintf("\r\n");
        if (EOF == c)
            return c;

        return i;
    }

    syscall _printint(int i)
    {
        return kprintf("%d\r\n", i);
    }

    syscall _print(char *s)
    {
        return kprintf("%s", s);
    }

    syscall _println(void)
    {
        return kprintf("\r\n");
    }

    syscall _yield(void)
    {
        return yield();
    }

    syscall _sleep(int time)
    {
        return sleep(time);
    }

    int * _new(int n, int init)
    {
        int size = (n + 2) * 4;
        int *p = (int *)memget(size);
        bzero(p, size);
        p[0] = moncreate(1);
        p[1] = init;
        return p + 2;
    }

    syscall _lock(int * objAdder)
    {
        int * A = objAdder - 2;
        monitor m = (monitor) *A;
        return lock(m);
    }

    syscall _unlock(int * objAdder)
    {
        int * A = objAdder - 2;
        monitor m = (monitor) *A;
        return unlock(m);
    }

    syscall _threadCreate(int * threadObjAdder)
    {
        int * A = threadObjAdder - 1;
        int * B = (int*) *A;
        int * C = (int*) *B;
        void * procadder = (void *) C;
        return ready(create(procadder, INITSTK, INITPRIO, "MiniJavaThread", 1, threadObjAdder), RESCHED_NO);
    }


    void _BADPTR(void)
    {
        fprintf(CONSOLE, "FATAL ERROR: Null Pointer Exception!\n");
        kill(thrcurrent);
    }

    void _BADSUB(void)
    {
            fprintf(CONSOLE, "FATAL ERROR: Index Out Of Bounds Exception!\n");
            kill(thrcurrent);
    }

