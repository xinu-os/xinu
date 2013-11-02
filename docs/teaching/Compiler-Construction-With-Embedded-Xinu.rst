Compiler Construction With Embedded Xinu
========================================

Overview
--------

Having students construct a compiler which targets a runtime that uses
their own, or a provided, Xinu operating system is one of the
potential tracks for a professor that is :doc:`Teaching With Xinu
<index>`.

Including Embedded Xinu in a compiler construction course allows
students to explore the compilation of high level language constructs
that rely on interacting with the underlying runtime. Many traditional
compilers courses simply target a processor or simulator, but by
targeting a *platform* (a processor and operating system combination)
one can extend the source language to include more advanced language
features such as I/O operations and thread creation, manipulation, and
concurrency. This also allows students to run their test cases on real
hardware and see these programs actually interacting with a real
runtime. In modern programming these high level language features are
vital, and it is important for students to see what the processor and
runtime are doing when they use these features in their own programs.

Course Outcomes
---------------

Course development can parallel learning objectives and topics
associated with many Programming Language Translation or Compiler
Construction courses.  [#acm2001]_ However, by targeting a platform
with an operating system students can also focus on learning how
compilers interact with the runtime to achieve thread concurrency and
synchronization; topics which many traditional compilers courses
avoid. [#appel2002]_ [#aho1985]_ [#muchnick1997]_

Topics
~~~~~~

-  Lexical Analysis
-  Syntax Analysis
-  Semantic Analysis
-  IR Translation
-  Instruction Selection
-  Register Allocation

Learning Objectives
~~~~~~~~~~~~~~~~~~~

-  Recognize various classes of grammars, languages, and automata, and
   employ these to solve common software problems.
-  Explain the major steps involved in compiling a high-level
   programming language down to a low-level target machine language.
-  Construct and use the major components of a modern compiler.
-  Work together effectively in teams on a substantial software
   implementation project.

Potential Course Structure
--------------------------

The course outlined below describes a compiler construction course
focusing on a semester long project in which students build most of
the pieces of a complete working compiler. For this example course we
take the compiler project from Appel and Palsberg's *Modern Compiler
Implementation in Java* [#appel2002]_ and modify it to target a MIPS
platform running the Xinu operating system. We take advantage of the
fact that our compiler targets a runtime with an operating system and
add high-level I/O and concurrency features to Appel and Palsberg's
`MiniJava <http://www.cambridge.org/resources/052182060X/>`__
language, creating our own `Concurrent MiniJava
<http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__
language.

We allow Java-like threading and synchronization with our added support
for class declarations inheriting the built in *Thread* class and with
added support for Java's *synchronized* keyword. We also add external
operating system calls for I/O operations and for operations to create
and manipulate multiple threads of execution. Specifically, we add the
ability to print strings with *Xinu.print(String s)*, the ability to
print a new line with *Xinu.println()*, the ability to print an integer
with *Xinu.printint(int x)*, the ability to read in an integer input
with *Xinu.readint()*, the ability to create a thread of execution with
*Xinu.threadCreate(Thread t)*, the ability for a thread to yield control
of the processor with *Xinu.yield()*, and the ability for a thread to
sleep for a given number of milliseconds with *Xinu.sleep(int time)*.

The links in the outline below describe the changes necessary in each
assignment to add these high-level I/O and concurrency features to the
language, including the modifications for targeting a Xinu backend
instead of the book's intended MIPS simulator. In addition to these
compiler changes, modifications must also be made to Xinu to offer the
runtime support required by the *synchronized* keyword. Since Java's
*synchronized* feature depends on the JVM monitor system, which has
subtly different semantics from standard O/S semaphores, :doc:`monitor
constucts <Adding-Monitors-To-Xinu>` must be added to Xinu.

Appel and Palsberg's
`MiniJava <http://www.cambridge.org/resources/052182060X/>`__ language
is a subset of the standard Java language, and this means test cases
written in MiniJava can be compiled and run using standard Java
compilers. To use standard Java compilers to compile programs written in
our `Concurrent
MiniJava <http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__
language one needs our :doc:`Xinu.java <Xinu-Helper-Class>` helper class.

Course Outline
~~~~~~~~~~~~~~

.. list-table::
    :widths: 5 20 30
    :header-rows: 1

    * - Week
      - Topics
      - Assignments
    * - 01
      - Introduction
      - Project 1: Interpreter
    * - 02
      - Lexical Analysis, Automata
      - :doc:`Project 2: Scanner <assignments/Scanner>`
    * - 03
      - Syntax Analysis, Grammars
      - Homework 1: Automata and Grammars
    * - 04
      - Parser Generators
      - :doc:`Project 3: Parser <assignments/Parser>`
    * - 05
      - Abstract Syntax Trees
      -
    * - 06
      - Semantic Analysis
      - :doc:`Project 4: Semantic Analysis <assignments/Semantic-Analysis>`
    * - 07
      - Activation Records
      -
    * - 08
      - IR Translation
      -
    * - 09
      - Basic Blocks
      - :doc:`Project 5: Translation <assignments/Translation>`
    * - 10
      - Instruction Selection
      - Homework 2: Activation Records
    * - 11
      - Liveness Analysis
      -
    * - 12
      - Register Allocation
      -
    * - 13
      - Register Allocation
      - :doc:`Project 6: Instruction Selection <assignments/Instruction-Selection>`
    * - 14
      - Advanced Topics
      -
    * - 15
      - Advanced Topics
      - Homework 3: Register Allocation

Books
~~~~~

- [#appel2002]_

References
----------

.. [#acm2001] Course topics and learning objectives have been adapted
 from the ACM's `Computing Curricula 2001 Computer Science
 <http://www.acm.org/education/education/education/curric_vols/cc2001.pdf>`__.

.. [#appel2002] `Andrew W. Appel and Jens Palsberg, Modern Compiler Implementation
 in Java, 2nd Edition, Cambridge University Press, 2002
 <http://www.cambridge.org/us/catalogue/catalogue.asp?isbn=052182060x>`__

.. [#aho1985] A. V. Aho, M. Lam, R. Sethi, and J. D. Ullman. *Compilers:
 Principles, Techniques and Tools*. Pearson, 2nd edition, 1985.

.. [#muchnick1997] S. Muchnick. *Advanced Compiler Design and Implementation*. Morgan
 Kaufmann, 1997.

--------------

This work funded in part by NSF grant DUE-CCLI-0737476.
