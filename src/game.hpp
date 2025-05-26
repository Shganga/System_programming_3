#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include "roles/player.hpp"

class Game {
public:
    Game();

    void add_player(std::shared_ptr<Player> player);

    std::string turn() const;       
    std::vector<std::shared_ptr<Player>> getPlayers();       
    std::vector<std::string> players() const;  
    std::string winner() const;         
    std::string roleGenerator() const;
    int currentPlayer() const;
    void resetArrest();
    void next_turn(); 
    void bribe();    

    

private:
    std::vector<std::shared_ptr<Player>> _players_list;  
    std::vector<std::shared_ptr<Player>> _out_list;
    size_t _current_turn;     
    size_t _current_round;
    bool isbribe;
};

#endif
