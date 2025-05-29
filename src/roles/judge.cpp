#include "judge.hpp"
#include "game.hpp"

std::string Judge::get_type() const{
    return "Judge";
}

/**
 * @brief Executes the Judge's targeted ability.
 * 
 * This ability sets the Judge's last action to Ability,
 * resets the target player's last action to None,
 * and disables the bribe state in the game.
 * 
 * @param target The player targeted by the Judge's ability.
 */
void Judge::ability(Player& target){
    _last_action = Action::Ability;
    target.setAction(Action::None);
    _game.setBribe(false);
}