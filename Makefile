# Author : Byunghun Hwang <bh.hwang@iae.re.kr>


# -----------------------------------------------------------------------------
# Architecture & OS Detection
# -----------------------------------------------------------------------------
ARCH := $(shell uname -m)
OS := $(shell uname)

CURRENT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
CURRENT_DIR_NAME := $(notdir $(patsubst %/,%,$(dir $(CURRENT_DIR))))

#Compilers
ifeq ($(ARCH),arm64)
	CC := /usr/bin/aarch64-linux-gnu-g++
	GCC := /usr/bin/aarch64-linux-gnu-g++
	LD_LIBRARY_PATH += -L./lib/arm64
	OUTDIR		= $(CURRENT_DIR)/bin/arm64/
	BUILDDIR		= $(CURRENT_DIR)/bin/arm64/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR)/ -I$(CURRENT_DIR)/include/ -I$(CURRENT_DIR)/include/dep -I/usr/include
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/arm64
else ifeq ($(ARCH), armhf)
	CC := /usr/bin/arm-linux-gnueabihf-g++-9
	GCC := /usr/bin/arm-linux-gnueabihf-gcc-9
	LD_LIBRARY_PATH += -L./lib/armhf
	OUTDIR		= $(CURRENT_DIR)/bin/armhf/
	BUILDDIR		= $(CURRENT_DIR)/bin/armhf/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR)/ -I$(CURRENT_DIR)/include/ -I$(CURRENT_DIR)/include/dep -I/usr/include
	LD_LIBRARY_PATH += -L/usr/local/lib -L./lib/armhf
else ifeq ($(ARCH), aarch64) # for Mac Apple Silicon
	CC := g++
	GCC := gcc
	LD_LIBRARY_PATH += -L./lib/aarch64
	OUTDIR		= $(CURRENT_DIR)/bin/aarch64/
	BUILDDIR		= $(CURRENT_DIR)/bin/aarch64/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR) -I$(CURRENT_DIR)/include -I$(CURRENT_DIR)/include/dep
	LD_LIBRARY_PATH = -L/usr/local/lib -L$(CURRENT_DIR)/lib/aarch64/
else
	CC := g++
	GCC := gcc
#	LD_LIBRARY_PATH += -L./lib/x86_64
	OUTDIR		= $(CURRENT_DIR)/bin/x86_64/
	BUILDDIR		= $(CURRENT_DIR)/bin/x86_64/
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR) -I$(CURRENT_DIR)/include -I$(CURRENT_DIR)/include/dep -I$(CURRENT_DIR)/include/dep/libzmq -I/usr/include -I/usr/local/include -I/opt/pylon/include -I/usr/include/opencv4 -I/usr/local/cuda/include
	LIBDIR = -L/usr/local/lib -L$(CURRENT_DIR)/lib/x86_64/ -L/usr/lib/x86-64-linux-gnu -L/usr/local/cuda/lib64 -L/opt/pylon/lib/
export LD_LIBRARY_PATH := $(LIBDIR):$(LD_LIBRARY_PATH)
endif

# LDFLAGS definition
# Always include LIBDIR
LDFLAGS = $(LIBDIR)

# OS
ifeq ($(OS),Linux) #for Linux
	LDFLAGS += -Wl,--export-dynamic -Wl,-rpath=.
	LDLIBS = -pthread -lrt -ldl -lm -lzmq
endif



$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(BUILDDIR))
REV_COUNT = $(shell git rev-list --all --count)
MIN_COUNT = $(shell git tag | wc -l)

#if release(-O3), debug(-O0)
# if release mode compile, remove -DNDEBUG
CXXFLAGS = -O3 -fPIC -Wall -std=c++20 -D__cplusplus=202002L -Wno-deprecated-enum-enum-conversion

#custom definitions
CXXFLAGS += -D__MAJOR__=0 -D__MINOR__=$(MIN_COUNT) -D__REV__=$(REV_COUNT)
RM	= rm -rf

#directories

INCLUDES = $(CURRENT_DIR)/include
SOURCE_FILES = .


# flame service engine
flame:	$(BUILDDIR)flame.o \
		$(BUILDDIR)config.o \
		$(BUILDDIR)manager.o \
		$(BUILDDIR)driver.o \
		$(BUILDDIR)instance.o
		$(CC) $(LDFLAGS) -o $(BUILDDIR)$@ $^ $(LDLIBS)

$(BUILDDIR)flame.o:	$(CURRENT_DIR)/tools/flame/flame.cc
					$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)instance.o: $(CURRENT_DIR)/tools/flame/instance.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)manager.o: $(CURRENT_DIR)/tools/flame/manager.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)driver.o: $(INCLUDES)/flame/component/driver.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@
$(BUILDDIR)config.o: $(INCLUDES)/flame/config.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@



all : flame

deploy : FORCE
	cp $(BUILDDIR)*.comp $(BUILDDIR)flame $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)flame
debug:
	@echo "Building for Architecture : $(ARCH)"
	@echo "Building for OS : $(OS)"

FORCE :
 