/**
 * @file     usbdebug.c
 * @ingroup  usbcore
 * @ingroup  usb
 *
 * This file contains USB core functions not strictly necessary for USB to
 * actually operate.  This includes:
 *
 * - Printing debugging, informational, and error messages
 * - Printing the state of the USB (through usbinfo()).
 * - Obtaining human-readable string descriptors from USB devices.
 * - Translating certain constants into strings describing them.
 *
 * To disable all the above functionality, set USB_EMBEDDED to TRUE in
 * usb_util.h.  Obviously, only do this if you know what you're doing.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include <interrupt.h>
#include <usb_core_driver.h>
#include <usb_hub_driver.h>
#include <usb_std_defs.h>
#include <usb_subsystem.h>

#if !USB_EMBEDDED

#if USB_MIN_LOG_PRIORITY <= USB_LOG_PRIORITY_ERROR
/*
 * Synchronously writes an error, informational, or debugging message to the
 * serial device.  Don't call this directly; use usb_debug(), usb_info(), or
 * usb_error() instead.
 */
void usb_log(int priority, const char *func,
             struct usb_device *dev, const char *format, ...)
{
    va_list va;
    irqmask im;

    if (priority < USB_MIN_LOG_PRIORITY)
    {
        return;
    }

    im = disable();

    kprintf("USB: ");
    if (priority <= USB_LOG_PRIORITY_DEBUG)
    {
        kprintf("[DEBUG] ");
    }
    else if (priority >= USB_LOG_PRIORITY_ERROR)
    {
        kprintf("[ERROR] ");
    }
    if (dev != NULL)
    {
        kprintf("Device %u: ", dev->address);
    }
    if (priority <= USB_LOG_PRIORITY_DEBUG)
    {
        kprintf("%s(): ", func);
    }
    va_start(va, format);
    kvprintf(format, va);
    va_end(va);
    restore(im);
}
#endif

/**
 * @ingroup usbcore
 *
 * Translates a ::usb_status_t into a string describing it.
 *
 * @param status
 *      A ::usb_status_t error code returned by one of the functions in Xinu's
 *      USB subsystem.
 *
 * @return
 *      A constant string describing the error, or "unknown error" if the error
 *      code is not recognized.
 */
const char *
usb_status_string(usb_status_t status)
{
    switch (status)
    {
        case USB_STATUS_SUCCESS:
            return "success";
        case USB_STATUS_OUT_OF_MEMORY:
            return "out of memory";
        case USB_STATUS_UNSUPPORTED_REQUEST:
            return "unsupported request";
        case USB_STATUS_DEVICE_UNSUPPORTED:
            return "device unsupported";
        case USB_STATUS_TIMEOUT:
            return "request timed out";
        case USB_STATUS_HARDWARE_ERROR:
            return "hardware error";
        case USB_STATUS_INVALID_DATA:
            return "invalid data";
        case USB_STATUS_INVALID_PARAMETER:
            return "invalid parameter";
        case USB_STATUS_NOT_PROCESSED:
            return "transfer not processed yet";
        case USB_STATUS_DEVICE_DETACHED:
            return "device was detached";
    }
    return "unknown error";
}

/**
 * @ingroup usbcore
 *
 * Translates a USB class code into a string describing it.
 *
 * @param class_code
 *      A USB class code constant.
 * @return
 *      A constant string describing the USB class, or "Unknown" if the class
 *      code was not recognized.
 */
const char *
usb_class_code_to_string(enum usb_class_code class_code)
{
    switch (class_code)
    {
        case USB_CLASS_CODE_INTERFACE_SPECIFIC:
            return "None (see interface descriptors)";
        case USB_CLASS_CODE_AUDIO:
            return "Audio";
        case USB_CLASS_CODE_COMMUNICATIONS_AND_CDC_CONTROL:
            return "Communications and CDC Control";
        case USB_CLASS_CODE_HID:
            return "HID (Human Interface Device)";
        case USB_CLASS_CODE_IMAGE:
            return "Image";
        case USB_CLASS_CODE_PRINTER:
            return "Printer";
        case USB_CLASS_CODE_MASS_STORAGE:
            return "Mass Storage";
        case USB_CLASS_CODE_HUB:
            return "Hub";
        case USB_CLASS_CODE_VIDEO:
            return "Video";
        case USB_CLASS_CODE_WIRELESS_CONTROLLER:
            return "Wireless Controller";
        case USB_CLASS_CODE_MISCELLANEOUS:
            return "Miscellaneous";
        case USB_CLASS_CODE_VENDOR_SPECIFIC:
            return "Vendor Specific";
    }
    return "Unknown";
}

/**
 * @ingroup usbcore
 *
 * Translates a USB speed constant into a string describing it.
 *
 * @param speed
 *      A USB speed constant.
 *
 * @return
 *      "high", "full", "low", or "unknown".
 */
const char *
usb_speed_to_string(enum usb_speed speed)
{
    switch (speed)
    {
        case USB_SPEED_HIGH:
            return "high";
        case USB_SPEED_FULL:
            return "full";
        case USB_SPEED_LOW:
            return "low";
    }
    return "unknown";
}

/**
 * @ingroup usbcore
 *
 * Translates a USB transfer type constant into a string describing it.
 *
 * @param type
 *      A USB transfer type constant.
 *
 * @return
 *      "Control", "Isochronous", "Bulk", "Interrupt", or "Unknown".
 */
const char *
usb_transfer_type_to_string(enum usb_transfer_type type)
{
    switch (type)
    {
        case USB_TRANSFER_TYPE_CONTROL:
            return "Control";
        case USB_TRANSFER_TYPE_ISOCHRONOUS:
            return "Isochronous";
        case USB_TRANSFER_TYPE_BULK:
            return "Bulk";
        case USB_TRANSFER_TYPE_INTERRUPT:
            return "Interrupt";
    }
    return "Unknown";
}

/**
 * @ingroup usbcore
 *
 * Translates a USB direction constant into a string describing it.
 *
 * @param dir
 *      A USB direction constant.
 *
 * @return
 *      "OUT", "IN", or "Unknown".
 */
const char *
usb_direction_to_string(enum usb_direction dir)
{
    switch (dir)
    {
        case USB_DIRECTION_OUT:
            return "OUT";
        case USB_DIRECTION_IN:
            return "IN";
    }
    return "Unknown";
}


/**
 * Lazy conversion from UTF-16LE to ASCII that replaces non-ASCII UTF-16LE code
 * points with question marks.
 */
static void
utf16le_to_ascii(const uint16_t utf16le_str[], uint nchars, char *ascii_str)
{
    uint i;

    for (i = 0; i < nchars; i++)
    {
        if (utf16le_str[i] <= 0x7f)
        {
            ascii_str[i] = utf16le_str[i];
        }
        else
        {
            ascii_str[i] = '?';
        }
    }
}

/**
 * @ingroup usbcore
 *
 * Reads a USB string descriptor from a device.
 *
 * @param dev
 *      Pointer to the USB device structure for the device from which to read
 *      the string descriptor.
 * @param index
 *      Index of the string descriptor to read.  For example, the iProduct
 *      member of the device descriptor specifies the index of the string
 *      descriptor that contains the product name.
 * @param lang_id
 *      Language ID of the language to request.  Note: the available language
 *      IDs can be retrieved by requesting the string descriptor of index 0 with
 *      any language ID.
 * @param buf
 *      Buffer in which to place the string descriptor.
 * @param buflen
 *      Maximum length in bytes to read.
 *
 * @return
 *      See usb_get_descriptor().
 */
usb_status_t
usb_get_string_descriptor(struct usb_device *dev, uint8_t index, uint16_t lang_id,
                          struct usb_string_descriptor *buf, uint16_t buflen)
{
    return usb_get_descriptor(dev,
                              USB_DEVICE_REQUEST_GET_DESCRIPTOR,
                              USB_BMREQUESTTYPE_DIR_IN |
                                    USB_BMREQUESTTYPE_TYPE_STANDARD |
                                    USB_BMREQUESTTYPE_RECIPIENT_DEVICE,
                              (USB_DESCRIPTOR_TYPE_STRING << 8) | index, lang_id,
                              buf, buflen);
}

/**
 * @ingroup usbcore
 *
 * Retrieves the English version (if any) of a USB string descriptor and
 * "translates" it from UTF-16LE to ASCII.  The resulting string is
 * null-terminated.  UTF-16LE codepoints outside the ASCII range are simply
 * replaced with question marks.
 *
 * @param dev
 *      Pointer to the USB device structure for the device from which to read
 *      the string descriptor.
 * @param iString
 *      Index of the string descriptor to read.
 * @param strbuf
 *      Buffer in which to place the ASCII string.
 * @param strbufsize
 *      Length of the @p strbuf buffer.
 *
 * @return
 *      Any value that can be returned by usb_get_string_descriptor(), plus
 *      ::USB_STATUS_INVALID_DATA if the list of available language IDs is
 *      empty.
 */
usb_status_t
usb_get_ascii_string(struct usb_device *dev, uint32_t iString,
                     char *strbuf, uint32_t strbufsize)
{
    struct {
        struct usb_string_descriptor desc;
        uint16_t padding[128];
    } buf;
    uint16_t lang_id;
    usb_status_t status;
    uint i;
    uint num_languages;
    uint num_chars;

    /* Get the list of available languages.  */
    status = usb_get_string_descriptor(dev, 0, 0, &buf.desc, sizeof(buf));
    if (status != USB_STATUS_SUCCESS)
    {
        return status;
    }

    /* Make sure the list of available languages is nonempty.  */
    num_languages = (buf.desc.bLength - sizeof(struct usb_descriptor_header)) /
                        sizeof(uint16_t);
    if (num_languages == 0)
    {
        usb_dev_error(dev, "String descriptor language list is empty\n");
        return USB_STATUS_INVALID_DATA;
    }

    /* Choose the first listed variant of English, or fall back to the first
     * language listed if English is not found.  */
    lang_id = buf.desc.bString[0];
    for (i = 0; i < num_languages; i++)
    {
        if ((buf.desc.bString[i] & USB_PRIMARY_LANGUAGE_MASK) == USB_LANG_ENGLISH)
        {
            lang_id = buf.desc.bString[i];
            break;
        }
    }

    /* Get the actual string descriptor we wanted.  */
    status = usb_get_string_descriptor(dev, iString, lang_id,
                                       &buf.desc, sizeof(buf));
    if (status != USB_STATUS_SUCCESS)
    {
        return status;
    }

    /* "Translate" the string from UTF-16LE to ASCII.  */
    num_chars = min((buf.desc.bLength - sizeof(struct usb_descriptor_header)) /
                                sizeof(uint16_t), strbufsize - 1);
    utf16le_to_ascii(buf.desc.bString, num_chars, strbuf);
    strbuf[num_chars] = '\0';
    return USB_STATUS_SUCCESS;
}

/**
 * Translates a bcdUSB (binary-coded-decimal USB version) value into a
 * human-readable string.
 *
 * @param bcdUSB
 *      The bcdUSB value (e.g. from a USB device descriptor) to translate.
 *
 * @return
 *      A pointer to a staticly allocated string describing the USB version.  It
 *      will be changed on the next call to this function.
 */
static const char *
usb_bcd_version_to_string(uint16_t bcdUSB)
{
    static char string[3 + 1 + 2 + 1 + 2 + 1];
    char *p = string;
    p += sprintf(string, "%u.%u",
                 (bcdUSB >> 8) & 0xff,  /* At most 3 digits */
                 (bcdUSB >> 4) & 0xf);  /* At most 2 digits */
                                        /* (plus period)    */
    if (bcdUSB & 0xf)
    {
        sprintf(p, ".%u", bcdUSB & 31);  /* At most 2 digits (plus period)  */
    }
    return string;
}

/**
 * @ingroup usbcore
 *
 * Returns a fairly detailed, human-readable description of a USB device.
 *
 * @param dev
 *      USB device to get a description of.
 *
 * @return
 *      A string describing the device.  The returned string is statically
 *      allocated and will be changed on the next call to this function.
 */
const char *
usb_device_description(const struct usb_device *dev)
{
    uint i;
    enum usb_class_code class;
    static char device_description[512];
    char *p;

    p = device_description;

    /* Start with speed and USB version information.  */
    p += sprintf(p, "%s-speed USB %s",
                 usb_speed_to_string(dev->speed),
                 usb_bcd_version_to_string(dev->descriptor.bcdUSB));

    /* Try to find a class description of the device, taking into account that
     * the class may be stored either in the device descriptor or an interface
     * descriptor.  */
    class = dev->descriptor.bDeviceClass;
    if (class == 0)
    {
        for (i = 0; i < dev->config_descriptor->bNumInterfaces; i++)
        {
            if (dev->interfaces[i]->bInterfaceClass != 0)
            {
                class = dev->interfaces[i]->bInterfaceClass;
            }
        }
    }

    /* Add the class description if we found one and it was not something
     * meaningless like the vendor specific class.  */
    if (class != 0 &&
        class != USB_CLASS_CODE_VENDOR_SPECIFIC &&
        class != USB_CLASS_CODE_MISCELLANEOUS)
    {
        p += sprintf(p, " %s class",
                     usb_class_code_to_string(class));
    }

    /* This is indeed a device.  */
    p += sprintf(p, " device");

    /* Add the product name, if the device provides it.  */
    if (dev->product[0] != '\0')
    {
        p += sprintf(p, " (%s)", dev->product);
    }

    /* Add vendor and product IDs.  */
    p += sprintf(p, " (idVendor=0x%04x, idProduct=0x%04x)",
                 dev->descriptor.idVendor,
                 dev->descriptor.idProduct);

    /* Return the resulting string.  */
    return device_description;
}

static void
usb_print_device_descriptor(const struct usb_device *dev,
                            const struct usb_device_descriptor *desc)
{
    printf("    [Device Descriptor]\n");
    printf("    bLength:             %u\n", desc->bLength);
    printf("    bDescriptorType:     0x%02x (Device)\n", desc->bDescriptorType);
    printf("    bcdUSB:              0x%02x (USB %s compliant)\n",
           desc->bcdUSB, usb_bcd_version_to_string(desc->bcdUSB));
    printf("    bDeviceClass:        0x%02x (%s)\n",
           desc->bDeviceClass, usb_class_code_to_string(desc->bDeviceClass));
    printf("    bDeviceSubClass:     0x%02x\n", desc->bDeviceSubClass);
    printf("    bDeviceProtocol:     0x%02x\n", desc->bDeviceProtocol);
    printf("    bMaxPacketSize0:     %u\n", desc->bMaxPacketSize0);
    printf("    idVendor:            0x%04x\n", desc->idVendor);
    printf("    idProduct:           0x%04x\n", desc->idProduct);
    printf("    iManufacturer:       %u\n", desc->iManufacturer);
    if (dev->manufacturer[0] != '\0')
    {
        printf("        (%s)\n", dev->manufacturer);
    }
    printf("    iProduct:            %u\n", desc->iProduct);
    if (dev->product[0] != '\0')
    {
        printf("        (%s)\n", dev->product);
    }
    printf("    iSerialNumber:       %u\n", desc->iSerialNumber);
    printf("    bNumConfigurations:  %u\n", desc->bNumConfigurations);
    putchar('\n');
}

static void
usb_print_configuration_descriptor(const struct usb_device *dev,
                                   const struct usb_configuration_descriptor *desc)
{
    printf("        [Configuration Descriptor]\n");
    printf("        bLength:             %u\n", desc->bLength);
    printf("        bDescriptorType:     0x%02x (Configuration)\n",
           desc->bDescriptorType);
    printf("        wTotalLength:        %u\n", desc->wTotalLength);
    printf("        bNumInterfaces:      %u\n", desc->bNumInterfaces);
    printf("        bConfigurationValue: %u\n", desc->bConfigurationValue);
    printf("        iConfiguration:      %u\n", desc->iConfiguration);
    printf("        bmAttributes:        0x%02x\n", desc->bmAttributes);
    if (desc->bmAttributes & USB_CONFIGURATION_ATTRIBUTE_SELF_POWERED)
    {
        printf("            (Self powered)\n");
    }
    if (desc->bmAttributes & USB_CONFIGURATION_ATTRIBUTE_REMOTE_WAKEUP)
    {
        printf("            (Remote wakeup)\n");
    }
    printf("        bMaxPower:           %u (%u mA)\n",
           desc->bMaxPower, desc->bMaxPower * 2);
    putchar('\n');
}

static void
usb_print_interface_descriptor(const struct usb_device *dev,
                               const struct usb_interface_descriptor *desc)
{
    printf("            [Interface Descriptor]\n");
    printf("            bLength:             %u\n", desc->bLength);
    printf("            bDescriptorType:     0x%02x (Interface)\n", desc->bDescriptorType);
    printf("            bInterfaceNumber:    %u\n", desc->bInterfaceNumber);
    printf("            bAlternateSetting:   %u\n", desc->bAlternateSetting);
    printf("            bNumEndpoints:       %u\n", desc->bNumEndpoints);
    printf("            bInterfaceClass:     0x%02x (%s)\n", desc->bInterfaceClass,
           usb_class_code_to_string(desc->bInterfaceClass));
    printf("            bInterfaceSubClass:  0x%02x\n", desc->bInterfaceSubClass);
    printf("            bInterfaceProtocol:  0x%02x\n", desc->bInterfaceProtocol);
    printf("            iInterface:          %u\n", desc->iInterface);
    putchar('\n');
}

static void
usb_print_endpoint_descriptor(const struct usb_device *dev,
                              const struct usb_endpoint_descriptor *desc)
{
    printf("                [Endpoint Descriptor]\n");
    printf("                bLength:             %u\n", desc->bLength);
    printf("                bDescriptorType:     0x%02x (Endpoint)\n", desc->bDescriptorType);
    printf("                bEndpointAddress:    0x%02x (Number %u, %s)\n",
           desc->bEndpointAddress, desc->bEndpointAddress & 0xf,
           ((desc->bEndpointAddress >> 7) ? "IN" : "OUT"));
    printf("                bmAttributes:        0x%02x (%s endpoint)\n",
           desc->bmAttributes,
           ((desc->bmAttributes & 0x3) == USB_TRANSFER_TYPE_CONTROL ? "control" :
            (desc->bmAttributes & 0x3) == USB_TRANSFER_TYPE_ISOCHRONOUS ? "isochronous" :
            (desc->bmAttributes & 0x3) == USB_TRANSFER_TYPE_BULK ? "bulk" :
            "interrupt"));
    printf("                wMaxPacketSize:      0x%02x (max packet size %u bytes)\n",
           desc->wMaxPacketSize, desc->wMaxPacketSize & 0x7ff);
    printf("                bInterval:           %u\n", desc->bInterval);
    putchar('\n');
}

static void
usb_print_configuration(const struct usb_device *dev)
{
    uint i, j;

    usb_print_configuration_descriptor(dev, dev->config_descriptor);
    for (i = 0; i < dev->config_descriptor->bNumInterfaces; i++)
    {
        usb_print_interface_descriptor(dev, dev->interfaces[i]);
        for (j = 0; j < dev->interfaces[i]->bNumEndpoints; j++)
        {
            usb_print_endpoint_descriptor(dev, dev->endpoints[i][j]);
        }
    }
}

/**
 * Print information about a USB device.
 */
static usb_status_t
usbinfo_device_callback(struct usb_device *dev)
{
    printf("[USB Device %03u]\n", dev->address);
    usb_print_device_descriptor(dev, &dev->descriptor);
    usb_print_configuration(dev);

    putchar('\n');
    return USB_STATUS_SUCCESS;
}

#define USBINFO_TREE_SPACES_PER_LEVEL  6
#define USBINFO_TREE_LINES_PER_PORT    2

static usb_status_t
usbinfo_tree_callback(struct usb_device *dev)
{
    uint i, j, n;

    if (dev->depth != 0)
    {
        n = (dev->depth - 1) * USBINFO_TREE_SPACES_PER_LEVEL;
        for (j = 0; j < USBINFO_TREE_LINES_PER_PORT; j++)
        {
            for (i = 0; i < n; i++)
            {
                putchar(' ');
            }
            putchar('|');
            putchar('\n');
        }
        for (i = 0; i < n; i++)
        {
            putchar(' ');
        }
        for (i = 0; i < USBINFO_TREE_SPACES_PER_LEVEL; i++)
        {
            putchar('-');
        }
    }
    printf("%03u [%s]\n", dev->address, usb_device_description(dev));
    return USB_STATUS_SUCCESS;
}

#endif /* !USB_EMBEDDED */

/**
 * @ingroup usb
 *
 * Print information about all devices attached to the USB.  This uses printf()
 * and therefore should be called with interrupts enabled (e.g. by the shell).
 *
 * @return
 *      ::SYSERR if USB subsystem is not initialized; ::OK otherwise.
 */
syscall usbinfo(void)
{
#if USB_EMBEDDED
    fprintf(stderr, "usbinfo not supported in USB_EMBEDDED mode.\n");
    return SYSERR;
#else
    struct usb_device *root_hub = usb_root_hub;

    if (root_hub == NULL)
    {
        fprintf(stderr, "USB subsystem not initialized.\n");
        return SYSERR;
    }

    /* USB is a dynamic bus and devices may be attached/detached at any time.
     * Therefore, we need to "lock" the bus to prevent devices from being
     * attached or detached from the bus.  Otherwise we may not get a consistent
     * snapshot of the bus, or a device could even be freed while we're printing
     * it.  */
    usb_lock_bus();
    usb_hub_for_device_in_tree(root_hub, usbinfo_device_callback);

    printf("\nDiagram of USB:\n\n");
    usb_hub_for_device_in_tree(root_hub, usbinfo_tree_callback);
    usb_unlock_bus();
    return OK;
#endif /* !USB_EMBEDDED */
}

