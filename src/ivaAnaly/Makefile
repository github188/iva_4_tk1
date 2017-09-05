SUB_DIR1 = image/
SUB_DIR2 = record/

#算法库
ifeq ($(BUILD_EDITION),$(PAS_EDITION))
ANALY_LIB = -lEngine -lLH_LPR -lVehicleDetect
ANALY_LIB_DIR = algorithm/pas/
ANALY_INC_DIR = algorithm/include/
VERSION_STRING = -------The Parking Analysis System------
else
ANALY_LIB = -lEngine -lLH_LPR -lVehicleDetect
ANALY_LIB_DIR = algorithm/pas/
ANALY_INC_DIR = algorithm/include/
VERSION_STRING = -------The Fast Surveillance System------
endif

INSTALL_DIR = $(BIN_DIR) 

LDFLAGS = $(MQ_LIB)
LDFLAGS += $(SQ_LIB)
LDFLAGS += $(OAL_LIB)
LDFLAGS += $(GLOBAL_LIB)
LDFLAGS += $(RECWR_LIB)
#LDFLAGS += $(FFMPEG_LIB)
LDFLAGS += $(ANALY_LIB)
LDFLAGS += -lmp4v2
LDFLAGS += -lavutil -lswscale -lswresample -lavcodec -lavformat
LDFLAGS += -lopencv_highgui
LDFLAGS += -lopencv_imgproc
#LDFLAGS += -lopencv_calib3d
LDFLAGS += -lopencv_core
#LDFLAGS += -lopencv_contrib
#LDFLAGS += -lopencv_features2d
#LDFLAGS += -lopencv_legacy
#LDFLAGS += -lopencv_flann
#LDFLAGS += -lopencv_ml
#LDFLAGS += -lopencv_video
#LDFLAGS += -lopencv_objdetect
#LDFLAGS += -llibjpeg
LDFLAGS += -ltbb
LDFLAGS += -luuid
LDFLAGS += -lfreetype
LDFLAGS += -liconv
LDFLAGS += -lssl -lcrypto
LDFLAGS += -lpthread -lrt

LDFLAGS += -L$(LIB_DIR) -L$(ALG_DIR)

#CFLAGS =-O2
CFLAGS = -g
CFLAGS += -I$(SUB_DIR1) -I$(SUB_DIR2) -I$(ANALY_INC_DIR)
CFLAGS += -I$(GLOBAL_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR) -I$(MQ_INCLUDE_DIR)
CFLAGS += -I$(SQ_INCLUDE_DIR)
CFLAGS += -I$(RECWR_INCLUDE_DIR)
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)
#CFLAGS += -I$(3RDLIB_INCLUDE_DIR)opencv/
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)freetype2/
CFLAGS += -I$(3RDLIB_INCLUDE_DIR)ffmpeg/

TARGET = ivaAnaly
RELTARGET = $(TARGET)

SOURCES  = $(wildcard ./*.cpp)
SOURCES += $(wildcard $(SUB_DIR1)*.cpp)
SOURCES += $(wildcard $(SUB_DIR2)*.cpp)

RELOBJFILES = $(SOURCES:%.cpp=%.o)

.PHONY: clean install

all: before $(RELTARGET)

before: 
	@echo $(VERSION_STRING)
	cp -fr $(ANALY_LIB_DIR)* $(ALG_DIR)

$(RELTARGET): $(RELOBJFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(RELOBJFILES): %.o: %.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@ 

install: 
	install -d $(INSTALL_DIR)
	install $(RELTARGET) $(INSTALL_DIR)
	install simhei.ttf $(INSTALL_DIR)
	
clean:
	rm -f $(RELOBJFILES) $(RELTARGET)

