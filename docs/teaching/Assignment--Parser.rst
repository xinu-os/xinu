Assignment: Parser
==================

Overview
--------

In this project students use `JavaCC <https://javacc.dev.java.net/>`__
to implement the scanner and parser rules for our `Concurrent MiniJava
<http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__
language.

Notes
-----

`Concurrent
MiniJava <http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__
requires that *Xinu*, *Thread*, *run*, and *synchronized* be added as
reserved words for the Scanner and Parser. The external call productions
must keep track of the name of the *Xinu* method being called.

Also, since our modified language allows programmers to declare classes
which extend the *Thread* class without ever actually declaring a
*Thread* class, the compiler must be informed implicitly that there
exists a *Thread* class. To avoid the complexity of making the compiler
aware of the entire Java *Thread* class, the Parser can automatically
generate an Abstract Syntax Tree (AST) node for a *Thread* class with an
empty *run* method. This ensures there are no runtime errors when the
external call *Xinu.threadCreate* calls the *run* method of a class
which extends *Thread* but does not override the *run* method. To do
this, class declarations which extend *Thread* are handled by the
additional *ThreadClassDeclaration* production in our `modified
grammar <http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__.

Further changes must be made to account for the declaration of the *run*
method since *void* return types are not allowed in
`MiniJava <http://www.cambridge.org/resources/052182060X/>`__, except in
the special case of the main method declaration. To handle this, we add
a *RunMethodDeclaration* production to our `modified
grammar <http://www.mscs.mu.edu/~brylow/cosc4400/Spring2011/ConcurrentMiniJava.html>`__.

To support the *synchronized* keyword the AST node for method
declarations needs a new field to keep track of whether a method is
synchronized or not.
