#include "GUI.hpp"
#include <iostream>
#include <algorithm>
#include "roles/player_factory.hpp"
#include <cmath>

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

void Button::updateHover(sf::Vector2i mousePos) {
    sf::FloatRect bounds = shape.getGlobalBounds();
    bool wasHovered = isHovered;
    isHovered = bounds.contains(static_cast<sf::Vector2f>(mousePos));
    
    // Change color on hover
    if (isHovered && !wasHovered) {
        // Store original color if not already stored
        if (originalColor == sf::Color::Transparent) {
            originalColor = shape.getFillColor();
            // Create a lighter hover color
            hoverColor = sf::Color(
                std::min(255, static_cast<int>(originalColor.r) + 40),
                std::min(255, static_cast<int>(originalColor.g) + 40),
                std::min(255, static_cast<int>(originalColor.b) + 40)
            );
        }
        shape.setFillColor(hoverColor);
    } else if (!isHovered && wasHovered) {
        shape.setFillColor(originalColor);
    }
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

void GameSetupGUI::setupGameScreen(std::string message) {
    buttons.clear();
    labels.clear();
    playerInputs.clear();

    // === כותרת המשחק ===
    sf::Text title;
    if (fontLoaded) title.setFont(font);
    title.setString("C O U P");
    title.setCharacterSize(32);
    title.setFillColor(sf::Color(220, 20, 60));
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((900 - titleBounds.width) / 2, 10);
    labels.push_back(title);

    // === כפתורי הפעולות העליונים ===
    float buttonStartX = 50;
    float buttonY = 60;
    float buttonWidth = 100;
    float buttonHeight = 35;
    float buttonSpacing = 110;

    struct ActionInfo {
        std::string name;
        std::string displayName;
        sf::Color color;
    };

    std::vector<ActionInfo> actions = {
        {"gather", "GATHER", sf::Color(0, 160, 0)},
        {"tax", "TAX", sf::Color(0, 160, 0)},
        {"bribe", "BRIBE", sf::Color(0, 160, 0)},
        {"arrest", "ARREST", sf::Color(0, 160, 0)},
        {"sanction", "SANCTION", sf::Color(0, 160, 0)},
        {"coup", "COUP", sf::Color(0, 160, 0)},
        {"ability", "SPECIAL", sf::Color(0, 160, 0)}
    };

    for (size_t i = 0; i < actions.size(); ++i) {
        auto actionBtn = std::make_unique<Button>(
            buttonStartX + i * buttonSpacing, buttonY, buttonWidth, buttonHeight,
            actions[i].displayName, font
        );
        actionBtn->shape.setFillColor(actions[i].color);
        buttons.push_back(std::move(actionBtn));
    }

    // === מיקום השחקנים סביב שולחן ===
    // נניח שהחלון 900x600
    // נגדיר נקודות ישיבה סביב שולחן מלבני:
    // למעלה: שני מקומות, למטה שני מקומות, שמאל וימין (עד 6 שחקנים)

    std::vector<sf::Vector2f> seats = {
        {350, 150}, // Top left seat
        {550, 150}, // Top right seat
        {650, 300}, // Right middle seat
        {550, 450},  // Bottom right seat
        {350, 450}, // Bottom left seat
        {250, 300} // Left middle seat 
    };

    size_t numPlayers = _game.getPlayers().size();

    for (size_t i = 0; i < numPlayers && i < seats.size(); ++i) {
        sf::Vector2f pos = seats[i];
        bool isCurrentPlayer = (i == static_cast<size_t>(_game.currentPlayer()));


        sf::Text playerName;
        if (fontLoaded) playerName.setFont(font);
        playerName.setString(_game.getPlayers()[i]->getName());
        playerName.setCharacterSize(18);
        playerName.setFillColor(isCurrentPlayer ? sf::Color::Green : sf::Color::Red);
        playerName.setStyle(sf::Text::Bold);
        sf::FloatRect nameBounds = playerName.getLocalBounds();
        playerName.setPosition(pos.x - nameBounds.width / 2, pos.y - 20);

        sf::Text roleText;
        if (fontLoaded) roleText.setFont(font);
        roleText.setString(_game.getPlayers()[i]->get_type());
        roleText.setCharacterSize(14);
        roleText.setFillColor(sf::Color(200, 200, 200));
        sf::FloatRect roleBounds = roleText.getLocalBounds();
        roleText.setPosition(pos.x - roleBounds.width / 2, pos.y);

        sf::Text coinText;
        if (fontLoaded) coinText.setFont(font);
        coinText.setString("♦ " + std::to_string(_game.getPlayers()[i]->getCoins()));
        coinText.setCharacterSize(16);
        coinText.setFillColor(sf::Color::Black);
        coinText.setStyle(sf::Text::Bold);
        sf::FloatRect coinBounds = coinText.getLocalBounds();
        coinText.setPosition(pos.x - coinBounds.width / 2, pos.y + 20);


        // הוסף טקסטים לרשימת התוויות לציור
        labels.push_back(playerName);
        labels.push_back(roleText);
        labels.push_back(coinText);

    }

    sf::Text statusText;
    if (fontLoaded) statusText.setFont(font);
    statusText.setString(message);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Blue);
    statusText.setStyle(sf::Text::Italic);
    statusText.setPosition(50, 550); // קרוב לתחתית המסך
    labels.push_back(statusText);

    // === מידע משחק בפינה ימנית עליונה ===
    sf::Text gameInfo;
    if (fontLoaded) gameInfo.setFont(font);
    gameInfo.setString("Turn: " + _game.turn());
    gameInfo.setCharacterSize(18);
    gameInfo.setFillColor(sf::Color(50, 50, 50));
    gameInfo.setStyle(sf::Text::Bold);
    gameInfo.setPosition(700, 120);
    labels.push_back(gameInfo);

    sf::Text playerCount;
    if (fontLoaded) playerCount.setFont(font);
    playerCount.setString("Players: " + std::to_string(numPlayers));
    playerCount.setCharacterSize(16);
    playerCount.setFillColor(sf::Color(100, 100, 100));
    playerCount.setPosition(700, 145);
    labels.push_back(playerCount);
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
            currentScreen = GAME_SCREEN;
            startGame();
        }
    }
    else if (currentScreen == GAME_SCREEN) {
    // Loop through all action buttons
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i]->isClicked(mousePos)) {
                handleGameAction(i);
                break;
            }
        }
    }
}

void GameSetupGUI::handleGameAction(size_t buttonIndex) {
    // Assuming order matches your actions vector from setupGameScreen()
    int turn = _game.currentPlayer();
    bool action;
    std::string message;
    switch (buttonIndex) {
        case 0:  // Gather
            action = _game.getPlayers()[turn]->gather();
            if(!action){
                message = "you cant use gather";
                break;
            }
            message = "Gather action triggered\n";
            _game.next_turn();
            break;
        case 1:  // Tax
            action = _game.getPlayers()[turn]->tax();
            if(!action){
                message = "you cant use tax\n";
                break;
            }
            _game.next_turn();
            message = "Tax action triggered\n";
            break;
        case 2:  // Bribe
            action = _game.getPlayers()[turn]->bribe();
            if(!action){
                message = "you cant use bribe";
                break;
            }
            _game.bribe();
            std::cout << "Bribe action triggered\n";
            break;
        case 3:  // Arrest
            //_game.getPlayers()[turn]->arrest();
            std::cout << "Arrest action triggered\n";
            break;
        case 4:  // Sanction
            //_game.getPlayers()[turn]->sanction();
            std::cout << "Sanction action triggered\n";
            break;
        case 5:  // Coup
            //_game.getPlayers()[turn]->coup();
            std::cout << "Coup action triggered\n";
            break;
        case 6:  // Special / Ability
            //_game.getPlayers()[turn]->ability();
            std::cout << "Special ability triggered\n";
            break;
        default:
            std::cout << "Unknown action\n";
            break;
    }

    // After action, maybe update the screen or game state
    setupGameScreen(message);
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

    
    setupGameScreen("Game started");
}

std::vector<std::string> GameSetupGUI::getPlayerNames() const {
    std::vector<std::string> names;
    for (const auto& input : playerInputs) {
        names.push_back(input->getText());
    }
    return names;
}