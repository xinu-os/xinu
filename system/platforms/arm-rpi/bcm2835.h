/**
 * @file bcm2835.h
 *
 * This file is included by usb_dwc_hcd.c to separate functionality that is
 * specific to the BCM2835 SoC.
 */

#ifndef _BCM2835_H_

#include <stddef.h>

/** Physical memory address at which software can communicate with the DWC USB
 * Controller via memory-mapped registers.  This is documented on Page 202 of
 * the BCM2835 ARM Peripherals document, but the stated address of 0x7e980000 is
 * a bus address must be translated into a physical address as per page 6 first.
 * */
#define DWC_REGS_BASE        0x20980000

/**
 * IRQ line for the USB host controller on the Raspberry Pi.  This isn't
 * actually documented anywhere.  I found it in
 * arch/arm/mach-bcm2708/include/mach/platform.h in the Linux kernel sources
 * (may be in the Raspberry Pi repository only).  There, INTERRUPT_USB is
 * defined as (64 + 11), indicating that the USB host controller interrupt
 * corresponds to bit 11 in the IRQ Basic Pending register, which is an alias
 * for "GPU" IRQ 9.  Yes, that code is used on the BCM2835 despite being in a
 * directory named after BCM2708, which likely is a previous version of the
 * board.
 */
#define IRQ_USB 9

enum board_power_feature {
    POWER_SD     = 0,
    POWER_UART_0 = 1,
    POWER_UART_1 = 2,
    POWER_USB    = 3,
};

extern int bcm2835_setpower(enum board_power_feature feature, bool on);
extern void bcm2835_power_init(void);

#define board_setpower bcm2835_setpower

#endif /* _BCM2835_H_ */
