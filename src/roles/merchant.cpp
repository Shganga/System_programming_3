#include "merchant.hpp"

std::string Merchant::get_type() const{
    return "Merchant";
}

/**
 * @brief Executes the Merchant's ability.
 * 
 * If the Merchant has at least 3 coins, this ability
 * increases the Merchant's coins by 1 and updates the
 * last action to Ability.
 */
void Merchant::ability(){
    if(_coins >= 3){
        _coins++;
        _last_action = Action::Ability;
    }
}