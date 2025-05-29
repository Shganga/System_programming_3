#include "game.hpp"
#include "roles/player.hpp"
#include <random>
#include <chrono>
#include "roles/player_factory.hpp"

/**
 * @brief Default constructor for the Game class.
 *
 * Initializes the game with the current turn set to 0
 * and the current round set to 1.
 */
Game::Game() : _current_turn(0), _current_round(1), isbribe(false),isStillActive(true) {}

/**
 * @brief Destructor for the Game class.
 *
 * Clears player and out lists. Since these are smart pointers,
 * memory management is automatic.
 */
Game::~Game() {
    // Smart pointers clean themselves up, but you can clear explicitly
    _players_list.clear();
    _out_list.clear();
}

/**
 * @brief Copy constructor for the Game class.
 *
 * Performs a deep copy of the game state including current turn,
 * round, bribery status, and player lists.
 *
 * @param other The Game object to copy from.
 */
Game::Game(const Game& other)
    : _current_turn(other._current_turn),
      _current_round(other._current_round),
      isbribe(other.isbribe),
      isStillActive(other.isStillActive)
{
    _players_list = other._players_list; // shared_ptr allows safe copy
    _out_list = other._out_list;
}

/**
 * @brief Copy assignment operator for the Game class.
 *
 * Safely assigns one Game object to another, ensuring no self-assignment,
 * and copies all game state data members.
 *
 * @param other The Game object to assign from.
 * @return Reference to this Game object.
 */
Game& Game::operator=(const Game& other) {
    if (this != &other) {
        _current_turn = other._current_turn;
        _current_round = other._current_round;
        isbribe = other.isbribe;
        _players_list = other._players_list;
        _out_list = other._out_list;
    }
    return *this;
}

/**
 * @brief Adds a player to the game.
 * check for duplicated names
 *
 * @param player A shared pointer to the Player to add.
 */
void Game::add_player(const std::string& name) {
    for (const std::string& _name : players()){
        if(_name == name){
            throw std::runtime_error("Cant use duplicated names");
        }
    }
    std::string role = roleGenerator();
    std::shared_ptr<Player> player = PlayerFactory::createPlayer(*this,role,name,players().size() -1);
    _players_list.push_back(player);
}



/**
 * @brief Returns the name of the player whose turn it currently is.
 *
 * Throws a runtime error if there are no players in the game.
 *
 * @return The name of the current player as a std::string.
 * @throws std::runtime_error if no players exist.
 */
std::string Game::turn() const {
    if (_players_list.empty()) {
        throw std::runtime_error("No players in the game");
    }
    return currentPlayer()->getName();
}

/**
 * @brief Advances the game to the next turn.
 * 
 * If a bribe is not active, increments the current turn. 
 * When all players have taken their turn, increments the round and resets arrests.
 * If a bribe is active, just clears the bribe flag without advancing turn.
 */
void Game::next_turn(){
    manageAfterTrun();
    if(!isbribe){
        _current_turn++;
        if(_current_turn % _players_list.size() == 0){
            _current_round++;
            resetArrest();
        }
        if(_players_list.size() > 1){
            std::shared_ptr<Player> current = currentPlayer();
            if(current->get_type() == "Merchant"){
                current->ability(); 
            }
            if(!canAction()){
                current->setSanctioned(false);
                next_turn();
            }
        }
    }
    else{
        isbribe = false;
    }
}


/**
 * @brief Sets the bribe flag to true.
 * 
 * This flag affects turn progression in the game.
 * if it is on the player gets 2 more turns (using the bribe)
 */
void Game::bribe(){
    isbribe = true;
}

/**
 * @brief Resets the arrest status of all players.
 * 
 * Sets arrest flag to false for every player in the players list.
 */
void Game::resetArrest(){
    for (size_t i = 0; i < _players_list.size(); i++)
    {
        _players_list[i]->setArrest(false);
    }
    
}

/**
 * @brief Returns the index of the current player.
 * 
 * If the game just started (_current_turn == 0), returns 0.
 * Otherwise returns current turn modulo the number of players.
 * 
 * @return int Index of the current player in the players list.
 */
int Game::currentPlayerIndex() const{
    if (_players_list.empty()) {
        throw std::runtime_error("No players in the game");
    }
    if(_current_turn == 0)
        return 0;
    return _current_turn % _players_list.size();
}


/**
 * @brief Returns a list of names of active players.
 * 
 * Iterates over the players list and collects names of players who are in the game.
 * 
 * @return std::vector<std::string> Vector containing names of active players.
 */
std::vector<std::string> Game::players() const {
    std::vector<std::string> active_players;
    for (const auto& p : _players_list) {
        active_players.push_back(p->getName());
    }
    return active_players;
}

/**
 * @brief Checks if the current player can perform an action.
 * 
 * A player can act if they are not sanctioned or have more than 2 coins.
 * If sanctioned, checks if there is at least one other player who is not arrested.
 * As arrest costs 0 coins.
 * 
 * @return true if the current player can act, false otherwise.
 */
bool Game::canAction(){
    std::shared_ptr<Player> player = currentPlayer();
    if(!player->isSanctioned() || player->getCoins() > 2){
        return true;
    }
    for (const auto& p : _players_list) {
        if (p->getName() != player->getName() && !p->isArrested()) {
            return true;
        }
    }
    return false;
}


/**
 * @brief Returns the vector of all players as shared pointers.
 * 
 * @return std::vector<std::shared_ptr<Player>> Vector containing shared pointers to all players.
 */
std::vector<std::shared_ptr<Player>> Game::getPlayers(){
    return _players_list;
}

/**
 * @brief Randomly generates and returns a role name.
 * 
 * The role is chosen randomly from a fixed set of role names.
 * Uses a Mersenne Twister random number generator seeded with current time.
 * 
 * @return std::string A randomly selected role name.
 */
std::string Game::roleGenerator() const {
    static const std::vector<std::string> roles = {
        "Spy", "Merchant", "Judge", "Governor", "General", "Baron"
    };

    static std::mt19937 rng(
        static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
    );

    std::uniform_int_distribution<size_t> dist(0, roles.size() - 1);
    return roles[dist(rng)];
}


/**
 * @brief Removes a player from active players and adds them to the out list by name.
 * 
 * Iterates through the active players to find a player with the given name.
 * If found, adds the player to the out list and removes them from active players.
 * 
 * @param name The name of the player to remove from the game.
 */
void Game::gameCoup(const std::string& name){
    for (auto it = _players_list.begin(); it != _players_list.end(); ++it) {
        if ((*it)->getName() == name) {
            _out_list.push_back(*it);       
            _players_list.erase(it);    
            isGameDone();   
            return;                        
        }
    }
}


/**
 * @brief Returns a list of players excluding the one with the specified name.
 * 
 * Used to get all players available for selection except the player identified by 'name'.
 * 
 * @param name The name of the player to exclude from the selection list.
 * @return std::vector<std::shared_ptr<Player>> Vector of players excluding the given name.
 */
std::vector<std::shared_ptr<Player>> Game::playersForSelection(const std::string& name) {
    std::vector<std::shared_ptr<Player>> result;
    for (const auto& player : _players_list) {
        if (player->getName() != name) {
            result.push_back(player);
        }
    }
    return result;
}

/**
 * @brief Returns the name of the winning player if only one player remains.
 * 
 * If there is exactly one player left in the game, returns that player's name.
 * Throws an exception if the game is still ongoing (more than one player remains).
 * 
 * @return std::string Name of the winning player.
 * @throws std::runtime_error if the game is still ongoing.
 */
std::string Game::winner() const {
    if(_players_list.size() == 1)
        return _players_list[0]->getName();
        
    throw std::runtime_error("The game is still ongoing");
}

/**
 * @brief Manage state updates after the current player's turn ends.
 * 
 * Resets sanctions and arrest permissions for the current player.
 * - If the player is sanctioned, sanction is cleared.
 * - If the player cannot arrest, permission is restored.
 */
void Game::manageAfterTrun(){
    std::shared_ptr<Player> current = currentPlayer();
    if(current->isSanctioned()){
        current->setSanctioned(false);
    }
    if(!current->getCanArrest()){
        current->setCanArrest(true);
    }
}

/**
 * @brief Get a shared pointer to the current player.
 * 
 * @return std::shared_ptr<Player> The player whose turn it currently is.
 * @throws std::runtime_error If no players exist in the game.
 */
std::shared_ptr<Player> Game::currentPlayer() const{
    if (_players_list.empty()) {
        throw std::runtime_error("No players available to retrieve current player.");
    }
    return _players_list[currentPlayerIndex()];
}

/**
 * @brief Check if the game has finished.
 * 
 * The game is considered done if there is only one player left.
 * Sets the `isStillActive` flag to false if the game is over.
 */
void Game::isGameDone(){
    if(_players_list.size() == 1){
        isStillActive = false;
    }
}

/**
 * @brief Query if the game is still active.
 * 
 * @return true if the game is ongoing, false otherwise.
 */
bool Game::isGame(){
    return isStillActive;
}

/**
 * @brief Restore a player from the out list back to the active players list.
 * 
 * Removes the last player from the out list and inserts them back to their original position
 * in the players list if possible; otherwise, adds to the end.
 * 
 * @throws std::runtime_error If the out list is empty (no players to restore).
 */
void Game::restorePlayer() {
    if (_out_list.empty()) {
        throw std::runtime_error("No players to restore.");
    }

    std::shared_ptr<Player> restored = _out_list.back();
    _out_list.pop_back();

    size_t idx = restored->getIndex();  // You need a getIndex() function in Player

    if (idx <= _players_list.size()) {
        // Insert at original index if possible
        if (idx == _players_list.size()) {
            _players_list.push_back(restored);
        } else {
            _players_list.insert(_players_list.begin() + idx, restored);
        }
    } else {
        // If the index is too large, push to the end
        _players_list.push_back(restored);
    }
}

/**
 * @brief Set the bribe state of the game.
 * 
 * @param bribe Boolean indicating whether bribery is currently active.
 */
void Game::setBribe(bool bribe){
    isbribe = bribe;
}

/**
 * @brief Get the current turn number.
 * 
 * @return int The current turn index.
 */
int Game::getTurn() const{
    return _current_turn;
}

/**
 * @brief Retrieve the list of players who have been removed from the game.
 * 
 * @return std::vector<std::shared_ptr<Player>> Vector containing pointers to players out of the game.
 */
std::vector<std::shared_ptr<Player>> Game::getOutList(){
    return _out_list;
}

/**
 * @brief Check if bribery is currently active in the game.
 * 
 * @return true if bribery is active, false otherwise.
 */
bool Game::getBribe() const{
    return isbribe;
}

std::shared_ptr<Player> Game::lastPlayer(){
    return _players_list[(_current_turn - 1) % _players_list.size()];
}


