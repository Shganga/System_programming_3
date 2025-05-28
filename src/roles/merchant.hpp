#ifndef MERCHANT_HPP
#define MERCHANT_HPP

#include "player.hpp"

class Merchant : public Player {
public:
    std::string get_type() const override;
    Merchant(Game& game,const std::string& name,size_t index) : Player(game, name,index) { }
    void ability() override;
};

#endif
