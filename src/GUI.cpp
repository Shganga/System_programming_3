#include "GUI.hpp"
#include <iostream>
#include <algorithm>
#include "roles/player_factory.hpp"
#include <cmath>
#include "roles/baron.hpp"
#include "roles/spy.hpp"

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

void TextInput::update() {
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
GameSetupGUI::GameSetupGUI()
    : _game()
    , window(sf::VideoMode(900, 700), "Game Setup - Player Selection")
    , font()                // sf::Font default constructor
    , fontLoaded(false)
    , playerBoxes()         // vector default constructor
    , errorText()           // sf::Text default constructor
    , currentScreen(PLAYER_COUNT_SELECTION)
    , selectedPlayerCount(2)
    , buttons()
    , labels()
    , playerInputs()
{
    fontLoaded = loadFont();
    setupPlayerCountScreen();
}

// GameSetupGUI::~GameSetupGUI() {
//     // Clear all containers first to avoid dangling references
//     buttons.clear();
//     labels.clear();
//     playerInputs.clear();

    
//     // Close window safely
//     if (window.isOpen()) {
//         window.close();
//     }
// }

bool GameSetupGUI::loadFont() {
    // רשימת נתיבים פונט אופציונליים לפי סדר ניסיון
    static const std::vector<std::string> fontPaths = {
        // Ubuntu system fonts (מומלץ להשתמש באלו קודם)
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

    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            std::cout << "Font loaded successfully from: " << path << std::endl;
            return true;
        } else {
            // אופציונלי: להדפיס אם תרצה לדעת מה לא נטען
            // std::cerr << "Failed to load font from: " << path << std::endl;
        }
    }

    std::cerr << "Warning: Could not load any font. Text might not display correctly." << std::endl;
    std::cerr << "To fix this, install fonts with: sudo apt install fonts-dejavu-core fonts-liberation" << std::endl;
    return false;
}


void GameSetupGUI::run() {
    std::cout << "Starting Game Setup GUI..." << std::endl;
    std::cout << "Choose number of players (2-6) by clicking on the numbers." << std::endl;
    
    try {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in main loop: " << e.what() << std::endl;
        if (window.isOpen()) {
            window.close();
        }
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
    
    // Input fields
    float startY = 140;
    for (int i = 0; i < selectedPlayerCount; ++i) {
        float y = startY + i * 60;
        
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
        
        playerInputs.push_back(std::make_unique<TextInput>(380, y, 250, 40, font));
    }
    
    float buttonY = startY + selectedPlayerCount * 60 + 40;
    
    auto backButton = std::make_unique<Button>(250, buttonY, 120, 50, "BACK", font);
    backButton->shape.setFillColor(sf::Color(150, 150, 150));
    backButton->buttonText.setCharacterSize(20);
    buttons.push_back(std::move(backButton));
    
    auto startButton = std::make_unique<Button>(430, buttonY, 200, 50, "START GAME", font);
    startButton->shape.setFillColor(sf::Color(220, 60, 60));
    startButton->buttonText.setCharacterSize(20);
    startButton->buttonText.setStyle(sf::Text::Bold);
    buttons.push_back(std::move(startButton));
    
    if (fontLoaded) {
        errorText.setFont(font);
    }
    errorText.setCharacterSize(18);
    errorText.setFillColor(sf::Color::Red);
    errorText.setString("");
    errorText.setPosition(250, buttonY + 60);
    labels.push_back(errorText);
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
        std::string coinDisplay = isCurrentPlayer ? std::to_string(_game.getPlayers()[i]->getCoins()) : "???";
        coinText.setString("coins: " + coinDisplay);
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
    bool block = false;
    std::string message;
    switch (buttonIndex) {
        case 0:  // Gather
            try {
                _game.getPlayers()[turn]->gather();
                message = "Gather action triggered\n";
                _game.manageAfterTrun();
                _game.next_turn();
                break;
            } catch (const std::exception& e) {
                message = e.what();
                break;
            }
        case 1:  // Tax
            try {
                _game.getPlayers()[turn]->tax();
                _game.manageAfterTrun();
                _game.next_turn();
                message = "Tax action triggered\n";
                break;
            } catch (const std::exception& e) {
                message = e.what();
                break;
            }
            block = askAllWithRole("Governor");
            if(block){
                message = "A Governor blocked your tax\n";
                _game.next_turn();
                break;
            }
            
        case 2:  // Bribe
            try {
                _game.getPlayers()[turn]->bribe();
                _game.manageAfterTrun();
                _game.bribe();
                message = "Bribe action triggered\n";
                break;
            } catch (const std::exception& e) {
                message = e.what();
                break;
            }
            block = askAllWithRole("Judge");
            if(block){
                message = "A Judge blocked your bribe\n";
                _game.next_turn();
                break;
            }
            
        case 3:{//arrest  
            std::shared_ptr<Player> selected = displayPlayerSelection("Choose Arrest");
            if (selected) {
                std::cout << "Selected player: " << selected->getName() << std::endl;
                try {
                    _game.getPlayers()[turn]->arrest(*selected);
                    message = "Arrest was triggerd on " + selected->getName();
                    _game.manageAfterTrun();
                    _game.next_turn();
                    break;
                } catch (const std::exception& e) {
                    message = e.what();
                    break;
                }
            }
            message = "You didnt select a player";
            break;
        }
        case 4:{  // Sanction
            std::shared_ptr<Player> selected = displayPlayerSelection(" Choose Sunction");
            if (selected) {
                std::cout << "Selected player: " << selected->getName() << std::endl;
                try {
                    _game.getPlayers()[turn]->sanction(*selected);
                    message = "sanction was triggerd on " + selected->getName();
                    _game.manageAfterTrun();
                    _game.next_turn();
                    break;
                } catch (const std::exception& e) {
                    message = e.what();
                    break;
                }
                
            }
            message = "You didnt select a player";
            break;
        }
        case 5:{  // Coup
            std::shared_ptr<Player> selected = displayPlayerSelection(" Choose Coup");
            try {
                _game.getPlayers()[turn]->arrest(*selected);
                _game.gameCoup(selected->getName());
                _game.manageAfterTrun();
                _game.next_turn();
                std::cout << "Coup action triggered\n";
                break;
            } catch (const std::exception& e) {
                message = e.what();
                break;
            }
            block = askAllWithRole("General");
            if(block){
                message = "A General blocked your Coup\n";
                _game.next_turn();
                break;
            }
            
        }
        case 6:{  // Special / Ability
            if(_game.getPlayers()[turn]->get_type() == "Baron"){
                auto& player = _game.getPlayers()[turn];
                Baron* baron = dynamic_cast<Baron*>(player.get());
                try {
                baron->ability();
                message = "You used Baron's ability";
                _game.manageAfterTrun();
                _game.next_turn();
                } catch (const std::exception& e) {
                    message = e.what();  // Or: message = "Ability didn't work: " + std::string(e.what());
                }

            }
            else if(_game.getPlayers()[turn]->get_type() == "Spy"){
                auto& player = _game.getPlayers()[turn];
                Spy* spy = dynamic_cast<Spy*>(player.get());
                std::shared_ptr<Player> selected = displayPlayerSelection(" Choose for spy ability");
                int coins = spy->ability(*selected);
                if(coins != selected->getCoins()){
                    message = "ability didnt work";
                    break;
                }
                message = "The player " + selected->getName() + " has " + std::to_string(coins) + " coins";
                _game.manageAfterTrun();
                break;
            }
            else{
                message = "your roles doesnt have an ability";
                break;
            }
        }
        default:
            std::cout << "Unknown action\n";
            break;
    }
    
    
    if(_game.getPlayers().size() == 1){
        currentScreen = GAME_END;
        showGameEndScreen();
    }
    if(_game.getPlayers()[_game.currentPlayer()]->get_type() == "Merchant" && _game.getPlayers()[_game.currentPlayer()]->getCoins() > 2){
        _game.getPlayers()[_game.currentPlayer()]->setCoins(_game.getPlayers()[_game.currentPlayer()]->getCoins() + 1); 
    }
    if(_game.getPlayers()[_game.currentPlayer()]->getCoins() >= 10){
        handleGameAction(5);
    }
    if(!_game.canAction()){
        _game.getPlayers()[_game.currentPlayer()]->setSanctioned(false);
        _game.next_turn();
    }
    setupGameScreen(message);
}



bool GameSetupGUI::askAllWithRole(const std::string& role) {
    for (const auto& player : _game.getPlayers()) {
        std::string name = player->getName();
        if (player->get_type() == role && name != _game.getPlayers()[_game.currentPlayer()]->getName()) { // נניח שאתה בודק גם אם השחקן חי
            
            if(role == "General" && player->getCoins() < 5){
                    continue;
            }
            bool approved = allowAction(name); // מציג שם מלא
            if (approved) {
                if(role == "General" && player->getCoins() < 5){
                    continue;
                }
                player->setCoins(player->getCoins() - 5);
                return true; // פעולה אושרה על ידי שחקן אחד לפחות
            }
        }
    }
    return false; // כולם דחו
}



bool GameSetupGUI::allowAction(const std::string& playerName) {
    // Create a separate window for this dialog
    sf::RenderWindow dialogWindow(sf::VideoMode(400, 200), "Block Action?", 
                                 sf::Style::Titlebar | sf::Style::Close);
    dialogWindow.setFramerateLimit(60);
    
    // Use a local copy of font or check if font is valid
    if (!fontLoaded) {
        std::cerr << "Warning: Font not loaded for dialog" << std::endl;
        return false; // or handle without font
    }
    
    sf::Text title("Player: " + playerName + "\nDo you want to BLOCK this action?", font, 20);
    title.setFillColor(sf::Color::White);
    title.setPosition(30, 30);

    sf::RectangleShape blockButton(sf::Vector2f(120, 50));
    blockButton.setPosition(50, 120);
    blockButton.setFillColor(sf::Color::Green);

    sf::Text blockText("BLOCK", font, 18);
    blockText.setFillColor(sf::Color::Black);
    blockText.setPosition(80, 135);

    sf::RectangleShape allowButton(sf::Vector2f(120, 50));
    allowButton.setPosition(230, 120);
    allowButton.setFillColor(sf::Color::Red);

    sf::Text allowText("ALLOW", font, 18);
    allowText.setFillColor(sf::Color::White);
    allowText.setPosition(260, 135);

    bool result = false;
    bool actionTaken = false;

    // Ensure dialog window stays on top and is focused
    dialogWindow.requestFocus();

    try {
        while (dialogWindow.isOpen() && !actionTaken) {
            sf::Event event;
            while (dialogWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    actionTaken = true;
                    result = false; // Default to allow (don't block) if window is closed
                    break;
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                        static_cast<float>(event.mouseButton.y));

                    if (blockButton.getGlobalBounds().contains(mousePos)) {
                        result = true;  // Return true = BLOCK the action
                        actionTaken = true;
                    }
                    else if (allowButton.getGlobalBounds().contains(mousePos)) {
                        result = false; // Return false = ALLOW the action (don't block)
                        actionTaken = true;
                    }
                }

                // Keyboard support
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::B || event.key.code == sf::Keyboard::Y) {
                        result = true;  // BLOCK
                        actionTaken = true;
                    }
                    else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::N || 
                             event.key.code == sf::Keyboard::Escape) {
                        result = false; // ALLOW (don't block)
                        actionTaken = true;
                    }
                }
            }

            // Only render if window is still open and no action taken
            if (dialogWindow.isOpen() && !actionTaken) {
                dialogWindow.clear(sf::Color(50, 50, 50));
                dialogWindow.draw(title);
                dialogWindow.draw(blockButton);
                dialogWindow.draw(blockText);
                dialogWindow.draw(allowButton);
                dialogWindow.draw(allowText);
                dialogWindow.display();
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in allowAction: " << e.what() << std::endl;
        result = false;
    }

    // Ensure dialog window is closed
    if (dialogWindow.isOpen()) {
        dialogWindow.close();
    }

    return result;
}



std::shared_ptr<Player> GameSetupGUI::displayPlayerSelection(const std::string& title) {
    std::vector<std::unique_ptr<Button>> playerButtons;
    const auto& players = _game.playersForSelection(_game.players()[_game.currentPlayer()]); // assumes std::vector<Player>

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString(_game.players()[_game.currentPlayer()] + title);
    titleText.setCharacterSize(30);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(100, 50);

    float startY = 150;
    for (size_t i = 0; i < players.size(); ++i) {
        float y = startY + i * 60;
        playerButtons.push_back(std::make_unique<Button>(
            100, y, 300, 50, players[i]->getName(), font
        ));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return nullptr;
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (size_t i = 0; i < playerButtons.size(); ++i) {
                    if (playerButtons[i]->isClicked(mousePos)) {
                        return players[i]; 
                    }
                }
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (auto& btn : playerButtons)
            btn->update(mousePos);

        window.clear(sf::Color::Black);
        window.draw(titleText);
        for (auto& btn : playerButtons)
            btn->draw(window);
        window.display();
    }

    return nullptr; // fallback if window is closed
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
            input->update();
        }
    }
}

void GameSetupGUI::render() {
    window.clear(sf::Color(245,245,250)); // Clear the window with black color
    
    // Draw player boxes (e.g. for highlighting players or UI decoration)
    for (auto& box : playerBoxes) {
        window.draw(box);
    }

    // Draw buttons
    for (auto& button : buttons) {
        button->draw(window);
    }
    
    // Draw labels (sf::Text objects)
    for (auto& label : labels) {
        window.draw(label);
    }
    
    // Draw player name text inputs only if on PLAYER_NAMES_INPUT screen
    if (currentScreen == PLAYER_NAMES_INPUT) {
        for (auto& input : playerInputs) {
            input->draw(window);
        }
    }
    
    // Draw error text if any
    window.draw(errorText);

    // Display everything on the window
    window.display();
}


void GameSetupGUI::startGame() {
    std::vector<std::string> playerNames;

    // Clear previous error message
    errorText.setString("");

    // Check for empty names
    for (const auto& input : playerInputs) {
        if (input == nullptr) {
            std::cout << "nullptr found in playerInputs!" << std::endl;
            continue;
        }
        std::string name = input->getText();
        if (name.empty()) {
            errorText.setString("Please enter names for all players!");
            std::cout << "Error: Please enter names for all players!" << std::endl;
            currentScreen = PLAYER_NAMES_INPUT;  // stay on the input screen
            return;  // exit so user can fix input
        }
        playerNames.push_back(name);
    }

    // Check for duplicates
    for (size_t i = 0; i < playerNames.size(); ++i) {
        for (size_t j = i + 1; j < playerNames.size(); ++j) {
            if (playerNames[i] == playerNames[j]) {
                errorText.setString("Player names must be unique! Duplicate: " + playerNames[i]);
                std::cout << "Error: Player names must be unique! Found duplicate: " << playerNames[i] << std::endl;
                currentScreen = PLAYER_NAMES_INPUT;  // stay on input screen
                return;  // exit so user can fix input
            }
        }
    }

    // If all good, add players and move to game screen
    for (const auto& name : playerNames) {
        std::string role = _game.roleGenerator();
        std::shared_ptr<Player> player_ptr = PlayerFactory::createPlayer(role, name);
        _game.add_player(player_ptr);
    }

    // Clear error message on success
    errorText.setString("");
    setupGameScreen("Game started");
}


std::vector<std::string> GameSetupGUI::getPlayerNames() const {
    std::vector<std::string> names;
    for (const auto& input : _game.players()) {
        names.push_back(input);
    }
    return names;
}

void GameSetupGUI::showGameEndScreen() {
    window.clear(sf::Color(0, 0, 0)); // Black background

    const std::string& winnerName = _game.winner();

    sf::Text title("Game Over", font, 48);
    title.setFillColor(sf::Color::White);
    title.setPosition(250, 150);

    sf::Text winnerText("Winner: " + winnerName, font, 36);
    winnerText.setFillColor(sf::Color::Green);
    winnerText.setPosition(250, 250);

    sf::Text exitText("Press ESC to exit", font, 24);
    exitText.setFillColor(sf::Color(200, 200, 200));
    exitText.setPosition(250, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        window.clear();
        window.draw(title);
        window.draw(winnerText);
        window.draw(exitText);
        window.display();
    }
}
