CC = gcc
CXX = g++
CFLAGS = -g -O2 
CXXFLAG =  -O2 -Wall  

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
CC_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
C_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS)) $(patsubst %.c,%.o,$(C_SRCS)) $(patsubst %.cc,%.o,$(CC_SRCS))


TARGET = libfmt.a

all: $(TARGET)

#$(CXX) $(LIBFLAGS) -g -o $@ $^ $(LDFLAGS)
$(TARGET):$(OBJS)
	ar rcs $(LIBFLAGS) $@ $^

install:
	mkdir fmt/include -p
	mkdir fmt/lib -p
	cp $(TARGET) fmt/lib -f
	cp *.h fmt/include -f

update:
	mkdir ../../includes/fmt -p
	mkdir ../../libs/
	cp fmt/include/* ../../includes/fmt -rf
	cp fmt/lib/$(TARGET) ../../libs -rf

%.o:%.cpp
	$(CXX) $(LIBFLAGS) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.cc
	$(CXX) $(LIBFLAGS) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.c
	$(CXX) $(LIBFLAGS) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -rf fmt/*   
        
