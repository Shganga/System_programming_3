#include "spy.hpp"

int Spy::ability(Player& player){
    player.setCanArrest(false);
    return player.getCoins();
}

std::string Spy::get_type() const{
    return "Spy";
}