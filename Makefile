CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
ROLES_DIR = roles
TEST_DIR = test
BUILD_DIR = build
DOCTEST_DIR = test

MAIN_TARGET = main
TEST_TARGET = test_runner

# All source files for main (include everything except GUI if needed)
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
ROLE_SRC_FILES := $(wildcard $(SRC_DIR)/$(ROLES_DIR)/*.cpp)

# For main build, include all source files except GUI
# Assuming GUI sources are in src/GUI.cpp or src/GUI/*.cpp - exclude them here
# Adjust the exclusion pattern as needed:
SRC_FILES_NO_GUI := $(filter-out $(SRC_DIR)/GUI.cpp,$(SRC_FILES))
# If GUI files in src/GUI/*, exclude them too (optional)
# ROLE_SRC_FILES_NO_GUI := $(filter-out $(SRC_DIR)/$(ROLES_DIR)/GUI%.cpp,$(ROLE_SRC_FILES))

MAIN_SOURCES := $(SRC_FILES) $(ROLE_SRC_FILES)

MAIN_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MAIN_SOURCES))

# Test depends only on minimal sources your tests need:
TEST_DEPENDENT_SRCS := $(SRC_DIR)/game.cpp $(ROLE_SRC_FILES)
TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_DEPENDENT_SRCS))

# Test source files
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

# Create build directory if not exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files for main build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(SRC_DIR)/$(ROLES_DIR) -c $< -o $@

$(BUILD_DIR)/$(ROLES_DIR)/%.o: $(SRC_DIR)/$(ROLES_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(SRC_DIR)/$(ROLES_DIR) -c $< -o $@

# Build main executable
$(MAIN_TARGET): $(MAIN_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

# Compile source files for test build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(SRC_DIR)/$(ROLES_DIR) -c $< -o $@

$(BUILD_DIR)/$(ROLES_DIR)/%.o: $(SRC_DIR)/$(ROLES_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(SRC_DIR)/$(ROLES_DIR) -c $< -o $@

# Build tests executable: only with needed objects + tests
$(TEST_TARGET): $(TEST_OBJECTS) $(TEST_FILES)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(SRC_DIR)/$(ROLES_DIR) -I$(DOCTEST_DIR) $^ -o $@

# Run main executable
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

Main: $(MAIN_TARGET)

# Run valgrind on main executable
valgrind: $(MAIN_TARGET)
	valgrind --leak-check=full ./$(MAIN_TARGET)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean everything
clean:
	rm -rf $(BUILD_DIR) $(MAIN_TARGET) $(TEST_TARGET)

.PHONY: all run valgrind test clean

# Default target
all: $(MAIN_TARGET)