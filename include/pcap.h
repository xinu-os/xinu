/*
 * @file pcap.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the Computer Systems
 *  Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef _PCAP_H_
#define _PCAP_H_

#include <stddef.h>

#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4
#define PCAP_MAGIC         0xA1B2C3D4

#define PCAP_ERRBUF_SIZE 256

#define endswap(x)   ((((x)& 0xff)<<24) | (((x)>>24) & 0xff) | \
    (((x) & 0xff0000)>>8) | (((x) & 0xff00)<<8))

/**
 *
 */
struct bpf_insn
{
    ushort code;
    uchar jt;
    uchar jf;
    uint k;
};

/**
 *
 */
struct bpf_program
{
    uint bf_len;
    struct bpf_insn *bf_insns;
};

/**
 *  PCAP file header 
 */
struct pcap_file_header
{
    uint magic;
    ushort version_major;
    ushort version_minor;
    uint thiszone;
    uint sigfigs;
    uint snaplen;
    uint linktype;
};

/**
 * PCAP packet header
 */
struct pcap_pkthdr
{
    uint sec;
    uint usec;
    uint caplen;
    uint len;
};

/* Structure typedefs */
typedef struct pcap_if pcap_if_t;
typedef struct pcap_addr pcap_addr_t;
typedef struct pcap pcap_t;

/**
 * Item in a list of interfaces.
 */
struct pcap_if
{
    struct pcap_if *next;
    char *name;                    /**< name to hand to "pcap_open_live()" */
    char *description;             /**< textual description or NULL        */
    struct pcap_addr *addresses;
    uint flags;                    /**< PCAP_IF_ interface flags           */
};

/**
 * Representation of an interface address.
 */
struct pcap_addr
{
    struct pcap_addr *next;
    struct netaddr *addr;          /**< address                            */
    struct netaddr *netmask;       /**< netmask for that address           */
    struct netaddr *broadaddr;     /**< broadcast address for that address */
    struct netaddr *dstaddr;       /**< P2P destination address for addr   */
};

/**
 * PCAP statistics.
 */
struct pcap_stat
{
    /* TODO */
};

/** 
 * Used when reading a savefile.
 */
struct pcap_sf
{
    /* TODO */
};

/**
 * Used when doing a live capture.
 */
struct pcap_md
{
    /* TODO */
    struct pcap_stat stat;
     /*XXX*/ int use_bpf;          /**< using kernel filter                */
    uint TotPkts;                  /**< can't oflow for 79 hrs on ether    */
    uint TotAccepted;              /**< count accepted by filter           */
    uint TotDrops;                 /**< count of dropped packets           */
    int TotMissed;                 /**< missed by i/f during this run      */
    int OrigMissed;                /**< missed by i/f before this run      */
    int *device;                   /**< device name                        */
    int timeout;                   /**< timeout for buffering              */
    int must_clear;                /**< stuff we must clear when we close  */
    struct pcap *next;             /**< open pcaps need clear on close     */
//#ifdef linux
//    int sock_packet;    /* using Linux 2.0 compatible interface */
//    int cooked;     /* using SOCK_DGRAM rather than SOCK_RAW */
//    int ifindex;    /* interface index of device we're bound to */
//    int lo_ifindex; /* interface index of the loopback device */
//    u_int   packets_read;   /* count of packets read with recvfrom() */
//    bpf_u_int32 oldmode;    /* mode to restore when turning monitor off */
//    u_int   tp_version; /* version of tpacket_hdr for mmaped ring */
//    u_int   tp_hdrlen;  /* hdrlen of tpacket_hdr for mmaped ring */
//#endif /* linux */

//#ifdef HAVE_ZEROCOPY_BPF
    /*
     * Zero-copy read buffer -- for zero-copy BPF.  'buffer' above will
     * alternative between these two actual mmap'd buffers as required.
     * As there is a header on the front size of the mmap'd buffer, only
     * some of the buffer is exposed to libpcap as a whole via bufsize;
     * zbufsize is the true size.  zbuffer tracks the current zbuf
     * assocated with buffer so that it can be used to decide which the
     * next buffer to read will be.
     */
//       u_char *zbuf1, *zbuf2, *zbuffer;
//       u_int zbufsize;
//       u_int zerocopy;
//       u_int interrupted;
//       struct timespec firstsel;
    /*
     * If there's currently a buffer being actively processed, then it is
     * referenced here; 'buffer' is also pointed at it, but offset by the
     * size of the header.
     */
//       struct bpf_zbuf_header *bzh;
//#endif /* HAVE_ZEROCOPY_BPF */
};

/**
 * Structure for PCAP options.
 */
struct pcap_opt
{
    int buffer_size;
    char *source;
    int promisc;
    int rfmon;
};

/**
 * Direction of packets
 */
typedef enum
{
    PCAP_D_INOUT = 0,
    PCAP_D_IN,
    PCAP_D_OUT
} pcap_direction_t;

/* Function typedefs */
typedef void (*pcap_handler) (uchar *, const struct pcap_pkthdr *,
                              const uchar *);
typedef int (*activate_op_t) (pcap_t *);
typedef int (*can_set_rfmon_op_t) (pcap_t *);
typedef int (*read_op_t) (pcap_t *, int cnt, pcap_handler, uchar *);
typedef int (*inject_op_t) (pcap_t *, const void *, uint);
typedef int (*setfilter_op_t) (pcap_t *, struct bpf_program *);
typedef int (*setdirection_op_t) (pcap_t *, pcap_direction_t);
typedef int (*set_datalink_op_t) (pcap_t *, int);
typedef int (*getnonblock_op_t) (pcap_t *, char *);
typedef int (*setnonblock_op_t) (pcap_t *, int, char *);
typedef int (*stats_op_t) (pcap_t *, struct pcap_stat *);
typedef void (*cleanup_op_t) (pcap_t *);

/**
 * Control block for an open packet capture.
 */
struct pcap
{
    int fd;
    int selectable_fd;
    int send_fd;

    int snapshot;
    int linktype;                  /**< Network linktype                   */
    int linktype_ext;              /**< Extended info in linktype in  file */
    int tzoff;                     /**< timezone offset                    */
    int offset;                    /**< offset for proper alignment        */
    int activated;                 /**< true if capture is really started  */
    int oldstyle;                  /**< if opening with pcap_open_live()   */

    int break_loop;                /**< flag set break packet-read loop    */

    struct pcap_sf sf;
    struct pcap_md md;
    struct pcap_opt opt;

    /*
     * Read buffer.
     */
    int bufsize;
    uchar *buffer;
    uchar *bp;
    int cc;

    /*
     * Place holder for pcap_next().
     */
    uchar *pkt;

    /* We're accepting only packets in this direction/these directions. */
    pcap_direction_t direction;

    /*
     * Methods.
     */
    activate_op_t activate_op;
    can_set_rfmon_op_t can_set_rfmon_op;
    read_op_t read_op;
    inject_op_t inject_op;
    setfilter_op_t setfilter_op;
    setdirection_op_t setdirection_op;
    set_datalink_op_t set_datalink_op;
    getnonblock_op_t getnonblock_op;
    setnonblock_op_t setnonblock_op;
    stats_op_t stats_op;
    cleanup_op_t cleanup_op;

    /*
     * Placeholder for filter code if bpf not in kernel.
     */
    struct bpf_program fcode;

    char errbuf[PCAP_ERRBUF_SIZE + 1];
    int dlt_count;
    uint *dlt_list;

    struct pcap_pkthdr pcap_header;  /**< Needed for pcap_next_ex()        */
};

#define PCAP_ERROR                -1 /**< generic error code               */
#define PCAP_ERROR_NO_SUCH_DEVICE -2 /**< device does not exist            */
#define PCAP_ERROR_NOT_ACTIVATED  -3 /**< capture needs to be activated    */
#define PCAP_ERROR_ACTIVATED      -4 /**< can't do action while active     */
#define PCAP_ERROR_PERM_DENIED    -8 /**< permissions denied               */

/* Function prototypes */
int pcap_activate(pcap_t * p);
void pcap_cleanup_live_common(pcap_t * p);
pcap_t *pcap_create(const char *device, char *errbuf);
int pcap_findalldevs(pcap_if_t ** alldevsp, char *errbuf);
void pcap_freealldevs(pcap_if_t ** alldevsp);
void pcap_freecode(struct bpf_program *program);
char *pcap_lookupdev(char *errbuf);
pcap_t *pcap_open_live(const char *device, int snaplen, int promisc,
                       int to_ms, char *errbuf);
int pcap_set_buffer_size(pcap_t * p, int buffer_size);
int pcap_set_promisc(pcap_t * p, int promisc);
int pcap_set_rfmon(pcap_t * p, int rfmon);
int pcap_set_snaplen(pcap_t * p, int snaplen);
int pcap_set_timeout(pcap_t * p, int timeout_ms);

#endif                          /* _PCAP_H_ */
