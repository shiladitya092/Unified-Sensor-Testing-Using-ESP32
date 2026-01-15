
ENVFILE?=.env
CONFIG=.config
$(info env is $(ENVFILE))

ifneq ("$(wildcard $(ENVFILE))","")
	include $(ENVFILE)
endif

ifneq ("$(wildcard $(CONFIG))","")
	include $(CONFIG)
endif

# Do not edit these, create a .env for these variables if you want different values than defined.
ESP_ROOT ?= $(HOME)/arduino-esp32
ARDUINO_LIB_DIR ?= $(HOME)/Arduino/libraries
ESP_MAKE_DIR ?= $(HOME)/makeEspArduino
ROOT_DIR = $(shell pwd)
SKETCH_FOLDER ?= miniviruswarn_1_0_1
CHIP = esp32
BOARD ?= esp32
BUILD_ROOT_DIR=$(ROOT_DIR)/build
PRODUCT ?= viruswarn

BUILD_DIR=$(BUILD_ROOT_DIR)/$(PRODUCT)

#LIBS += $(HOME)/devel/testing/Viruswarn/miniviruswarn_1_0_1 
#LIBS+=	$(HOME)/Arduino/libraries
LIBS+=	$(ARDUINO_LIB_DIR)

LIBS += $(ROOT_DIR)/libraries

BUILD_OPTIONS += -DINCLUDE_BUILD_INFO

#BUILD_OPTIONS += -DUSE_SQLITE_STORAGE=1

BUILD_VARS += "ESP_ROOT=$(ESP_ROOT)" \
	      "ESP_MAKE_DIR=$(ESP_MAKE_DIR)" \
	      "CHIP=$(CHIP)" \
	      "BUILD_DIR=$(BUILD_DIR)" \
	      "PRODUCT=$(PRODUCT)" \
	      "LIBS=$(LIBS)" \
	      "BUILD_EXTRA_FLAGS=$(BUILD_OPTIONS)" 

ifdef BOARD 
	BUILD_VARS += "BOARD=$(BOARD)"
endif

ifdef UPLOAD_SPEED
	BUILD_VARS += "UPLOAD_SPEED=$(UPLOAD_SPEED)"
endif

ifdef UPLOAD_PORT
	BUILD_VARS += "UPLOAD_PORT=$(UPLOAD_PORT)"
endif

ifdef PART_FILE 
	BUILD_VARS += "PART_FILE=$(PART_FILE)"
endif

all:
	@echo $(BUILD_VARS)
	$(MAKE) -C $(SKETCH_FOLDER) $(BUILD_VARS) all

clean:
	$(MAKE) -C $(SKETCH_FOLDER) $(BUILD_VARS) clean

flash:
	$(MAKE) -C $(SKETCH_FOLDER) $(BUILD_VARS) flash
flash_fs:
	$(MAKE) -C $(SKETCH_FOLDER) $(BUILD_VARS) flash_fs

