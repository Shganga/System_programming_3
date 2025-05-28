#include "baron.hpp"

bool Baron::ability() {
    if(_coins < 3)
        return false;
    _coins +=3;
    return true;
}

std::string Baron::get_type() const{
    return "Baron";
}