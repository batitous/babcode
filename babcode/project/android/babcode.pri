# Babcode project file for Android NDK
#
# The symbol BABCODE_PATH must be defined in the project that include this file.
#
# How to add it in your Qt project:
# BABCODE_PATH = ../../Baptiste/projects/babcode/babcode
# include($$BABCODE_PATH/project/android/babcode.pri)
#


INCLUDEPATH +=  $$BABCODE_PATH/include \
                $$BABCODE_PATH/source \
                $$BABCODE_PATH/platform/unix \
                $$BABCODE_PATH/platform/linux


HEADERS +=  $$BABCODE_PATH/include/babcode.h

# babcode source commom to all platform
SOURCES +=  $$BABCODE_PATH/source/array.c \
            $$BABCODE_PATH/source/bytestream.c \
            $$BABCODE_PATH/source/conversion.c \
            $$BABCODE_PATH/source/crc.c \
            $$BABCODE_PATH/source/hashtable.c \
            $$BABCODE_PATH/source/list.c \
            $$BABCODE_PATH/source/log.c \
            $$BABCODE_PATH/source/networkstack-options.c \
            $$BABCODE_PATH/source/networkstack-tcp.c \
            $$BABCODE_PATH/source/networkstack-udp.c \
            $$BABCODE_PATH/source/random.c \
            $$BABCODE_PATH/source/randomLfsr.c \
            $$BABCODE_PATH/source/str.c \
            $$BABCODE_PATH/source/time.c \
            $$BABCODE_PATH/source/timer.c \
            $$BABCODE_PATH/source/utils.c


SOURCES +=  $$BABCODE_PATH/platform/linux/networkstack_linux.c \
            $$BABCODE_PATH/platform/unix/conditionvar.c \
            $$BABCODE_PATH/platform/unix/file.c \
            $$BABCODE_PATH/platform/unix/mutex.c \
            $$BABCODE_PATH/platform/unix/thread.c \
            $$BABCODE_PATH/platform/unix/wait.c

# currently android ndk dont support cfsetspeed
#            $$BABCODE_PATH/platform/unix/uart_unix.c \
#            $$BABCODE_PATH/platform/linux/uart.c \
