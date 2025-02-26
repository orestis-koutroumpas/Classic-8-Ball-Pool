#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>


class Ball;
class Table;

void handleBallBallCollision(Ball& ball1, Ball& ball2);
void handleBallTableCollision(Ball& ball);
void handleBallHoleCollision(Ball& ball);

#endif
