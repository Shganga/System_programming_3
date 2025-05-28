#include "baron.hpp"

void Baron::ability() {
    if (_coins < 3) {
        throw std::runtime_error("Baron needs at least 3 coins to use ability.");
    }
    _coins += 3;
}

std::string Baron::get_type() const{
    return "Baron";
}