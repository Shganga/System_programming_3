#ifndef GOVERNOR_HPP
#define GOVERNOR_HPP

#include "player.hpp"


class Governor : public Player{
    public:
        void tax() override;
        std::string get_type() const override;
};

#endif