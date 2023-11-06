// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022-2023 IMD Technologies
 */

#ifndef PICO_HW_VER_H
#define PICO_HW_VER_H

#include <stdbool.h>

/**
 * @brief Initialises the HW version inputs and caches the version number
 */
void hw_ver_init(void);

/**
 * @brief Returns the hardware version number
 * @return State of the two HW_VER GPIO inputs
 */
int hw_ver_get(void);

#endif
