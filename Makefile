# 2D Physics Engine Makefile
# Cross-platform build system for Windows and Linux

# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    EXE_EXT := .exe
    PATH_SEP := \\
    RM := del /Q
    MKDIR := mkdir
else
    DETECTED_OS := $(shell uname -s)
    EXE_EXT :=
    PATH_SEP := /
    RM := rm -f
    MKDIR := mkdir -p
endif

# Project information
PROJECT_NAME := 2DPhysicsEngine
VERSION := 1.0.0
TARGET := $(PROJECT_NAME)$(EXE_EXT)

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin
LIB_DIR := lib
VENDOR_DIR := vendor
ASSETS_DIR := assets
OBJ_DIR := $(BUILD_DIR)/obj

# Compiler and flags
CXX := g++
CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
CFLAGS := -std=c11 -Wall -Wextra -O2
DEBUG_FLAGS := -g -DDEBUG -O0
RELEASE_FLAGS := -DNDEBUG -O3

# Include paths
INCLUDES := -I$(INCLUDE_DIR) -I$(VENDOR_DIR)/SDL2/include

# Library paths and libraries
LDFLAGS := -L$(LIB_DIR)
ifeq ($(DETECTED_OS),Windows)
    LIBS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
else
    LIBS := -lSDL2main -lSDL2 -lSDL2_image -lm
endif

# Source files
CPP_SOURCES := $(shell find $(SRC_DIR) -name "*.cpp" 2>/dev/null || find $(SRC_DIR) -name "*.cpp")
C_SOURCES := $(shell find $(SRC_DIR) -name "*.c" 2>/dev/null || find $(SRC_DIR) -name "*.c")

# Object files
CPP_OBJECTS := $(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
C_OBJECTS := $(C_SOURCES:%.c=$(OBJ_DIR)/%.o)
OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

# Multiple executables configuration
MAIN_TARGET := $(PROJECT_NAME)$(EXE_EXT)
DEMO_TARGET := demo$(EXE_EXT)
TEST_TARGET := test$(EXE_EXT)

# Main sources (exclude other main files)
MAIN_SOURCES := $(filter-out $(SRC_DIR)/demo_main.cpp $(SRC_DIR)/test_main.cpp, $(CPP_SOURCES))
DEMO_SOURCES := $(filter-out $(SRC_DIR)/Application.cpp $(SRC_DIR)/test_main.cpp, $(CPP_SOURCES))
TEST_SOURCES := $(filter-out $(SRC_DIR)/Application.cpp $(SRC_DIR)/demo_main.cpp, $(CPP_SOURCES))

# Object files for each target
MAIN_OBJECTS := $(MAIN_SOURCES:%.cpp=$(OBJ_DIR)/%.o) $(C_OBJECTS)
DEMO_OBJECTS := $(DEMO_SOURCES:%.cpp=$(OBJ_DIR)/%.o) $(C_OBJECTS)
TEST_OBJECTS := $(TEST_SOURCES:%.cpp=$(OBJ_DIR)/%.o) $(C_OBJECTS)

# Build modes
BUILD_MODE ?= release

ifeq ($(BUILD_MODE),debug)
    CXXFLAGS += $(DEBUG_FLAGS)
    CFLAGS += $(DEBUG_FLAGS)
    TARGET := $(PROJECT_NAME)_debug$(EXE_EXT)
else
    CXXFLAGS += $(RELEASE_FLAGS)
    CFLAGS += $(RELEASE_FLAGS)
endif

# Colors for output
ifeq ($(DETECTED_OS),Windows)
    # Windows doesn't support ANSI colors in standard cmd
    COLOR_RESET :=
    COLOR_GREEN :=
    COLOR_BLUE :=
    COLOR_YELLOW :=
    COLOR_RED :=
else
    COLOR_RESET := \033[0m
    COLOR_GREEN := \033[32m
    COLOR_BLUE := \033[34m
    COLOR_YELLOW := \033[33m
    COLOR_RED := \033[31m
endif

# Default target
.PHONY: all
all: main

# Create directories
$(OBJ_DIR):
	@echo "$(COLOR_BLUE)Creating object directory...$(COLOR_RESET)"
	@$(MKDIR) $(OBJ_DIR)

$(BIN_DIR):
	@echo "$(COLOR_BLUE)Creating binary directory...$(COLOR_RESET)"
	@$(MKDIR) $(BIN_DIR)

# Compile C++ source files
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "$(COLOR_GREEN)Compiling C++: $<$(COLOR_RESET)"
	@$(MKDIR) $(dir $@) 2>/dev/null || true
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "$(COLOR_GREEN)Compiling C: $<$(COLOR_RESET)"
	@$(MKDIR) $(dir $@) 2>/dev/null || true
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link main executable
$(BIN_DIR)/$(MAIN_TARGET): $(MAIN_OBJECTS) | $(BIN_DIR)
    @echo "$(COLOR_BLUE)Linking main executable: $(MAIN_TARGET)$(COLOR_RESET)"
    $(CXX) $(MAIN_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Link demo executable  
$(BIN_DIR)/$(DEMO_TARGET): $(DEMO_OBJECTS) | $(BIN_DIR)
    @echo "$(COLOR_BLUE)Linking demo executable: $(DEMO_TARGET)$(COLOR_RESET)"
    $(CXX) $(DEMO_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Link test executable
$(BIN_DIR)/$(TEST_TARGET): $(TEST_OBJECTS) | $(BIN_DIR)
    @echo "$(COLOR_BLUE)Linking test executable: $(TEST_TARGET)$(COLOR_RESET)"
    $(CXX) $(TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Debug build
.PHONY: debug
debug:
	@$(MAKE) BUILD_MODE=debug

# Release build
.PHONY: release
release:
	@$(MAKE) BUILD_MODE=release

# Run targets
.PHONY: run-main run-demo run-test
run-main: main
    @echo "$(COLOR_YELLOW)Running main application...$(COLOR_RESET)"
    @cd $(BIN_DIR) && ./$(MAIN_TARGET)

run-demo: demo
    @echo "$(COLOR_YELLOW)Running demo...$(COLOR_RESET)"
    @cd $(BIN_DIR) && ./$(DEMO_TARGET)

run-test: test
    @echo "$(COLOR_YELLOW)Running tests...$(COLOR_RESET)"
    @cd $(BIN_DIR) && ./$(TEST_TARGET)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "$(COLOR_RED)Cleaning build artifacts...$(COLOR_RESET)"
ifeq ($(DETECTED_OS),Windows)
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@if exist $(BIN_DIR)\$(PROJECT_NAME)*.exe del /q $(BIN_DIR)\$(PROJECT_NAME)*.exe
else
	@$(RM) -r $(BUILD_DIR) 2>/dev/null || true
	@$(RM) $(BIN_DIR)/$(PROJECT_NAME)* 2>/dev/null || true
endif
	@echo "$(COLOR_GREEN)Clean complete.$(COLOR_RESET)"

# Install (copy executable to system path - requires admin/sudo)
.PHONY: install
install: $(BIN_DIR)/$(TARGET)
	@echo "$(COLOR_BLUE)Installing $(TARGET)...$(COLOR_RESET)"
ifeq ($(DETECTED_OS),Windows)
	@echo "Please manually copy $(BIN_DIR)/$(TARGET) to your desired location"
else
	sudo cp $(BIN_DIR)/$(TARGET) /usr/local/bin/
endif
	@echo "$(COLOR_GREEN)Installation complete.$(COLOR_RESET)"

# Display help
.PHONY: help
help:
    @echo "2D Physics Engine Build System"
    @echo "=============================="
    @echo ""
    @echo "Available targets:"
    @echo "  main        - Build main application (default)"
    @echo "  demo        - Build demo application"
    @echo "  test        - Build test suite"
    @echo "  all-targets - Build all executables"
    @echo "  run-main    - Build and run main application"
    @echo "  run-demo    - Build and run demo"
    @echo "  run-test    - Build and run tests"
    @echo "  debug       - Build in debug mode"
    @echo "  release     - Build in release mode"
    @echo "  clean       - Remove all build artifacts"
    @echo "  help        - Show this help message"
    @echo ""
    @echo "Examples:"
    @echo "  make main           # Build main app"
    @echo "  make demo           # Build demo"
    @echo "  make run-demo       # Build and run demo"
    @echo "  make all-targets    # Build everything"

# Show project information
.PHONY: info
info:
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Target: $(BIN_DIR)/$(TARGET)"
	@echo "OS: $(DETECTED_OS)"
	@echo "Compiler: $(CXX)"
	@echo "Build Mode: $(BUILD_MODE)"
	@echo "Sources: $(words $(CPP_SOURCES)) C++ files, $(words $(C_SOURCES)) C files"

# Dependencies target (for advanced dependency tracking)
.PHONY: deps
deps:
	@echo "$(COLOR_BLUE)Checking dependencies...$(COLOR_RESET)"
	@echo "SDL2 development libraries required"
	@echo "On Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-image-dev"
	@echo "On Fedora: sudo dnf install SDL2-devel SDL2_image-devel"
	@echo "On macOS: brew install sdl2 sdl2_image"
	@echo "On Windows: Download SDL2 development libraries from libsdl.org"
