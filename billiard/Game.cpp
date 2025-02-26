#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() {
    isPlayer1Turn = true;
    foulCommitted = false;
    shotInProgress = false;
    aimMode = false;
    spinMode = false;
    openTable = false;
    gameStart = true;
    gameOver = false;
    areBallsDetermined = false;
}

void Game::update(std::vector<Ball*>& balls, std::vector<glm::vec3> ballPositions) {
    if (gameOver) {
        return;
    }

    if (gameStart) {
        if (!checkBreakRules(balls)) {
            gameStart = false;
            openTable = true;
        }
        else {
            resetGame(balls, ballPositions);
            return;
        }
    }

    if (openTable) {
        determineBallGroups(balls);
    }

    for (Ball* ball : balls) {

        if (ball->isPocketed && ball->isCueBall) {
            ball->isPocketed = false;
            std::cout << "Cue ball scratched! Opponent gets ball-in-hand.\n";
            ballInHand(balls);
            handleTurnSwitch();
        }

        if (isPlayer1Turn) {
            // If a ball (not cueBall either 8 ball) is pocketed by player 1
            if (ball->isPocketed && !ball->isCueBall && ball->num != 8) {
                // check if it belongs to player's 1 balls
                if (std::find(player1Balls.begin(), player1Balls.end(), ball->num) != player1Balls.end()) {
                    // If it belongs check if already is pocketed before
                    if (!player1PocketedBalls.count(ball->num)) {
                        // If it is not, insert it to pocketed balls by player 1
                        player1PocketedBalls.insert(ball->num);
                    }
                }
                // If player 1 pockets a ball belonging to players 2 balls -> change turns
                else {
                    std::cout << "Wrong ball pocketed! Change turns!.\n";
                    handleTurnSwitch();
                    continue;
                }
            }
        }

        else {
            // If a ball (not cueBall either 8 ball) is pocketed by player 1
            if (ball->isPocketed && !ball->isCueBall && ball->num != 8) {
                // check if it belongs to player's 2 balls
                if (std::find(player2Balls.begin(), player2Balls.end(), ball->num) != player2Balls.end()) {
                    // If it belongs check if already is pocketed before
                    if (!player2PocketedBalls.count(ball->num)) {
                        // If it is not, insert it to pocketed balls by player 1
                        player2PocketedBalls.insert(ball->num);
                    }
                }
                // If player 1 pockets a ball belonging to players 2 balls -> change turns
                else {
                    std::cout << "Wrong ball pocketed! Change turns!.\n";
                    handleTurnSwitch();
                    continue;
                }
            }
        }

    }

    // Check if the cue ball was scratched
    if (isCueBallScratched(balls)) {
        std::cout << "Cue ball scratched! Opponent gets ball-in-hand.\n";
        ballInHand(balls);
        handleTurnSwitch();
        return;
    }
}

// The break is legal as long as four balls hit a cushion (including the cue ball) or a ball is pocketed.
bool Game::checkBreakRules(std::vector<Ball*>& balls) {
    int count = 0;
    for (Ball* ball : balls) {
        if (ball->hitCushion) {
            count++;
        }
        if (ball->isPocketed && !ball->isCueBall) {
            std::cout << "Ball pocketed. Break is legal!\n";
            return false;
        }
        if (ball->isPocketed && ball->isCueBall) {
            std::cout << "Cue ball scratched!\n";
            ball->isPocketed = false;
            return true;
        }
    }
    if (count > 3) {
        std::cout << "At least 4 balls hit cushion. Break is legal!.\n";
        return false;
    }
    std::cout << "Less than 4 balls hit cushion. Break is illegal!.\n";
    return true;
}

void Game::determineBallGroups(std::vector<Ball*>& balls) {
    for (Ball* ball : balls) {
        if (ball->isPocketed && !ball->isCueBall && ball->num != 8) {
            // Assign groups based on the first pocketed ball
            if (isPlayer1Turn) {
                if (ball->num % 2 == 1) {  // Odd numbers / Solids
                    player1Balls = { 1, 3, 5, 7, 9, 11, 13 };
                    player2Balls = { 2, 4, 6, 10, 12, 14 };
                }
                else {  // Even numbers / Stripes
                    player1Balls = { 2, 4, 6, 10, 12, 14 };
                    player2Balls = { 1, 3, 5, 7, 9, 11, 13 };
                }
                std::cout << "Player 1 gets " << (ball->num % 2 == 1 ? "solids" : "stripes") << "\n";
                std::cout << "Player 2 gets " << (ball->num % 2 == 1 ? "stripes" : "solids") << "\n";
            }
            else {
                if (ball->num % 2 == 1) {  // Odd numbers / Solids
                    player2Balls = { 1, 3, 5, 7, 9, 11, 13 };
                    player1Balls = { 2, 4, 6, 10, 12, 14 };
                }
                else {  // Even numbers / Stripes
                    player2Balls = { 2, 4, 6, 10, 12, 14 };
                    player1Balls = { 1, 3, 5, 7, 9, 11, 13 };
                }
                std::cout << "Player 1 gets " << (ball->num % 2 == 1 ? "stripes" : "solids") << "\n";
                std::cout << "Player 2 gets " << (ball->num % 2 == 1 ? "solids" : "stripes") << "\n";
            }
            openTable = false;
            break;
        }
    }
}

bool Game::isCueBallScratched(std::vector<Ball*>& balls) {
    for (Ball* ball : balls) {
        if (ball->isPocketed && ball->isCueBall) {
            ball->isPocketed = false;
            return true;
        }
    }
    return false;
}

void Game::ballInHand(std::vector<Ball*>& balls) {
    for (Ball* ball : balls) {
        if (ball->isCueBall) {
            ball->resetPosition(glm::vec3(-55.0f * ball->r, 0.0f, 0.0f));
        }
    }
}

void Game::handleTurnSwitch() {
    isPlayer1Turn = !isPlayer1Turn;
}

bool Game::areBallsStopped(std::vector<Ball*>& balls) {
    for (Ball* ball : balls) {
        if (ball->P != glm::vec3(0, 0, 0)) {
            return false;
        }
    }
    return true;
}

void Game::resetGame(std::vector<Ball*>& balls, std::vector<glm::vec3> ballPositions) {
    std::cout << "Resetting game...\n";
    isPlayer1Turn = !isPlayer1Turn;
    foulCommitted = false;
    shotInProgress = false;
    aimMode = false;
    spinMode = false;
    gameOver = false;
    gameStart = true;
    player1Balls.clear();
    player2Balls.clear();

    int i = 0;
    for (Ball* ball : balls) {
        ball->resetPosition(ballPositions[i]);
        ball->isPocketed = false;
        ball->wasHitted = false;
        ball->hitCushion = false;
        i++;
    }
}