#pragma once
#include <linux/types.h>
#include <linux/errno.h>
// Copied from mem.c
/* Memory test - returns number of errors, or -1 if interrupted
@param buf - buffer to test
@param start_addr - start address of the buffer
@param end_addr - end address of the buffer
@param pattern - pattern to use in test
@param iteration - number of iterations to perform
@param address_step - step size for address increments
*/
ulong fastboot_mem_test(vu_long *buf, ulong start_addr, ulong end_addr,
			    vu_long pattern, int iteration, int address_step);