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
    void determineBallGroups(std::vector<Ball*>& balls);
    bool isCueBallScratched(std::vector<Ball*>& balls);
    void handleTurnSwitch();
    void resetGame(std::vector<Ball*>& balls, std::vector<glm::vec3> ballPositions);
    void ballInHand(std::vector<Ball*>& balls);
    bool areBallsStopped(std::vector<Ball*>& balls);

    // Game state variables
    bool isPlayer1Turn;
    bool foulCommitted;
    bool shotInProgress;
    bool aimMode;
    bool spinMode;
    bool gameOver;
    bool openTable;
    bool gameStart;
    bool areBallsDetermined;

private:
    std::vector<int> player1Balls;
    std::vector<int> player2Balls;
    std::unordered_set<int> player1PocketedBalls;
    std::unordered_set<int> player2PocketedBalls;
};

#endif // GAME_H
