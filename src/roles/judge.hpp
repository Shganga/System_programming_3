#ifndef JUDGE_HPP
#define JUDGE_HPP

#include "player.hpp"

class Judge : public Player {
public:
    std::string get_type() const override;
};

#endif
