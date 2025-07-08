/// @brief Convert memory configuration index to size in bytes.
#include "../common/pico_hw_ver.h"
#include <efi_loader.h>
#include <env.h>
#include <errno.h>
#include <linux/sizes.h>
u64 get_ddr_size_for_IMDT_Pico_hw_config_index(u8 mem_cfg)
{
    // Only the stack works at this point
	const u64 index_to_size[] =  {
		1ULL * SZ_1G,  // memcfg == 0	
		2ULL * SZ_1G,  // memcfg == 1
		4ULL * SZ_1G,  // memcfg == 2
		6ULL * SZ_1G,  // memcfg == 3
		8ULL * SZ_1G,  // memcfg == 4 
	};
	if(mem_cfg >= ARRAY_SIZE(index_to_size)) {
		printf("Unknown memcfg: %u\n", mem_cfg);
		return 2ULL * SZ_1G;  // Fallback safe value
	}
	return index_to_size[mem_cfg];
}