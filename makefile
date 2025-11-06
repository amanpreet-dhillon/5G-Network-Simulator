# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -Wall -I$(INCLUDE_DIR) -std=c++17

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
BIN_DIR := bin

# Target executable name
TARGET := $(BIN_DIR)/5G_simulator

# Find all .cpp files in src/
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Replace src/filename.cpp -> build/filename.o
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default rule (build + run)
all: run

# Link object files into final executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking objects..."
	$(CXX) $(OBJS) -o $(TARGET)

# Compile .cpp into .o in /build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create necessary directories if not exist
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(BIN_DIR):
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Run the program after building
run: $(TARGET)
	@echo "Running 5G simulator..."
	@$(TARGET)

# Clean rule
clean:
	@echo "Cleaning..."
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)

# Rebuild rule
rebuild: clean all

# Phony targets
.PHONY: all clean rebuild run