#pragma once
int fastboot_emmc_get_emmc_info(int dev, ulong *capacity, 
    ulong *version, ulong *manufacturer_id, ulong *pre_eol_info);