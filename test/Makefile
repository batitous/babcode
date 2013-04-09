# 
# X86 Linux Babcode Makefile
#

# Application's name
EXE=./test

# File source list: Add your source!
SRC_FILES=main.c

# Temporary repertory
BUILD_PATH=Build/

# Source repertory
SRC_PATH=.

SOURCE_FILES =$(SRC_FILES)

# Library define
# -Dxxx=1
COMPILER_DEFINES=

# Include search path
# -IInclude/
BAB_INCLUDE=-I../babcode/include

# Path to Gcc compiler:
PATH_TO_GCC=

# Gcc Compiler
GCC_COMPILER=gcc

# Compiler options
COMPILER_OPTION=-O2 -Wall

# Linker options
LINKER_OPTION=$(COMPILER_OPTION)

LIB_USE=-lm -lrt -L../babcode/project/linux -lbabcode-linux-x86


## Compiler/linker macro:

# .c source macro:
COMPILER=$(GCC_COMPILER) $(COMPILER_OPTION) $(COMPILER_DEFINES) $(BAB_INCLUDE) -c

# .o linker macro:
LINKER=$(GCC_COMPILER) $(LINKER_OPTION) -o $(EXE) 

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
