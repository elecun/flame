# Author : Byunghun Hwang <bh.hwang@iae.re.kr>


# Build for architecture selection (editable!!)
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
	INCLUDE_DIR = -I./ -I$(CURRENT_DIR) -I$(CURRENT_DIR)/include -I$(CURRENT_DIR)/include/dep -I/usr/include -I/usr/local/include -I/opt/pylon/include
	LIBDIR = -L/usr/local/lib -L$(CURRENT_DIR)/lib/x86_64/ -L/opt/pylon/lib/
export LD_LIBRARY_PATH := $(LIBDIR):$(LD_LIBRARY_PATH)
endif

# OS
ifeq ($(OS),Linux) #for Linux
#LDFLAGS = -Wl,--export-dynamic -Wl,-rpath,$(LD_LIBRARY_PATH)
	LDFLAGS = -Wl,--export-dynamic -Wl,-rpath,$(LIBDIR) -L$(LIBDIR)
	LDLIBS = -pthread -lrt -ldl -lm -lzmq -lpylonbase -lpylonutility
endif



$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(BUILDDIR))
REV_COUNT = $(shell git rev-list --all --count)
MIN_COUNT = $(shell git tag | wc -l)

#if release(-O3), debug(-O0)
# if release mode compile, remove -DNDEBUG
CXXFLAGS = -O3 -fPIC -Wall -std=c++20 -D__cplusplus=202002L

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
		$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -o $(BUILDDIR)$@ $^ $(LDLIBS)

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


# components
data_push.comp:	$(BUILDDIR)data.push.o
				$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)data.push.o:	$(CURRENT_DIR)/components/data.push/data.push.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

data_pull.comp:	$(BUILDDIR)data.pull.o
				$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)data.pull.o:	$(CURRENT_DIR)/components/data.pull/data.pull.cc
						$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

device_uvccam_multi.comp:	$(BUILDDIR)device.uvccam.multi.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)device.uvccam.multi.o:	$(CURRENT_DIR)/components/device.uvccam.multi/device.uvccam.multi.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

# for dk project
basler_gige_cam_linker.comp:	$(BUILDDIR)basler.gige.cam.linker.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDFLAGS) $(LDLIBS)
$(BUILDDIR)basler.gige.cam.linker.o:	$(CURRENT_DIR)/components/basler.gige.cam.linker/basler.gige.cam.linker.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_gui_supporter.comp:	$(BUILDDIR)dk.gui.supporter.o
						$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.gui.supporter.o:	$(CURRENT_DIR)/components/dk.gui.supporter/dk.gui.supporter.cc
								$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_level_data_gateway.comp:	$(BUILDDIR)dk.level.data.gateway.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.level.data.gateway.o:	$(CURRENT_DIR)/components/dk.level.data.gateway/dk.level.data.gateway.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_sdd_inference.comp:	$(BUILDDIR)dk.sdd.inference.o
						$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.sdd.inference.o:	$(CURRENT_DIR)/components/dk.sdd.inference/dk.sdd.inference.cc
								$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_presdd_inference.comp:	$(BUILDDIR)dk.presdd.inference.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.presdd.inference.o:	$(CURRENT_DIR)/components/dk.presdd.inference/dk.presdd.inference.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_sys_op_trigger.comp:	$(BUILDDIR)dk.sys.op.trigger.o
						$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.sys.op.trigger.o:	$(CURRENT_DIR)/components/dk.sys.op.trigger/dk.sys.op.trigger.cc
								$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

synology_nas_file_stacker.comp:	$(BUILDDIR)synology.nas.file.stacker.o
						$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)synology.nas.file.stacker.o:	$(CURRENT_DIR)/components/synology.nas.file.stacker/synology.nas.file.stacker.cc
							$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

ni_pulse_generator.comp:	$(BUILDDIR)ni.pulse.generator.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)ni.pulse.generator.o:	$(CURRENT_DIR)/components/ni.pulse.generator/ni.pulse.generator.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_remote_light_linker.comp:	$(BUILDDIR)dk.remote.light.linker.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.remote.light.linker.o:	$(CURRENT_DIR)/components/dk.remote.light.linker/dk.remote.light.linker.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@

dk_remote_lens_linker.comp:	$(BUILDDIR)remote.lens.linker.o
							$(CC) $(LDFLAGS) $(LD_LIBRARY_PATH) -shared -o $(BUILDDIR)$@ $^ $(LDLIBS)
$(BUILDDIR)dk.remote.lens.linker.o:	$(CURRENT_DIR)/components/dk.remote.lens.linker/dk.remote.lens.linker.cc
									$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^ -o $@


all : flame
dk_h_inspector : basler_gige_cam_linker.comp  dk_level_data_gateway.comp dk_sdd_inference.comp dk_presdd_inference.comp dk_sys_op_trigger.comp synology_nas_file_stacker.comp ni_pulse_generator.comp 

dk_h_inspector_remote : dk_remote_light_linker.comp dk_remote_lens_linker.comp

dk_h_inspector_monitor : dk_data_aggregator.comp

components : device.uvccam.multi.comp data_push.comp data_pull_test.comp basler_gige_cam_linker.comp dk_gui_supporter.comp dk_level_data_gateway.comp dk_sdd_inference.comp dk_presdd_inference.comp dk_sys_op_trigger.comp synology_nas_file_stacker.comp ni_pulse_generator.comp dk_remote_light_linker.comp dk_remote_lens_linker.comp

deploy : FORCE
	cp $(BUILDDIR)*.comp $(BUILDDIR)flame $(BINDIR)
clean : FORCE 
		$(RM) $(BUILDDIR)*.o $(BUILDDIR)flame
debug:
	@echo "Building for Architecture : $(ARCH)"
	@echo "Building for OS : $(OS)"

FORCE : 