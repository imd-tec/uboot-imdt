// SPDX-License-Identifier: GPL-2.0-or-later
/*
* Copyright 2018-2019, 2021 NXP
*
*/

#include <common.h>
#include <hang.h>
#include <init.h>
#include <log.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/boot_mode.h>
#include <power/pmic.h>

#include <power/pca9450.h>
#include <asm/arch/clock.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <fsl_esdhc_imx.h>
#include <mmc.h>
#include <asm/arch/ddr.h>
#include <asm/sections.h>
#include "../common/pico_hw_ver.h"
#include "lpddr4_timing.h"

DECLARE_GLOBAL_DATA_PTR;

int spl_board_boot_device(enum boot_device boot_dev_spl)
{
#ifdef CONFIG_SPL_BOOTROM_SUPPORT
	return BOOT_DEVICE_BOOTROM;
#else
	switch (boot_dev_spl) {
	case SD1_BOOT:
	case MMC1_BOOT:
	case SD2_BOOT:
	case MMC2_BOOT:
		return BOOT_DEVICE_MMC1;
	case SD3_BOOT:
	case MMC3_BOOT:
		return BOOT_DEVICE_MMC2;
	case QSPI_BOOT:
		return BOOT_DEVICE_NOR;
	case NAND_BOOT:
		return BOOT_DEVICE_NAND;
	case USB_BOOT:
		return BOOT_DEVICE_BOARD;
	default:
		return BOOT_DEVICE_NONE;
	}
#endif
}

static struct dram_timing_info *dram_timing_info[5] = {
	NULL,
	&imdt_pico_dram_timing_2g,	/* 2048 MiB */
	&imdt_pico_dram_timing_4g,	/* 4096 MiB */
	&imdt_pico_dram_timing_6g,	/* 6144 MiB */
	NULL,
};

void spl_dram_init(void)
{
	const u16 size[] = { 1024, 2048, 4096, 6144, 8192 };
	volatile u8 memcfg = mem_cfg_get();
	int i;

	printf("DDR:   %d MiB [0x%x]\n", size[memcfg], memcfg);

	if (!dram_timing_info[memcfg]) {
		printf("Unsupported DRAM strapping, trying lowest supported. MEMCFG=0x%x\n",
		       memcfg);
		for (i = 0; i < ARRAY_SIZE(dram_timing_info); i++)
			if (dram_timing_info[i])	/* Configuration found */
				break;
	}

	ddr_init(dram_timing_info[memcfg]);
}

void spl_board_init(void)
{
	arch_misc_init();

	/*
	* Set GIC clock to 500Mhz for OD VDD_SOC. Kernel driver does
	* not allow to change it. Should set the clock after PMIC
	* setting done. Default is 400Mhz (system_pll1_800m with div = 2)
	* set by ROM for ND VDD_SOC
	*/
#if defined(CONFIG_IMX8M_LPDDR4) && !defined(CONFIG_IMX8M_VDD_SOC_850MV)
	clock_enable(CCGR_GIC, 0);
	clock_set_target_val(GIC_CLK_ROOT, CLK_ROOT_ON | CLK_ROOT_SOURCE_SEL(5));
	clock_enable(CCGR_GIC, 1);

	puts("Normal Boot\n");
#endif
}

#if CONFIG_IS_ENABLED(DM_PMIC_PCA9450)
int power_init_board(void)
{
	struct udevice *dev;
	int ret;

	ret = pmic_get("pmic@25", &dev);
	if (ret == -ENODEV) {
		puts("No pmic@25\n");
		return 0;
	}
	if (ret < 0)
		return ret;

	/* BUCKxOUT_DVS0/1 control BUCK123 output */
	pmic_reg_write(dev, PCA9450_BUCK123_DVS, 0x29);

#ifdef CONFIG_IMX8M_LPDDR4
	/*
	* Increase VDD_SOC to typical value 0.95V before first
	* DRAM access, set DVS1 to 0.85V for suspend.
	* Enable DVS control through PMIC_STBY_REQ and
	* set B1_ENMODE=1 (ON by PMIC_ON_REQ=H)
	*/
	if (CONFIG_IS_ENABLED(IMX8M_VDD_SOC_850MV))
		pmic_reg_write(dev, PCA9450_BUCK1OUT_DVS0, 0x14);
	else
		pmic_reg_write(dev, PCA9450_BUCK1OUT_DVS0, 0x1C);

	pmic_reg_write(dev, PCA9450_BUCK1OUT_DVS1, 0x14);
	pmic_reg_write(dev, PCA9450_BUCK1CTRL, 0x59);

	/*
	* Kernel uses OD/OD freq for SOC.
	* To avoid timing risk from SOC to ARM,increase VDD_ARM to OD
	* voltage 0.95V.
	*/

	pmic_reg_write(dev, PCA9450_BUCK2OUT_DVS0, 0x1C);
#elif defined(CONFIG_IMX8M_DDR4)
	/* DDR4 runs at 3200MTS, uses default ND 0.85v for VDD_SOC and VDD_ARM */
	pmic_reg_write(dev, PCA9450_BUCK1CTRL, 0x59);

	/* Set NVCC_DRAM to 1.2v for DDR4 */
	pmic_reg_write(dev, PCA9450_BUCK6OUT, 0x18);
#endif

	return 0;
}
#endif

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	/* Just empty function now - can't decide what to choose */
	debug("%s: %s\n", __func__, name);

	return 0;
}
#endif
void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret;

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	arch_cpu_init();

	board_early_init_f();

	timer_init();

	ret = spl_early_init();
	if (ret) {
		debug("spl_early_init() failed: %d\n", ret);
		hang();
	}

	ret = uclass_get_device_by_name(UCLASS_CLK,
					"clock-controller@30380000",
					&dev);
	if (ret < 0) {
		printf("Failed to find clock node. Check device tree\n");
		hang();
	}

	preloader_console_init();

	enable_tzc380();

	power_init_board();

	 // Initialize the hardware version pins
	hw_ver_init();
	int hw_ver = hw_ver_get();
	int mem_cfg = mem_cfg_get();
	int mem_speed = mem_speed_get();
	 printf("HW: %s, MEM_CFG: %d, MEM_SPEED: %d\n",
		(hw_ver == IMDT_PICO_E) ? "Pico-E" :
		(hw_ver == IMDT_PICO_EM) ? "Pico-EM" : "Unknown",
		mem_cfg, mem_speed);
	#ifndef CONFIG_IMDT_PICO_AUTO_DDR_SIZE_DETECTION
		printf("Warning: Memory size override active with index %d\n",CONFIG_IMDT_PICO_FORCE_DDR_SIZE_INDEX );
	#endif
	/* DDR initialization */
	spl_dram_init();

	board_init_r(NULL, 0);
}
