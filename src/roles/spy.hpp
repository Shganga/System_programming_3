#ifndef SPY_HPP
#define SPY_HPP

#include "player.hpp"

class Spy : public Player{
    public:
        Spy(Game& game,const std::string& name, size_t index) : Player(game,name,index) { }
        int spyAbility(Player& player);
        std::string get_type() const override;
};
#endif