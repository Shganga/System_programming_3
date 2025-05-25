#ifndef SPY_HPP
#define SPY_HPP

#include "player.hpp"

class Spy : public Player{
    public:
        Spy(const std::string& name) : Player(name) { }
        int LookAndBlock(Player& player);
        std::string get_type() const override;
};
#endif