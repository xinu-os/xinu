/**
 * @file uartGetc.c
 * @provides uartGetc
 *
 * $Id: uartGetd.c 1369 2008-07-04 00:09:43Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include <device.h>

/**
 * Read a single character from UART.
 * @param pdev pointer to UART device
 */
devcall uartGetc(device *pdev)
{  
  uchar ch = 0;  
  uartRead(pdev, &ch, 1);
  return ch;
}
