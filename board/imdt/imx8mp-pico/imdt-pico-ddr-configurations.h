#pragma once
/** @brief
 * Convert memory configuration index to size in bytes for the IMDT Pico board.
 * @param index Memory configuration index.
 * @return Size in bytes corresponding to the given memory configuration index or 0 if the index is invalid.
 */
u64 get_ddr_size_for_IMDT_Pico_hw_config_index(u8 index);