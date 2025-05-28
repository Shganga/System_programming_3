#include "governor.hpp"

void Governor::tax() {
    if (_sanctioned) {
        throw std::runtime_error("Governor is sanctioned and cannot collect tax.");
    }
    _coins += 3;
}
std::string Governor::get_type() const{
    return "Governor";
}