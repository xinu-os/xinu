Assignment: C Basics
====================

:wikipedia:`All Your Base Are Belong To Us`
===========================================

This assignment is part of the :doc:`/teaching/Student-Built-Xinu` track for
professors that are `teaching with Xinu </teaching/index>`. This particular
assignment will help develop the student's proficiency for programming
in C.

The Multi-Base Calculator
-------------------------

-  Write a calculator program that reads in expressions consisting of
   integers and the operators +, -, \*, /, %, ^ (exponentiation), &
   (bitwise AND), \| (bitwise OR), << (left shift), and >> (right
   shift), and prints the results when evaluated in simple left-to-right
   order.

-  Your calculator should understand positive integers in binary
   (starting with "0b",) octal, decimal, and hexadecimal, but all output
   will be in decimal (base-10).

-  You should use the ``getchar()`` library function to read console input one
   character at a time. The use of any other library functions for input
   is not recommended at this time.

Notes
-----

-  This project can seem deceptively complex, but is quite tractable if
   you first take the time to design suitable helper functions, and test
   those function thoroughly before moving on to the overall calculator.
   We encourage the disciplined practice of test-driven development.
-  The internal data representation of the calculator values can be
   assumed to be a ``signed int``. You are not required to deal with
   overflow and underflow issues.
-  Parsing input is always tedious, particularly when the input is not
   rigidly constrained with rules like, "all operators must be
   surrounded by space on both sides," (which is NOT a constraint for
   this assignment). In lieu of a parser-generator, consider drawing
   simple state diagrams for how to recognize an integer in octal, or
   how to recognize an integer in hexadecimal. In many cases with input
   of this type, it is useful to be able to put back a character of
   input once you realize you have read too far. Useful C routines for
   implementing this kind of buffering include ``getch()`` and ``ungetch()``.
-  Consider various operational and parsing errors that can take place.
-  Devise test cases to discover the expected behavior, and do your best
   to match it precisely. Creativity will be valued in later
   assignments, but first one must master the tools.
