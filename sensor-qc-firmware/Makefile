INO_FILE ?=sensor-qc-firmware.ino
SKETCH_DIR ?= $(shell pwd)
SKETCH ?= $(SKETCH_DIR)/$(INO_FILE)

#include $(HOME)/devel/testing/ESP32/makefile-tests/makeEspArduino/makeEspArduino.mk
ESP_MAKE_DIR ?= $(HOME)/devel/testing/ESP32/makefile-tests/makeEspArduino
CHIP ?= esp32
BOARD ?= esp32
include $(ESP_MAKE_DIR)/makeEspArduino.mk

REPO_VERSION=$(SRC_GIT_VERSION)
MAIN_NAME = $(PRODUCT)-$(BUILD_DATE)-$(SRC_GIT_VERSION)-$(CHIP)-$(ESP_ARDUINO_VERSION)


bin_name:
	@echo $(MAIN_NAME)


