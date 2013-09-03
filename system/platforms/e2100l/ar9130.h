/**
 * @file ar9130.h
 * 
 * Constants and declarations associated with the Atheros 9130
 *  wireless System On A Chip.
 *
 */

#define RST_MISC_INTERRUPT_MASK   0xB8060014
#define RST_MISC_INTERRUPT_STATUS 0xB8060010

#define RST_MISC_IRQ_TIMER        (1 << 0)
#define RST_MISC_IRQ_ERROR        (1 << 1)
#define RST_MISC_IRQ_GPIO         (1 << 2)
#define RST_MISC_IRQ_UART         (1 << 3)
#define RST_MISC_IRQ_WATCHDOG     (1 << 4)
#define RST_MISC_IRQ_PERF         (1 << 5)
#define RST_MISC_IRQ_MBOX         (1 << 7)
#define RST_MISC_IRQ_MASK         0x00FF
