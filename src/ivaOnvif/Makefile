
LDFLAGS := $(MQ_LIB)
LDFLAGS += $(OAL_LIB)
LDFLAGS += $(ONVIF2_LIB)
LDFLAGS += $(CLICKZOOM_LIB)

LDFLAGS += -lssl -lcrypto
LDFLAGS += -lnvssdk -lossdk
LDFLAGS += -lcurl
LDFLAGS += -lrt -lpthread -ldl

LDFLAGS += -L$(LIB_DIR)

CFLAGS = -g
CFLAGS += -I$(GLOBAL_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR) -I$(MQ_INCLUDE_DIR) -I$(ONVIF2_INCLUDE_DIR) -I$(CLICKZOOM_INCLUDE_DIR)

INSTALL_DIR = $(BIN_DIR) 

EXE_NAME:= ivaOnvif

OBJS = OnvifMan.o main.o

all: $(EXE_NAME)

$(EXE_NAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

install: 
	install -d $(INSTALL_DIR)
	install $(EXE_NAME) $(INSTALL_DIR)
	
clean:
	rm -f $(OBJS) $(EXE_NAME)

