#include <segment.h>
#include <memory.h>

extern struct segdesc gdt[NSEGS];
extern struct segdescreg gdtr;

/**
 * This will insert a segment with the passed values into the descriptor
 * table entry.
 * @param index index of descriptor to write to
 * @param base  base address of segment
 * @param length length of segment
 * @param properties other properties segment has
 */
void insertseg(int index, int base, int limit, int properties)
{
	struct segdesc *seg;

	if (!(0 <= index && index < NSEGS))
	{
		return;
	}

	seg = &gdt[index];

	// base split hi/mid/lo;
	seg->lobase = base & 0xffff;
	seg->midbase = (base >> 16) & 0xff;
	seg->hibase = (base >> 24) & 0xff;

	// limit split hi/lo;
	/* round up to nearest 4k */
	if (limit != 0xffffffff)
	{
		limit = (limit + 4095) & ~4095;
	}
	seg->lolimit = (limit >> 12) & 0xffff;
	seg->hilimit = (limit >> 28) & 0xf;

	seg->type = properties & 0xf;
	seg->dataseg = (properties >> SEG_DSEG_SHIFT) & 0x1;
	seg->dpl = (properties >> SEG_DPL_SHIFT) & 0x3;
	seg->present = (properties >> SEG_P_SHIFT) & 0x1;

	seg->avl = (properties >> SEG_AVL_SHIFT) & 0x1;
	seg->mbz = 0;
	seg->db = (properties >> SEG_DB_SHIFT) & 0x1;
	seg->gran = (properties >> SEG_GRAN_SHIFT) & 0x1;
}

void initialsegs(void)
{
	/* some segment setup */
	insertseg(0, 0x00000000, 0x00000000, 0x00000000);
	insertseg(1, 0x00000000, 0xffffffff, SEG_CODE_KERNEL);
	insertseg(2, 0x00000000, 0xffffffff, SEG_DATA_KERNEL);
	insertseg(3, 0x00000000, 0xffffffff, SEG_DATA_KERNEL);
}

