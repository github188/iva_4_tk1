LIB_INSTALL_DIR = $(COMLIB_LIB_DIR)
INCLUDE_INSTALL_DIR = $(SQ_INCLUDE_DIR)

CFLAGS = -O2
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)
CFLAGS += -I$(OAL_INCLUDE_DIR)

RELTARGET = libsq.a

SOURCES = $(wildcard ./*.cpp)
HEADERS = $(wildcard include/*.h)

RELOBJFILE = $(SOURCES:%.cpp=%.o)

.PHONY: clean install

all: install

install: $(RELTARGET)
	@mkdir -p $(INCLUDE_INSTALL_DIR)
	install -d $(INCLUDE_INSTALL_DIR)
	install $(HEADERS) $(INCLUDE_INSTALL_DIR)
	install -d $(LIB_INSTALL_DIR)
	install $(RELTARGET) $(LIB_INSTALL_DIR)

$(RELTARGET): $(RELOBJFILE)
	$(AR) $@ $^ $(LDFLAGS)
	
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@
	
clean:
	rm -f $(RELOBJFILE) $(RELTARGET)


