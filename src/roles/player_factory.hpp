#ifndef PLAYER_FACTORY_HPP
#define PLAYER_FACTORY_HPP

#include <memory>
#include <string>
#include "player.hpp"  // מחלקת הבסיס Player

class PlayerFactory {
public:
    static std::shared_ptr<Player> createPlayer(const std::string& role, const std::string& name);
};

#endif // PLAYER_FACTORY_HPP
