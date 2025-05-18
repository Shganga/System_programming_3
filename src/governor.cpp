#include <governor.hpp>

void Governor::tax() {
    if (_sanctioned) {
        throw std::runtime_error("Player " + _name + " is sanctioned and cannot tax.");
    }
    _coins += 3;
}
std::string Governor::get_type() const{
    return "Governor";
}