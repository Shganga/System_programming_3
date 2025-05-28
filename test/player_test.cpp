#include "doctest.h"
#include "player.hpp"

// מחלקה דמה בשביל get_type()
class TestPlayer : public Player {
public:
    TestPlayer(const std::string& name, const std::string& type)
        : Player(name), type_(type) {}
    std::string get_type() const override { return type_; }
private:
    std::string type_;
};

TEST_CASE("Player creation and basic getters/setters") {
    TestPlayer p("Alice", "Spy");
    CHECK(p.getName() == "Alice");
    CHECK(p.getCoins() == 0);
    CHECK(p.isSanctioned() == false);
    CHECK(p.isArrested() == false);

    p.setCoins(5);
    CHECK(p.getCoins() == 5);

    p.setSanctioned(true);
    CHECK(p.isSanctioned() == true);

    p.setArrest(true);
    CHECK(p.isArrested() == true);

    p.setCanArrest(false);
    CHECK(p.getCanArrest() == false);
}

TEST_CASE("Player::gather and tax") {
    TestPlayer p("Bob", "General");

    CHECK(p.gather() == true);
    CHECK(p.getCoins() == 1);

    CHECK(p.tax() == true);
    CHECK(p.getCoins() == 3);

    p.setSanctioned(true);
    CHECK(p.gather() == false);
    CHECK(p.tax() == false);
}

TEST_CASE("Player::bribe") {
    TestPlayer p("Cleo", "Governor");

    p.setCoins(3);
    CHECK(p.bribe() == false);  // Not enough coins

    p.setCoins(4);
    CHECK(p.bribe() == true);
    CHECK(p.getCoins() == 0);
}

TEST_CASE("Player::arrest") {
    TestPlayer arrestor("Dan", "Spy");
    TestPlayer target("Eli", "Merchant");

    target.setCoins(5);
    CHECK(arrestor.arrest(target) == true);
    CHECK(target.isArrested() == true);
    CHECK(target.getCoins() == 3); // lost 2 coins (Merchant)
    CHECK(arrestor.getCoins() == 0);

    // Arrest again should fail
    CHECK(arrestor.arrest(target) == false);
}

TEST_CASE("Player::arrest with General") {
    TestPlayer arrestor("Fred", "General");
    TestPlayer target("Gabe", "Spy");

    target.setCoins(5);
    CHECK(arrestor.arrest(target) == true);
    CHECK(target.getCoins() == 4); 
    CHECK(arrestor.getCoins() == 1); 
}

TEST_CASE("Player::sanction") {
    TestPlayer s("Hana", "Spy");
    TestPlayer target("Isaac", "Baron");

    s.setCoins(3);
    CHECK(s.sanction(target) == true);
    CHECK(target.isSanctioned() == true);
    CHECK(s.getCoins() == 0);
    CHECK(target.getCoins() == 1);  

    TestPlayer target2("Judgey", "Judge");
    TestPlayer j("Jill", "Spy");
    j.setCoins(4);
    CHECK(j.sanction(target2) == true);
    CHECK(j.getCoins() == 0);  
}

TEST_CASE("Player::coup") {
    TestPlayer p("Kate", "Spy");

    p.setCoins(6);
    CHECK(p.coup() == false);

    p.setCoins(7);
    CHECK(p.coup() == true);
    CHECK(p.getCoins() == 0);
}
