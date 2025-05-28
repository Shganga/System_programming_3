#ifndef GOVERNOR_HPP
#define GOVERNOR_HPP

#include "player.hpp"


class Governor : public Player{
    public:
        void tax() override;
        std::string get_type() const override;
        Governor(Game& game, const std::string& name, size_t index) : Player(game, name,index) { }
        void ability(Player& target) override;
};

#endif