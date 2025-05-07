#include "player.hpp"
#include <stdexcept>

Player::Player(const std::string& name)
    : _name(name), _coins(0), _sanctioned(false), _lastArrestedby("") {}

Player::~Player() {}

Player::Player(const Player& other)
    : _name(other._name), _coins(other._coins), _sanctioned(other._sanctioned), _lastArrestedby(other._lastArrestedby) {}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        _name = other._name;
        _coins = other._coins;
        _sanctioned = other._sanctioned;
        _lastArrestedby = other._lastArrestedby;
    }
    return *this;
}

void Player::gather() {
    if (_sanctioned) {
        throw std::runtime_error("Player " + _name + " is sanctioned and cannot gather.");
    }
    _coins += 1;
}

void Player::tax() {
    if (_sanctioned) {
        throw std::runtime_error("Player " + _name + " is sanctioned and cannot tax.");
    }
    _coins += 2;
}

void Player::bribe() {
    if (_coins < 4) {
        throw std::runtime_error("Player " + _name + " does not have enough coins to bribe (needs 4).");
    }
    _coins -= 4;
    // Allow extra action (external handling if needed)
}

void Player::arrest(Player& target) {
    if (target._lastArrestedby == _name) {
        throw std::runtime_error("Cannot arrest the same player twice in a row.");
    }
    if (target._coins <= 0) {
        throw std::runtime_error("Target player " + target._name + " has no coins to arrest.");
    }
    target._coins -= 1;
    _coins += 1;
    target._lastArrestedby = _name;
}

void Player::sanction(Player& target) {
    if (_coins < 3) {
        throw std::runtime_error("Player " + _name + " does not have enough coins to sanction (needs 3).");
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
