# Project Name: Optimization
# Author: Chandler Scott
# Description: Code optimization

# Variables
BUILD_TOOL := gcc
SRC_DIR := ./src
BUILD_DIR := ./build

# Automatically find all .c files in src/
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# Generate a list of executables (one per .c file)
TARGETS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%, $(SOURCES))

.PHONY: all clean setup profile profile-gprof profile-perf

# Default target: build everything
all: setup $(TARGETS)

setup:
	mkdir -p $(BUILD_DIR)

# Build each binary from its .c file
$(BUILD_DIR)/%: $(SRC_DIR)/%.c
	$(BUILD_TOOL) $< -O0 -pg -o $@

# Profiling with gprof (output to terminal)
profile: $(TARGETS)
	@echo "Running with gprof profiling..."
	@set -e; \
	for binary in $(TARGETS); do \
		name=$$(basename "$$binary"); \
		echo ""; \
		echo "=== Profiling $$name ==="; \
		"$$binary" 1000000 >/dev/null 2>&1 || true; \
		if [ -f gmon.out ]; then \
			gprof "$$binary" gmon.out; \
			rm -f gmon.out; \
		else \
			echo "No gmon.out produced by $$name (did it run?)"; \
		fi; \
	done



clean:
	rm -rf $(BUILD_DIR)
	rm -f gmon.out *.profile.txt
