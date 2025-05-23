#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

// Forward declarations
class Button;
class TextInput;

// Button class for clickable GUI elements
class Button {
public:
    sf::RectangleShape shape;
    sf::Text buttonText;
    bool isHovered;
    
    Button(float x, float y, float width, float height, const std::string& text, sf::Font& font);
    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void update(sf::Vector2i mousePos);
    void setSelected(bool selected);
};

// TextInput class for text entry fields
class TextInput {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::RectangleShape cursor;
    sf::Clock cursorClock;
    std::string inputString;
    bool isActive;
    bool showCursor;
    
public:
    TextInput(float x, float y, float width, float height, sf::Font& font);
    void draw(sf::RenderWindow& window);
    void update(sf::Vector2i mousePos);
    bool isClicked(sf::Vector2i mousePos);
    void setActive(bool active);
    void handleTextInput(char inputChar);
    std::string getText() const;
    void clear();
};

// Main GUI class for game setup
class GameSetupGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;
    
    enum Screen {
        PLAYER_COUNT_SELECTION,
        PLAYER_NAMES_INPUT
    };
    
    Screen currentScreen;
    int selectedPlayerCount;
    
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<sf::Text> labels;
    std::vector<std::unique_ptr<TextInput>> playerInputs;
    
    void setupPlayerCountScreen();
    void setupPlayerNamesScreen();
    void handleEvents();
    void handleMouseClick(sf::Vector2i mousePos);
    void update();
    void render();
    void startGame();
    bool loadFont();
    
public:
    GameSetupGUI();
    void run();
    std::vector<std::string> getPlayerNames() const;
    int getPlayerCount() const { return selectedPlayerCount; }
};

#endif // GUI_HPP