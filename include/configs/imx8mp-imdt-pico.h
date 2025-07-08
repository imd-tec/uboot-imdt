/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

 #ifndef __IMX8MP_IMDT_PICO_H
 #define __IMX8MP_IMDT_PICO_H
 
 #include <linux/sizes.h>
 #include <linux/stringify.h>
 #include <asm/arch/imx-regs.h>
 #include "imx_env.h"

  /* Link Definitions */
 #define CFG_SYS_INIT_RAM_ADDR	0x40000000
 #define CFG_SYS_INIT_RAM_SIZE	0x80000
 
 
 /* Totally 2GB DDR */
 #define CFG_SYS_SDRAM_BASE		0x40000000
 #define PHYS_SDRAM				0x40000000
 #define PHYS_SDRAM_SIZE		0x80000000	/* 2 GB */


 #define CFG_MXC_UART_BASE		UART2_BASE_ADDR
  
  #define PHY_ANEG_TIMEOUT 20000

 #define CFG_SYS_FSL_USDHC_NUM	2
 
 
 #define CFG_SYS_UBOOT_BASE	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)
 
 #if defined(CONFIG_CMD_NET)
 #define CFG_FEC_MXC_PHYADDR          1
 #endif
 
 #ifdef CONFIG_DISTRO_DEFAULTS
 #define BOOT_TARGET_DEVICES(func) \
	 func(USB, usb, 0) \
	 func(MMC, mmc, 1) \
	 func(MMC, mmc, 2)
 
 #include <config_distro_bootcmd.h>
 #else
 #define BOOTENV
 #endif
 

 #define CFG_MFG_ENV_SETTINGS \
	 CFG_MFG_ENV_SETTINGS_DEFAULT \
	 "initrd_addr=0x43800000\0" \
	 "initrd_high=0xffffffffffffffff\0" \
	 "emmc_dev=2\0"\
	 "sd_dev=1\0"
 
 /* Initial environment variables */
 #define CFG_EXTRA_ENV_SETTINGS		\
	 CFG_MFG_ENV_SETTINGS \
	 BOOTENV \
	 "prepare_mcore=setenv mcore_clk clk-imx8mp.mcore_booted;\0" \
	 "scriptaddr=0x43500000\0" \
	 "kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	 "image=Image\0" \
	 "splashimage=0x50000000\0" \
	 "console=ttymxc1,115200\0" \
	 "fdt_addr_r=0x43000000\0"			\
	 "fdt_addr=0x43000000\0"			\
	 "boot_fdt=try\0" \
	 "fdt_high=0xffffffffffffffff\0"		\
	 "boot_fit=no\0" \
	 "fdtfile=" __stringify(CONFIG_DEFAULT_FDT_FILE) "\0" \
	 "apply_overlays="__stringify(CONFIG_ADD_OVERLAYS)"\0" \
	 "board_variant=A\0"  \
	 "bootm_size=0x10000000\0" \
	 "mmcdev=" __stringify(CONFIG_SYS_MMC_ENV_DEV) "\0" \
	 "mmcpart=1\0" \
	 "mmcroot=/dev/mmcblk1p2 rootwait rw\0" \
	 "mmcautodetect=yes\0" \
	 "mmcargs=setenv bootargs ${jh_clk} ${mcore_clk} console=${console} root=${mmcroot}\0 " \
	 "loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	 "loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdtfile}\0" \
	 "do_overlays=" \
		"fdt addr ${fdt_addr_r}; fdt resize 0x10000; " \
		"if test \"${board_name}\" = \"E\"; then " \
			"echo Applying pico-e overlay; " \
			"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} imx8mp-imdt-pico-e.dtbo; " \
			"fdt apply ${loadaddr}; " \
		"fi; " \
		    "if test \"${apply_overlays}\" != \"\\\\\\\"\\\\\\\"\"; then " \
			"for overlay in ${apply_overlays}; do " \
				"echo Applying overlay: $overlay; " \
				"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} $overlay; " \
				"fdt apply ${loadaddr}; " \
			"done; " \
		"else " \
			"echo No overlays to apply.; " \
		"fi;\0" \
	 "mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if run loadfdt; then " \
			"run do_overlays; " \
			"booti ${loadaddr} - ${fdt_addr_r}; " \
		"else " \
			"echo WARN: Cannot load the DT; " \
		"fi;\0" \
	 "bsp_bootcmd=echo Running BSP bootcmd ...; mmc dev ${mmcdev}; " \
		"if mmc rescan; then " \
			"if run loadimage; then " \
				"run mmcboot; " \
			"fi; " \
		"fi;\0" 
 #endif
 