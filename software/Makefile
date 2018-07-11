SHELL            := /bin/bash
ROOT_DIR         := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BRICKLIB_PATH    := $(realpath $(ROOT_DIR)/src/bricklib)

include $(BRICKLIB_PATH)/toolchains/Makefile_Brick.mk
