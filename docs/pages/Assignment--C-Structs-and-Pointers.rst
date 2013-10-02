====================================
 Assignment: C structs and pointers
====================================
---------------------------------------------------
 A Stroll Down Memory Lane, or A Stunning Reversal
---------------------------------------------------

This assignment is intended to develop the student's proficiency for
programming in C. This assignment is part of the `Student Built
Xinu <Student Built Xinu>`__ track for professors that are `Teaching
With Xinu <Teaching With Xinu>`__. This assignment may be completed in
teams of two.

Program
-------

-  Write a C program that reads in an arbitrarily long sequence of
   positive integers and outputs that same sequence in reverse order.
-  Your program should understand positive integers in binary (starting
   with "0b"), octal, decimal, and hexadecimal, but all output will be
   in decimal (base 10). This should reuse code from your calculator
   project.
-  Your program should ignore any amount of white space between
   integers, including tabs, spaces, newlines, etc. See the isspace()
   function in your text or in the man pages for details. However, your
   output should consist entirely of base 10 representation integers in
   a line by themselves.
-  To store an arbitrary list of integers, your program will need to
   request more memory from the operating system when the space it
   already has runs out. This kind of dynamic allocation is performed
   using the malloc() function, as detailed in your text and in the man
   pages. You may assume that we will test your program with at least
   100,000,000 integers. Your program should exit gracefully if a
   malloc() request is denied.
-  Billionrandom is a random number generator that takes a single
   command-line parameter for the number of integers desired, and then
   outputs a predictable sequence of `pseudorandom number
   generator <wikipedia:Pseudorandom number generator>`__ using a
   `linear congruential
   generator <wikipedia:Linear_congruential_generator>`__ and rotating
   through the various bases. This program can be used to generate test
   inputs of varying sizes.

Notes
-----

-  There are a variety of approaches for storing an arbitrarily long
   list of integers. The approach I recommend is building a linked list
   of structures that store a reasonable number of integers. So, for
   example, you could define a struct that contains an array of a few
   thousand integers. Every time you fill up that structure, request
   another one and string it into a linked list with all of the previous
   blocks. This is an excellent balance in efficiency -- if your block
   size were too small (one integer per malloc() in the most absurd
   case,) your program would spend all of its time asking the O/S to
   allocate more memory. If the block size were too large, (say, a
   Gigabyte,) your program would needlessly waste resources when the
   input was only a small list, and might not even be able to allocate a
   single chunk that large.
-  This project is inherently dangerous; please exercise both great care
   and ethical consideration during this assignment. For large numbers
   of integers, this program is essentially a stress test for the
   operating system's virtual memory subsystem. On a late model 3 GHz
   Pentium IV running the program with a test list of 1,000,000,000 runs
   for over 15 minutes; practically locking up all user interfaces for
   the last 10 minutes of that. The reference implementation, in
   combination with the test generator, can bring pretty much any server
   to its knees in a matter of minutes. As a result, the following are
   particularly important:

   -  list of DO NOTs for this project:

      -  DO NOT run your program with a large test size on any other
         large server relied upon by multiple users.
      -  DO NOT pipe the output of billionrandom or your program to a
         text file for list sizes over a few million. If you do the
         math, you'll see that this could quickly fill up a lot of the
         space and clog up the network with file server traffic.
      -  DO NOT run large test cases when it is apparent that a bunch of
         other people are trying to do their work on the same machine.
         See the top and w UNIX commands for more info.

   -  list of DOs for this project:

      -  DO check your dynamic allocation code by adjusting your block
         size downward to insure that even small numbers of integers
         will require multiple malloc() requests.
      -  DO check your malloc() error handling by adjusting your block
         size upward until your first request fails.
      -  DO run your program with large test input (more than a few
         million) only once you are relatively certain it is working
         properly -- so that you only have to do it once.

`Category:Student Built Xinu <Category:Student Built Xinu>`__
