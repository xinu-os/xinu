Message passing
===============

Message passing is one method used by XINU threads for interprocess
communication. It allows threads to send individual messages to other
threads by using the system calls ``send``, ``receive``, and
``recvclr``. Each thread has memory allocated for a single message in
its thread control block specifically for messages sent and received
using this method. This form of message passing should not be confused
with the mailbox messaging queue system also used for interprocess
communication.

Upon creation, each thread is allocated memory in its thread control
block for two fields which apply to this message passing system: a 4
byte (``int`` type) message box to contain a single message sent to this
thread, and a one byte flag (``bool`` type) to signal if there is an
unreceived message waiting in that thread's message box.

Threads use the functions ``send(tid_typ tid, int msg)``, ``receive()``,
and ``recvclr()`` to utilize this system of message passing.

``send(tid_typ tid, int msg)`` delivers the message passed in as the
parameter ``msg`` to the message box in the thread control block of the
thread with a thread id of ``tid``, also passed in as a parameter.
``send`` will always yield the processor by calling reschedule if the
receiving thread was in a state of waiting to receive a message
(``THRRECV``).

``receive()`` returns the message waiting in the message box of the
thread control block of the thread which called ``receive``. If there is
no message waiting for the thread then the thread will go into a state
of waiting to receive a message (``THRRECV``) until a message is passed
to the thread.

``recvclr()`` is a non-blocking version of ``receive()``. If there is a
message waiting in the message box of the thread control block of the
thread which called ``receive`` it returns the message. If there is no
message waiting for the thread, then it will simply return ``OK``,
signifying that there was no message waiting for the thread. Notice that
this does not block the thread that called ``receive`` and will always
immediately return either the message or ``OK``.
