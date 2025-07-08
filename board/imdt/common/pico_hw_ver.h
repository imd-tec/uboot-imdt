// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022-2023 IMD Technologies
 */

#ifndef PICO_HW_VER_H
#define PICO_HW_VER_H

#include <stdbool.h>
#include <linux/types.h>


enum pico_hw_version {
	IMDT_PICO_E = 2,
	IMDT_PICO_EM = 3
};

enum pico_mem_size {
	IMDT_PICO_MEM_2GB = 1,
	IMDT_PICO_MEM_4GB = 2,
	IMDT_PICO_MEM_6GB = 3
};

enum pico_mem_speed {
 IMDT_PICO_MEM_SPEED_1866 = 0,
 IMDT_PICO_MEM_SPEED_2133 = 1,
};

/**
 * @brief Initialises the HW version inputs and caches the version number
 */
void hw_ver_init(void);

/**
 * @brief Returns the hardware version number
 * @return State of the two HW_VER GPIO inputs
 */
int hw_ver_get(void);
/* Disables the IR Led*/
void disable_ir_led(void);

/**
 * @brief Tests to see if this is a Pico e board
 */
bool hw_ver_is_pico_e(void);

/**
 * @brief Tests to see if this is a Pico e board
 */
bool hw_ver_is_pico_em(void);

int mem_cfg_get(void);
int mem_speed_get(void);

#endif
