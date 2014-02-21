SOURCES = EV3_Input.cpp EV3_Sound.cpp EV3_Output.cpp EV3_Ui.cpp EV3_Entry.cpp EV3_File.cpp EV3_Com.cpp

TARGET = libvireobridge.so

CONF = Linuxlib
ARCH = AM1808

# include ../../open_first/rules.mk
# Part of rules.mk is used below with modifications for C++ source and linking against libvireo.so

#
# ARCH defines
#

ifeq ($(ARCH),X86)
CROSS_COMPILE =
else ifeq ($(ARCH),AM1808)
CROSS_COMPILE = arm-none-linux-gnueabi-
else
$(error unknown ARCH)
endif

#
# Libraries and programs.
#

ifneq ($(filter Linux Linuxlib,$(CONF)),)

BASE = ../..

OBJS = $(SOURCES:%.cpp=%.o)
DEPS = $(OBJS:%.o=%.d)

INCLUDES = -I$(BASE)/lms2012/source \
           -I$(BASE)/c_com/source \
           -I$(BASE)/c_input/source \
           -I$(BASE)/c_memory/source \
           -I$(BASE)/c_output/source \
           -I$(BASE)/c_sound/source \
           -I$(BASE)/c_ui/source \
           -I$(BASE)/c_vireobridge/source \
           -I$(BASE)/c_vireobridge/includeVireo \
           -I$(BASE)/c_dynload/source

ifeq ($(ARCH),X86)
DBUS_CFLAGS := $(shell pkg-config dbus-1 --cflags)
CFLAGS = -DLinux_X86 $(INCLUDES) $(DBUS_CFLAGS) -Os -g3 -Wall -fPIC
LDFLAGS = -L$(BASE)/lms2012/Linux_$(ARCH)/sys/lib
else
DEVKIT = $(BASE)/../extra/linux-devkit/arm-none-linux-gnueabi
INCLUDES += -I$(DEVKIT)/usr/include/dbus-1.0
INCLUDES += -I$(DEVKIT)/usr/lib/dbus-1.0/include
INCLUDES += -I$(DEVKIT)/usr/include
CFLAGS = -DPCASM $(INCLUDES) -Os -Wall -fPIC
# LDFLAGS = -L$(BASE)/lms2012/Linux_$(ARCH)/sys/lib -L$(DEVKIT)/usr/lib
LDFLAGS = -L. -lvireo -Wl,-rpath=/home/root/lms2012/3rdparty-vm
endif

ifeq ($(CONF),Linuxlib)
LDFLAGS += -shared
INSTALL_DIR = sys/lib
else
INSTALL_DIR = sys
VPATH = sys/lib
endif

INSTALL_TARGET = $(BASE)/lms2012/Linux_$(ARCH)/$(INSTALL_DIR)/$(TARGET)

all: install

%.o: ../source/%.cpp
	$(CROSS_COMPILE)g++ $(CFLAGS) -c -MMD -MP -o $@ $<

$(TARGET): $(OBJS) $(filter -lc_%,$(LIBS))
	$(CROSS_COMPILE)g++ $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

install: $(INSTALL_TARGET)
$(INSTALL_TARGET): $(TARGET)
	mkdir -p $(dir $@) && cp $< $@ && cp libvireo.so $(BASE)/lms2012/Linux_$(ARCH)/$(INSTALL_DIR)

$(SUBDIRS:%=-l%): FORCE
	$(MAKE) -C ../../$(@:-l%=%)/Linuxlib_$(ARCH) install

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

uninstall:
	rm -f $(INSTALL_TARGET)

.PHONY: all install clean uninstall FORCE
FORCE:

-include $(DEPS)

endif

