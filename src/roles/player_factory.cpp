#include "player_factory.hpp"

#include "spy.hpp"
#include "merchant.hpp"
#include "judge.hpp"
#include "governor.hpp"
#include "general.hpp"
#include "baron.hpp"

#include <algorithm>

std::shared_ptr<Player> PlayerFactory::createPlayer(Game& game, const std::string& role, const std::string& name, int index) {
    std::string role_lower = role;
    std::transform(role_lower.begin(), role_lower.end(), role_lower.begin(), ::tolower);

    if (role_lower == "spy") {
        return std::make_shared<Spy>(game,name,index);
    } else if (role_lower == "merchant") {
        return std::make_shared<Merchant>(game,name,index);
    } else if (role_lower == "judge") {
        return std::make_shared<Judge>(game,name,index);
    } else if (role_lower == "governor") {
        return std::make_shared<Governor>(game,name,index);
    } else if (role_lower == "general") {
        return std::make_shared<General>(game,name,index);
    } else if (role_lower == "baron") {
        return std::make_shared<Baron>(game,name,index);
    } else {
        return nullptr;
    }
}
