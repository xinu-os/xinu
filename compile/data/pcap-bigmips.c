/**
 * @file     pcap-bigmips.c
 *
 * This standalone UNIX utility is meant to run on a little-endian
 *  (e.g. Pentium) workstation to convert PCAP network capture files
 *  with little-endian metadata into big-endian metadata.
 * The PCAP packet data payloads should remain in network order
 *  (big-endian) both before and after this conversion.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned short ushort;

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

#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4
#define PCAP_MAGIC         0xA1B2C3D4

#define hs2net(x) (unsigned) ((((x)>>8) &0xff) | (((x) & 0xff)<<8))
#define net2hs(x) hs2net(x)
#define hl2net(x)   ((((x)& 0xff)<<24) | (((x)>>24) & 0xff) | \
    (((x) & 0xff0000)>>8) | (((x) & 0xff00)<<8))
#define net2hl(x) hl2net(x)


/**
 * Rewrites files with little-endian PCAP metadata to be big-endian.
 */
void convertPcapFile(const char *filename)
{
    FILE *stream = NULL;
    size_t size = 0;
    int records = 0;
    struct pcap_file_header phdr, bigendhdr;
    struct pcap_pkthdr ppkt, bigendpkt;


    stream = fopen(filename, "r+");

    if (NULL == stream)
    {
        fprintf(stderr, "Error: could not open %s\n", filename);
        return;
    }

    size = fread(&phdr, sizeof(phdr), 1, stream);
    if (size != 1)
    {
        fprintf(stderr, "Error: could not read 1 PCAP header %d\n", size);
        fclose(stream);
        return;
    }

    if (PCAP_MAGIC != phdr.magic)
    {

        fprintf(stderr,
                "Error: doesn't seem to be a PCAP file, magic = 0x%08X\n",
                phdr.magic);
        fclose(stream);
        return;
    }

    if ((PCAP_VERSION_MAJOR != phdr.version_major) ||
        (PCAP_VERSION_MINOR != phdr.version_minor))
    {
        fprintf(stderr,
                "Error: don't like PCAP version %d:%d\n",
                phdr.version_major, phdr.version_minor);
        fclose(stream);
        return;
    }


    bigendhdr.magic = hl2net(phdr.magic);
    bigendhdr.version_major = hs2net(phdr.version_major);
    bigendhdr.version_minor = hs2net(phdr.version_minor);
    bigendhdr.thiszone = hl2net(phdr.thiszone);
    bigendhdr.sigfigs = hl2net(phdr.sigfigs);
    bigendhdr.snaplen = hl2net(phdr.snaplen);
    bigendhdr.linktype = hl2net(phdr.linktype);

    fseek(stream, 0, SEEK_SET);
    size = fwrite(&bigendhdr, sizeof(bigendhdr), 1, stream);
    if (size != 1)
    {
        fprintf(stderr, "Error: could not write PCAP header, %d\n", size);
        fclose(stream);
        return;
    }

    while (1)
    {

        size = fread(&ppkt, sizeof(ppkt), 1, stream);
        if (size != 1)
        {
            break;
        }

        bigendpkt.sec = hl2net(ppkt.sec);
        bigendpkt.usec = hl2net(ppkt.usec);
        bigendpkt.caplen = hl2net(ppkt.caplen);
        bigendpkt.len = hl2net(ppkt.len);

        fseek(stream, -sizeof(bigendpkt), SEEK_CUR);
        size = fwrite(&bigendpkt, sizeof(bigendpkt), 1, stream);
        if (size != 1)
        {
            fprintf(stderr, "Error: could not write PCAP metadata, %d\n",
                    size);
            fclose(stream);
            return;
        }
        records++;

        if (fseek(stream, ppkt.len, SEEK_CUR))
        {
            fprintf(stderr,
                    "Error: could not seek past packet length %d\n",
                    ppkt.len);
            fclose(stream);
            return;
        }
    }

    fclose(stream);
    printf("%s: Processed %d PCAP records.\n", filename, records);
    return;
}

int main(int argc, char **argv)
{
    int i = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(-1);
    }
    for (i = 1; i < argc; i++)
        convertPcapFile(argv[i]);

    return 0;
}
