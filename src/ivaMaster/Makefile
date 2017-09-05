SUB_DIR1 = Manager/
SUB_DIR2 = Profile/
SUB_DIR3 = ISAPI/


INSTALL_DIR = $(BIN_DIR) 

LDFLAGS = $(OAL_LIB)
LDFLAGS += $(MQ_LIB)
LDFLAGS += $(SQ_LIB)
LDFLAGS += $(GLOBAL_LIB)

LDFLAGS += $(OPENSSL_LIB)

LDFLAGS += -lxml2
LDFLAGS += -lcurl
LDFLAGS += -ljsoncpp
#LDFLAGS += -lmosquitto
LDFLAGS += -luuid
LDFLAGS += -lparted
LDFLAGS += -lxls
LDFLAGS += -liconv
LDFLAGS += -lssl -lcrypto
LDFLAGS += -lsensors
LDFLAGS += -ldl -lpthread

LDFLAGS += -L$(LIB_DIR)

#CFLAGS =-O2
CFLAGS = -g
CFLAGS += -I$(SUB_DIR1) -I$(SUB_DIR2) -I$(SUB_DIR3)
CFLAGS += -I$(GLOBAL_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR) -I$(MQ_INCLUDE_DIR)
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)xlslib
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)libxml2

TARGET = ivaMaster
RELTARGET = $(TARGET)

SOURCES  = $(wildcard ./*.cpp)
SOURCES += $(wildcard $(SUB_DIR1)*.cpp)
SOURCES += $(wildcard $(SUB_DIR2)*.cpp)
SOURCES += $(wildcard $(SUB_DIR3)*.cpp)

RELOBJFILES = $(SOURCES:%.cpp=%.o)

.PHONY: clean install

all: before $(RELTARGET)

before: 
	#@rm -f $(RELTARGET)
	#@rm -f release/obj/$(IVRCONTROL_DIR9)/systembinding.o
	#@rm -f release/obj/$(IVRCONTROL_DIR9)/uicontrolProxy.o
	
$(RELTARGET): $(RELOBJFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(RELOBJFILES): %.o: %.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@ 

install: 
	install -d $(INSTALL_DIR)
	install $(RELTARGET) $(INSTALL_DIR)
	
clean:
	rm -f $(RELOBJFILES) $(RELTARGET)
