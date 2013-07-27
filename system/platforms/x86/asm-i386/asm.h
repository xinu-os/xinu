/* asm.h - DELAY */

#define	NBPG    4096

#define	NID     48
#define	NGD     8

#define	IRQBASE 32   /* base ivec for IRQ0 */

struct idt {
	ushort igd_loffset;
	ushort igd_segsel;
	ulong  igd_rsvd    : 5;
	ulong  igd_mbz     : 3;
	ulong  igd_type    : 5;
	ulong  igd_dpl     : 2;
	ulong  igd_present : 1;
	ushort igd_hoffset;
};

#define	IGDT_TASK	 5	/* task gate IDT descriptor		*/
#define	IGDT_INTR	14	/* interrupt gate IDT descriptor	*/
#define	IGDT_TRAPG	15	/* Trap Gate				*/


/* Segment Descriptor */

struct sd {
	ushort sd_lolimit;
	ushort sd_lobase;
	uchar  sd_midbase;
	ulong  sd_perm    : 3;
	ulong  sd_iscode  : 1;
	ulong  sd_isapp   : 1;
	ulong  sd_dpl     : 2;
	ulong  sd_present : 1;
	ulong  sd_hilimit : 4;
	ulong  sd_avl     : 1;
	ulong  sd_mbz     : 1; /* must be '0' */
	ulong  sd_32b     : 1;
	ulong  sd_gran    : 1;
	uchar  sd_hibase;
};

#define	sd_type		sd_perm

/* System Descriptor Types */

#define	SDT_INTG	14	/* Interrupt Gate	*/

/* Segment Table Register */
struct segtr {
	ulong len  : 16;
	ulong addr : 32;
};

/*
 *  Delay units are in microseconds.
 */
#define DELAY(n)                              \
{                                             \
    extern long cpudelay;                     \
    register char i;                          \
    register long N = (((n)<<4) >> cpudelay); \
                                              \
    for ( i = 0; i <= 4; i++ )                \
    {                                         \
        N = (((n) << 4) >> cpudelay);         \
        while (--N > 0) ;                     \
    }                                         \
}
