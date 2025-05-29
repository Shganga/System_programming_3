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
    ~Game(); // Destructor
    Game(const Game& other); // Copy constructor
    Game& operator=(const Game& other); // Copy assignment

    void add_player(const std::string& name);

    std::string turn() const;       
    std::vector<std::shared_ptr<Player>> getPlayers();       
    std::vector<std::string> players() const;  
    std::string winner() const;         
    std::string roleGenerator() const;
    int currentPlayerIndex() const;
    std::shared_ptr<Player> currentPlayer() const;
    void resetArrest();
    void next_turn(); 
    void bribe();    
    std::vector<std::shared_ptr<Player>> playersForSelection(const std::string& name);
    void gameCoup(const std::string& name);
    bool canAction();
    void manageAfterTrun();
    void manageNextTurn();
    void isGameDone();
    bool isGame();
    void restorePlayer();
    void setBribe(bool bribe);
    int getTurn() const;
    std::vector<std::shared_ptr<Player>> getOutList();
    bool getBribe() const;

private:
    std::vector<std::shared_ptr<Player>> _players_list;  
    std::vector<std::shared_ptr<Player>> _out_list;
    size_t _current_turn;     
    size_t _current_round;
    bool isbribe;
    bool isStillActive;
};

#endif
