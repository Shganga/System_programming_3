#ifndef GENERAL_HPP
#define GENERAL_HPP

#include "player.hpp"

class General : public Player {
public:
    std::string get_type() const override;
    General(Game& game, const std::string& name,size_t index) : Player(game, name,index) { }
    void ability(Player& player) override;
};

#endif
