SILENT = @
CC = gcc

# # Compiler flags to help with better code quality
# CFLAGS = -Wall -Wextra
# # CFLAGS += -Wconversion
# CFLAGS += -Wshadow
# CFLAGS += -Wunsafe-loop-optimizations
# CFLAGS += -Waddress
# CFLAGS += -Wlogical-op
# CFLAGS += -Wstrict-prototypes
# CFLAGS += -Wold-style-definition
# CFLAGS += -Wmissing-prototypes
# CFLAGS += -Wno-missing-braces
# CFLAGS += -Wmissing-field-initializers
# CFLAGS += -Wmissing-declarations
# CFLAGS += -Wpacked
# CFLAGS += -Wundef
# CFLAGS += -Wredundant-decls
# #CFLAGS += -Wnested-externs
# CFLAGS += -Wunreachable-code
# # CFLAGS += -Winline
# CFLAGS += -Woverlength-strings
# CFLAGS += -Wpointer-sign
# CFLAGS += -Wdisabled-optimization
# CFLAGS += -Wvariadic-macros
# CFLAGS += -fstrict-aliasing
# CFLAGS += -Wstrict-overflow=5 -fstrict-overflow
# CFLAGS += -funsigned-char
# CFLAGS += -funsigned-bitfields
# # CFLAGS += -fpack-struct
# CFLAGS += -fshort-enums
# CFLAGS += -Wformat=2
# CFLAGS += -ffunction-sections
# CFLAGS += -fdata-sections
# CFLAGS += -std=gnu18
# CFLAGS += -Werror=switch
# CFLAGS += -Werror=implicit-fallthrough=4

BINARY = $(patsubst ./%,./%,$(SOURCES:.c=))
SOURCES = $(shell find ./ -type f -name \*.c)
OBJS = $(patsubst ./%,./%,$(SOURCES:.c=.o))
LIBS_DIR = ../bin
CAN_LIB = can_parser

$(BINARY) : $(SOURCES)
	$(SILENT) $(CC) $< $(CFLAGS) -I../inc/ -L$(LIBS_DIR) -l$(CAN_LIB) -o $@ 

.PHONY: clean
clean:
	rm -f $(BINARY)