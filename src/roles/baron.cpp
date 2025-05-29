#include "baron.hpp"
#include "game.hpp"


/**
 * @brief Executes the Baron's special ability.
 * 
 * The Baron gains 3 coins as long as they have at least 3 coins and fewer than 10.
 * 
 * @throws std::runtime_error If the Baron has fewer than 3 coins.
 * @throws std::runtime_error If the Baron has 10 or more coins (must perform a coup instead).
 */
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