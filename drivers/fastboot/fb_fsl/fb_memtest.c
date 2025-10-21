
#include <common.h>
#include <cpu_func.h>
#include <asm/global_data.h>
#include "cyclic.h"
ulong fastboot_mem_test(vu_long *buf, ulong start_addr, ulong end_addr,
			    vu_long pattern, int iteration, int address_step)
{
	vu_long *end;
	vu_long *addr;
	ulong errs = 0;
	ulong incr, length;
	ulong val, readback;
	const int plen = 2 * sizeof(ulong);

	/* Alternate the pattern */
	incr = 1;
	if (iteration & 1) {
		incr = -incr;
		/*
		 * Flip the pattern each time to make lots of zeros and
		 * then, the next time, lots of ones.  We decrement
		 * the "negative" patterns and increment the "positive"
		 * patterns to preserve this feature.
		 */
		if (pattern > (ulong)LONG_MAX)
			pattern = -pattern;	/* complement & increment */
		else
			pattern = ~pattern;
	}
	length = (end_addr - start_addr) / sizeof(ulong);
	end = buf + length;
	printf("\rPattern %0*lX  Writing..."
		"%12s"
		"\b\b\b\b\b\b\b\b\b\b \n",
		plen, pattern, "");
	printf("Expected number of reads/writes: %lu\n", length/address_step);

	for (addr = buf, val = pattern; addr < end; addr+= address_step) {
		schedule();
		*addr = val;
		val += incr;
	}

	puts("Reading... \n");

	for (addr = buf, val = pattern; addr < end; addr+= address_step) {
		schedule();
		readback = *addr;
		if (readback != val) {
			ulong offset = addr - buf;

			printf("\nMem error @ 0x%0*lX: found %0*lX, expected %0*lX\n",
			       plen, start_addr + offset * sizeof(vu_long),
			       plen, readback, plen, val);
			errs++;
			return -1;
		}
		val += incr;
	}

	return errs;
}
