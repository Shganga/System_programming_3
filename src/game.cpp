#include "game.hpp"
#include "roles/player.hpp"
#include <random>
#include <chrono>

Game::Game() : _current_turn(0), _current_round(1) {}

void Game::add_player(std::shared_ptr<Player> player) {
    _players_list.push_back(player);
}

std::string Game::turn() const {
    if (_players_list.empty()) {
        throw std::runtime_error("No players in the game");
    }
    return _players_list[_current_turn % _players_list.size()]->getName();
}

void Game::next_turn(){
    if(!isbribe){
        _current_turn++;
        if(_current_turn % _players_list.size() == 0){
            _current_round++;
            resetArrest();
        }
    }
    else{
        isbribe = false;
    }
}

void Game::bribe(){
    isbribe = true;
}

void Game::resetArrest(){
    for (size_t i = 0; i < _players_list.size(); i++)
    {
        _players_list[i]->setArrest(false);
    }
}

int Game::currentPlayer() const{
    return _current_turn % _players_list.size();
}

std::vector<std::string> Game::players() const {
    std::vector<std::string> active_players;
    for (const auto& p : _players_list) {
        // שחקן נחשב פעיל אם לא נעצר (בהנחה שאין שדה שממש מציין אם הוא "חי")
        if (!p->isArrested()) {
            active_players.push_back(p->getName());
        }
    }
    return active_players;
}

std::vector<std::shared_ptr<Player>> Game::getPlayers(){
    return _players_list;
}

std::string Game::roleGenerator() const {
    static const std::vector<std::string> roles = {
        "Spy", "Merchant", "Judge", "Governor", "General", "Baron"
    };

    // יצירת גנרטור אקראי עם seed מבוסס זמן
    static std::mt19937 rng(
        static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
    );

    std::uniform_int_distribution<size_t> dist(0, roles.size() - 1);
    return roles[dist(rng)];
}

std::string Game::winner() const {
    if(_players_list.size() == 1)
        return _players_list[0]->getName();
        
    throw std::runtime_error("The game is still ongoing");
}


