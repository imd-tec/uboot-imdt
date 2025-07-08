// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022-2023 IMD Technologies
 */

#include "pico_hw_ver.h"
#include <asm/arch/ddr.h>
#include <asm/io.h>
#include <stdbool.h>
#include <asm/arch-imx8m/imx-regs.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/gpio.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm-generic/gpio.h>

#define PICO_HW_VER_0			IMX_GPIO_NR(4, 22)
#define PICO_HW_VER_1			IMX_GPIO_NR(4, 23)

#define PICO_MEM_CFG_0			IMX_GPIO_NR(4, 0)
#define PICO_MEM_CFG_1			IMX_GPIO_NR(4, 1)
#define PICO_MEM_CFG_2			IMX_GPIO_NR(1, 1)

#define PICO_MEM_SPEED_0		IMX_GPIO_NR(1, 11)


int hw_ver_gpios[] = {
	PICO_HW_VER_0,
	PICO_HW_VER_1,
	-1
};

int mem_cfg_gpios[] = {
	PICO_MEM_CFG_0,
	PICO_MEM_CFG_1,
	PICO_MEM_CFG_2,
	-1
};

int mem_speed_gpios[] = {
 PICO_MEM_SPEED_0,
 -1
};



#define HW_VER_PAD_CTRL		(PAD_CTL_DSE1 | PAD_CTL_PE)
#define HW_VER_NO_PULL_PAD_CTRL	(PAD_CTL_DSE1)
#define IR_LED_ENABLE_GPIO IMX_GPIO_NR(2, 19)

static iomux_v3_cfg_t const hw_ver_pads[] = {
	MX8MP_PAD_SAI2_RXC__GPIO4_IO22 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
	MX8MP_PAD_SAI2_RXD0__GPIO4_IO23 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
	MX8MP_PAD_SAI1_RXFS__GPIO4_IO00 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
	MX8MP_PAD_SAI1_RXC__GPIO4_IO01 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
	MX8MP_PAD_GPIO1_IO01__GPIO1_IO01 | MUX_PAD_CTRL(HW_VER_PAD_CTRL),
	MX8MP_PAD_GPIO1_IO11__GPIO1_IO11 | MUX_PAD_CTRL(HW_VER_PAD_CTRL)
};

static iomux_v3_cfg_t const hw_ver_no_pull_pads[] = {
	MX8MP_PAD_SAI2_RXC__GPIO4_IO22 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
	MX8MP_PAD_SAI2_RXD0__GPIO4_IO23 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
	MX8MP_PAD_SAI1_RXFS__GPIO4_IO00 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
	MX8MP_PAD_SAI1_RXC__GPIO4_IO01 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
	MX8MP_PAD_GPIO1_IO01__GPIO1_IO01 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL),
	MX8MP_PAD_GPIO1_IO11__GPIO1_IO11 | MUX_PAD_CTRL(HW_VER_NO_PULL_PAD_CTRL)
};


#define GPIO_GDIR     0x04
#define GPIO_PSR      0x08

static void gpio_set_input(uint32_t gpio_base, uint32_t pin)
{
    volatile uint32_t *gdir = (volatile uint32_t *)(uintptr_t)(gpio_base + GPIO_GDIR);
    uint32_t val = readl(gdir);
    val &= ~(1 << pin);
    writel(val, gdir);
}

static int gpio_read_input(uint32_t gpio_base, uint32_t pin)
{
    volatile uint32_t *psr = (volatile uint32_t *)(uintptr_t)(gpio_base + GPIO_PSR);
    uint32_t val = readl(psr);
    return (val >> pin) & 0x1;
}

static int s_hw_ver, s_mem_cfg, s_mem_speed;

void hw_ver_init(void) {

	imx_iomux_v3_setup_multiple_pads(hw_ver_pads, ARRAY_SIZE(hw_ver_pads));

	// Configure all as inputs
	gpio_set_input(GPIO4_BASE_ADDR, 22);
	gpio_set_input(GPIO4_BASE_ADDR, 23);
	gpio_set_input(GPIO4_BASE_ADDR, 0);
	gpio_set_input(GPIO4_BASE_ADDR, 1);
	gpio_set_input(GPIO1_BASE_ADDR, 1);
	gpio_set_input(GPIO1_BASE_ADDR, 11);

	// Read values directly
	int hw0 = gpio_read_input(GPIO4_BASE_ADDR, 22);
	int hw1 = gpio_read_input(GPIO4_BASE_ADDR, 23);
	s_hw_ver = (hw1 << 1) | hw0;

	int cfg0 = gpio_read_input(GPIO4_BASE_ADDR, 0);
	int cfg1 = gpio_read_input(GPIO4_BASE_ADDR, 1);
	int cfg2 = gpio_read_input(GPIO1_BASE_ADDR, 1);
	s_mem_cfg = (cfg2 << 2) | (cfg1 << 1) | cfg0;

	s_mem_speed = gpio_read_input(GPIO1_BASE_ADDR, 11);

	#ifndef CONFIG_IMDT_PICO_AUTO_DDR_SIZE_DETECTION 
	// Force memory size if configured
		printf("Forcing memory size to be configuration number: %d \n", CONFIG_IMDT_PICO_FORCE_DDR_SIZE_INDEX);
		s_mem_cfg = CONFIG_IMDT_PICO_FORCE_DDR_SIZE_INDEX;
	#endif

	// Disable pull-ups if needed (optional)
	imx_iomux_v3_setup_multiple_pads(hw_ver_no_pull_pads, ARRAY_SIZE(hw_ver_no_pull_pads));
}

/**
 *
 */
int hw_ver_get(void) {
	return s_hw_ver;
}

int mem_cfg_get(void) {
	return  s_mem_cfg;
}

int mem_speed_get(void) {
	return s_mem_speed;
}

void disable_ir_led(void)
{
	#if 1
	// Uncommented as this prevents booting , I think its because the SPL is getting too big
	gpio_request(IR_LED_ENABLE_GPIO, "ir_led_enable");
	gpio_direction_output(IR_LED_ENABLE_GPIO, 0);
	gpio_free(IR_LED_ENABLE_GPIO);
	#endif

}

/**
 *
 */
bool hw_ver_is_pico_em(void) {
	return (s_hw_ver == IMDT_PICO_EM);
}

/**
 *
 */
bool hw_ver_is_pico_e(void) {
	return (s_hw_ver == IMDT_PICO_E);
}

