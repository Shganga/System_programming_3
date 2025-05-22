#include "game.hpp"
#include "roles/player.hpp"

GameBoard::GameBoard() : _current_turn(0), _current_round(1) {}

void GameBoard::add_player(std::shared_ptr<Player> player) {
    _players_list.push_back(player);
}

std::string GameBoard::turn() const {
    if (_players_list.empty()) {
        throw std::runtime_error("No players in the game");
    }
    return _players_list[_current_turn % _players_list.size()]->getName();
}

std::vector<std::string> GameBoard::players() const {
    std::vector<std::string> active_players;
    for (const auto& p : _players_list) {
        // שחקן נחשב פעיל אם לא נעצר (בהנחה שאין שדה שממש מציין אם הוא "חי")
        if (!p->isArrested()) {
            active_players.push_back(p->getName());
        }
    }
    return active_players;
}

std::string GameBoard::winner() const {
    if(_players_list.size() == 1)
        return _players_list[0]->getName();
        
    throw std::runtime_error("The game is still ongoing");
}


