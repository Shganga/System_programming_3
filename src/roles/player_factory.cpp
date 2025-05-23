#include "player_factory.hpp"

#include "spy.hpp"
#include "merchant.hpp"
#include "judge.hpp"
#include "governor.hpp"
#include "general.hpp"
#include "baron.hpp"

#include <algorithm>

std::shared_ptr<Player> PlayerFactory::createPlayer(const std::string& role, const std::string& name) {
    std::string role_lower = role;
    std::transform(role_lower.begin(), role_lower.end(), role_lower.begin(), ::tolower);

    if (role_lower == "spy") {
        return std::make_shared<Spy>(name);
    } else if (role_lower == "merchant") {
        return std::make_shared<Merchant>(name);
    } else if (role_lower == "judge") {
        return std::make_shared<Judge>(name);
    } else if (role_lower == "governor") {
        return std::make_shared<Governor>(name);
    } else if (role_lower == "general") {
        return std::make_shared<General>(name);
    } else if (role_lower == "baron") {
        return std::make_shared<Baron>(name);
    } else {
        return nullptr;
    }
}
