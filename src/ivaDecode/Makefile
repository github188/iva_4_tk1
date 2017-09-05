

INSTALL_DIR = $(BIN_DIR) 


LDFLAGS = $(MQ_LIB)
LDFLAGS += $(SQ_LIB)
LDFLAGS += $(OAL_LIB)
LDFLAGS += $(GLOBAL_LIB)
ifeq ($(DECODE_OPENMAX),1)
LDFLAGS += -L/usr/lib/arm-linux-gnueabihf/tegra -lnvomx
else
LDFLAGS += -lavutil -lswscale -lswresample -lavcodec -lavformat
endif
LDFLAGS += -lrt -lpthread

LDFLAGS += -L$(LIB_DIR)

#CFLAGS =-O2
CFLAGS = -g
CFLAGS += -I$(GLOBAL_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR)
CFLAGS += -I$(MQ_INCLUDE_DIR) -I$(SQ_INCLUDE_DIR)
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)
ifeq ($(DECODE_OPENMAX),1)
CFLAGS += -Iomx-heads -DDECODE_OPENMAX
endif

TARGET = ivaDecode
RELTARGET = $(TARGET)

SOURCES  = $(wildcard ./*.cpp)

RELOBJFILES = $(SOURCES:%.cpp=%.o)

.PHONY: clean install

all: $(RELTARGET)
	
$(RELTARGET): $(RELOBJFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(RELOBJFILES): %.o: %.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@ 

install: 
	install -d $(INSTALL_DIR)
	install $(RELTARGET) $(INSTALL_DIR)
	
clean:
	rm -f $(RELOBJFILES) $(RELTARGET)
