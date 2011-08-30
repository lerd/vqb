# Makefile - make rules for target/config/versatilepb
#
# modification history
# --------------------
# 01a,12aug11,d_l  created.
#
# DESCRIPTION
# This file contains rules for building VxWorks for the ARM versatilepb
# fited with an ARM926EjS core module.
#
# INCLUDES
#     makeTarget
#*/

CPU		= ARMARCH5
TOOL		= diab
EXTRA_DEFINE	+= -DCPU_926E \
                  -DARMMMU=ARMMMU_926E -DARMCACHE=ARMCACHE_926E

TGT_DIR = $(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.bsp

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

TARGET_DIR	= versatilepb
VENDOR		= ARM Limited
BOARD		= ap926ej-s

RELEASE		+= bootrom.bin

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h and Makefile.
# All definitions for these constants must be identical.
#

ROM_TEXT_ADRS	= 7c00000 # ROM entry address
ROM_SIZE	= 00200000 # number of bytes of ROM space

RAM_LOW_ADRS	=  00100000 # RAM text/data address
RAM_HIGH_ADRS	= 00600000 # RAM text/data address

VMA_START	= 0x$(ROM_TEXT_ADRS)

# Binary version of VxWorks ROM images, suitable for programming
# into Flash using tools provided by ARM.  If other ROM images need to
# be put into Flash, add similar rules here.

bootrom.bin: bootrom
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary bootrom $@

bootrom_res.bin: bootrom_res
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary bootrom_res $@

bootrom_uncmp.bin: bootrom_uncmp
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary bootrom_uncmp $@

vxWorks_rom.bin: vxWorks_rom
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary vxWorks_rom $@

vxWorks.st_rom.bin: vxWorks.st_rom
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary vxWorks.st_rom $@

vxWorks.res_rom.bin: vxWorks.res_rom
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary vxWorks.res_rom $@

vxWorks.res_rom_nosym.bin: vxWorks.res_rom_nosym
	- @ $(RM) $@
	$(EXTRACT_BIN) -O binary vxWorks.res_rom_nosym $@

MACH_EXTRA	=
LIB_EXTRA	=

## Only redefine make definitions above this point, or the expansion of
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

