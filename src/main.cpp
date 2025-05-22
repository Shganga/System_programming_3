#include "GUI.hpp"
#include <iostream>

int main() {
    try {
        GameSetupGUI gameSetup;
        gameSetup.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}