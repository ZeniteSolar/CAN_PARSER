CC = gcc

SOURCES = can_ids.h can_parser.h tests.c
TARGET = tests

$(TARGET) : $(SOURCES)
	$(CC)  $(SOURCES) -I./ -o $(TARGET)

can_ids.h : .lib_version
	

can_parse.h : .lib_version
	

.PHONY update:

.lib_version : update
	./update_libs.sh

