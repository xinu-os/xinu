/**
 * @file date.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <clock.h>
#include <date.h>

static uint date_time = 0;

char days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

char *month_names[] = { "January", "February", "March", "April",
    "May", "June", "July", "August",
    "September", "October", "November", "December"
};

char *weekday_names[] = { "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

char *tz_names[] = {            /* UTC */
    "", "", "", "", "", "",     /* UTC+5 */
    "", "", "", "", "", "",     /* UTC+11 */
    /* UTC-12 */
    "", "", "H", "AK", "P", "M",        /* UTC-7 */
    "C", "E", "A", "", "", ""   /* UTC-1 */
};

/**
 * Get the system date time.
 * @return current system date time
 */
uint get_datetime(void)
{
#if RTCLOCK
    return (date_time + clktime);
#else
    date_time = 0;
    return SYSERR;
#endif                          /* RTCLOCK */
}

/**
 * Set the system date time.
 * @param dt current date time (in seconds since Jan. 1, 1900)
 * @return current system date time
 */
uint set_datetime(uint dt)
{
#if RTCLOCK
    date_time = dt - clktime;
    return (date_time + clktime);
#else
    return SYSERR;
#endif                          /* RTCLOCK */
}

/**
 * Print the current date and time based on the system timezone.
 * @param dt date time (seconds since Jan. 1, 1900)
 */
void printDate(uint dt)
{
    printDateTZ(dt, TZ_CST);
}

/**
 * Print the current date and time based on passed timezone.
 * @param dt date time (seconds since Jan. 1, 1900)
 * @param timezone timezone offset in hours
 */
void printDateTZ(uint dt, char timezone)
{
    struct time t;
    int tz;
    convertDate(&t, dt + (timezone * SECS_IN_HOUR));

    if (1 == t.is_dst)
    {
        convertDate(&t, dt + ((timezone + 1) * SECS_IN_HOUR));
    }

    tz = timezone;
    tz += (tz < 0) ? 24 : 0;

    printf("%02d:%02d:%02d %s%s %s, %s %d, %d\n",
           t.hour, t.minute, t.second, tz_names[tz],
           (1 == t.is_dst) ? "DT" : "ST", weekday_names[t.weekday],
           month_names[t.month], t.monthday, 1900 + t.year);
}

/**
 * Convert seconds (since 1900) to a more clear time structure.
 * @param time    time structure
 * @param seconds seconds since 1900
 */
void convertDate(struct time *t, uint seconds)
{
    int leapyear;
    int dst_day;
    int leapday = 0;

    /* weekday (time started on a Monday) */
    t->weekday = (1 + (seconds / SECS_IN_DAY)) % 7;

    /* year */
    t->year = (seconds / SECS_IN_YEAR);

    /* number of leapyears that have occured up to this year */
    leapyear = ((t->year - 1) / 4 + (t->year - 1) / 100) * SECS_IN_DAY;
    t->year = ((seconds - leapyear) / SECS_IN_YEAR);

    seconds -= (t->year * SECS_IN_YEAR);

    /* Is this a leap year? */
    if (((0 == (t->year % 4)) && (0 != (t->year % 100)))
        || (0 == ((1900 + t->year) % 400)))
    {
        leapday = 1;
    }

    /* day in year */
    t->yearday = (seconds / SECS_IN_DAY);
    seconds -= ((t->yearday) * SECS_IN_DAY);

    /* missed leapyears */
    t->yearday -= t->year / 4;

    /* overcounted leapyears */
    t->yearday += t->year / 100;

    /* undercounted leapyears since 1900 */
    t->yearday -= (((1900 + t->year) / 400) - 4);

    /* and that leapday */
    t->yearday += leapday;

    /* day of month and month */
    t->monthday = t->yearday;

    /* yearday - days of the month loop */
    for (t->month = 0; t->month < sizeof(days_in_month) - 1; t->month++)
    {
        if (1 == leapday && 1 == t->month)
        {
            leapyear = 1;
        }
        else
        {
            leapyear = 0;
        }

        if (t->monthday < days_in_month[t->month] + leapyear)
        {
            break;
        }

        t->monthday -= days_in_month[t->month] + leapyear;
    }
    t->monthday += 1;

    /* hour */
    t->hour = (seconds / SECS_IN_HOUR);
    seconds -= (t->hour * SECS_IN_HOUR);

    /* minute */
    t->minute = (seconds / SECS_IN_MIN);
    seconds -= (t->minute * SECS_IN_MIN);

    /* second */
    t->second = seconds;

    /* is_dst (valid in U.S., calculation adapted from: */
    /* http://webexhibits.org/daylightsaving/i.html)    */
    t->is_dst = 0;
    dst_day = DAYS_IN_YEAR + 1;
    if (55 <= t->year && t->year <= 106)
    {
        if (4 < t->month && t->month < 8)
        {
            t->is_dst = 1;
        }
        else if (4 == t->month)
        {
            dst_day = (2 + 6 * t->year - t->year / 4) % 7 + 1;
        }
        else if (8 == t->month)
        {
            dst_day = (31 - (t->year * 5 / 4 + 1)) % 7;
        }
    }
    else if (107 <= t->year)
    {
        if (3 < t->month && t->month < 9)
        {
            t->is_dst = 1;
        }
        else if (3 == t->month)
        {
            dst_day = 14 - (1 + t->year * 5 / 4) % 7;
        }
        else if (9 == t->month)
        {
            dst_day = 7 - (1 + t->year * 5 / 4) % 7;
        }
    }
    else
    {
        t->is_dst = -1;         /* DST didn't exist */
    }

    if ((t->monthday == dst_day && 2 <= t->hour) || t->monthday > dst_day)
    {
        t->is_dst = 1;
    }
}
