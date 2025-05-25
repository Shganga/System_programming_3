#include "spy.hpp"

int Spy::LookAndBlock(Player& player){
    _can_arrest = false;
    return player.getCoins();
}

std::string Spy::get_type() const{
    return "Spy";
}