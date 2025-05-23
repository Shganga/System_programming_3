#ifndef BARON_HPP
#define BARON_HPP

#include "player.hpp"

class Baron : public Player {
public:
    void invest();
    std::string get_type() const override;
    Baron(const std::string& name) : Player(name) { }
};

#endif
