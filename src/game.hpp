#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include "player.hpp"

class GameBoard {
public:
    GameBoard();

    void add_player(std::shared_ptr<Player> player);

    std::string turn() const;               
    std::vector<std::string> players() const;  
    std::string winner() const;           

    void next_turn();                      

    

private:
    std::vector<std::shared_ptr<Player>> _players_list;  
    std::vector<std::shared_ptr<Player>> _out_list;
    size_t _current_turn;     
    size_t _current_round;
};

#endif
