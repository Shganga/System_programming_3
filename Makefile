CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
BUILD_DIR = build
ROLES_DIR = roles

# All .cpp files directly in src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)

# All .cpp files recursively inside src/roles
ROLE_SRC_FILES := $(shell find $(SRC_DIR)/$(ROLES_DIR) -name '*.cpp')

# Combine all source files
SOURCES := $(SRC_FILES) $(ROLE_SRC_FILES)

# Convert all source files to object files in build/ preserving directory structure
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

TARGET = game_setup

all: $(TARGET)

# Create build directory and subdirs as needed
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile each .cpp to corresponding .o in build/, creating subdirs if needed
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(SFML_LIBS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./game_setup



.PHONY: all clean run
