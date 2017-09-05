
LDFLAGS := $(OAL_LIB)
LDFLAGS += $(MQ_LIB)

LDFLAGS += -lsqlite3
LDFLAGS += -ldl -lpthread

LDFLAGS += -L$(LIB_DIR)

CFLAGS = -g
CFLAGS += -I$(GLOBAL_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR) -I$(MQ_INCLUDE_DIR)
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)

INSTALL_DIR = $(BIN_DIR) 

EXE_NAME:= ivaDatabase

OBJS = Sqlite3Database.o main.o

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

