# 
# X86 Linux Babcode Makefile
#

# Application's name
EXE=./libbabcode-linux-x86.a

# File source list: Add your source!
SRC_FILES=conversion.c log.c random.c time.c utils.c
SRC_FILES+= networkstack-udp.c networkstack-tcp.c networkstack-options.c	
SRC_FILES+= file.c thread.c wait.c mutex.c conditionvar.c
SRC_FILES+= uart.c uart_unix.c networkstack_linux.c

# Temporary repertory
BUILD_PATH=build/

# Source repertory
SRC_PATH=../../source ../../platform/unix ../../platform/linux

SOURCE_FILES =$(SRC_FILES)

# Library define
# -Dxxx=1
COMPILER_DEFINES=

# Include search path
# -IInclude/
BAB_INCLUDE=

# Path to Gcc compiler:
PATH_TO_GCC=

# Gcc Compiler
GCC_COMPILER=gcc

# Compiler options
COMPILER_OPTION=-O2 -Wall


# Archiver 
AR=ar
LINKER_OPTION=-rv 

## Compiler/linker macro:

# .c source macro:
COMPILER=$(GCC_COMPILER) $(COMPILER_OPTION) $(COMPILER_DEFINES) $(BAB_INCLUDE) -c

# .o linker macro:
LINKER=$(AR) $(LINKER_OPTION) -o $(EXE) 

# .o search path:
OBJ_FILES =$(patsubst %.c,$(BUILD_PATH)/%.o,$(SOURCE_FILES))

### Make target:

# build application: 
all:  $(OBJ_FILES)
	$(LINKER) $(OBJ_FILES) $(LIB_USE)


# source search path:
vpath %.c  . $(SRC_PATH)

# building .o
$(BUILD_PATH)/%.o : %.c
	$(COMPILER) -o $@ $<
	
clean:
	rm -f $(BUILD_PATH)/*.o
	
##### END OF FILE
