#include "spy.hpp"


/**
 * @brief Executes the Spy's special ability on a given player.
 * 
 * This ability disables the target player's ability to arrest and returns the number of coins the target player has.
 * 
 * @param player Reference to the target player affected by the Spy.
 * @return int The number of coins the target player has.
 */
int Spy::spyAbility(Player& player){
    player.setCanArrest(false);
    _last_action = Action::Ability;
    return player.getCoins();
}

/**
 * @brief Returns the role type of this character.
 * 
 * @return std::string The string "Spy".
 */
std::string Spy::get_type() const{
    return "Spy";
}