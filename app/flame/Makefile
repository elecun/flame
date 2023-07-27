
# Author : Byunghun Hwang <bh.hwang@iae.re.kr>
# Usage : make ARCH=armhf
# Note : You should make with GCC/G++

# Makefile

OS := $(shell uname)

#Set Architecutre
#ARCH := armhf
ARCH := x86_64

CURRENT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
CURRENT_DIR_NAME := $(notdir $(patsubst %/,%,$(dir $(CURRENT_DIR))))

#Compilers
ifeq ($(ARCH),armhf)
	CC := /usr/bin/arm-linux-gnueabihf-g++-8
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-8
	LD_LIBRARY_PATH += -L./lib/armhf
	OUTDIR		= $(CURRENT_DIR)/../../bin/armhf/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/armhf/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR)/ -I$(CURRENT_DIR)/../../include/ -I$(CURRENT_DIR)/../../include/3rdparty/
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/armhf
else
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	BUILDDIR		= $(CURRENT_DIR)/../../bin/x86_64/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR) -I$(CURRENT_DIR)/../../include -I$(CURRENT_DIR)/../../include/3rdparty
	LD_LIBRARY_PATH += -L/usr/local/lib -L$(CURRENT_DIR)/../../lib/x86_64/
endif

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS = -Wl,--export-dynamic -Wl,-rpath=$(LD_LIBRARY_PATH)
	LDLIBS = -pthread -lrt -ldl -lm -lstdc++fs
	GTEST_LDLIBS = -lgtest
endif

$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(BUILDDIR))
REV_COUNT = $(shell git rev-list --all --count)
$(info $(REV_COUNT))

#if release(-O3), debug(-O0)
CXXFLAGS = -O3 -fPIC -Wall -std=c++17 -D__cplusplus=201703L

#custom definitions
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=0 -D__REV__=$(REV_COUNT)
RM	= rm -rf

#directories

INCLUDE_FILES = $(CURRENT_DIR)/../../include
SOURCE_FILES = .


# flame service engine
flame:	$(BUILDDIR)flame.o \
		$(BUILDDIR)flame_instance.o \
		$(BUILDDIR)taskmanager.o \
		$(BUILDDIR)driver.o \
		$(BUILDDIR)profile.o \
		$(BUILDDIR)rt_timer.o
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)

$(BUILDDIR)flame.o:	$(CURRENT_DIR)/flame.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)flame_instance.o: $(CURRENT_DIR)/instance.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

$(BUILDDIR)driver.o:	$(INCLUDE_FILES)/flame/core/driver.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# edge include files
$(BUILDDIR)rt_trigger.o:	$(INCLUDE_DIR)/flame/core/rt_trigger.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)rt_timer.o:		$(INCLUDE_FILES)/flame/core/rt_timer.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)profile.o:	$(INCLUDE_FILES)/flame/core/profile.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)uuid.o:	$(INCLUDE_FILES)/flame/util/uuid.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)general.o:	$(INCLUDE_FILES)/flame/device/general.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)info.o:	$(INCLUDE_FILES)/flame/sys/info.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)cpu.o:	$(INCLUDE_FILES)/flame/sys/cpu.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)network.o:	$(INCLUDE_FILES)/flame/sys/network.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)network_perf.o:	$(INCLUDE_FILES)/flame/sys/network_perf.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)memory.o:	$(INCLUDE_FILES)/flame/sys/memory.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)system.o:	$(INCLUDE_FILES)/flame/sys/system.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)taskmanager.o:	$(INCLUDE_FILES)/flame/core/taskmanager.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : flame

deploy : FORCE
	cp $(BUILDDIR)*.task $(BUILDDIR)flame $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)flame
FORCE : 