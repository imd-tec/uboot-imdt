// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022-2023 IMD Technologies
 */

#include "pico_hw_ver.h"

#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/gpio.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm-generic/gpio.h>

#define HW_VER0_GPIO			IMX_GPIO_NR(4, 22)
#define HW_VER1_GPIO			IMX_GPIO_NR(4, 23)

int hw_ver_gpios[] = {
	HW_VER0_GPIO,
	HW_VER1_GPIO,
	-1
};

#define HW_VER_PAD_CTRL		(PAD_CTL_DSE1 | PAD_CTL_PE)
#define HW_VER_NO_PULL_PAD_CTRL	(PAD_CTL_DSE1)

static iomux_v3_cfg_t const hw_ver_pads[] = {
	MX8MP_PAD_SAI2_RXC__GPIO4_IO22 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
	MX8MP_PAD_SAI2_RXD0__GPIO4_IO23 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
};

static iomux_v3_cfg_t const hw_ver_no_pull_pads[] = {
	MX8MP_PAD_SAI2_RXC__GPIO4_IO22 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
	MX8MP_PAD_SAI2_RXD0__GPIO4_IO23 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
};

static int s_hw_ver;

/**
 *
 */
void hw_ver_init(void) {

	imx_iomux_v3_setup_multiple_pads(hw_ver_pads, ARRAY_SIZE(hw_ver_pads));

	gpio_request(HW_VER0_GPIO, "hw_ver0");
	gpio_request(HW_VER1_GPIO, "hw_ver1");

	gpio_direction_input(HW_VER0_GPIO);
	gpio_direction_input(HW_VER1_GPIO);

	s_hw_ver = gpio_get_values_as_int(hw_ver_gpios);

	imx_iomux_v3_setup_multiple_pads(hw_ver_no_pull_pads, ARRAY_SIZE(hw_ver_no_pull_pads));

	gpio_free(HW_VER0_GPIO);
	gpio_free(HW_VER1_GPIO);
}

/**
 *
 */
int hw_ver_get(void) {
	return s_hw_ver;
}

/**
 *
 */
bool hw_ver_is_pico_e(void) {
	return (s_hw_ver == IMDT_PICO_E);
}

/**
 *
 */
bool hw_ver_is_pico_v2(void) {
	return (s_hw_ver == IMDT_PICO_V2);
}

/**
 *
 */
bool hw_ver_is_pico_v3(void) {
	return (s_hw_ver == IMDT_PICO_V3);
}
