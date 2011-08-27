/**
 * @file sctpTimer.c
 * @provides sctpTimerStart, sctpTimerCancel, sctpTimerMatch, sctpTimerThread.
 *
 * $Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <sctp.h>

struct sctpTimer *head;

void printList(void)
{
	struct sctpTimer *t;
	for (t = head; t != NULL; t = t->next) {
		printf("{%d, %d, %d, 0x%08x, 0x%08x, 0x%08x, 0x%08x}\n", t->id,
				t->ms, t->remain_ms, (uint)t->callback, (uint)t->args,
				(uint)t->prev, (uint)t->next);
	}
	printf("==\n");
}

/**
 * Insert a new timer into the system.  The timer will fire in `ms`
 * milliseconds and call `callback` with `args` as the first parameter.
 * @param ms milliseconds before timer fires
 * @param *callback pointer to callback function
 * @param *args arguments to pass to callback function
 * @return pointer to timer entry (NULL on failure)
 */
struct sctpTimer *sctpTimerStart(uint ms, int id,
								 void (*callback)(void *), void *args)
{
	struct sctpTimer *timer;
	struct sctpTimer *next;
	struct sctpTimer *prev = NULL;
	uint remain;
	//irqmask im;

	timer = memget(sizeof(*timer));
	if (SYSERR == (int)timer)
	{
		return NULL;
	}

	timer->id = id;
	timer->ms = timer->remain_ms = ms;
	timer->callback = callback;
	timer->args = args;

	//im = disable();
	/* find where we should `timer` */
	remain = 0;
	for (next = head; next != NULL; next = next->next)
	{
		remain += next->remain_ms;
		if (remain > ms)
		{
			break;
		}
		timer->remain_ms -= next->remain_ms;
		prev = next;
	}
	/* now we should have prev .. (timer) .. next */
	timer->next = next;
	timer->prev = prev;
	if (NULL == prev)
	{
		/* no one before us, we are the head */
		head = timer;
	}
	else
	{
		prev->next = timer;
	}
	if (NULL != next)
	{
		next->prev = timer;
	}
	if (NULL != timer->next)
	{
		timer->next->remain_ms -= timer->remain_ms;
	}
	//printList();
	//restore(im);

	return timer;
}

/**
 * Remove a specific timer from the list.
 * @param *timer timer to remove from list
 * @return remaining time of removed timer entry (-1 if no entry found)
 */
int sctpTimerCancel(struct sctpTimer *timer)
{
	//irqmask im;
	int remainder = timer->remain_ms;

	if (NULL == timer->callback)
	{
		return -1;
	}

	//im = disable();
	if (NULL != timer->next)
	{
		timer->next->remain_ms += remainder;
		timer->next->prev = timer->prev;
	}
	if (NULL == timer->prev)
	{
		/* timer was first in line */
		head = timer->next;
	}
	else
	{
		timer->prev->next = timer->next;
	}
	//im = disable();

	timer->id = 0;
	timer->ms = 0;
	timer->remain_ms = 0;
	timer->callback = NULL;
	timer->args = NULL;
	timer->prev = NULL;
	timer->next = NULL;
	memfree(timer);

	return remainder;
}

/**
 * Return the first timer entry with a matching id.
 * @param id timer id to search for
 * @return pointer to timer entry, NULL if not found
 */
struct sctpTimer *sctpTimerMatch(int id)
{
	struct sctpTimer *curr;
	//irqmask im;

	//im = disable();
	for (curr = head; curr != NULL; curr = curr->next)
	{
		if (curr->id == id)
		{
			break;
		}
	}
	//restore(im);

	return curr;
}

thread sctpTimerThread(void)
{
	//irqmask im;

	while (TRUE)
	{
		while (head != NULL && 0 == head->remain_ms)
		{
			/* we've timed out, call the callback */
			(head->callback)(head->args);
			sctpTimerCancel(head);
		}
		if (NULL != head)
		{
			head->remain_ms -= 1;
		}
		printf("tick\n");
		sleep(1);
	}
}
