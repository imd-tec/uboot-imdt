
#include <common.h>
#include "mmc.h"
#include <malloc.h>
#include <memalign.h>
#define EXT_CSD_PRE_EOL_INFO 267
int fastboot_emmc_get_emmc_info(int dev, ulong *capacity, 
    ulong *version, ulong * manufacturer_id, ulong *pre_eol_info)
{
    struct mmc *mmc;
    *capacity = 0;
    *version = 0;
    *manufacturer_id = 0;
    *pre_eol_info = 0;

    
    mmc = find_mmc_device(dev);
    if (!mmc) {
        printf("fastboot_emmc_get_emmc_info: no mmc device at slot %x\n", dev);
        return -ENODEV;
    }
    mmc_init(mmc);
    *capacity = mmc->capacity;
    *version = mmc->version;
    *manufacturer_id = mmc->cid[0] >> 24;
    // Read Pre Over Life Information and Health Status from EXT_CSD
    if (!IS_SD(mmc) && mmc->version >= MMC_VERSION_4_41) {
        u8 *ext_csd = memalign(ARCH_DMA_MINALIGN, MMC_MAX_BLOCK_LEN);
        int ret;
        if (!ext_csd)
            return -ENOMEM;
        ret = mmc_send_ext_csd(mmc, ext_csd);
        if (ret) {
            free(ext_csd);
            return ret;
        }
        *pre_eol_info = ext_csd[EXT_CSD_PRE_EOL_INFO];
        free(ext_csd);
    }
    mmc_deinit(mmc);
    return 0;
}