#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include <string>


class Player {
public:
    Player(const std::string& name);
    virtual ~Player();
    Player(const Player& other);
    Player& operator=(const Player& other);

    void gather();
    virtual void tax();
    void bribe();
    void arrest(Player& target);
    void sanction(Player& target);
    void coup(Player& target);


    std::string getName() const;
    int getCoins() const;
    bool isSanctioned() const;
    void setSanctioned(bool status);

protected:
    std::string _name;
    int _coins;
    bool _sanctioned;
    std::string _lastArrestedby;
};

#endif
