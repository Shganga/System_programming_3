#include "player.hpp"
#include <stdexcept>

Player::Player(const std::string& name)
    : _name(name), _coins(0), _sanctioned(false), _arrested(false), _can_arrest(true) {}

Player::~Player() {}

Player::Player(const Player& other)
    : _name(other._name), _coins(other._coins), _sanctioned(other._sanctioned), _arrested(false), _can_arrest(true) {}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        _name = other._name;
        _coins = other._coins;
        _sanctioned = other._sanctioned;
        _arrested = other._arrested;
        _can_arrest = other._can_arrest;
    }
    return *this;
}

bool Player::gather() {
    if (_sanctioned) {
        return false;
    }
    _coins += 1;
    return true;
}

bool Player::tax() {
    if (_sanctioned) {
        return false;
    }
    _coins += 2;
    return true;
}

bool Player::bribe() {
    if (_coins < 4) {
        return false;
    }
    _coins -= 4;
    return true;
    // Allow extra action (external handling if needed)
}

void Player::arrest(Player& target) {
    if (!target._arrested) {
        throw std::runtime_error("Cannot arrest the same player twice in a row.");
    }
    if (!_can_arrest){
        throw std::runtime_error("Cannot arrest because of a spy");
    }
    if (target._coins <= 0) {
        throw std::runtime_error("Target player " + target._name + " has no coins to arrest.");
    }
    if (target.get_type() == "Merchant"){
        target._coins -= 2;
    }
    else{
        target._coins -= 1;
        _coins += 1;
    }
    target._arrested = true;
}

void Player::sanction(Player& target) {
    if (_coins < 3) {
        throw std::runtime_error("Player " + _name + " does not have enough coins to sanction (needs 3).");
    }
    if (target.get_type() == "Baron"){
        target._coins++;
    }
    if (target.get_type() == "Judge"){
        _coins--;
    }
    _coins -= 3;
    target._sanctioned = true;
}

void Player::coup(Player& target) {
    if (_coins < 7) {
        throw std::runtime_error("Player " + _name + " does not have enough coins to coup (needs 7).");
    }
    _coins -= 7;
    // External game logic should handle actual removal
}

std::string Player::getName() const {
    return _name;
}

int Player::getCoins() const {
    return _coins;
}


bool Player::isSanctioned() const {
    return _sanctioned;
}

void Player::setSanctioned(bool status) {
    _sanctioned = status;
}

bool Player::isArrested() const{
    return _arrested;
}

void Player::setArrest(bool status) {
    _arrested = status;
}