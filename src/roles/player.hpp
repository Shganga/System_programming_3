#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include <string>
#include "actions.hpp"

class Game;

class Player {
public:
    Player(Game& game,const std::string& name, size_t index);
    virtual ~Player();
    Player(const Player& other);
    Player& operator=(const Player& other);

    void gather();
    virtual void tax();
    void bribe();
    void arrest(Player& target);
    void sanction(Player& target);
    void coup(Player& target);
    virtual std::string get_type() const;
    virtual void ability();
    virtual int spyAbility(Player& target);
    virtual void ability(Player& target);


    std::string getName() const;
    int getCoins() const;
    bool isSanctioned() const;
    void setSanctioned(bool status);
    bool isArrested() const;
    void setArrest(bool status);
    void setCoins(int coins);
    void setCanArrest(bool can);
    bool getCanArrest();
    size_t getIndex() const;
    void setIndex(size_t index);
    void setAction(Action action);
    Action getLastAction() const;

protected:
    std::string _name;
    int _coins;
    bool _sanctioned;
    bool _arrested;
    bool _can_arrest;
    Game& _game;
    Action _last_action;
    size_t _index;
};

#endif
