#ifndef JUDGE_HPP
#define JUDGE_HPP

#include "player.hpp"

class Judge : public Player {
public:
    std::string get_type() const override;
    Judge(Game& game, const std::string& name, size_t index) : Player(game, name, index) { }
    void ability(Player& target) override;
};

#endif
