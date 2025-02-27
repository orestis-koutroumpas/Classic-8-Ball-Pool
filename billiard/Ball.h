#ifndef BALL_H
#define BALL_H

#include "RigidBody.h"

class Drawable;

class Ball : public RigidBody {
public:
    Drawable* ball;
    float r;
    int num;
    bool isPocketed = false;
    bool wasHitted = false;
    bool hitCushion = false;
    bool isCueBall;
    glm::mat4 modelMatrix;

    glm::vec3 om; 
    glm::vec3 L;  
    glm::mat3 I, I_inv;

    Ball(glm::vec3 pos, glm::vec3 vel, glm::vec3 omega, float radius, float mass, int number);
    ~Ball();

    void draw(unsigned int drawable = 0);
    void launch(double factor, float angle, glm::vec3 spin);
    void update(float t = 0, float dt = 0);
    void resetPosition(glm::vec3 pos);
    void resetState();

};

#endif
