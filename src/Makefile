#####################################################################
# Copyright : 
# Name      : Makefile
# Date      : 2017/02/22
# Author    : libo
######################################################################

include build_config.mk

###########################################################
#                       ∞Ê±æ–≈œ¢                          #
###########################################################
ifeq ($(BUILD_EDITION),$(PAS_EDITION))
PRODUCT_NAME    = pas
MAJOR_VERSION   = 1
MINOR_VERSION   = 0
PATCH_VISION    = 0
GREEK           = D
else ifeq ($(BUILD_EDITION),$(FSS_EDITION))
PRODUCT_NAME    = fss
MAJOR_VERSION   = 2
MINOR_VERSION   = 0
PATCH_VISION    = 0
GREEK           = R
endif

BUILD_TIME =  ${shell date "+%Y%m%d"}
VERSION_CODE = $(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VISION).$(BUILD_TIME)$(GREEK)
VERSION_STR = \"$(VERSION_CODE)\"

export VERSION_STR

###########################################################

SUB_DIR := 
SUB_DIR += ComLib
SUB_DIR += ivaBoa/src
SUB_DIR += ivaUpload
SUB_DIR += ivaDatabase
SUB_DIR += ivaOnvif
SUB_DIR += ivaVideoIn
SUB_DIR += ivaVideoOut
SUB_DIR += ivaRecord
SUB_DIR += ivaMaster
ifeq ($(BUILD_EDITION),$(PAS_EDITION))
SUB_DIR += ivaDecode
SUB_DIR += ivaAnaly
endif


websrc = Web/$(PRODUCT_NAME)/
package = $(PRODUCT_NAME)$(VERSION_CODE).tar.gz


default:befor_install app

app:
	@echo -----------------------------------------------------------
	@echo ------------------$(PRODUCT_NAME)$(VERSION_CODE)-----------------------
	@echo -----------------------------------------------------------
	@for n in $(SUB_DIR); do $(MAKE) -C $$n ; done

befor_install:
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	@test -d $(WWW_DIR) || mkdir -p $(WWW_DIR)
	@test -d $(LIB_DIR) || mkdir -p $(LIB_DIR)
	@test -d $(ALG_DIR) || mkdir -p $(ALG_DIR)
	@test -d $(OUT_DIR)Package || mkdir -p $(OUT_DIR)Package
	cd $(3RDLIB_DIR); chmod 777 install_3rdlib.sh && ./install_3rdlib.sh $(LIB_DIR)

install:
	@for n in $(SUB_DIR); do $(MAKE) -C $$n install; done
	cp -Rf sh/*.sh $(BIN_DIR)
	cp -Rf sh/$(PRODUCT_NAME)/*.sh $(BIN_DIR)
	cp -Rf $(websrc)* $(WWW_DIR)
	cp -Rf Web/WebPlugin $(WWW_DIR)
	cd $(BIN_DIR) && chmod 777 *.sh
	
pack:
	cd $(CFG_DIR) && rm -rf *
	cd $(OUT_DIR) && tar -zcvf Package/$(package) iva/*

clean:
	@for n in $(SUB_DIR); do $(MAKE) -C $$n clean; done
	rm -rf $(ALG_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(WWW_DIR)
	cd $(OUT_DIR)Package && rm -rf *

