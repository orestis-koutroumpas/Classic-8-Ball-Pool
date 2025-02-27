#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() {
    isPlayer1Turn = true;
    ballPocketed = false;
    shotInProgress = false;
    aimMode = false;
    spinMode = false;
    gameStart = true;
    areBallsDetermined = false;
    ballHitted = false;
}

void Game::update(std::vector<Ball*>& balls, std::vector<glm::vec3> ballPositions) {

    if (gameStart) {
        if (!checkBreakRules(balls)) {
            gameStart = false;
        }
        else {
            resetGame(balls, ballPositions);
            return;
        }
    }

    for (Ball* ball : balls) {
        // Check if no balls are hitted (except cue ball)
        if (ball->wasHitted) {
            ballHitted = true;
        }
         
        // If cue ball scratched
        if (ball->isPocketed && ball->isCueBall) {
            ball->isPocketed = false;
            ballPocketed = true;
            std::cout << "Cue ball scratched! Opponent gets ball-in-hand.\n";
            ball->resetPosition(glm::vec3(-55.0f * ball->r, 0.0f, 0.0f));
            isPlayer1Turn = !isPlayer1Turn;
            break;

        }
        // Check game 
        if (ball->isPocketed && ball->num == 8) {
            ballPocketed = true;
            if (isPlayer1Turn) {
                // Check winning condition
                if (player1PocketedBalls.size() == 7) {
                    std::cout << "Player 1 wins!\n";
                }
                else {
                    std::cout << "Player 1 pocketed Ball 8 too early. Player 2 wins!\n";
                }
            }
            else {
                if (player2PocketedBalls.size() == 7) {
                    std::cout << "Player 2 wins!\n";
                }
                else {
                    std::cout << "Player 2 pocketed Ball 8 too early. Player 1 wins!\n";
                }
            }
            resetGame(balls, ballPositions);
        }
        // If a ball (not cueBall either 8 ball) is pocketed by player
        if (ball->isPocketed && !ball->isCueBall && ball->num != 8) {
            ballPocketed = true;

            std::cout << "Ball " << ball->num << " pocketed!\n";
            ball->isPocketed = false;

            if (isPlayer1Turn) {
                // If first ball to pocket
                if (player1PocketedBalls.size() == 0 && !areBallsDetermined) {
                    if (ball->num < 8) {  
                        // / Solids
                        player1Balls = { 1, 2, 3, 4, 5, 6, 7 };
                        player2Balls = { 9, 10, 11, 12, 13, 14 };
                    }
                    else {  
                        // Even numbers / Stripes
                        player1Balls = { 9, 10, 11, 12, 13, 14 };
                        player2Balls = { 1, 2, 3, 4, 5, 6, 7 };
                    }
                    std::cout << "Player 1 gets " << (ball->num < 8 ? "solids" : "stripes") << "\n";
                    std::cout << "Player 2 gets " << (ball->num < 8 ? "stripes" : "solids") << "\n";
                    areBallsDetermined = true;
                    player1PocketedBalls.insert(ball->num);
                }
                else {
                    // check if it belongs to player's 1 balls
                    if (std::find(player1Balls.begin(), player1Balls.end(), ball->num) != player1Balls.end()) {
                        player1PocketedBalls.insert(ball->num);
                    }
                    else {
                        std::cout << "Wrong ball pocketed! Change turns.\n";
                        player2PocketedBalls.insert(ball->num);
                        isPlayer1Turn = !isPlayer1Turn;
                    }
                }
            }
            if (!isPlayer1Turn) {
                // If first ball to pocket
                if (player2PocketedBalls.size() == 0 && !areBallsDetermined) {
                    if (ball->num < 8) {
                        // Odd numbers / Solids
                        player2Balls = { 1, 2, 3, 4, 5, 6, 7 };
                        player1Balls = { 9, 10, 11, 12, 13, 14 };
                    }
                    else {
                        // Even numbers / Stripes
                        player2Balls = { 9, 10, 11, 12, 13, 14 };
                        player1Balls = { 1, 2, 3, 4, 5, 6, 7 };
                    }
                    std::cout << "Player 1 gets " << (ball->num < 8 ? "stripes" : "solids") << "\n";
                    std::cout << "Player 2 gets " << (ball->num < 8 ? "solids" : "stripes") << "\n";
                    player2PocketedBalls.insert(ball->num);
                    areBallsDetermined = true;
                }
                else {
                    // check if it belongs to player's 1 balls
                    if (std::find(player2Balls.begin(), player2Balls.end(), ball->num) != player2Balls.end()) {
                        player2PocketedBalls.insert(ball->num);
                    }
                    else {
                        std::cout << "Wrong ball pocketed! Change turns.\n";
                        player1PocketedBalls.insert(ball->num);
                        isPlayer1Turn = !isPlayer1Turn;
                    }
                }
            }
         
            // Print Player 1 Pocketed Balls
            std::cout << "Player 1 Pocketed Balls: ";
            for (int ball : player1PocketedBalls) {
                std::cout << ball << " ";
            }
            std::cout << std::endl;

            // Print Player 2 Pocketed Balls
            std::cout << "Player 2 Pocketed Balls: ";
            for (int ball : player2PocketedBalls) {
                std::cout << ball << " ";
            }
            std::cout << std::endl;
        }

        // Reset state for next round.
        ball->resetState();
    }

    if (!ballHitted) {
        std::cout << "Zero balls hitted! Opponent gets ball-in-hand.\n";
        isPlayer1Turn = !isPlayer1Turn;
        balls[15]->resetPosition(glm::vec3(-55.0f * balls[15]->r, 0.0f, 0.0f));
        shotInProgress = false;
    }

    if (!ballPocketed && ballHitted) {
        std::cout << "Zero balls pocketed! Change turns.\n";
        isPlayer1Turn = !isPlayer1Turn;
        shotInProgress = false;
        ballPocketed = false;
        ballHitted = false;
    }

    if (ballPocketed && ballHitted) {
        std::cout << "Continue ...\n";
        shotInProgress = false;
        ballPocketed = false;
        ballHitted = false;
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
    ballPocketed = false;
    shotInProgress = false;
    aimMode = false;
    spinMode = false;
    gameStart = true;
    areBallsDetermined = false;
    ballHitted = false;
    player1PocketedBalls.clear();
    player1Balls.clear();
    player2PocketedBalls.clear();
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