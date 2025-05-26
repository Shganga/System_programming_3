#include "baron.hpp"

bool Baron::ability() {
    _coins +=3;
    return true;
}

std::string Baron::get_type() const{
    return "Baron";
}