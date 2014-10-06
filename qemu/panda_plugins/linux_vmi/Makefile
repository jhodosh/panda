# Don't forget to add your plugin to config.panda!

# Set your plugin name here. It does not have to correspond to the name
# of the directory in which your plugin resides.
PLUGIN_NAME=linux_vmi

# Include the PANDA Makefile rules
include ../panda.mak

# If you need custom CFLAGS or LIBS, set them up here
QEMU_CXXFLAGS+=-std=c++11
# LIBS+=

DS_UTIL_DIR:
	mkdir -p $(PLUGIN_TARGET_DIR)/DroidScope/DS_utils

UTIL_DIR:
	mkdir -p $(PLUGIN_TARGET_DIR)/utils

DS_LINAPI_DIR:
	mkdir -p $(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI

DS_DIR:
	mkdir -p $(PLUGIN_TARGET_DIR)/DroidScope

# make the subdirs of the plugin director
#$(PLUGIN_TARGET_DIR)/DECAF_linux_vmi.o
#$(PLUGIN_TARGET_DIR)/DECAF_main.o
#$(PLUGIN_TARGET_DIR)/linux_vmi_main.o
$(PLUGIN_TARGET_DIR)/DroidScope/DS_Init.o: DS_DIR
$(PLUGIN_TARGET_DIR)/DroidScope/DS_utils/SymbolMap.o: DS_UTIL_DIR
$(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI/Context.o: DS_LINAPI_DIR
$(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI/ModuleServer.o: DS_LINAPI_DIR
$(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI/ProcessInfo.o: DS_LINAPI_DIR
$(PLUGIN_TARGET_DIR)/utils/TULStringMapWrapper.o: UTIL_DIR
$(PLUGIN_TARGET_DIR)/utils/OutputWrapper.o: UTIL_DIR
$(PLUGIN_TARGET_DIR)/utils/HelperFunctions.o: UTIL_DIR

O_FILES= $(PLUGIN_TARGET_DIR)/DECAF_linux_vmi.o
O_FILES+=$(PLUGIN_TARGET_DIR)/DECAF_main.o
O_FILES+=$(PLUGIN_TARGET_DIR)/linux_vmi_main.o
O_FILES+=$(PLUGIN_TARGET_DIR)/DroidScope/DS_Init.o
O_FILES+=$(PLUGIN_TARGET_DIR)/DroidScope/DS_utils/SymbolMap.o
O_FILES+=$(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI/Context.o
O_FILES+=$(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI/ModuleServer.o
O_FILES+=$(PLUGIN_TARGET_DIR)/DroidScope/linuxAPI/ProcessInfo.o
O_FILES+=$(PLUGIN_TARGET_DIR)/utils/TULStringMapWrapper.o
O_FILES+=$(PLUGIN_TARGET_DIR)/utils/OutputWrapper.o
O_FILES+=$(PLUGIN_TARGET_DIR)/utils/HelperFunctions.o

$(PLUGIN_TARGET_DIR)/panda_$(PLUGIN_NAME).so: $(O_FILES)
	$(call quiet-command,$(CXX) $(QEMU_CFLAGS) -shared -o $@ $^ $(LIBS),"  PLUGIN  $@")

all: $(PLUGIN_TARGET_DIR)/panda_$(PLUGIN_NAME).so
