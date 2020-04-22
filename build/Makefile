CC := gcc
CFLAGS :=  -g -Wall -lrt

SRC_DIR = ../src

SOURCES = $(SRC_DIR)/thread_lib.c $(SRC_DIR)/main.c

BUILD_DIR = .
TARGET  = $(BUILD_DIR)/test

target: $(TARGET)

$(TARGET): $(SOURCES)
	@echo $(SOURCES)
	$(CC) $? -o $@ $(CFLAGS)

.phony: target
