#include <baron.hpp>

void Baron::invest() {
    _coins +=3;
}

std::string Baron::get_type() const{
    return "Baron";
}