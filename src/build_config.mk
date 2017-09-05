#####################################################################
# Copyright : 
# Name      : Makefile
# Date      : 2017/02/22
# Author    : libo
######################################################################

ROOT:= $(shell pwd)

PAS_EDITION := 5
FSS_EDITION := 6
BUILD_EDITION := $(PAS_EDITION)

DECODE_OPENMAX := 1

BUILD_HOST:=tegra-linux

BUILD_TOOL_PREFIX:=

CC = $(BUILD_TOOL_PREFIX)g++
CPLUSPLUS = $(BUILD_TOOL_PREFIX)g++
AR:= $(BUILD_TOOL_PREFIX)ar rcs
STRIP:= $(BUILD_TOOL_PREFIX)strip

OUT_DIR := $(ROOT)/build/
BIN_DIR := $(OUT_DIR)iva/bin/
WWW_DIR := $(OUT_DIR)iva/web/
LIB_DIR := $(OUT_DIR)iva/lib/
CFG_DIR := $(OUT_DIR)iva/profiles/
ALG_DIR := $(OUT_DIR)iva/Alg/

3RDLIB_DIR = $(ROOT)/3rdLib/$(BUILD_HOST)/
3RDLIB_INCLUDE_DIR = $(3RDLIB_DIR)include/

LIVE555_LIB  = $(3RDLIB_DIR)live555/lib/libliveMedia.a
LIVE555_LIB += $(3RDLIB_DIR)live555/lib/libgroupsock.a
LIVE555_LIB += $(3RDLIB_DIR)live555/lib/libBasicUsageEnvironment.a
LIVE555_LIB += $(3RDLIB_DIR)live555/lib/libUsageEnvironment.a


COMLIB_DIR  = $(ROOT)/ComLib/
COMLIB_INCLUDE_DIR = $(COMLIB_DIR)include/
COMLIB_LIB_DIR = $(COMLIB_DIR)lib/

GLOBAL_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)Global/
OAL_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)OAL/
MQ_INCLUDE_DIR  = $(COMLIB_INCLUDE_DIR)MQ/
SQ_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)SQ/
ONVIF2_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)Onvif2/
CLICKZOOM_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)ClickZoom/
RECWR_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)RecWR/
FTP_INCLUDE_DIR = $(COMLIB_INCLUDE_DIR)TinyFTP/

OAL_LIB = $(COMLIB_LIB_DIR)liboal.a
MQ_LIB  = $(COMLIB_LIB_DIR)libmq.a
SQ_LIB  = $(COMLIB_LIB_DIR)libsq.a
ONVIF2_LIB = $(COMLIB_LIB_DIR)libonvif2.a
CLICKZOOM_LIB = $(COMLIB_LIB_DIR)libclickzoom.a
GLOBAL_LIB = $(COMLIB_LIB_DIR)libglobal.a
RECWR_LIB = $(COMLIB_LIB_DIR)librecwr.a
TINYFTP_LIB = $(COMLIB_LIB_DIR)libtinyftp.a

export DECODE_OPENMAX

export BUILD_HOST
export BUILD_TOOL_PREFIX
export CC
export CPLUSPLUS
export AR
export STRIP

export OUT_DIR
export BIN_DIR
export WWW_DIR
export LIB_DIR
export CFG_DIR
export ALG_DIR

export 3RDLIB_INCLUDE_DIR

export LIVE555_LIB

export COMLIB_DIR
export COMLIB_INCLUDE_DIR
export COMLIB_LIB_DIR

export OAL_INCLUDE_DIR
export MQ_INCLUDE_DIR
export SQ_INCLUDE_DIR
export ONVIF2_INCLUDE_DIR
export CLICKZOOM_INCLUDE_DIR
export GLOBAL_INCLUDE_DIR
export RECWR_INCLUDE_DIR
export FTP_INCLUDE_DIR

export OAL_LIB
export MQ_LIB
export SQ_LIB
export ONVIF2_LIB
export CLICKZOOM_LIB
export GLOBAL_LIB
export RECWR_LIB
export TINYFTP_LIB

export PAS_EDITION
export FSS_EDITION
export BUILD_EDITION 
