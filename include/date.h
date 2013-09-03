/*
 * @file date.h
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#ifndef _DATE_H_
#define _DATE_H_

#include <stddef.h>
#include <stdio.h>

struct time
{
    int second;                 /* 0-59 */
    int minute;                 /* 0-59 */
    int hour;                   /* 0-23 */
    int monthday;               /* 1-31 */
    int month;                  /* 0 - Jan, ..., 11 - Dec */
    int weekday;                /* 0 - Sun, ..., 6 - Sat */
    int year;                   /* 0 is 1900 */
    int yearday;                /* day in year */
    int is_dst;                 /* true if is DST */
};

extern char days_in_month[12];
extern char *month_names[12];
extern char *weekday_names[7];
extern char *tz_names[24];

#define TZ_HST (-10)
#define TZ_AST (-9)
#define TZ_PST (-8)
#define TZ_MST (-7)
#define TZ_CST (-6)
#define TZ_EST (-5)

#define DAYS_IN_YEAR 365
#define SECS_IN_MIN  60
#define SECS_IN_HOUR (60 * SECS_IN_MIN)
#define SECS_IN_DAY  (24 * SECS_IN_HOUR)
#define SECS_IN_YEAR (365 * SECS_IN_DAY)

#define RDATE_SERVER "192.168.6.10"     /* Default rdate server */
#define TIMESERVER_BUFSIZE 1024

uint get_datetime(void);
uint set_datetime(uint dt);

void printDate(uint dt);
void printDateTZ(uint dt, char timezone);
void convertDate(struct time *time, uint seconds);
syscall rdate(char *);
long getRdate(char *);
thread timeServer(int descrp, int localport);

#endif                          /* _DATE_H_ */
