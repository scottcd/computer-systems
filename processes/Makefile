# Project Name: Optimization
# Author: Chandler Scott
# Description: Code optimization

BUILD_TOOL := gcc
SRC_DIR    := ./src
BUILD_DIR  := ./build

# --- Flags ---
CFLAGS_NORMAL := -O0 -g -Wall -Wextra
LDFLAGS       :=

# Automatically find all .c files in src/
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# One executable per .c file
TARGETS       := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%,$(SOURCES))

.PHONY: all clean setup

# Default target: build everything (normal + prof)
all: setup $(TARGETS) 

setup:
	@mkdir -p $(BUILD_DIR)

# Normal binaries (no -pg)
$(BUILD_DIR)/%: $(SRC_DIR)/%.c
	$(BUILD_TOOL) $(CFLAGS_NORMAL) $< -o $@ $(LDFLAGS)


clean:
	rm -rf $(BUILD_DIR)
