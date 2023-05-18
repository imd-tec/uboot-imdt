#
# Copyright 2023 IMD Technologies
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mp_imdt_pico_e.o
obj-y += mmc.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
ifdef CONFIG_TARGET_IMX8MP_IMDT_PICO_E
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
endif
endif
