/**
 * @file usbKbdInterrupt.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <usbkbd.h>
#include <usb_core_driver.h>
#include <string.h>

#define HID_BOOT_LEFT_CTRL   (1 << 0)
#define HID_BOOT_LEFT_SHIFT  (1 << 1)
#define HID_BOOT_LEFT_ALT    (1 << 2)
#define HID_BOOT_LEFT_GUI    (1 << 3)
#define HID_BOOT_RIGHT_CTRL  (1 << 4)
#define HID_BOOT_RIGHT_SHIFT (1 << 5)
#define HID_BOOT_RIGHT_ALT   (1 << 6)
#define HID_BOOT_RIGHT_GUI   (1 << 7)

/* Map of HID keyboard usage IDs to characters.
 *
 * Entries not filled in are left 0 and are interpreted as unrecognized input
 * and ignored.
 *
 * Source: section 10 of the Universal Serial Bus HID Usage Tables v1.11.
 */
static const char keymap[256][2] = {
    [4]  = {'a', 'A'},
    [5]  = {'b', 'B'},
    [6]  = {'c', 'C'},
    [7]  = {'d', 'D'},
    [8]  = {'e', 'E'},
    [9]  = {'f', 'F'},
    [10] = {'g', 'G'},
    [11] = {'h', 'H'},
    [12] = {'i', 'I'},
    [13] = {'j', 'J'},
    [14] = {'k', 'K'},
    [15] = {'l', 'L'},
    [16] = {'m', 'M'},
    [17] = {'n', 'N'},
    [18] = {'o', 'O'},
    [19] = {'p', 'P'},
    [20] = {'q', 'Q'},
    [21] = {'r', 'R'},
    [22] = {'s', 'S'},
    [23] = {'t', 'T'},
    [24] = {'u', 'U'},
    [25] = {'v', 'V'},
    [26] = {'w', 'W'},
    [27] = {'x', 'X'},
    [28] = {'y', 'Y'},
    [29] = {'z', 'Z'},
    [30] = {'1', '!'},
    [31] = {'2', '@'},
    [32] = {'3', '#'},
    [33] = {'4', '$'},
    [34] = {'5', '%'},
    [35] = {'6', '^'},
    [36] = {'7', '&'},
    [37] = {'8', '*'},
    [38] = {'9', '('},
    [39] = {'0', ')'},
    [40] = {'\n', '\n'},     /* Enter      */
    /* ... */
    [42] = {'\b', '\b'},     /* Backspace  */
    [43] = {'\t', '\t'},     /* Tab        */
    [44] = {' ', ' '},       /* Space      */
    [45] = {'-', '_'},
    [46] = {'=', '+'},
    [47] = {'[', '{'},
    [48] = {']', '}'},
    [49] = {'\\', '|'},
    /* ... */
    [51] = {';', ':'},
    [52] = {'\'', '"'},
    [53] = {'`', '~'},
    [54] = {',', '<'},
    [55] = {'.', '>'},
    [56] = {'/', '?'},
    [57] = {},               /* Caps lock  */
    /* ... */
    [76] = {'\x7f', '\x7f'}, /* Delete                    */
    /* ... */
    [84] = {'/'},            /* Keypad /                  */
    [85] = {'*'},            /* Keypad *                  */
    [86] = {'-'},            /* Keypad -                  */
    [87] = {'+'},            /* Keypad +                  */
    [88] = {'\n'},           /* Keypad Enter              */
    [89] = {'1'},            /* Keypad 1 and End          */
    [90] = {'2'},            /* Keypad 2 and Down Arrow   */
    [91] = {'3'},            /* ...                       */
    [92] = {'4'},
    [93] = {'5'},
    [94] = {'6'},
    [95] = {'7'},
    [96] = {'8'},
    [97] = {'9'},
    [98] = {'0'},
    [99] = {'.', '\x7f'},    /* Keypad . and Delete       */
    [103] = {'=', '='},      /* Keypad =                  */
};

/**
 * Called when the USB transfer request from a USB keyboard's IN interrupt
 * endpoint completes or fails.
 */
void usbKbdInterrupt(struct usb_xfer_request *req)
{
    struct usbkbd *kbd = req->private;

    if (req->status == USB_STATUS_SUCCESS && req->actual_size == 8)
    {
        /* An 8 byte report was successfully received from the USB keyboard.
         * Note that we're using the boot protocol, so the interpretation of
         * this data is fixed and not affected by the HID report descriptor.  */

        USBKBD_TRACE("Keyboard report received");

        const uchar *data = req->recvbuf;
        uint mod_idx = 0;
        uint count = 0;
        uint i;

        /* Byte 0 is modifiers mask.  We currently only care about shift.  */
        if (data[0] & (HID_BOOT_LEFT_SHIFT | HID_BOOT_RIGHT_SHIFT))
        {
            mod_idx++;
        }

        /* Byte 1 must be ignored.  */

        /* Bytes 2 through 8 are the usage IDs of non-modifier keys currently
         * pressed, or 0 to indicate no key pressed.
         *
         * Note that the keyboard sends a full report when any key is pressed or
         * released.  If a key is down in two consecutive reports, it should
         * only be interpreted as one keypress.
         */
        for (i = 2; i < 8; i++)
        {
            uchar usage_id = data[i];
            uchar c = keymap[usage_id][mod_idx];

            if (0 != c &&
                NULL == memchr(kbd->recent_usage_ids, usage_id, 6))
            {
                if (kbd->icount < USBKBD_IBLEN)
                {
                    kbd->in[(kbd->istart + kbd->icount) % USBKBD_IBLEN] = c;
                    kbd->icount++;
                    count++;
                }
                else
                {
                    /* XXX:  Overrun  */
                }
            }
        }
        USBKBD_TRACE("Reported %u new characters", count);
        signaln(kbd->isema, count);
        memcpy(kbd->recent_usage_ids, data + 2, 6);
    }
    else
    {
        USBKBD_TRACE("Bad xfer: status=%d, actual_size=%u",
                     req->status, req->actual_size);
    }
    usb_submit_xfer_request(req);
}
