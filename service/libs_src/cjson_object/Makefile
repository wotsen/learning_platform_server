CC = gcc
CXX = g++
CFLAGS = -g -O2 
CXXFLAG =  -O2 -Wall -ggdb -m64 -D_GNU_SOURCE=1 -D_REENTRANT -D__GUNC__ 

ARCH:=$(shell uname -m)

ARCH32:=i686
ARCH64:=x86_64

ifeq ($(ARCH),$(ARCH64))
SYSTEM_LIB_PATH:=/usr/lib64
else
SYSTEM_LIB_PATH:=/usr/lib
endif

DIRS=.


INC := $(INC) 

LIBFLAGS =  #-fPIC -shared
LDFLAGS := $(LDFLAGS) -D_LINUX_OS_ -std=c++11 

CPP_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
C_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS)) $(patsubst %.c,%.o,$(C_SRCS)) $(patsubst %.cc,%.o,$(CC_SRCS))


TARGET = libCJsonObject.a # libCJsonObject.so

all: $(TARGET)

#$(CXX) $(LIBFLAGS) -g -o $@ $^ $(LDFLAGS)
$(TARGET):$(OBJS)
	ar rcs $(LIBFLAGS) $@ $^

install:
	mkdir CJsonObject/include -p
	mkdir CJsonObject/lib -p
	cp $(TARGET) CJsonObject/lib -f
	cp CJsonObject.hpp CJsonObject/include -f
	cp cJSON.h CJsonObject/include -f

update:
	mkdir ../../includes/CJsonObject -p
	mkdir ../../libs/
	cp CJsonObject/include/* ../../includes/CJsonObject -rf
	cp CJsonObject/lib/$(TARGET) ../../libs -rf

%.o:%.cpp
	$(CXX) $(LIBFLAGS) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.cc
	$(CXX) $(LIBFLAGS) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.c
	$(CXX) $(LIBFLAGS) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -rf CJsonObject/*   
        
