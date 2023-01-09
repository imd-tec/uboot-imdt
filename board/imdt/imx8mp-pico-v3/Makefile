#
# Copyright 2022 IMD Technologies
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mp_imdt_pico_v3.o
obj-y += mmc.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
ifdef CONFIG_TARGET_IMX8MP_IMDT_PICO_V3_0004
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_1g.o
else ifdef CONFIG_TARGET_IMX8MP_IMDT_PICO_V3_0005
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_4g.o
else
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
endif
endif
