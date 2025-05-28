#include "general.hpp"
#include "game.hpp"

std::string General::get_type() const{
    return "General";
}
void General::ability(Player& target){
    if(_coins < 5){
        throw std::runtime_error("General ability costs 5");
    }
    _coins -= 5;
    _game.restorePlayer();
    _last_action = Action::Ability;
    target.setAction(Action::None);
}