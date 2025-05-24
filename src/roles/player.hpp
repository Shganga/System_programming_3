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

    bool gather();
    virtual bool tax();
    bool bribe();
    void arrest(Player& target);
    void sanction(Player& target);
    void coup(Player& target);
    virtual std::string get_type() const = 0;


    std::string getName() const;
    int getCoins() const;
    bool isSanctioned() const;
    void setSanctioned(bool status);
    bool isArrested() const;
    void setArrest(bool status);

protected:
    std::string _name;
    int _coins;
    bool _sanctioned;
    bool _arrested;
    bool _can_arrest;
};

#endif
