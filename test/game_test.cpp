#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "game.hpp"
#include "roles/player.hpp" // Assuming this is your base Player header for role classes
#include "roles/player_factory.hpp" // For creating specific player roles in tests

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>



TEST_CASE("Game Class") {
    SUBCASE("Default Constructor") {
        Game game;
        CHECK(game.getTurn() == 0);
        CHECK(game.isGame() == true);
        CHECK(game.players().empty());
        CHECK(game.getBribe() == false);
    }


    SUBCASE("add_player and players()") {
        Game game;
        game.add_player("Alice");
        CHECK(game.players().size() == 1);
        CHECK(game.players()[0] == "Alice");

        game.add_player("Bob");
        CHECK(game.players().size() == 2);
        CHECK(game.players()[1] == "Bob");

        CHECK_THROWS_AS(game.add_player("Alice"), std::runtime_error); // Duplicate name
    }

    SUBCASE("turn() and currentPlayerIndex()") {
        Game game;
        CHECK_THROWS_AS(game.turn(), std::runtime_error); // No players yet
        CHECK_THROWS_AS(game.currentPlayer(), std::runtime_error); // No players yet
        CHECK_THROWS_AS(game.currentPlayerIndex(), std::runtime_error); // No players yet

        game.add_player("Alice");
        game.add_player("Bob");
        game.add_player("Charlie");

        CHECK(game.turn() == "Alice");
        CHECK(game.currentPlayerIndex() == 0);
        CHECK(game.currentPlayer()->getName() == "Alice");

        // Simulate a turn sequence via player actions to properly advance game state
        game.currentPlayer()->gather(); // Alice's turn action, advances turn
        CHECK(game.turn() == "Bob");
        CHECK(game.currentPlayerIndex() == 1);
        CHECK(game.currentPlayer()->getName() == "Bob");

        game.currentPlayer()->gather(); // Bob's turn action, advances turn
        CHECK(game.turn() == "Charlie");
        CHECK(game.currentPlayerIndex() == 2);
        CHECK(game.currentPlayer()->getName() == "Charlie");

        game.currentPlayer()->gather(); // Charlie's turn action, advances turn, new round
        CHECK(game.turn() == "Alice");
        CHECK(game.getTurn() == 3); // Total turns taken
        CHECK(game.currentPlayerIndex() == 0);
        CHECK(game.currentPlayer()->getName() == "Alice");
    }
}

TEST_CASE("Game Class - Turn Management and Flow") {
    Game game;
    game.add_player("Alice"); 
    game.add_player("Bob");   
    game.add_player("Charlie"); 

    std::shared_ptr<Player> alice_ptr = game.getPlayers()[0];
    std::shared_ptr<Player> bob_ptr = game.getPlayers()[1];
    std::shared_ptr<Player> charlie_ptr = game.getPlayers()[2];

    alice_ptr->setCoins(0);
    bob_ptr->setCoins(0);
    charlie_ptr->setCoins(0);


    SUBCASE("next_turn without bribe ") {
        CHECK(game.turn() == "Alice");
        CHECK(game.getTurn()== 0);
        CHECK(game.getBribe() == false);

        // Simulate a player action that triggers next_turn()
        alice_ptr->gather(); // This calls game.next_turn() internally
        CHECK(game.turn() == "Bob");
        CHECK(game.getTurn() == 1);
        CHECK(game.getBribe() == false);

        bob_ptr->gather();
        CHECK(game.turn() == "Charlie");
        CHECK(game.getTurn() == 2);

        charlie_ptr->gather(); // End of round for 3 players
        CHECK(game.turn() == "Alice");
        CHECK(game.getTurn()== 3);
        CHECK(alice_ptr->isArrested() == false); // Arrest status should be reset by manageAfterTurn
        CHECK(bob_ptr->isArrested() == false);
        CHECK(charlie_ptr->isArrested() == false);
    }

    SUBCASE("next_turn with bribe -") {
        alice_ptr->setCoins(4); 
        alice_ptr->bribe(); 
        CHECK(game.getBribe() == true); 
        CHECK(game.turn() == "Alice"); 
        CHECK(game.getTurn() == 0);
    }

    SUBCASE("resetArrest") {
        alice_ptr->setArrest(true);
        bob_ptr->setArrest(true);
        charlie_ptr->setArrest(false);

        game.resetArrest();
        CHECK(alice_ptr->isArrested() == false);
        CHECK(bob_ptr->isArrested() == false);
        CHECK(charlie_ptr->isArrested() == false);
    }


    SUBCASE("canAction - sanctioned, few coins, other players not arrested") {
        alice_ptr->setSanctioned(true);
        alice_ptr->setCoins(1); // Less than 2
        bob_ptr->setArrest(false); // Bob not arrested
        charlie_ptr->setArrest(true); // Charlie is arrested
        CHECK(game.canAction() == true); // Bob can still be targeted for actions
    }

    SUBCASE("canAction - sanctioned, few coins, all other players arrested") {
        alice_ptr->setSanctioned(true);
        alice_ptr->setCoins(1);
        bob_ptr->setArrest(true);
        charlie_ptr->setArrest(true);
        CHECK(game.canAction() == false);
    }

    SUBCASE("manageAfterTrun resets flags") {
        // Manually set player state, then call the game function
        alice_ptr->setSanctioned(true);
        alice_ptr->setCanArrest(false);

        game.manageAfterTrun(); // Manually call, typically called by next_turn
        CHECK(alice_ptr->isSanctioned() == false);
        CHECK(alice_ptr->getCanArrest() == true); // Should be reset to true
    }
}

TEST_CASE("Game Class ") {
    Game game;
    game.add_player("Alice");
    game.add_player("Bob");
    game.add_player("Charlie");

    SUBCASE("gameCoup and isGameDone") {
        CHECK(game.isGame() == true); // Game is active

        // Coup Bob
        game.getPlayers()[0]->setCoins(7); // Alice needs coins for coup
        game.getPlayers()[0]->coup(*game.getPlayers()[1]); // Alice coups Bob
        // After coup, Bob is moved to out_list.
        // Game::gameCoup updates players_list and calls isGameDone.
        CHECK(game.players().size() == 2);
        CHECK(game.players()[0] == "Alice");
        CHECK(game.players()[1] == "Charlie"); // Charlie shifted to index 1
        CHECK(game.isGame() == true); // Still active (2 players left)
        CHECK(game.getOutList().size() == 1);
        CHECK(game.getOutList()[0]->getName() == "Bob");


        // Coup Charlie
        game.getPlayers()[0]->setCoins(7); // Alice needs coins for next coup
        game.getPlayers()[0]->coup(*game.getPlayers()[1]); // Alice coups Charlie (now at index 1)
        CHECK(game.players().size() == 1);
        CHECK(game.players()[0] == "Alice");
        CHECK(game.isGame() == false); // Game should be done now
        CHECK(game.getOutList().size() == 2);
        CHECK(game.getOutList()[1]->getName() == "Charlie"); // Charlie is now the last one out
    }



    SUBCASE("playersForSelection") {
        Game game;
        game.add_player("Alice");
        game.add_player("Bob");
        game.add_player("Charlie");

        std::vector<std::shared_ptr<Player>> selection_for_alice = game.playersForSelection("Alice");
        CHECK(selection_for_alice.size() == 2);
        CHECK(selection_for_alice[0]->getName() == "Bob");
        CHECK(selection_for_alice[1]->getName() == "Charlie");

        std::vector<std::string> selection_names;
        for (const auto& p : game.playersForSelection("Bob")) {
            selection_names.push_back(p->getName());
        }
        CHECK(selection_names.size() == 2);
        CHECK((selection_names[0] == "Alice" || selection_names[1] == "Alice"));
        CHECK((selection_names[0] == "Charlie" || selection_names[1] == "Charlie"));
    }
}

TEST_CASE("Player Class - Basic Actions") {
    Game game;

    game.add_player("P1"); 
    game.add_player("P2"); 
    std::shared_ptr<Player> p1 = game.getPlayers()[0];
    std::shared_ptr<Player> p2 = game.getPlayers()[1];

    p1->setCoins(0);
    p2->setCoins(0);
    p1->setSanctioned(false);
    p2->setSanctioned(false);

    SUBCASE("gather") {
        p1->gather();
        CHECK(p1->getCoins() == 1);
        CHECK(p1->getLastAction() == Action::Gather); // Corrected enum access
        CHECK(game.turn() == "P2"); // Turn advanced
    }

    SUBCASE("gather - sanctioned") {
        p1->setSanctioned(true);
        CHECK_THROWS_AS(p1->gather(), std::runtime_error);
        p1->setSanctioned(false); // Reset for other tests
    }

    SUBCASE("gather - 10 or more coins") {
        p1->setCoins(10);
        CHECK_THROWS_AS(p1->gather(), std::runtime_error);
    }

    SUBCASE("tax") {
        p1->tax();
        CHECK(p1->getCoins() == 2);
        CHECK(p1->getLastAction() == Action::Tax); // Corrected enum access
        CHECK(game.turn() == "P2");
    }

    SUBCASE("tax - sanctioned") {
        p1->setSanctioned(true);
        CHECK_THROWS_AS(p1->tax(), std::runtime_error);
        p1->setSanctioned(false);
    }

    SUBCASE("tax - 10 or more coins") {
        p1->setCoins(10);
        CHECK_THROWS_AS(p1->tax(), std::runtime_error);
    }

    SUBCASE("bribe") {
        p1->setCoins(4);
        p1->bribe();
        CHECK(p1->getCoins() == 0);
        CHECK(game.getBribe() == true); 
        CHECK(p1->getLastAction() == Action::Bribe); 
    }

    SUBCASE("bribe - not enough coins") {
        p1->setCoins(3);
        CHECK_THROWS_AS(p1->bribe(), std::runtime_error);
    }

    SUBCASE("bribe - 10 or more coins") {
        p1->setCoins(10);
        CHECK_THROWS_AS(p1->bribe(), std::runtime_error);
    }

    SUBCASE("coup") {
        p1->setCoins(7);
        p1->coup(*p2);
        CHECK(p1->getCoins() == 0);
        CHECK(game.players().size() == 1);
        CHECK(game.players()[0] == "P1");
        CHECK(game.isGame() == false); // Game should end
        CHECK(p1->getLastAction() == Action::Coup); // Corrected enum access
        CHECK(game.turn() == "P1"); // P2 is out, current player remains P1 (game done)
    }

    SUBCASE("coup - not enough coins") {
        p1->setCoins(6);
        CHECK_THROWS_AS(p1->coup(*p2), std::runtime_error);
    }
}

TEST_CASE("Player Class - Targeted Actions (Arrest, Sanction)") {
    Game game;
    // Set up players specifically with PlayerFactory for controlled testing
    game.add_player("Alice"); // P0
    game.add_player("Bob");   // P1
    game.add_player("Charlie"); // P2

    std::shared_ptr<Player> alice = game.getPlayers()[0];
    std::shared_ptr<Player> bob = game.getPlayers()[1];
    std::shared_ptr<Player> charlie = game.getPlayers()[2];


    SUBCASE("arrest") {
        alice->setCoins(5);
        bob->setCoins(5);

        alice->arrest(*bob);
        CHECK(bob->getCoins() == 4);
        if (bob->get_type() == "Merchant")
            CHECK(alice->getCoins() == 5); 
        else
            CHECK(alice->getCoins() == 6); 
        CHECK(bob->isArrested() == true);
        CHECK(alice->getLastAction() == Action::Arrest); // Corrected enum access
        CHECK(game.turn() == "Bob");
    
    }

    SUBCASE("arrest - already arrested") {
        bob->setArrest(true);
        alice->setCoins(5);
        bob->setCoins(1);
        CHECK_THROWS_AS(alice->arrest(*bob), std::runtime_error);
    }

    SUBCASE("arrest - current player cannot arrest") {
        alice->setCanArrest(false); // Simulate ability preventing arrest
        alice->setCoins(5);
        bob->setCoins(1);
        CHECK_THROWS_AS(alice->arrest(*bob), std::runtime_error);
    }

    SUBCASE("arrest - target no coins") {
        alice->setCoins(5);
        bob->setCoins(0);
        CHECK_THROWS_AS(alice->arrest(*bob), std::runtime_error);
    }

    SUBCASE("arrest - 10 or more coins (arresting player)") {
        alice->setCoins(10);
        bob->setCoins(1);
        CHECK_THROWS_AS(alice->arrest(*bob), std::runtime_error);
    }

    SUBCASE("sanction - normal cost") {
        std::vector<std::shared_ptr<Player>> players_ref = game.getPlayers();
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Spy", "SpyGuy", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "General", "Judy", 1));
        std::shared_ptr<Player> spy = players_ref[0];
        std::shared_ptr<Player> general = players_ref[1];
        general->setCoins(5);

        general->sanction(*spy);
        CHECK(general->getCoins() == 2);

        CHECK(spy->isSanctioned() == true);
        CHECK(general->getLastAction() == Action::Sanction); // Corrected enum access
    }

    SUBCASE("sanction - target is Judge") {
        std::vector<std::shared_ptr<Player>> players_ref = game.getPlayers();
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Spy", "SpyGuy", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "Judge", "Judy", 1));
        std::shared_ptr<Player> spy = players_ref[0];
        std::shared_ptr<Player> judge = players_ref[1];

        spy->setCoins(5);
        judge->setSanctioned(false);

        spy->sanction(*judge);
        CHECK(spy->getCoins() == 1);
        CHECK(judge->isSanctioned() == true);
        CHECK(spy->getLastAction() == Action::Sanction);
    }

    SUBCASE("sanction - target is Baron") {
        std::vector<std::shared_ptr<Player>> players_ref = game.getPlayers();
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Governor", "GovGary", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "Baron", "BaronessB", 1));
        std::shared_ptr<Player> governor = players_ref[0];
        std::shared_ptr<Player> baron = players_ref[1];

        governor->setCoins(5);
        baron->setCoins(0);
        baron->setSanctioned(false);

        governor->sanction(*baron);
        CHECK(governor->getCoins() == 2); 
        CHECK(baron->getCoins() == 1); 
        CHECK(baron->isSanctioned() == true);
        CHECK(governor->getLastAction() == Action::Sanction);
    }

    SUBCASE("sanction - not enough coins (normal)") {
        alice->setCoins(2);
        CHECK_THROWS_AS(alice->sanction(*bob), std::runtime_error);
    }

    SUBCASE("sanction - not enough coins (for Judge)") {
        std::vector<std::shared_ptr<Player>> players_ref = game.getPlayers();
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Spy", "SpyGuy", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "Judge", "JudgeJudy", 1));
        std::shared_ptr<Player> spy = players_ref[0];
        std::shared_ptr<Player> judge = players_ref[1];

        spy->setCoins(3); // Needs 4 for Judge
        CHECK_THROWS_AS(spy->sanction(*judge), std::runtime_error);
    }

    SUBCASE("sanction - 10 or more coins (sanctioning player)") {
        alice->setCoins(10);
        bob->setCoins(0);
        CHECK_THROWS_AS(alice->sanction(*bob), std::runtime_error);
    }
}

TEST_CASE("Player Class - Role Abilities (Specific Tests)") {
    Game game;

    game.add_player("Dummy1"); 
    game.add_player("Dummy2"); 


    std::vector<std::shared_ptr<Player>> players_ref = game.getPlayers();


    SUBCASE("Spy ability - targeted") {
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Spy", "SpySam", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "General", "OtherPlayer", 1));
        std::shared_ptr<Player> spy = players_ref[0];
        std::shared_ptr<Player> other = players_ref[1];

        spy->setCoins(0);
        other->setCoins(5); 

        int coins = spy->spyAbility(*other); 
        CHECK(coins == other->getCoins());
        CHECK(spy->getLastAction() == Action::Ability); 
    }


    SUBCASE("Judge ability") {
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Judge", "JudgeJudy", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "Spy", "SpyGuy", 1)); // A player who can be sanctioned
        std::shared_ptr<Player> judge = players_ref[0];
        std::shared_ptr<Player> spy = players_ref[1];

        judge->setCoins(0);
        spy->setCoins(5);

        spy->bribe();
        CHECK(game.getBribe() == true);
        judge->ability(*spy); 
        CHECK(game.getBribe() == false);
        CHECK(judge->getLastAction() == Action::Ability);
        CHECK(spy->getLastAction() == Action::None);
    }


    SUBCASE("Governor ability") {
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Governor", "GovGary", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "General", "OtherPlayer", 1));
        std::shared_ptr<Player> governor = players_ref[0];
        std::shared_ptr<Player> other = players_ref[1];

        governor->setCoins(0);
        governor->tax();
        other->tax();
        governor->ability(*other); 
        CHECK(governor->getCoins() == 3);
        CHECK(governor->getLastAction()== Action::Ability);
        CHECK(other->getCoins() == 0);
        CHECK(other->getLastAction()== Action::None);
    }


    SUBCASE("Baron ability") {
        players_ref.clear();
        players_ref.push_back(PlayerFactory::createPlayer(game, "Baron", "BaronVon", 0));
        players_ref.push_back(PlayerFactory::createPlayer(game, "Spy", "OtherPlayer", 1));
        std::shared_ptr<Player> baron = players_ref[0];
        std::shared_ptr<Player> other = players_ref[1];

        baron->setCoins(0);
        other->setCoins(5);
        other->sanction(*baron);
        CHECK(baron->getCoins() == 1);
        baron->setCoins(3);
        baron->ability(); 
        CHECK(baron->getCoins() == 6);
        CHECK(baron->getLastAction()== Action::Ability);
    }

}