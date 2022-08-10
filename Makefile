CC = gcc

# Compiler flags to help with better code quality
CFLAGS = -Wall -Wextra
# CFLAGS += -Wconversion
CFLAGS += -Wshadow
CFLAGS += -Wunsafe-loop-optimizations
CFLAGS += -Waddress
CFLAGS += -Wlogical-op
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wold-style-definition
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wmissing-declarations
CFLAGS += -Wpacked
CFLAGS += -Wundef
CFLAGS += -Wredundant-decls
#CFLAGS += -Wnested-externs
CFLAGS += -Wunreachable-code
# CFLAGS += -Winline
CFLAGS += -Woverlength-strings
CFLAGS += -Wpointer-sign
CFLAGS += -Wdisabled-optimization
CFLAGS += -Wvariadic-macros
CFLAGS += -fstrict-aliasing
CFLAGS += -Wstrict-overflow=5 -fstrict-overflow
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
# CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wformat=2
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -std=gnu18
CFLAGS += -Werror=switch
CFLAGS += -Werror=implicit-fallthrough=4


can_ids.h : .lib_version
	
can_parse.h : .lib_version
	
.PHONY : tests 

SOURCES = can_ids.h can_parser.h tests.c
TARGET = tests

$(TARGET) : $(SOURCES)
	$(CC)  $(SOURCES) -I./ $(CFLAGS) -o $(TARGET)


.PHONY update:

.lib_version : update
	./update_libs.sh

