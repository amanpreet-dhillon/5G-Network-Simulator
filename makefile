# 1. Detect OS
ifeq ($(OS),Windows_NT)
    # Windows Settings
    DETECTED_OS := Windows
    mkdir_cmd = if not exist $(1) mkdir $(1)
    rm_cmd = if exist $(1) rmdir /s /q $(1)
    EXE_EXT := .exe
    # Use ./ only if running in a shell that requires it (like Git Bash), 
    # but for pure CMD/PowerShell, $(TARGET) is usually fine.
    RUN_PREFIX := 
else
    # Linux/WSL/macOS Settings
    DETECTED_OS := Linux
    mkdir_cmd = mkdir -p $(1)
    rm_cmd = rm -rf $(1)
    EXE_EXT :=
    RUN_PREFIX := ./
endif

# 2. Compiler and Flags
CXX := g++
CXXFLAGS := -Wall -std=c++20

# 3. Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
BIN_DIR := bin

# 4. Target executable name
TARGET := $(BIN_DIR)/5G_simulator$(EXE_EXT)

# 5. Files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: run

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking objects for $(DETECTED_OS)..."
	$(CXX) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# 6. Commands using the OS variables
$(BUILD_DIR):
	@$(call mkdir_cmd, $(BUILD_DIR))

$(BIN_DIR):
	@$(call mkdir_cmd, $(BIN_DIR))

run: $(TARGET)
	@echo "Running 5G simulator on $(DETECTED_OS)..."
	@$(RUN_PREFIX)$(TARGET)

clean:
	@echo "Cleaning $(DETECTED_OS)..."
	@$(call rm_cmd, $(BUILD_DIR))
	@$(call rm_cmd, $(BIN_DIR))

.PHONY: all clean rebuild run