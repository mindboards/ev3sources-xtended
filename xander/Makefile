# This is the Vpath; source directory is in the src folder - all the .c files
#folder structure
#-->src/*.c
#-->include/*.h
#-->icons/*.xbm
VPATH = src:include:objects

# tools used for compiling
TARGET		= ev3demo
TOOLPREFIX      = arm-none-linux-gnueabi
SHELL           = /bin/sh
CC              = $(TOOLPREFIX)-gcc
LD              = $(TOOLPREFIX)-ld
AR              = $(TOOLPREFIX)-ar
DEFINES         = -DPCASM -DDEBUG -DBUFPRINTSIZE -DDEBUG_TRACE_MODE_CHANGE -DDISABLE_DAISYCHAIN_COM_CALL -DDISABLE_DAISYCHAIN
CFLAGS          = $(DEFINES) -fPIC -g -I ./include -I ./icons -Wall 

target: $(patsubst ./src/%.c,./obj/%.o,$(wildcard ./src/*.c)) 
	@echo "Linking $(TARGET)"
	@$(CC) $(patsubst ./src/%.c,./obj/%.o,$(wildcard ./src/*.c)) $(LIBS) -o $(TARGET)

./obj/%.o: ./src/%.c ./include/*.h
	@echo "Compiling: $<"
	@mkdir -p ./obj
	@$(CC) $(CFLAGS) -c $< -o $@
	
#It is always better to write a PHONY rule for a rules like clean.

.PHONY: clean 
clean: 
	@echo "cleaning everything"
	@rm -f $(TARGET)
	@rm -rf ./obj

all: 
	@echo "Building $(TARGET)"
	@make -j$(NPROCS) target	
