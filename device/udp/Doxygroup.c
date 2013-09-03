/**
 * @defgroup udp UDP
 * @ingroup devices
 * @brief User Datagram Protocol driver
 *
 * This module provides an interface to the User Datagram Protocol (UDP).
 *
 * @defgroup udpexternal UDP standard functions
 * @ingroup udp
 * @brief Primary UDP API, including functions compliant with Xinu's device
 * model.
 *
 * Users should use udpAlloc() to allocate a UDP device, then use the generic
 * device calls open(), read(), write(), control(), or close() on it, each of
 * which calls the corresponding function listed here.  A UDP device essentially
 * corresponds to a "socket" as used in other operating systems.  Local and
 * remote ports and addresses are specified in the open() calle.
 *
 * @defgroup udpinternal UDP internals
 * @ingroup udp
 * @brief Low-level functions mostly internal to the UDP driver.
 */
