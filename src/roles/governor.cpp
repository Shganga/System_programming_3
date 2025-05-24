#include "governor.hpp"

bool Governor::tax() {
    if (_sanctioned) {
        return false;
    }
    _coins += 3;
    return true;
}
std::string Governor::get_type() const{
    return "Governor";
}