CXX = g++
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)

CXXFLAGS += $(ROOTCFLAGS)
CXXFLAGS += -I$(ROOTSYS)/include

LIBS  = $(ROOTLIBS)
GLIBS = $(ROOTGLIBS)


CPP_FILES := $(wildcard *.cpp)

all:
	${CXX} ${CXXFLAGS} -o Cherenkov ${CPP_FILES}  ${LIBS} ${GLIBS}

clean:
	rm Cherenkov
