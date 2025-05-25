#ifndef GOVERNOR_HPP
#define GOVERNOR_HPP

#include "player.hpp"


class Governor : public Player{
    public:
        bool tax() override;
        std::string get_type() const override;
        Governor(const std::string& name) : Player(name) { }
};

#endif