#include "judge.hpp"
#include "game.hpp"

std::string Judge::get_type() const{
    return "Judge";
}

void Judge::ability(Player& target){
    _last_action = Action::Ability;
    target.setAction(Action::None);
    _game.setBribe(false);
}