#include "merchant.hpp"

std::string Merchant::get_type() const{
    return "Merchant";
}

void Merchant::ability(){
    if(_coins >= 3){
        _coins++;
        _last_action = Action::Ability;
    }
}