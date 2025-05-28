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

bool Player::arrest(Player& target) {
    if (target._arrested) {
        return false;
    }
    if (!_can_arrest){
        return false;
    }
    if (target._coins <= 0) {
        return false;
    }
    if (target.get_type() == "Merchant"){
        target._coins -= 2;
    }
    else{
        target._coins -= 1;
        _coins += 1;
    }
    target._arrested = true;
    return true;
}
void Player::setCanArrest(bool can){
    _can_arrest = can;
}
bool Player::getCanArrest(){
    return _can_arrest;
}

bool Player::sanction(Player& target) {
    if (_coins < 3) {
        return false;
    }
    if (target.get_type() == "Baron"){
        target._coins++;
    }
    if (target.get_type() == "Judge"){
        if (_coins < 4) {
            return false;
        }
        _coins--;
    }
    _coins -= 3;
    target._sanctioned = true;
    return true;
}

bool Player::coup(Player& target) {
    if (_coins < 7) {
        return false;
    }
    _coins -= 7;
    return true;
}

std::string Player::get_type() const {
    return "Player";
}


std::string Player::getName() const {
    return _name;
}

int Player::getCoins() const {
    return _coins;
}

void Player::setCoins(int coins){
    _coins = coins;
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