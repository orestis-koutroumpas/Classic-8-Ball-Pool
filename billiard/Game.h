#ifndef GAME_H
#define GAME_H

#include <vector>
#include "Ball.h"
#include <unordered_set>

class Game {
public:
    Game();
    void update(std::vector<Ball*>& balls, std::vector<glm::vec3> ballPositions);

    // Gameplay functions
    bool checkBreakRules(std::vector<Ball*>& balls);
    void resetGame(std::vector<Ball*>& balls, std::vector<glm::vec3> ballPositions);
    bool areBallsStopped(std::vector<Ball*>& balls);

    // Game state variables
    bool isPlayer1Turn;
    bool shotInProgress;
    bool aimMode;
    bool ballPocketed;
    bool spinMode;
    bool gameStart;
    bool areBallsDetermined;
    bool ballHitted;

private:
    std::vector<int> player1Balls;
    std::vector<int> player2Balls;
    std::unordered_set<int> player1PocketedBalls;
    std::unordered_set<int> player2PocketedBalls;
};

#endif // GAME_H