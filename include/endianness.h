/**
 * @file endianness.h
 *
 * Little-to-big and big-to-little endianness conversions.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _ENDIANNESS_H_
#define _ENDIANNESS_H_

#include <conf.h>
#include <stdint.h>

static inline uint16_t
bswap16(uint16_t n)
{
    return (n << 8) | (n >> 8);
}

static inline uint32_t
bswap32(uint32_t n)
{
    return (n << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | (n >> 24);
}

#if BYTE_ORDER == BIG_ENDIAN

/* Big-endian CPU  */

/* big endian to big endian:  no-op  */
#  define cpu_to_be16(n)  n
#  define cpu_to_be32(n)  n
#  define be16_to_cpu(n)  n
#  define be32_to_cpu(n)  n

/* big endian to little endian and vice versa  */
#  define cpu_to_le16(n)  bswap16(n)
#  define cpu_to_le32(n)  bswap32(n)
#  define le16_to_cpu(n)  bswap16(n)
#  define le32_to_cpu(n)  bswap32(n)

#else /* BYTE_ORDER == BIG_ENDIAN */

/* Little-endian CPU  */

/* little endian to big endian and vice versa  */
#  define cpu_to_be16(n)  bswap16(n)
#  define cpu_to_be32(n)  bswap32(n)
#  define be16_to_cpu(n)  bswap16(n)
#  define be32_to_cpu(n)  bswap32(n)

/* little endian to little endian:  no-op  */
#  define cpu_to_le16(n)  n
#  define cpu_to_le32(n)  n
#  define le16_to_cpu(n)  n
#  define le32_to_cpu(n)  n

#endif /* BYTE_ORDER != BIG_ENDIAN */

#endif /* _ENDIANNESS_H_ */
