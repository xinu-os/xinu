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

static inline uint64_t
bswap64(uint64_t n)
{
    return((n & 0xff00000000000000) >> 56)	
	| ((n & 0x00ff000000000000) >> 40)	
	| ((n & 0x0000ff0000000000) >> 24)	
	| ((n & 0x000000ff00000000) >> 8)	
	| ((n & 0x00000000ff000000) << 8)	
	| ((n & 0x0000000000ff0000) << 24)	
	| ((n & 0x000000000000ff00) << 40)	
	| ((n & 0x00000000000000ff) << 56);
}


#if BYTE_ORDER == BIG_ENDIAN

/* Big-endian CPU  */

/* big endian to big endian:  no-op  */
#  define cpu_to_be16(n)  n
#  define cpu_to_be32(n)  n
#  define cpu_to_be64(n)  n
#  define be16_to_cpu(n)  n
#  define be32_to_cpu(n)  n
#  define be64_to_cpu(n)  n

/* big endian to little endian and vice versa  */
#  define cpu_to_le16(n)  bswap16(n)
#  define cpu_to_le32(n)  bswap32(n)
#  define cpu_to_le64(n)  bswap64(n)
#  define le16_to_cpu(n)  bswap16(n)
#  define le32_to_cpu(n)  bswap32(n)
#  define le64_to_cpu(n)  bswap64(n)

#else /* BYTE_ORDER == BIG_ENDIAN */

/* Little-endian CPU  */

/* little endian to big endian and vice versa  */
#  define cpu_to_be16(n)  bswap16(n)
#  define cpu_to_be32(n)  bswap32(n)
#  define cpu_to_be64(n)  bswap64(n)
#  define be16_to_cpu(n)  bswap16(n)
#  define be32_to_cpu(n)  bswap32(n)
#  define be64_to_cpu(n)  bswap64(n)

/* little endian to little endian:  no-op  */
#  define cpu_to_le16(n)  n
#  define cpu_to_le32(n)  n
#  define cpu_to_le64(n)  n
#  define le16_to_cpu(n)  n
#  define le32_to_cpu(n)  n
#  define le64_to_cpu(n)  n    

#endif /* BYTE_ORDER != BIG_ENDIAN */

#endif /* _ENDIANNESS_H_ */
