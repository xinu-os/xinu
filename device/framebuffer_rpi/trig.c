/**
 * @file trig.c
 *
 * Provides sine and cosine for Taylor series.
 * Cannot handle values above 215 degrees--will cause overflow.
 * To correct this, only compute values between 0 and 90.
 * Answers are not exactly accurate, but accurate enough for our purposes.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stddef.h>
#include <framebuffer.h>

double power(double base, int exp)
{
    double result = 1.0;
    int i;
    for (i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

long factorial(int num)
{
    if (num < 2)
        return 1;
    return num*factorial(num-1);
}

double cosine_taylor(double x, int terms)
{
    int i;
    double result = 0.0;
    for (i = 0; i < terms; i++)
    {
        result += power(-1.0,i)*power(x,i*2)/factorial(i*2);
    }
    return result;
}

double cos(int x)
{
    double radx;
    while (x < 0) x += 360;
    x = x % 360;
    //handle basic angles.
    if (x == 0) return 1;
    else if ((x % 180) == 0) return -1;
    else if ((x % 90) == 0) return 0;
    //quadrant 1
    else if ( ((x > -360) && (x < -270)) || ((x > 0) && (x < 90)) ) {
        radx = PI * x / 180;
        return cosine_taylor(radx, 10);
    }
    //quadrant 2
    else if ( ((x > -270) && (x < -180)) || ((x > 90) && (x < 180)) ) {
        radx = PI * (180 - x) / 180;
        return -1 * cosine_taylor(radx, 10);
    } //quadrant 3
    else if ( ((x > -180) && (x < -90)) || ((x > 180) && (x < 270)) ) {
        radx = PI * (x - 180) / 180;
        return -1 * cosine_taylor(radx, 10);
    } //quadrant 4
    else {
        radx = PI * (360 - x) / 180;
        return cosine_taylor(radx, 10);
    }
}

double sine_taylor(double x, int terms)
{
    int i;
    double result = 0.0;
    for (i = 0; i < terms; i++)
    {
        result += power(-1,i)*power(x,i*2+1)/factorial(i*2+1);
    }
    return result;
}

/* Note: For graphics purposes, the sine must always be turned negative because
 * a monitor is technically in quadrant 4 instead of quadrant 1. */
double sin(int x)
{
    double radx;
    while (x < 0) x += 360;
    x = x % 360;
    //handle basic angles.
    if ((x % 180) == 0) return 0;
    else if ((x == 90) || (x == -270)) return -1;
    else if ((x == -90) || (x == 270)) return 1;
    //quadrant 1
    else if ( ((x > -360) && (x < -270)) || ((x > 0) && (x < 90)) ) {
        radx = PI * x / 180;
        return -1 * sine_taylor(radx, 10);
    } //quadrant 2
    else if ( ((x > -270) && (x < -180)) || ((x > 90) && (x < 180)) ) {
        radx = PI * (180 - x) / 180;
        return -1 * sine_taylor(radx, 10);
    } //quadrant 3
    else if ( ((x > -180) && (x < -90)) || ((x > 180) && (x < 270)) ) {
        radx = PI * (x - 180) / 180;
        return sine_taylor(radx, 10);
    } //quadrant 4
    else {
        radx = PI * (360 - x) / 180;
        return sine_taylor(radx, 10);
    }
}
