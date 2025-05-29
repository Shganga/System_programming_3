#ifndef PLAYER_FACTORY_HPP
#define PLAYER_FACTORY_HPP

#include <memory>
#include <string>
#include "player.hpp"  // מחלקת הבסיס Player

class PlayerFactory {
public:
    static std::shared_ptr<Player> createPlayer(Game& game, const std::string& role, const std::string& name,int index);
};

#endif // PLAYER_FACTORY_HPP
