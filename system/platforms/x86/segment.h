/* segment.h - DELAY */

#include <stddef.h>

#define	NBPG 4096

/* segment descriptor data */
#define	NSEGS 8

#define SEG_DSEG_SHIFT 4
#define SEG_DPL_SHIFT  5
#define SEG_P_SHIFT    7
#define SEG_AVL_SHIFT  8
#define SEG_DB_SHIFT   9
#define SEG_GRAN_SHIFT 10

#define SEG_TYPE_DATA_RO 0x0000
#define SEG_TYPE_DATA_RW 0x0002
#define SEG_TYPE_CODE_EO 0x0008
#define SEG_TYPE_CODE_ER 0x000a
#define SEG_DSEG_ON      0x0010
#define SEG_DPL_KERN     0x0000
#define SEG_DPL_USER     0x0020
#define SEG_P_ON         0x0080
#define SEG_AVL_ON       0x0100
#define SEG_DB_ON        0x0200
#define SEG_GRAN_ON      0x0400

#define SEG_CODE_KERNEL ( SEG_GRAN_ON | SEG_DB_ON | SEG_P_ON \
                          | SEG_DPL_KERN | SEG_DSEG_ON | SEG_TYPE_CODE_ER )
#define SEG_DATA_KERNEL ( SEG_GRAN_ON | SEG_DB_ON | SEG_P_ON \
                          | SEG_DPL_KERN | SEG_DSEG_ON | SEG_TYPE_DATA_RW )

struct segdesc
{
	ushort lolimit;
	ushort lobase;
	uchar  midbase;
	uchar  type : 4, dataseg : 1, dpl : 2, present : 1;
	uchar  hilimit : 4, avl : 1, mbz : 1, db : 1, gran : 1;
	uchar  hibase;
};

/* Segment Table Register */
struct segdescreg
{
	ushort limit;
	uint   base;
};

void insertseg(int, int, int, int);


#define	SDT_INTG	14	/* Interrupt Gate	*/
/*
 * Delay units are in microseconds.
 */
#define	DELAY(n)                            \
{                                           \
	extern int cpudelay;                    \
	register int i;                         \
	register long N = (((n)<<4)>>cpudelay); \
	for (i = 0; i <= 4; i++)                \
	{                                       \
		N = (((n) << 4) >> cpudelay);       \
		while (--N > 0);                    \
	}                                       \
}

