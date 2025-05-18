#ifndef SPY_HPP
#define SPY_HPP

#include "player.hpp"

class Spy : public Player{
    public:
        int LookAndBlock(Player& player);
        std::string get_type() const override;
};
#endif