/**
 * @file     ulaw2linear.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/*
 * This routine converts from ulaw to 16 bit linear.
 *
 * Craig Reese: IDA/Supercomputing Research Center
 * 29 September 1989
 * http://www.speech.cs.cmu.edu/comp.speech/Section2/Q2.7.html
 *
 * References:
 * 1) CCITT Recommendation G.711  (very difficult to follow)
 * 2) MIL-STD-188-113,"Interoperability and Performance Standards
 *     for Analog-to_Digital Conversion Techniques,"
 *     17 February 1987
 *
 * Input: 8 bit ulaw sample
 * Output: signed 16 bit linear sample
 */
int ulaw2linear(unsigned char ulawbyte)
{
    static int exp_lut[8] = { 0, 132, 396, 924, 1980, 4092, 8316, 16764 };
    int sign, exponent, mantissa, sample;

    ulawbyte = ~ulawbyte;
    sign = (ulawbyte & 0x80);
    exponent = (ulawbyte >> 4) & 0x07;
    mantissa = ulawbyte & 0x0F;
    sample = exp_lut[exponent] + (mantissa << (exponent + 3));
    if (sign != 0)
    {
        sample = -sample;
    }

    return sample;
}
