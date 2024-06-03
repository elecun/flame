
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=armhf
# Note : You should make with GCC/G++

# Makefile

OS := $(shell uname)

#Set Architecutre
#ARCH := armhf
#ARCH := arm64
ARCH := x86_64

CURRENT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
ROOT_DIR := $(shell pwd)/../..
CURRENT_DIR_NAME := $(notdir $(patsubst %/,%,$(dir $(CURRENT_DIR))))

INCLUDE_DIR = -I./ -I$(CURRENT_DIR) -I$(ROOT_DIR)/include -I$(ROOT_DIR) -I$(ROOT_DIR)/dep

#Compilers
ifeq ($(ARCH),arm64)
	CC := /usr/bin/aarch64-linux-gnu-g++
	GCC := /usr/bin/aarch64-linux-gnu-g++
	LD_LIBRARY_PATH += -L./lib/arm64
	OUTDIR		= $(CURRENT_DIR)/../../bin/arm64/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/arm64/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/arm64
else ifeq ($(ARCH), armhf)
	CC := /usr/bin/arm-linux-gnueabihf-g++-9
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-9
	LD_LIBRARY_PATH += -L./lib/armhf
	OUTDIR		= $(CURRENT_DIR)/../../bin/armhf/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/armhf/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/armhf
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	LD_LIBRARY_PATH += -L/usr/local/lib -L$(CURRENT_DIR)/../../lib/x86_64/
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -Wl,-rpath=$(LD_LIBRARY_PATH)
	LDLIBS = -pthread -lrt -ldl -lm -lzmq
	GTEST_LDLIBS = -lgtest
endif

$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(BUILDDIR))
REV_COUNT = $(shell git rev-list --all --count)
MIN_COUNT = $(shell git rev-list --tags --count)

#if release(-O3), debug(-O0)
CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

#custom definitions
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=$(MIN_COUNT) -D__REV__=$(REV_COUNT)
RM	= rm -rf


# flame service engine
burner:	$(BUILDDIR)burner_entry.o \
		$(BUILDDIR)instance.o \
		$(BUILDDIR)cli.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)

$(BUILDDIR)burner_entry.o:	$(CURRENT_DIR)/burner_entry.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)instance.o: $(CURRENT_DIR)/instance.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)cli.o: $(CURRENT_DIR)/cli.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : burner

deploy : FORCE
	cp $(BUILDDIR)*.task $(BUILDDIR)burner $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)burner
FORCE : 