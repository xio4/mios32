# $Id$

################################################################################
# following setup taken from environment variables
################################################################################

PROCESSOR =	$(MIOS32_PROCESSOR)
FAMILY    = 	$(MIOS32_FAMILY)
BOARD	  = 	$(MIOS32_BOARD)
LCD       =     $(MIOS32_LCD)


################################################################################
# Source Files, include paths and libraries
################################################################################

THUMB_SOURCE    = src/tasks.c \
		  src/mbcv_hwcfg.c \
		  src/components/CapChargeCurve.c

THUMB_CPP_SOURCE  = src/app.cpp \
		    src/scs_config.cpp \
		    src/mbcv_patch.cpp \
		    src/mbcv_map.cpp \
		    src/mbcv_lre.cpp \
		    src/mbcv_rgb.cpp \
		    src/mbcv_button.cpp \
		    src/mbcv_enc.cpp \
	 	    src/mbcv_file.cpp \
	 	    src/mbcv_file_hw.cpp \
	 	    src/mbcv_file_b.cpp \
	 	    src/mbcv_file_p.cpp \
	 	    src/mbcv_sysex.cpp \
	  	    src/terminal.cpp \
		    src/components/MbCvEnvironment.cpp \
		    src/components/MbCv.cpp \
		    src/components/MbCvPatch.cpp \
		    src/components/MbCvRandomGen.cpp \
		    src/components/MbCvClock.cpp \
		    src/components/MbCvLfo.cpp \
		    src/components/MbCvEnvBase.cpp \
		    src/components/MbCvEnv.cpp \
		    src/components/MbCvEnvMulti.cpp \
		    src/components/MbCvArp.cpp \
		    src/components/MbCvMod.cpp \
		    src/components/MbCvVoice.cpp \
		    src/components/MbCvVoiceQueue.cpp \
		    src/components/MbCvMidiVoice.cpp \
		    src/components/MbCvSeq.cpp \
		    src/components/MbCvSeqBassline.cpp \
		    src/components/MbCvTables.cpp \
		    src/components/MbCvScope.cpp


# (following source stubs not relevant for Cortex M3 derivatives)
THUMB_AS_SOURCE =
ARM_SOURCE      =
ARM_AS_SOURCE   =

C_INCLUDE = 	-I src \
		-I src/components

A_INCLUDE = 	-I src

LIBS = 		


################################################################################
# Remaining variables
################################################################################

LD_FILE   = 	$(MIOS32_PATH)/etc/ld/$(FAMILY)/$(PROCESSOR).ld
PROJECT   = 	project

DEBUG     =	-g
OPTIMIZE  =	-O3

CFLAGS =	$(DEBUG) $(OPTIMIZE)
CPPFLAGS =      -fpermissive


################################################################################
# Include source modules via additional makefiles
################################################################################

# sources of programming model
include $(MIOS32_PATH)/programming_models/traditional/programming_model.mk

# application specific LCD driver (selected via makefile variable)
include $(MIOS32_PATH)/modules/app_lcd/$(LCD)/app_lcd.mk

# MIDI Router (and port handling)
include $(MIOS32_PATH)/modules/midi_router/midi_router.mk

# MIDImon
include $(MIOS32_PATH)/modules/midimon/midimon.mk

# UIP driver
include $(MIOS32_PATH)/modules/uip/uip.mk

# UIP Standard Task (with DHCPC + OSC server and client)
include $(MIOS32_PATH)/modules/uip_task_standard/uip_task_standard.mk

# ESP8266 driver
include $(MIOS32_PATH)/modules/esp8266/esp8266.mk

# FATFS Driver
include $(MIOS32_PATH)/modules/fatfs/fatfs.mk

# FILE Access Layer
include $(MIOS32_PATH)/modules/file/file.mk

# Portable randomize module
include $(MIOS32_PATH)/modules/random/random.mk

# AOUT Driver
include $(MIOS32_PATH)/modules/aout/aout.mk

# WS2812 RGB LED strip driver
include $(MIOS32_PATH)/modules/ws2812/ws2812.mk

# Notestack functions
include $(MIOS32_PATH)/modules/notestack/notestack.mk

# USB Mass Storage Device Driver
include $(MIOS32_PATH)/modules/msd/msd.mk

# Sources for Standard Control Surface
include $(MIOS32_PATH)/modules/scs/scs.mk

# common make rules
# Please keep this include statement at the end of this Makefile. Add new modules above.
include $(MIOS32_PATH)/include/makefile/common.mk
