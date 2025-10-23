#include <common.h>
#include <cpu_func.h>
#include <asm/global_data.h>
#include "mapmem.h"
#include "fb_emmc.h"
#include "fb_memtest.h"
#include "log.h"
#include "fastboot.h"
/* Processes OEM commands that are specific to IMDT */
void run_imdt_oem_cmd(char *cmd_parameter, char *response)
{
    char reason[FASTBOOT_RESPONSE_LEN];
    memset(reason, '\0', FASTBOOT_RESPONSE_LEN);
    if(!cmd_parameter) {
        pr_err("missing oem command\n");
        fastboot_fail("missing command", response);
        return;
    }
    // Split the command into arguments
    memset(response, '\0', FASTBOOT_RESPONSE_LEN);
    char *args[10];
    int argc = 0;
    char *token = strtok(cmd_parameter, " ");
    while(token != NULL && argc < 10) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    if(argc == 0) {
        fastboot_fail("missing command", response);
        return;
    }
    debug("OEM Command received: %s, with %d args\n", cmd_parameter, argc);
    const char *cmd = args[0];
    // fastboot oem memtest <start_addr> <end_addr> <pattern>
    // I.e fastboot oem memtest 0x50000000 0xB0000000 0x2025DEADBEEF
    if((strcmp(cmd, "memtest") == 0) && argc == 4)
    {
        ulong start = hextoul(args[1], NULL);
        ulong end = hextoul(args[2], NULL);
        ulong pattern = hextoul(args[3], NULL);
        if(start >= end || start % sizeof(pattern) != 0 || end % sizeof(pattern) != 0) {
            fastboot_fail("FAIL: invalid address range", response);
            return;
        }
        vu_long *buf;
        debug("%s:%d: start %#08lx end %#08lx, with size of %#08lx MB\n", __func__, __LINE__,
        start, end, (end - start)/(1024*1024));
        buf = map_sysmem(start, end - start);
        for (int i = 0; i < CONFIG_FASTBOOT_MEMTEST_ITERATIONS; i++) {
            int failures = fastboot_mem_test(buf, start, end, pattern, i, 1);

            if(failures) {
                snprintf(reason, FASTBOOT_RESPONSE_LEN, "MEMTEST FAIL: iteration %d, failures %d", i, failures);
                fastboot_fail(reason, response);
                unmap_sysmem((ulong)buf);
                return;
            }
        }
        unmap_sysmem((ulong)buf);
        snprintf(reason, FASTBOOT_RESPONSE_LEN, "MEMTEST PASS: tested 0x%lx to 0x%lx", start, end);
        debug("%s\n", reason);

        fastboot_okay(reason, response);
        return;
    }
    fastboot_fail("unknown oem command", response);
    return;
}