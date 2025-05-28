#include "governor.hpp"

void Governor::tax() {
    if (_sanctioned) {
        throw std::runtime_error("Governor is sanctioned and cannot collect tax.");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    _coins += 3;
}
std::string Governor::get_type() const{
    return "Governor";
}

void Governor::ability(Player& target){
    if(target.get_type() == "Governor")
        target.setCoins(target.getCoins() - 3);
    else
        target.setCoins(target.getCoins() - 2);
    _last_action = Action::Ability;
    target.setAction(Action::None);
}