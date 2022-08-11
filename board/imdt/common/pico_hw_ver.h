// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022 IMD Technologies
 */

#ifndef PICO_HW_VER_H
#define PICO_HW_VER_H

#include <stdbool.h>

enum pico_hw_version {
	IMDT_PICO_V2 = 0,
	IMDT_PICO_V3,
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

/**
 * @brief Tests to see if this is a Pico v1/v2 board
 */
bool hw_ver_is_pico_v2(void);

/**
 * @brief Tests to see if this is a Pico v3 board
 */
bool hw_ver_is_pico_v3(void);

#endif
