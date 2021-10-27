#
# Copyright 2019 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mp_imdt_uevk_2g.o
obj-y += mmc.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
endif
