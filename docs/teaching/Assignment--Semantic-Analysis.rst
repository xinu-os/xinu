Assignment: Semantic Analysis
=============================

Overview
--------

In this project students implement a semantic analysis (type checking)
pass for our `Concurrent
MiniJava <http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__
language.

Notes
-----

Our `modifications to
MiniJava <http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__
require an additional step for initializing the class and type
environments. The class environment must be initialized with a
type-descriptor containing each of the method types in class
:doc:`*Xinu* <Xinu Helper Class>`, and the type environment must be
initialized with a binding of identifier *Xinu* to this class type. With
this, the type checker can verify that external calls are programmed
properly and also verify that only external calls which the compiler
knows how to map to the underlying runtime are being used in the
program.
