
# Хрестики-Нулики (Tic-Tac-Toe) Game

## About the Project

This is an advanced implementation of the classic Tic-Tac-Toe game written in C++. The game features:

- Multiple field sizes (3x3, 4x4, 5x5)
- Various game modes:
  - Player vs Player
  - Player vs Computer
  - Computer vs Computer (demonstration)
- Six levels of AI difficulty:
  - Newbie
  - Easy
  - Medium
  - Hard
  - Expert
  - Master
- Statistics tracking
- Colorful terminal interface
- Game state saving

## Compilation and Execution

### Prerequisites

- C++ compiler (clang++ or g++)
- Make utility
- Standard C++ libraries

### Compilation Commands

To compile the game, run:

```bash
make
```

Or manually:

```bash
clang++ -g -Wall -Werror ./main.cpp -o main
```

To compile a debug version:

```bash
make main-debug
```

### Running the Game

To run the game after compilation:

```bash
./main
```

### Cleaning Up

To remove compiled files:

```bash
make clean
```

## Game Controls

- When prompted, enter row and column numbers to make a move
- Follow on-screen instructions to navigate menus
- Use 'y' to confirm or 'n' to deny when asked yes/no questions

## Features

- **Minimax Algorithm**: Advanced AI uses minimax with alpha-beta pruning
- **Colorful Interface**: Enjoy a vibrant terminal experience
- **Statistics**: Track player performance with a persistent leaderboard
- **Adjustable Difficulty**: Challenge yourself with various AI skill levels
- **Flexible Board Size**: Play on different sized grids

## Project Structure

The game is built using object-oriented programming principles with the following main classes:

- `GameField`: Manages the game board
- `Player`: Base class for players (Human and AI)
- `GameManager`: Controls game flow and rules
- `GameSetup`: Handles menu and configuration
- `GameStats`: Tracks and displays statistics
