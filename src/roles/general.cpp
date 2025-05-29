#include "general.hpp"
#include "game.hpp"

std::string General::get_type() const{
    return "General";
}

/**
 * @brief Executes the General's targeted ability.
 * 
 * This ability revives a previously eliminated player in the game and resets the target's last action.
 * Costs 5 coins to use.
 * 
 * @param target The player whose last action will be cleared.
 * 
 * @throws std::runtime_error If the General has fewer than 5 coins.
 */
void General::ability(Player& target){
    if(_coins < 5){
        throw std::runtime_error("General ability costs 5");
    }
    _coins -= 5;
    _game.restorePlayer();
    _last_action = Action::Ability;
    target.setAction(Action::None);
}