#
# Copyright 2023 IMD Technologies
#
# SPDX-License-Identifier:      GPL-2.0+
#
#Common file for Pico-E and Pico-EM boards
obj-y += imx8mp_imdt_pico.o
obj-y += ../../freescale/common/
ifdef CONFIG_SPL_BUILD
obj-y += spl.o
ifdef CONFIG_TARGET_IMX8MP_IMDT_PICO
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
endif
endif
