#include "governor.hpp"


/**
 * @brief Collects tax as the Governor.
 * 
 * Adds 3 coins to the Governor's coin count if not sanctioned.
 * 
 * @throws std::runtime_error If the Governor is sanctioned and cannot collect tax.
 * @throws std::runtime_error If the Governor already has 10 or more coins (must coup instead).
 */
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

/**
 * @brief Executes the Governor's targeted ability.
 * 
 * Reduces the target player's coins by 3 if the target is also a Governor, otherwise by 2.
 * Sets the target's last action to None.
 * Updates the Governor's last action to Ability.
 * 
 * @param target The player to target for coin reduction.
 */
void Governor::ability(Player& target){
    if(target.get_type() == "Governor")
        target.setCoins(target.getCoins() - 3);
    else
        target.setCoins(target.getCoins() - 2);
    _last_action = Action::Ability;
    target.setAction(Action::None);
}