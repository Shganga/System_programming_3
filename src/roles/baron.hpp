#ifndef BARON_HPP
#define BARON_HPP

#include "player.hpp"

class Baron : public Player {
public:
    void ability() override;
    std::string get_type() const override;
    Baron(Game& game, const std::string& name,size_t index) : Player(game, name, index) { }
};

#endif
