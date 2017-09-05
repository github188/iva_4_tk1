SUBDIRS := OAL
SUBDIRS += Global
SUBDIRS += SQ
SUBDIRS += MQ
SUBDIRS += Onvif2
SUBDIRS += ClickZoom
SUBDIRS += RecWR
SUBDIRS += TinyFTP

CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))
INSTALLSUBDIRS = $(addsuffix .install, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) $(CLEANSUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C $@

install: $(INSTALLSUBDIRS)



$(INSTALLSUBDIRS):
	@echo
	@echo Executing make install in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) install

clean:$(CLEANSUBDIRS)
	@rm -rf lib
	@rm -rf include

$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean
