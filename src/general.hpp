#ifndef GENERAL_HPP
#define GENERAL_HPP

#include "player.hpp"

class General : public Player {
public:
    std::string get_type() const override;
};

#endif
