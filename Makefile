CFLAGS    :=`root-config --cflags --libs` -lGeom -lEve -lGed -lRGL -lEG -lTreePlayer -lMathMore `nuclust-config --cflags --libs` -L$(UNISimSrc)lib/ -lUNISDetectionSetup -lUNISStrip -lUNISSiliconDetector -lUNISOscar -lUNISLamp -lUNISFaziaQuartet

SrcSuf    := cpp
ObjSuf    := o
ExeSuf    := exe
ObjDir    := obj/
SrcDir    := src/
DepDir    := include/
DepSuf    := h
LibSuf    := so
PcmSuf    := _rdict.pcm

PROG      := $(wildcard exec_*.$(SrcSuf))
PROG      := $(patsubst %.$(SrcSuf), %, $(PROG))

REXPPROOTEVENT_DICT  := REXppRootEventDict.$(SrcSuf)
REXPPROOTEVENT_DICTH := REXppRootEventDict.$(DepSuf)
REXPPROOTEVENT_DICTO := REXppRootEventDict.$(ObjSuf)
REXPPROOTEVENT_PCM   := REXppRootEventDict$(PcmSuf)
REXPPROOTEVENT_HDRS  := REXppRootEventLinkDef.$(DepSuf)
REXPPROOTEVENT_LIB  := libREXppRootEvent.$(LibSuf)

OBJS      := $(ObjDir)REXppRootEvent.$(ObjSuf) $(ObjDir)REXppFramework.$(ObjSuf) $(ObjDir)$(REXPPROOTEVENT_DICTO)

DEPS      := $(_OBJS:.$(ObjSuf)=.$(DepSuf))

INCLUDES  := -I./detectors/DetectionSetup/
INCLUDES  += -I./detectors/Strip/
INCLUDES  += -I./LISETools/
INCLUDES  += -I./SRIMTools/
INCLUDES  += -I./include/
INCLUDES  += -I$(UNISimSrc)detectors/DetectionSetup/
INCLUDES  += -I$(UNISimSrc)detectors/Strip/
INCLUDES  += -I$(UNISimSrc)detectors/Fazia/
INCLUDES  += -I$(UNISimSrc)detectors/Oscar/
INCLUDES  += -I$(UNISimSrc)detectors/Silicon/
INCLUDES  += -I$(UNISimSrc)detectors/Lamp/

CXXFLAGS  += $(INCLUDES) -std=c++11 -fPIC -O3

all: $(PROG)
	@echo "cp $(REXPPROOTEVENT_PCM) $(REXPPROOTEVENT_LIB) $(REXppSrc)lib/."
	@cp $(REXPPROOTEVENT_LIB) $(REXPPROOTEVENT_PCM) $(REXppSrc)lib/.

.SUFFIXES: .$(SrcSuf)

$(PROG): $(OBJS) $(LibDir)$(REXPPROOTEVENT_LIB)
		$(CXX) $(CXXFLAGS) -o ${@}.$(ExeSuf) ${@}.cpp $^ $(SYSLIB) $(CFLAGS) $(RLIBS)

$(ObjDir)REXppFramework.$(ObjSuf):  ./$(SrcDir)REXppFramework.cpp  ./$(DepDir)REXppFramework.h
	$(CXX) $(CXXFLAGS) -c ./$(SrcDir)REXppFramework.cpp -o $(ObjDir)REXppFramework.$(ObjSuf) $(SYSLIB) $(CFLAGS) $(RLIBS)

$(ObjDir)REXppRootEvent.$(ObjSuf):  ./$(SrcDir)REXppRootEvent.cpp  ./$(DepDir)REXppRootEvent.h ./$(ObjDir)$(REXPPROOTEVENT_DICTO)
	$(CXX) $(CXXFLAGS) -c ./$(SrcDir)REXppRootEvent.cpp -o $(ObjDir)REXppRootEvent.$(ObjSuf) $(SYSLIB) $(CFLAGS) $(RLIBS)

$(LibDir)$(REXPPROOTEVENT_LIB):  $(ObjDir)REXppRootEvent.$(ObjSuf) ./$(ObjDir)$(REXPPROOTEVENT_DICTO)
	@$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(CFLAGS) $(RLIBS)
	
$(ObjDir)$(REXPPROOTEVENT_DICTO):  ./$(REXPPROOTEVENT_DICT)
	$(CXX) $(CXXFLAGS) -c ./$(REXPPROOTEVENT_DICT) -o ./$(ObjDir)$(REXPPROOTEVENT_DICTO) $(SYSLIB) $(CFLAGS) $(RLIBS)
	
$(REXPPROOTEVENT_DICT): ./$(DepDir)REXppRootEvent.h ./$(DepDir)$(REXPPROOTEVENT_HDRS)
	rootcling -f $@ -p $(INCLUDES) $^
	
.PHONY: clean
clean:
	@$(RM) *.$(ExeSuf)
	@$(RM) $(ObjDir)*.$(ObjSuf)
	@$(RM) $(REXPPROOTEVENT_DICT) $(REXPPROOTEVENT_DICTH) $(REXPPROOTEVENT_DICTO) $(REXPPROOTEVENT_PCM) $(REXppSrc)lib/$(REXPPROOTEVENT_LIB) $(REXppSrc)lib/$(REXPPROOTEVENT_PCM) $(REXPPROOTEVENT_LIB)
