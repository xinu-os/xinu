Mailboxes
=========

In XINU, a **mailbox** is a messaging queue used for interprocess
communication.  Mailboxes should not to be confused with the single
:doc:`message passing <Message-Passing>` capability built into the
thread control block which uses :source:`send() <system/send.c>` and
:source:`receive() <system/receive.c>`.

XINU allows for a finite number of mailboxes to be created. Each mailbox
is identified by a number. Any number of processes can send and receive
messages from the mailbox, provided the processes know the correct
mailbox number.

When a new mailbox is created (allocated) a maximum number of messages
allowed in the queue must be specified. Memory for the messages is
allocated when the mailbox is created. Once the mailbox message queue is
full, processes that attempt to send a message must wait for space in
the queue. If the queue is empty, processes that attempt to receive a
message must wait for a message to be enqueued. A message is 4 bytes
long (``int`` type).

When a mailbox is deleted all remaining messages in the queue are
destroyed. Processes waiting to send or receive are released from the
wait state.
