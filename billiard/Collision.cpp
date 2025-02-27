#include "Collision.h"
#include "Ball.h"
#include <iostream>
//#include <SFML/Audio.hpp>

using namespace glm;

// Load sound effects
//sf::SoundBuffer ballBallBuffer, ballTableBuffer, ballHoleBuffer;
//sf::Sound ballBallSound, ballTableSound, ballHoleSound;

//// Initialize sounds
//void initSounds() {
//    if (!ballBallBuffer.loadFromFile("ball_ball.wav")) {
//        std::cerr << "Failed to load ball_ball.wav" << std::endl;
//    }
//    if (!ballTableBuffer.loadFromFile("ball_table.wav")) {
//        std::cerr << "Failed to load ball_table.wav" << std::endl;
//    }
//    if (!ballHoleBuffer.loadFromFile("ball_hole.wav")) {
//        std::cerr << "Failed to load ball_hole.wav" << std::endl;
//    }
//
//    ballBallSound.setBuffer(ballBallBuffer);
//    ballTableSound.setBuffer(ballTableBuffer);
//    ballHoleSound.setBuffer(ballHoleBuffer);
//}

// Function declarations
void handleBallBallCollision(Ball& ball1, Ball& ball2);
void handleBallTableCollision(Ball& ball);
void handleBallHoleCollision(Ball& ball);
bool checkForBallBallCollision(Ball& ball1, Ball& ball2, glm::vec3& n);
bool checkForBallTableCollision(Ball& ball, glm::vec3& n);
bool checkForBallHoleCollision(Ball& ball, glm::vec3& n);

void handleBallBallCollision(Ball& ball1, Ball& ball2) {
    vec3 n;
    if (checkForBallBallCollision(ball1, ball2, n)) {
        float minDistance = ball1.r + ball2.r;
        vec3 diff = ball2.x - ball1.x;
        float distance = length(diff);

        if (distance < minDistance) {
            float overlap = minDistance - distance;
            vec3 correction = n * (overlap * 0.5f);
            ball1.x -= correction;
            ball2.x += correction;
        }

        vec3 relativeVelocity = ball2.v - ball1.v;
        if (dot(relativeVelocity, n) > 0) return;

        float v1n = dot(ball1.v, n);
        float v2n = dot(ball2.v, n);
        float m1 = ball1.m, m2 = ball2.m;

        float v1n_new = (v1n * (m1 - m2) + 2 * m2 * v2n) / (m1 + m2);
        float v2n_new = (v2n * (m2 - m1) + 2 * m1 * v1n) / (m1 + m2);

        vec3 v1t = ball1.v - v1n * n;
        vec3 v2t = ball2.v - v2n * n;

        ball1.v = v1n_new * n + v1t;
        ball2.v = v2n_new * n + v2t;

        ball1.P = ball1.m * ball1.v;
        ball2.P = ball2.m * ball2.v;

        ball1.wasHitted = true;
        ball2.wasHitted = true;
        //std::cout << "Ball " << ball1.num << " hitted!\n";
        //std::cout << "Ball " << ball2.num << " hitted!\n";

        //ballBallSound.play(); // Play ball collision sound
    }
    else {
    }
}

void handleBallTableCollision(Ball& ball) {
    vec3 n;
    if (checkForBallTableCollision(ball, n)) {
        ball.v = ball.v - n * dot(ball.v, n) * 2.0f;
        ball.x -= n * 0.01f;
        ball.P = ball.m * ball.v;

        //ballTableSound.play(); // Play table collision sound
        ball.hitCushion = true;
        //std::cout << "Ball " << ball.num << " hit cushion\n";

    }
}

void handleBallHoleCollision(Ball& ball) {
    vec3 n;
    if (checkForBallHoleCollision(ball, n)) {
       // ballHoleSound.play(); // Play ball falling into hole sound
        ball.isPocketed = true;
        ball.resetPosition(vec3(-2 + ball.num * 0.2, -0.3, 0));
    }
}

// Ball - Ball collision detection
bool checkForBallBallCollision(Ball& ball1, Ball& ball2, glm::vec3& n) {
    vec3 diff = ball2.x - ball1.x;
    float distance = length(diff);
    float minDistance = ball1.r + ball2.r * 2.25f;

    if (distance < minDistance) {
        n = normalize(diff);
        return true;
    }
    return false;
}

// Ball - Table collision detection
bool checkForBallTableCollision(Ball& ball, glm::vec3& n) {
    float ballRadius = ball.r;

    if (ball.x.x - ballRadius < -1.975f) {
        ball.x.x = -1.975f + ballRadius;
        n = glm::vec3(-1.0f, 0.0f, 0.0f);
        return true;
    }
    if (ball.x.x + ballRadius > 0.575f) {
        ball.x.x = 0.575f - ballRadius;
        n = glm::vec3(1.0f, 0.0f, 0.0f);
        return true;
    }
    if (ball.x.z - ballRadius < -0.625f) {
        ball.x.z = -0.625f + ballRadius;
        n = glm::vec3(0.0f, 0.0f, -1.0f);
        return true;
    }
    if (ball.x.z + ballRadius > 0.625f) {
        ball.x.z = 0.625f - ballRadius;
        n = glm::vec3(0.0f, 0.0f, 1.0f);
        return true;
    }

    return false;
}

// Ball - Hole collision detection
bool checkForBallHoleCollision(Ball& ball, glm::vec3& n) {
    const float holeRadius = 0.11f;
    const float ballRadius = 0.025f;

    float x1 = -2.025f;
    float x3 = 0.625f;
    float x2 = (abs(x1) + abs(x3)) / 2 + x1;
    float z1 = 0.675f;
    float z2 = -0.675f;
    float zOffset = 0.04f;
    float y = -0.04f;

    vec3 holes[] = {
        vec3(x1, y, z1), vec3(x2, y, z1 + zOffset),
        vec3(x1, y, z2), vec3(x2, y, z2 - zOffset),
        vec3(x3, y, z1), vec3(x3, y, z2)
    };

    for (const auto& hole : holes) {
        float distance = glm::length(ball.x - hole);
        if (distance <= (holeRadius + ballRadius)) {
            return true;
        }
    }

    return false;
}