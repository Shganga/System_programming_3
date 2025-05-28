//yanivg1000@gmail.com
#include "player.hpp"
#include <stdexcept>
#include "game.hpp"


/**
 * @brief Constructs a new Player with the given name.
 * 
 * Initializes the player's coin count to 0, and sets status flags to default values.
 * 
 * @param name The name of the player.
 */
Player::Player(Game& game,const std::string& name,size_t index)
    : _game(game) ,_name(name), _coins(0), _sanctioned(false), _arrested(false), _can_arrest(true), _last_action(Action::None), _index(index) {}

/**
 * @brief Destructor for the Player class.
 */
Player::~Player() {}

/**
 * @brief Copy constructor for Player.
 * 
 * Creates a new Player by copying another, resetting arrest and arrest permission flags.
 * 
 * @param other The Player object to copy.
 */
Player::Player(const Player& other)
    : _game(other._game),_name(other._name), _coins(other._coins), _sanctioned(other._sanctioned), _arrested(false), _can_arrest(true), _last_action(other._last_action), _index(other._index) {}


/**
 * @brief Assignment operator for Player.
 * 
 * Copies data from another Player object.
 * 
 * @param other The Player to copy from.
 * @return Player& Reference to this Player.
 */
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        _game = other._game;
        _name = other._name;
        _coins = other._coins;
        _sanctioned = other._sanctioned;
        _arrested = other._arrested;
        _can_arrest = other._can_arrest;
        _last_action = other._last_action;
        _index = other._index;
    }
    return *this;
}

/**
 * @brief Increases the player's coin count if not sanctioned.
 * 
 * @return true If the player successfully gathered a coin.
 * @return false If the player is sanctioned and cannot gather.
 */
void Player::gather() {
    if (_sanctioned) {
        throw std::runtime_error("Sanctioned players cannot gather coins.");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    _last_action = Action::Gather;
    _coins += 1;
    _game.next_turn();
}

/**
 * @brief Collects tax for the player.
 * 
 * Adds 2 coins to the player's total if they are not sanctioned.
 * 
 * @return true If the player successfully collects tax.
 * @return false If the player is sanctioned and cannot collect tax.
 */

void Player::tax() {
    if (_sanctioned) {
        throw std::runtime_error("Sanctioned players cannot use tax.");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    _coins += 2;
    _last_action = Action::Tax;
    _game.next_turn();
}

/**
 * @brief Attempts to bribe by spending 4 coins.
 * 
 * Deducts 4 coins from the player if they have enough coins.
 * Bribe allows an extra turn.
 * 
 * @return true If the bribe was successful.
 * @return false If the player does not have enough coins.
 */
void Player::bribe() {
    if (_coins < 4) {
        throw std::runtime_error("You must have 4 coins");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    _coins -= 4;
    _last_action = Action::Bribe;
    _game.bribe();
}


/**
 * @brief Attempts to arrest another player.
 *
 * The arrest is only possible if:
 * - The target is not already arrested.
 * - This player is allowed to arrest.
 * - The target has at least 1 coin.
 *
 * The cost of the arrest varies by role:
 * - If the target is a "Merchant", they lose 2 coins.
 * - Otherwise, the target loses 1 coin and the arresting player gains 1 coin.
 *
 * @param target The player to arrest.
 */
void Player::arrest(Player& target) {
    if (target._arrested) {
        throw std::runtime_error("Target is already arrested.");
    }
    if (!_can_arrest) {
        throw std::runtime_error("You are not allowed to arrest.");
    }
    if (target._coins <= 0) {
        throw std::runtime_error("Target does not have enough coins to be arrested.");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    if (target.get_type() == "Merchant") {
        target._coins -= 2;
    } else {
        target._coins -= 1;
        _coins += 1;
    }
    target._arrested = true;
    _last_action = Action::Arrest;
    _game.next_turn();
}


/**
 * @brief Sets whether the player can arrest others.
 * 
 * @param can A boolean indicating if the player can arrest.
 */
void Player::setCanArrest(bool can){
    _can_arrest = can;
}

/**
 * @brief Gets whether the player can currently arrest others.
 * 
 * @return true If the player can arrest.
 * @return false Otherwise.
 */
bool Player::getCanArrest(){
    return _can_arrest;
}

/**
 * @brief Attempts to sanction another player.
 *
 * The action costs 3 coins, except when targeting a "Judge", which costs 4 coins.
 * If the target is a "Baron", they gain 1 coin as a side effect.
 *
 * @param target The player to sanction.
 */
void Player::sanction(Player& target) {
    if (_coins < 3) {
        throw std::runtime_error("Not enough coins to sanction.");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    if (target.get_type() == "Baron") {
        target._coins++;
    }
    if (target.get_type() == "Judge") {
        if (_coins < 4) {
            throw std::runtime_error("Not enough coins to sanction a Judge.");
        }
        _coins--;
    }
    _coins -= 3;
    target._sanctioned = true;
    _last_action = Action::Sanction;
    _game.next_turn();
}


/**
 * @brief Performs a coup action.
 *
 * The player must have at least 7 coins to perform a coup.
 * The cost is 7 coins, and the action typically eliminates another player (handled externally).
 *
 * @return true If the player had enough coins and performed the coup.
 * @return false If the player had fewer than 7 coins.
 */
void Player::coup(Player& target) {
    if (_coins < 7) {
        throw std::runtime_error("Not enough coins to perform a coup.");
    }
    _coins -= 7;
    _game.gameCoup(target.getName());
    _last_action = Action::Coup;
    _game.next_turn();
}

/**
 * @brief Gets the type of this player.
 *
 * This is the base implementation and returns "Player".
 * Subclasses should override this method to return their specific role.
 *
 * @return The type of the player.
 */
std::string Player::get_type() const {
    return "Player";
}

/**
 * @brief Gets the player's name.
 *
 * @return The name of the player.
 */
std::string Player::getName() const {
    return _name;
}

/**
 * @brief Gets the number of coins the player currently holds.
 *
 * @return The number of coins.
 */
int Player::getCoins() const {
    return _coins;
}

/**
 * @brief Sets the player's coin count.
 *
 * This method directly sets the number of coins the player has.
 *
 * @param coins The new coin count.
 */
void Player::setCoins(int coins){
    _coins = coins;
}

/**
 * @brief Checks if the player is sanctioned.
 *
 * A sanctioned player will be limited in their available actions.
 *
 * @return true if the player is sanctioned.
 * @return false otherwise.
 */
bool Player::isSanctioned() const {
    return _sanctioned;
}

/**
 * @brief Sets the sanction status of the player.
 *
 * @param status true to sanction the player, false to remove sanction.
 */
void Player::setSanctioned(bool status) {
    _sanctioned = status;
}

/**
 * @brief Checks if the player is arrested.
 *
 * If allready arrested cant be arrested for the current round again
 *
 * @return true if the player is arrested.
 * @return false otherwise.
 */
bool Player::isArrested() const{
    return _arrested;
}

/**
 * @brief Sets the arrest status of the player.
 *
 * @param status true to arrest the player, false to release.
 */
void Player::setArrest(bool status) {
    _arrested = status;
}

size_t Player::getIndex() const{
    return _index;
}

void Player::setIndex(size_t index){
    _index = index;
}

void Player::setAction(Action action){
    _last_action = action;
}

void Player::ability() {
    throw std::runtime_error(get_type() + " does not have a default ability.");
}

int Player::spyAbility(Player& target) {
    throw std::runtime_error(get_type() + " does not support spyAbility.");
}

void Player::ability(Player& target) {
    throw std::runtime_error(get_type() + " does not have a targeted ability.");
}