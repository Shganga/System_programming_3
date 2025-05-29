# Coup Game

## Overview

**Coup** is a strategic multiplayer game implemented in C++ with an SFML graphical user interface. Players take on random roles, each with unique abilities, to outsmart and eliminate opponents by collecting coins and performing actions. The game simulates core Coup mechanics with several character roles, enforcing game rules, and providing turn-based gameplay.

---

## Features

- Turn-based multiplayer game (2-6 players)  
- Roles with unique abilities and strategic impact  
- Built with C++ and SFML for graphical interface  
- Dynamic player creation using a factory pattern  
- Rule enforcement including coin costs and sanctions  
- Action history and game state management  

---

## Roles and Abilities

### Baron
- Ability costs 3 coins  
- Gains 6 coins when ability is used  
- Must coup if holding 10 or more coins  

### Governor
- Collects 3 coins as tax if not sanctioned  
- Ability blocks tax of othr players (3 coins if Governor, else 2)
- Must coup if holding 10 or more coins  

### General
- Ability costs 5 coins  
- Blocks a coup  
- Sets target’s last action to none  

### Judge
- Ability negates target’s action and stops bribes in the game  

### Merchant
- Passive ability: gains 1 coin if holding at least 3 coins  

### Spy
- Ability to see a players coins and make him not be able to arrest

---

## Game Rules

- Players start with initial coins and a random role  
- Players take turns performing actions or abilities  
- Certain abilities require coin payment  
- Holding 10 or more coins forces a mandatory coup action  
- Players can be sanctioned or arrested affecting their abilities  
- The game progresses turn-by-turn until a win condition (only 1 player left)

---

## Project Structure

    .
    ├── src/            # Directory for the game logic and GUI
    │   └── roles/      # Directory for player, roles, and playerFactory
    ├── test/           # Directory for the tests
    ├── Makefile        # Build automation file
    └── README.md       # Project documentation




## Building the Project

### Requirements

- C++17 compiler (I used g++ can also work with clang)  
- SFML development libraries installed (graphics, window, system)  


## Installation

### Install SFML development libraries (Ubuntu/Debian):
```bash
sudo apt update && sudo apt install -y libsfml-dev
```

## Compile

```bash
make Main
./main
```

### Compile and run
```bash
make run
```

### Compile and run tests
```bash
make test
```

### Valgrind
```bash
make valgrind
```
