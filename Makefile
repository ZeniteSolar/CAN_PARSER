SILENT = @
CC = gcc
AR = ar

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
CFLAGS += -Wno-missing-braces
CFLAGS += -Wmissing-field-initializers
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

BIN_DIR = bin/
SOURCES_DIR = src/
INCLUDES_DIR = inc/
SUBDIRS = tests/

BINARY = libcan_parser.a
SOURCES = $(shell find ./$(SOURCES_DIR) -type f -name \*.c)
OBJS    =   $(patsubst $(SOURCES_DIR)/%,$(BIN_DIR)/%,$(SOURCES:.c=.o))
INCLUDES = $(INCLUDES_DIR)/$(shell find ./inc -type f -name \*.h)

LIB_VERSION_FILE = .lib_version

$(BIN_DIR)/$(BINARY) : $(OBJS)
	mkdir -p $(BIN_DIR)
	$(SILENT) $(AR) rcs $@ $<

$(OBJS) : $(SOURCES) $(INCLUDES)
	$(SILENT) $(CC) -c $< $(CFLAGS) -I./$(INCLUDES_DIR) -o $@

#download include files
$(INCLUDES) : $(LIB_VERSION_FILE)
	./update_libs.sh

.PHONY: clean

clean :
	$(SILENT) for i in $(SUBDIRS);\
	do make -C $$i clean; done
	rm -f $(OBJS)
	rm -f $(BIN_DIR)/$(BINARY)

.PHONY: tests

tests : $(BIN_DIR)/$(BINARY)
	$(SILENT) make -C tests/

.PHONY: all
all : $(BIN_DIR)/$(BINARY)
	$(SILENT) for i in $(SUBDIRS);\
	do make -C $$i; done