INSTALL_DIR = $(BIN_DIR) 

LDFLAGS = $(MQ_LIB)
LDFLAGS += $(SQ_LIB)
LDFLAGS += $(OAL_LIB)
LDFLAGS += $(RECWR_LIB)
#LDFLAGS += $(FFMPEG_LIB)
LDFLAGS += -lmp4v2
LDFLAGS += -lavutil -lswscale -lswresample -lavcodec -lavformat
LDFLAGS += -ldl -lpthread -lrt

LDFLAGS += -L$(LIB_DIR)

#CFLAGS =-O2
CFLAGS = -g
CFLAGS += -I$(GLOBAL_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR)
CFLAGS += -I$(MQ_INCLUDE_DIR) -I$(SQ_INCLUDE_DIR)
CFLAGS += -I$(RECWR_INCLUDE_DIR)
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)

TARGET = ivaRecord

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
