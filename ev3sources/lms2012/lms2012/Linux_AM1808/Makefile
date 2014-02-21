SOURCES = c_branch.c c_compare.c c_math.c c_move.c c_timer.c \
	  lms2012.c validate.c 
LIBS = -lrt -lusb-1.0 -ldbus-1 -lbluetooth -lm -ldl \
       -lc_com -lc_input -lc_memory -lc_output -lc_sound -lc_ui -lc_dynload 
SUBDIRS = c_com c_input c_memory c_output c_sound c_ui c_vireobridge c_dynload c_robotcvm

TARGET = lms2012

CONF = Linux
ARCH = AM1808

include ../../open_first/rules.mk
