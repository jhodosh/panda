# Don't forget to add your plugin to config.panda!

# Set your plugin name here. It does not have to correspond to the name
# of the directory in which your plugin resides.
PLUGIN_NAME=yaffs_live

# Include the PANDA Makefile rules
include ../panda.mak

# If you need custom CFLAGS or LIBS, set them up here
QEMU_CFLAGS+= -std=c++11
LIBS+= `pkg-config --libs protobuf`

PROTOC = protoc
PROTOCFLAGS = --cpp_out=.

PROTOS         := $(wildcard *.proto)
PROTO_OBJS     := $(PROTOS:.proto=.pb.o)

PBSRCS   := $(wildcard *.proto)
PBOBJS   := $(PROTOS:.proto=.pb.o)
PBGENS   := $(PROTOS:.proto=.pb.cc) $(PROTOS:.proto=.pb.h)
protos: ${PBSRCS}

%.pb:
	${PROTOC} ${PROTOCFLAGS} ${PROTOS}
	${CXX} ${CXXFLAGS} -c -fPIC -o $(PLUGIN_TARGET_DIR)/$*.pb.o $*.pb.cc

# The main rule for your plugin. Please stick with the panda_ naming
# convention.
$(PLUGIN_TARGET_DIR)/$(PLUGIN_NAME).o: protos $(PLUGIN_SRC_ROOT)/$(PLUGIN_NAME)/$(PLUGIN_NAME).cpp
$(PLUGIN_TARGET_DIR)/yaffs.o: $(PLUGIN_SRC_ROOT)/$(PLUGIN_NAME)/yaffs.cpp

$(PLUGIN_TARGET_DIR)/panda_$(PLUGIN_NAME).so: $(PLUGIN_TARGET_DIR)/$(PLUGIN_NAME).o $(PLUGIN_TARGET_DIR)/yaffs.o
	$(call quiet-command,$(CXX) $(QEMU_CFLAGS) -shared -o $@ $^ $(PLUGIN_TARGET_DIR)/storagelog.pb.o $(LIBS),"  PLUGIN  $@")

	
all: storagelog.pb $(PLUGIN_TARGET_DIR)/panda_$(PLUGIN_NAME).so
