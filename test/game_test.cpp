#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "game.hpp"
#include "roles/player.hpp"

#include <memory>
#include <vector>
#include <stdexcept>

TEST_CASE("Game basic lifecycle") {
    Game game;

    CHECK(game.players().empty());
    CHECK_THROWS(game.turn());  // no players, should throw

    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");
    game.add_player(p1);
    game.add_player(p2);

    CHECK(game.turn() == "Alice");
    CHECK(game.currentPlayerIndex() == 0);
    
    game.next_turn();
    CHECK(game.turn() == "Bob");
    CHECK(game.currentPlayerIndex() == 1);

    // Test next_turn increments round after full cycle
    game.next_turn(); // back to Alice, round 2
    CHECK(game.turn() == "Alice");
    CHECK(game.currentPlayerIndex() == 0);

    // test bribe affects next_turn logic
    game.bribe();
    int old_turn = game.currentPlayerIndex();
    game.next_turn();
    CHECK(game.currentPlayerIndex() == old_turn); // turn should not advance due to bribe
}

TEST_CASE("Game::add_player and players list") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");
    game.add_player(p1);
    game.add_player(p2);

    std::vector<std::string> names = game.players();
    CHECK(names.size() == 2);
    CHECK((names[0] == "Alice" || names[1] == "Alice"));
    CHECK((names[0] == "Bob" || names[1] == "Bob"));
}

TEST_CASE("Game::canAction logic") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");

    game.add_player(p1);
    game.add_player(p2);

    // Default: not sanctioned, coins = 0
    p1->setCoins(3);
    p1->setSanctioned(false);
    p1->setArrest(false);
    p2->setArrest(false);

    CHECK(game.canAction() == true);

    SUBCASE("Sanctioned player with > 2 coins can act") {
        p1->setSanctioned(true);
        p1->setCoins(3);
        CHECK(game.canAction() == true);
    }

    SUBCASE("Sanctioned player with 0 coins, no valid targets") {
        p1->setSanctioned(true);
        p1->setCoins(0);
        p2->setArrest(true);
        CHECK(game.canAction() == false);
    }

    SUBCASE("Sanctioned player with 0 coins but has unarrested targets") {
        p1->setSanctioned(true);
        p1->setCoins(0);
        p2->setArrest(false);
        CHECK(game.canAction() == true);
    }
}

TEST_CASE("Game::resetArrest") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");
    game.add_player(p1);
    game.add_player(p2);

    p1->setArrest(true);
    p2->setArrest(true);

    game.resetArrest();

    CHECK(!p1->isArrested());
    CHECK(!p2->isArrested());
}

TEST_CASE("Game::gameCoup removes player") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");
    game.add_player(p1);
    game.add_player(p2);

    game.gameCoup("Alice");

    std::vector<std::string> remaining_names = game.players();
    CHECK(remaining_names.size() == 1);
    CHECK(remaining_names[0] == "Bob");
}

TEST_CASE("Game::playersForSelection returns others") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    auto p2 = std::make_shared<Player>("Bob");
    auto p3 = std::make_shared<Player>("Charlie");
    game.add_player(p1);
    game.add_player(p2);
    game.add_player(p3);

    auto others = game.playersForSelection("Alice");
    CHECK(others.size() == 2);
    for (auto& p : others) {
        CHECK(p->getName() != "Alice");
    }
}

TEST_CASE("Game::winner returns the only player") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice");
    game.add_player(p1);

    CHECK(game.winner() == "Alice");

    auto p2 = std::make_shared<Player>("Bob");
    game.add_player(p2);

    CHECK_THROWS_AS(game.winner(), std::runtime_error);
}

TEST_CASE("Game::roleGenerator returns valid role") {
    Game game;

    // Roles from your code
    std::vector<std::string> roles = {
        "Spy", "Merchant", "Judge", "Governor", "General", "Baron"
    };

    for (int i = 0; i < 100; ++i) {
        std::string role = game.roleGenerator();
        CHECK(std::find(roles.begin(), roles.end(), role) != roles.end());
    }
}

TEST_CASE("Game copy constructor and assignment") {
    Game game1;
    auto p1 = std::make_shared<Player>("Alice");
    game1.add_player(p1);

    Game game2(game1);
    CHECK(game2.turn() == "Alice");

    Game game3;
    game3 = game1;
    CHECK(game3.turn() == "Alice");
}
