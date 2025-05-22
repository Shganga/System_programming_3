CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/GUI.cpp
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/GUI.o

TARGET = game_setup

all: $(TARGET)

# Create build dir if missing
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile main.o, depends on build dir
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile GUI.o, depends on build dir
$(BUILD_DIR)/GUI.o: $(SRC_DIR)/GUI.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(SFML_LIBS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
