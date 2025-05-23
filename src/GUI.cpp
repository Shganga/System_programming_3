#include "GUI.hpp"
#include <iostream>
#include <algorithm>
#include "roles/player_factory.hpp"

// Button implementation
Button::Button(float x, float y, float width, float height, const std::string& text, sf::Font& font) {
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(70, 175, 80)); // Green
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::Black);
    
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setStyle(sf::Text::Bold);
    
    // Center text in button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2,
        y + (height - textBounds.height) / 2 - 5
    );
    
    isHovered = false;
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(buttonText);
}

bool Button::isClicked(sf::Vector2i mousePos) {
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::update(sf::Vector2i mousePos) {
    bool wasHovered = isHovered;
    isHovered = shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    
    if (isHovered && !wasHovered) {
        shape.setFillColor(sf::Color(60, 155, 70)); // Darker green on hover
    } else if (!isHovered && wasHovered) {
        shape.setFillColor(sf::Color(70, 175, 80)); // Normal green
    }
}

void Button::setSelected(bool selected) {
    if (selected) {
        shape.setFillColor(sf::Color(50, 120, 200)); // Blue for selected
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(30, 100, 180));
    } else {
        shape.setFillColor(sf::Color(70, 175, 80)); // Green for normal
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
    }
}

// TextInput implementation
TextInput::TextInput(float x, float y, float width, float height, sf::Font& font) {
    box.setPosition(x, y);
    box.setSize(sf::Vector2f(width, height));
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color::Black);
    
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::Black);
    text.setPosition(x + 5, y + 8);
    
    cursor.setSize(sf::Vector2f(2, height - 16));
    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition(x + 5, y + 8);
    
    isActive = false;
    showCursor = false;
    cursorClock.restart();
}

void TextInput::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(text);
    
    if (isActive && showCursor) {
        window.draw(cursor);
    }
}

void TextInput::update(sf::Vector2i mousePos) {
    // Update cursor blinking
    if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
    
    // Update cursor position
    if (isActive) {
        sf::FloatRect textBounds = text.getLocalBounds();
        cursor.setPosition(box.getPosition().x + 5 + textBounds.width, box.getPosition().y + 8);
    }
}

bool TextInput::isClicked(sf::Vector2i mousePos) {
    return box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void TextInput::setActive(bool active) {
    isActive = active;
    if (active) {
        box.setOutlineColor(sf::Color::Blue);
        box.setOutlineThickness(3);
    } else {
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(2);
    }
}

void TextInput::handleTextInput(char inputChar) {
    if (!isActive) return;
    
    if (inputChar == '\b' && !inputString.empty()) { // Backspace
        inputString.pop_back();
    } else if (inputChar >= 32 && inputChar < 127 && inputString.length() < 25) { // Printable characters
        inputString += inputChar;
    }
    
    text.setString(inputString);
}

std::string TextInput::getText() const {
    return inputString;
}

void TextInput::clear() {
    inputString.clear();
    text.setString("");
}

// GameSetupGUI implementation
GameSetupGUI::GameSetupGUI() : window(sf::VideoMode(900, 700), "Game Setup - Player Selection"), 
                               currentScreen(PLAYER_COUNT_SELECTION), 
                               selectedPlayerCount(2),
                               fontLoaded(false) {
    _game = Game();
    fontLoaded = loadFont();
    setupPlayerCountScreen();
}

bool GameSetupGUI::loadFont() {
    // Ubuntu-specific font paths
    std::vector<std::string> fontPaths = {
        // Try local fonts first
        "arial.ttf",
        "assets/fonts/arial.ttf",
        "fonts/arial.ttf",
        
        // Ubuntu system fonts
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/LiberationSans-Regular.ttf",
        
        // Fallback fonts
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/opentype/noto/NotoSans-Regular.ttf",
        "/system/fonts/DroidSans.ttf"
    };
    
    for (const auto& fontPath : fontPaths) {
        if (font.loadFromFile(fontPath)) {
            std::cout << "Font loaded successfully from: " << fontPath << std::endl;
            return true;
        }
    }
    
    std::cout << "Warning: Could not load any font. Text might not display correctly." << std::endl;
    std::cout << "To fix this, install fonts with: sudo apt install fonts-dejavu-core fonts-liberation" << std::endl;
    return false;
}

void GameSetupGUI::run() {
    std::cout << "Starting Game Setup GUI..." << std::endl;
    std::cout << "Choose number of players (2-6) by clicking on the numbers." << std::endl;
    
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void GameSetupGUI::setupPlayerCountScreen() {
    buttons.clear();
    labels.clear();
    
    // Main Title
    sf::Text title;
    if (fontLoaded) {
        title.setFont(font);
    }
    title.setString("CHOOSE NUMBER OF PLAYERS");
    title.setCharacterSize(32);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    
    // Center the title
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((900 - titleBounds.width) / 2, 60);
    labels.push_back(title);
    
    // Subtitle
    sf::Text subtitle;
    if (fontLoaded) {
        subtitle.setFont(font);
    }
    subtitle.setString("Select between 2-6 players for your game");
    subtitle.setCharacterSize(20);
    subtitle.setFillColor(sf::Color(80, 80, 80));
    
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition((900 - subtitleBounds.width) / 2, 110);
    labels.push_back(subtitle);
    
    // Player count buttons (2-6) - arranged horizontally
    for (int i = 2; i <= 6; ++i) {
        float x = 150 + (i - 2) * 120; // Space them 120 pixels apart
        float y = 200;
        
        auto button = std::make_unique<Button>(x, y, 90, 90, std::to_string(i), font);
        
        // Highlight selected button
        if (i == selectedPlayerCount) {
            button->setSelected(true);
        }
        
        // Make number text larger
        button->buttonText.setCharacterSize(36);
        button->buttonText.setStyle(sf::Text::Bold);
        
        // Re-center the text after size change
        sf::FloatRect textBounds = button->buttonText.getLocalBounds();
        button->buttonText.setPosition(
            x + (90 - textBounds.width) / 2,
            y + (90 - textBounds.height) / 2 - 8
        );
        
        buttons.push_back(std::move(button));
    }
    
    // Current selection display
    sf::Text selection;
    if (fontLoaded) {
        selection.setFont(font);
    }
    selection.setString("Selected: " + std::to_string(selectedPlayerCount) + " players");
    selection.setCharacterSize(24);
    selection.setFillColor(sf::Color(50, 120, 200));
    selection.setStyle(sf::Text::Bold);
    
    sf::FloatRect selectionBounds = selection.getLocalBounds();
    selection.setPosition((900 - selectionBounds.width) / 2, 350);
    labels.push_back(selection);
    
    // Instructions
    sf::Text instruction;
    if (fontLoaded) {
        instruction.setFont(font);
    }
    instruction.setString("Click on a number above, then click NEXT to continue");
    instruction.setCharacterSize(18);
    instruction.setFillColor(sf::Color(120, 120, 120));
    
    sf::FloatRect instructionBounds = instruction.getLocalBounds();
    instruction.setPosition((900 - instructionBounds.width) / 2, 400);
    labels.push_back(instruction);
    
    // Next button - prominent blue button
    auto nextButton = std::make_unique<Button>(375, 480, 150, 60, "NEXT", font);
    nextButton->shape.setFillColor(sf::Color(50, 120, 200)); // Blue
    nextButton->buttonText.setCharacterSize(26);
    nextButton->buttonText.setStyle(sf::Text::Bold);
    
    // Re-center text in next button
    sf::FloatRect nextTextBounds = nextButton->buttonText.getLocalBounds();
    nextButton->buttonText.setPosition(
        375 + (150 - nextTextBounds.width) / 2,
        480 + (60 - nextTextBounds.height) / 2 - 5
    );
    
    buttons.push_back(std::move(nextButton));
}

void GameSetupGUI::setupPlayerNamesScreen() {
    buttons.clear();
    labels.clear();
    playerInputs.clear();
    
    // Title
    sf::Text title;
    if (fontLoaded) {
        title.setFont(font);
    }
    title.setString("ENTER PLAYER NAMES");
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((900 - titleBounds.width) / 2, 50);
    labels.push_back(title);
    
    // Subtitle
    sf::Text subtitle;
    if (fontLoaded) {
        subtitle.setFont(font);
    }
    subtitle.setString("Enter a unique name for each player");
    subtitle.setCharacterSize(18);
    subtitle.setFillColor(sf::Color(80, 80, 80));
    
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition((900 - subtitleBounds.width) / 2, 85);
    labels.push_back(subtitle);
    
    // Create input fields for each player
    float startY = 140;
    for (int i = 0; i < selectedPlayerCount; ++i) {
        float y = startY + i * 60;
        
        // Player label
        sf::Text playerLabel;
        if (fontLoaded) {
            playerLabel.setFont(font);
        }
        playerLabel.setString("Player " + std::to_string(i + 1) + ":");
        playerLabel.setCharacterSize(20);
        playerLabel.setFillColor(sf::Color::Black);
        playerLabel.setStyle(sf::Text::Bold);
        playerLabel.setPosition(250, y + 10);
        labels.push_back(playerLabel);
        
        // Input field - make it wider
        playerInputs.push_back(std::make_unique<TextInput>(380, y, 250, 40, font));
    }
    
    // Calculate button Y position based on number of players
    float buttonY = startY + selectedPlayerCount * 60 + 40;
    
    // Back button
    auto backButton = std::make_unique<Button>(250, buttonY, 120, 50, "BACK", font);
    backButton->shape.setFillColor(sf::Color(150, 150, 150)); // Gray
    backButton->buttonText.setCharacterSize(20);
    buttons.push_back(std::move(backButton));
    
    // Start Game button
    auto startButton = std::make_unique<Button>(430, buttonY, 200, 50, "START GAME", font);
    startButton->shape.setFillColor(sf::Color(220, 60, 60)); // Red
    startButton->buttonText.setCharacterSize(20);
    startButton->buttonText.setStyle(sf::Text::Bold);
    buttons.push_back(std::move(startButton));
}

void GameSetupGUI::setupGameScreen(){
    buttons.clear();
    labels.clear();
    playerInputs.clear();

    // Game title
    sf::Text title;
    if (fontLoaded) title.setFont(font);
    title.setString("COUP GAME");
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Black);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((900 - titleBounds.width) / 2, 20);
    labels.push_back(title);

    // Player display (names + coins)
    float yStart = 80;
    for (size_t i = 0; i < _game.getPlayers().size(); ++i) {
        sf::Text playerText;
        if (fontLoaded) playerText.setFont(font);
        playerText.setString(_game.getPlayers()[i]->getName() + " - Coins: " + std::to_string(_game.getPlayers()[i]->getCoins()));
        playerText.setCharacterSize(20);
        playerText.setFillColor(sf::Color::Black);
        playerText.setPosition(50, yStart + i * 35);
        labels.push_back(playerText);
    }

    // Action buttons (column 1)
    std::vector<std::string> actions = {
        "gather", "tax", "bribe", "arrest", "sanction", "coup", "ability"
    };
    float actionStartY = 80;
    for (size_t i = 0; i < actions.size(); ++i) {
        auto actionBtn = std::make_unique<Button>(600, actionStartY + i * 50, 200, 40, actions[i], font);
        actionBtn->buttonText.setCharacterSize(18);
        buttons.push_back(std::move(actionBtn));
    }

    // End turn button
    auto endTurnBtn = std::make_unique<Button>(600, actionStartY + actions.size() * 50 + 20, 200, 45, "End Turn", font);
    endTurnBtn->shape.setFillColor(sf::Color(100, 180, 100));
    endTurnBtn->buttonText.setCharacterSize(20);
    endTurnBtn->buttonText.setStyle(sf::Text::Bold);
    buttons.push_back(std::move(endTurnBtn));
}

void GameSetupGUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            handleMouseClick(mousePos);
        }
        
        if (event.type == sf::Event::TextEntered) {
            if (currentScreen == PLAYER_NAMES_INPUT) {
                char inputChar = static_cast<char>(event.text.unicode);
                for (auto& input : playerInputs) {
                    input->handleTextInput(inputChar);
                }
            }
        }
    }
}

void GameSetupGUI::handleMouseClick(sf::Vector2i mousePos) {
    if (currentScreen == PLAYER_COUNT_SELECTION) {
        // Check player count buttons (2-6) - first 5 buttons
        for (int i = 0; i < 5; ++i) {
            if (buttons[i]->isClicked(mousePos)) {
                selectedPlayerCount = i + 2;
                setupPlayerCountScreen(); // Refresh to update selection display
                std::cout << "Selected " << selectedPlayerCount << " players" << std::endl;
                break;
            }
        }
        
        // Check Next button (last button)
        if (buttons.size() > 5 && buttons[5]->isClicked(mousePos)) {
            currentScreen = PLAYER_NAMES_INPUT;
            setupPlayerNamesScreen();
            std::cout << "Moving to name input screen for " << selectedPlayerCount << " players" << std::endl;
        }
    }
    else if (currentScreen == PLAYER_NAMES_INPUT) {
        // Deactivate all inputs first
        for (auto& input : playerInputs) {
            input->setActive(false);
        }
        
        // Check which input field was clicked
        for (auto& input : playerInputs) {
            if (input->isClicked(mousePos)) {
                input->setActive(true);
                break;
            }
        }
        
        // Check Back button
        if (buttons.size() > 0 && buttons[0]->isClicked(mousePos)) {
            currentScreen = PLAYER_COUNT_SELECTION;
            setupPlayerCountScreen();
            std::cout << "Going back to player count selection" << std::endl;
        }
        
        // Check Start Game button
        if (buttons.size() > 1 && buttons[1]->isClicked(mousePos)) {
            startGame();
            // setupGameScreen();
        }
    }
}

void GameSetupGUI::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    // Update buttons hover state
    for (auto& button : buttons) {
        button->update(mousePos);
    }
    
    // Update text inputs
    if (currentScreen == PLAYER_NAMES_INPUT) {
        for (auto& input : playerInputs) {
            input->update(mousePos);
        }
    }
}

void GameSetupGUI::render() {
    window.clear(sf::Color(245, 245, 250)); // Very light blue-gray background
    
    // Draw all labels
    for (const auto& label : labels) {
        window.draw(label);
    }
    
    // Draw all buttons
    for (const auto& button : buttons) {
        button->draw(window);
    }
    
    // Draw text inputs if in name input screen
    if (currentScreen == PLAYER_NAMES_INPUT) {
        for (const auto& input : playerInputs) {
            input->draw(window);
        }
    }
    
    window.display();
}

void GameSetupGUI::startGame() {
    // Collect player names
    std::vector<std::string> playerNames;
    bool allNamesValid = true;
    
    for (const auto& input : playerInputs) {
        std::string name = input->getText();
        if (name.empty()) {
            allNamesValid = false;
            std::cout << "Error: Please enter names for all players!" << std::endl;
            return;
        }
        playerNames.push_back(name);
    }
    
    // Check for duplicate names
    for (size_t i = 0; i < playerNames.size(); ++i) {
        for (size_t j = i + 1; j < playerNames.size(); ++j) {
            if (playerNames[i] == playerNames[j]) {
                std::cout << "Error: Player names must be unique! Found duplicate: " << playerNames[i] << std::endl;
                return;
            }
        }
    }

    for (size_t i = 0; i < playerNames.size(); ++i) {
        std::string role = _game.roleGenerator();
        std::shared_ptr<Player> player_ptr = PlayerFactory::createPlayer(role,playerNames[i]);
        _game.add_player(player_ptr);
    }

    
    setupGameScreen();
}

std::vector<std::string> GameSetupGUI::getPlayerNames() const {
    std::vector<std::string> names;
    for (const auto& input : playerInputs) {
        names.push_back(input->getText());
    }
    return names;
}