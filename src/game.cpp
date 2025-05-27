#include "game.hpp"
#include "roles/player.hpp"
#include <random>
#include <chrono>

Game::Game() : _current_turn(0), _current_round(1) {}
Game::~Game() {
    // Smart pointers clean themselves up, but you can clear explicitly
    _players_list.clear();
    _out_list.clear();
}

Game::Game(const Game& other)
    : _current_turn(other._current_turn),
      _current_round(other._current_round),
      isbribe(other.isbribe)
{
    _players_list = other._players_list; // shared_ptr allows safe copy
    _out_list = other._out_list;
}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        _current_turn = other._current_turn;
        _current_round = other._current_round;
        isbribe = other.isbribe;
        _players_list = other._players_list;
        _out_list = other._out_list;
    }
    return *this;
}

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
    if(_current_turn == 0)
        return 0;
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

void Game::gameCoup(const std::string& name){
    for (auto it = _players_list.begin(); it != _players_list.end(); ++it) {
        if ((*it)->getName() == name) {
            _out_list.push_back(*it);       // מוסיף לרשימת המודחים
            _players_list.erase(it);        // מוחק מרשימת השחקנים הפעילים
            return;                         // יוצא אחרי שמצא
        }
    }
}

std::vector<std::shared_ptr<Player>> Game::playersForSelection(const std::string& name) {
    std::vector<std::shared_ptr<Player>> result;
    for (const auto& player : _players_list) {
        if (player->getName() != name) {
            result.push_back(player);
        }
    }
    return result;
}

std::string Game::winner() const {
    if(_players_list.size() == 1)
        return _players_list[0]->getName();
        
    throw std::runtime_error("The game is still ongoing");
}




