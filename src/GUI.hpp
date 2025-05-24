#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "game.hpp"

// Forward declarations
class Button;
class TextInput;

// Button class for clickable GUI elements
class Button {
public:
    sf::RectangleShape shape;
    sf::Text buttonText;
    bool isHovered;
    sf::Color originalColor = sf::Color::Transparent;
    sf::Color hoverColor = sf::Color::Transparent;


    
    void updateHover(sf::Vector2i mousePos);
    
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
    Game _game;
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;
    std::vector<sf::RectangleShape> playerBoxes;
    
    
    enum Screen {
        PLAYER_COUNT_SELECTION,
        PLAYER_NAMES_INPUT,
        GAME_SCREEN
    };
    
    Screen currentScreen;
    int selectedPlayerCount;
    
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<sf::Text> labels;
    std::vector<std::unique_ptr<TextInput>> playerInputs;
    
    void setupPlayerCountScreen();
    void setupPlayerNamesScreen();
    void setupGameScreen(std::string message);
    void handleEvents();
    void handleMouseClick(sf::Vector2i mousePos);
    void update();
    void render();
    void startGame();
    bool loadFont();
    void handleGameAction(size_t buttonIndex);
    
public:
    GameSetupGUI();
    void run();
    std::vector<std::string> getPlayerNames() const;
    int getPlayerCount() const { return selectedPlayerCount; }
};

#endif // GUI_HPP