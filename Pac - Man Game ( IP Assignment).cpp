#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>

using namespace std;

// Game constants
const int WIDTH = 15;
const int HEIGHT = 13;
const char WALL = '#';
const char DOT = '.';
const char POWER = 'O';
const char PACMAN = 'C';
const char GHOST = 'M';
const char EMPTY = ' ';

// Game state
struct Position {
    int x, y;
};

struct Ghost {
    Position pos;
    char prevChar;
};

class Game {
private:
    vector<vector<char>> board;
    Position pacman;
    vector<Ghost> ghosts;
    int score;
    int lives;
    bool gameOver;
    bool won;
    bool powerMode;
    int powerTimer;

    // Initial board layout
    const vector<string> INITIAL_BOARD = {
        "###############",
        "#.....#...O...#",
        "#.###.#.###.#.#",
        "#.....#.....#.#",
        "#.#.#######.#.#",
        "#.............#",
        "#.###.#.###.#.#",
        "#...#.#.#...#.#",
        "###.#.#.#.#.###",
        "#.............#",
        "#.###.#.###.#.#",
        "#O....#.....O.#",
        "###############"
    };

public:
    Game() {
        initGame();
    }

    void initGame() {
        // Initialize board
        board.resize(HEIGHT, vector<char>(WIDTH));
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                board[y][x] = INITIAL_BOARD[y][x];
            }
        }

        // Initialize Pac-Man
        pacman = {7, 6};
        board[pacman.y][pacman.x] = PACMAN;

        // Initialize ghosts
        ghosts = {
            {{1, 1}, DOT},
            {{13, 1}, DOT},
            {{1, 11}, DOT},
            {{13, 11}, DOT}
        };

        // Initialize game state
        score = 0;
        lives = 3;
        gameOver = false;
        won = false;
        powerMode = false;
        powerTimer = 0;
    }

    void draw() {
        system("cls");
        cout << "Score: " << score << " Lives: " << lives << (powerMode ? " POWER MODE!" : "") << endl;
        
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                cout << board[y][x];
            }
            cout << endl;
        }
    }

    bool isValidMove(int x, int y) {
        return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && board[y][x] != WALL;
    }

    void movePacman(char direction) {
        int newX = pacman.x;
        int newY = pacman.y;

        switch (direction) {
            case 'w': newY--; break;
            case 's': newY++; break;
            case 'a': newX--; break;
            case 'd': newX++; break;
        }

        if (isValidMove(newX, newY)) {
            // Collect dots/power pellets
            if (board[newY][newX] == DOT) {
                score += 10;
            } else if (board[newY][newX] == POWER) {
                score += 50;
                powerMode = true;
                powerTimer = 20;
            }

            board[pacman.y][pacman.x] = EMPTY;
            pacman.x = newX;
            pacman.y = newY;
            board[pacman.y][pacman.x] = PACMAN;
        }
    }

    void moveGhosts() {
        for (auto& ghost : ghosts) {
            board[ghost.pos.y][ghost.pos.x] = ghost.prevChar;

            // Simple ghost AI - random movement
            vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
            vector<pair<int, int>> validMoves;

            for (const auto& dir : directions) {
                int newX = ghost.pos.x + dir.first;
                int newY = ghost.pos.y + dir.second;
                if (isValidMove(newX, newY)) {
                    validMoves.push_back(dir);
                }
            }

            if (!validMoves.empty()) {
                int randDir = rand() % validMoves.size();
                ghost.pos.x += validMoves[randDir].first;
                ghost.pos.y += validMoves[randDir].second;
            }

            ghost.prevChar = board[ghost.pos.y][ghost.pos.x];
            board[ghost.pos.y][ghost.pos.x] = GHOST;

            // Check collision with Pac-Man
            if (ghost.pos.x == pacman.x && ghost.pos.y == pacman.y) {
                if (powerMode) {
                    score += 200;
                    ghost.pos = {7, 6};
                } else {
                    lives--;
                    if (lives == 0) {
                        gameOver = true;
                    } else {
                        // Reset Pac-Man position
                        board[pacman.y][pacman.x] = EMPTY;
                        pacman = {7, 6};
                        board[pacman.y][pacman.x] = PACMAN;
                    }
                }
            }
        }
    }

    void checkWin() {
        int remainingDots = 0;
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (board[y][x] == DOT || board[y][x] == POWER) {
                    remainingDots++;
                }
            }
        }
        if (remainingDots == 0) {
            won = true;
            gameOver = true;
        }
    }

    void update() {
        if (powerMode) {
            powerTimer--;
            if (powerTimer <= 0) {
                powerMode = false;
            }
        }
    }

    void run() {
        while (!gameOver) {
            draw();

            if (_kbhit()) {
                char input = tolower(_getch());
                if (input == 'q') break;
                movePacman(input);
            }

            moveGhosts();
            checkWin();
            update();

            this_thread::sleep_for(chrono::milliseconds(200));
        }

        draw();
        cout << "\nGame Over! " << (won ? "You Won!" : "Try Again!") << endl;
    }
};

int main() {
    srand(time(0));
    Game game;
    cout << "Welcome to Pac-Man!\n";
    cout << "Controls: W/A/S/D to move, Q to quit\n";
    cout << "Press any key to start...\n";
    _getch();
    game.run();
    return 0;
}