# Project Name: Optimization
# Author: Chandler Scott
# Description: Code optimization

BUILD_TOOL := gcc
SRC_DIR    := ./src
BUILD_DIR  := ./build

# --- Flags ---
CFLAGS_NORMAL := -O0 -g -Wall -Wextra
CFLAGS_PROF   := -O0 -g -pg -Wall -Wextra
LDFLAGS       :=

# Automatically find all .c files in src/
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# One executable per .c file
TARGETS       := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%,$(SOURCES))
TARGETS_PROF  := $(patsubst $(BUILD_DIR)/%,$(BUILD_DIR)/%-prof,$(TARGETS))

.PHONY: all clean setup profile profile-gprof profile-perf

# Default target: build everything (normal + prof)
all: setup $(TARGETS) $(TARGETS_PROF)

setup:
	@mkdir -p $(BUILD_DIR)

# Normal binaries (no -pg)
$(BUILD_DIR)/%: $(SRC_DIR)/%.c
	$(BUILD_TOOL) $(CFLAGS_NORMAL) $< -o $@ $(LDFLAGS)

# Profiling binaries (with -pg) -> suffix "-prof"
$(BUILD_DIR)/%-prof: $(SRC_DIR)/%.c
	$(BUILD_TOOL) $(CFLAGS_PROF) $< -o $@ $(LDFLAGS)

# Profiling with gprof (output to terminal)
# Runs each -prof binary once, collects the newest per-binary gmon, and prints a report.
profile: $(TARGETS_PROF)
	@echo "Running with gprof profiling..."
	@set -e; \
	for binary in $(TARGETS_PROF); do \
		name=$$(basename "$$binary"); \
		echo ""; \
		echo "=== Profiling $$name ==="; \
		GMON_OUT_PREFIX="gmon-$$name" "$$binary" 1000000 >/dev/null 2>&1 || true; \
		gmon=$$(ls -t gmon-$$name.* 2>/dev/null | head -n1); \
		if [ -n "$$gmon" ]; then \
			gprof "$$binary" "$$gmon"; \
			rm -f "$$gmon"; \
		else \
			echo "No gmon output produced by $$name (did it run and exit normally?)"; \
		fi; \
	done


clean:
	rm -rf $(BUILD_DIR)
	rm -f gmon.out gmon-*-*

