#include "baron.hpp"
#include "game.hpp"

void Baron::ability() {
    if (_coins < 3) {
        throw std::runtime_error("Baron needs at least 3 coins to use ability.");
    }
    if(_coins >= 10){
        throw std::runtime_error("You have 10 or more coins must coup.");
    }
    _coins += 3;
    _last_action = Action::Ability;
    _game.next_turn();
}

std::string Baron::get_type() const{
    return "Baron";
}