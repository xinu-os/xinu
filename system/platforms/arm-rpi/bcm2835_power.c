/**
 * @file bcm2835_power.c
 *
 * This driver provides the ability to power on hardware, such as the USB
 * Controller, on the BCM2835 (Raspberry Pi).
 *
 */
#include "bcm2835.h"

/**
 * Pointer to the "mailbox" on the BCM2835.  This is needed to communicate with
 * the GPU and is needed here to physically power on the USB hardware before
 * accessing it.
 */
static volatile uint *const mailbox_regs = (volatile uint*)0x2000B880;

#define MAILBOX_READ               0
#define MAILBOX_STATUS             6
#define MAILBOX_WRITE              8

#define MAILBOX_FULL               0x80000000
#define MAILBOX_EMPTY              0x40000000

#define MAILBOX_CHANNEL_POWER_MGMT 0
#define MAILBOX_CHANNEL_MASK       0xf

static void
bcm2835_mailbox_write(uint channel, uint value)
{
    while (mailbox_regs[MAILBOX_STATUS] & MAILBOX_FULL)
    {
    }
    mailbox_regs[MAILBOX_WRITE] = (value & ~MAILBOX_CHANNEL_MASK) | channel;
}

static uint
bcm2835_mailbox_read(uint channel)
{
    uint value;

    while (mailbox_regs[MAILBOX_STATUS] & MAILBOX_EMPTY)
    {
    }
    do
    {
        value = mailbox_regs[MAILBOX_READ];
    } while ((value & MAILBOX_CHANNEL_MASK) != channel);
    return (value & ~MAILBOX_CHANNEL_MASK);
}

static uint
bcm2835_get_power_mask(void)
{
    return (bcm2835_mailbox_read(MAILBOX_CHANNEL_POWER_MGMT) >> 4);
}

static void
bcm2835_set_power_mask(uint mask)
{
    bcm2835_mailbox_write(MAILBOX_CHANNEL_POWER_MGMT, mask << 4);
}

/**
 * Current on/off state of the various hardware.
 */
static uint bcm2835_power_mask;

/**
 * Powers on or off a piece of hardware on the board.
 *
 * @param feature
 *      Device or hardware to power on or off
 * @param on
 *      TRUE to power on; FALSE to power on.
 *
 * @return
 *      OK if successful; SYSERR otherwise.
 */
int bcm2835_setpower(enum board_power_feature feature, bool on)
{
    uint bit;
    uint newmask;
    bool is_on;

    bit = 1 << feature;
    is_on = (bcm2835_power_mask & bit) != 0;
    if (on != is_on)
    {
        newmask = bcm2835_power_mask ^ bit;
        bcm2835_set_power_mask(newmask);
        bcm2835_power_mask = bcm2835_get_power_mask();
        if (bcm2835_power_mask != newmask)
        {
            return SYSERR;
        }
    }
    return OK;
}

/**
 * Resets power to default state.
 */
void bcm2835_power_init(void)
{
    bcm2835_set_power_mask(0);
    bcm2835_power_mask = 0;
}
