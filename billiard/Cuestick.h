#ifndef CUESTICK_H
#define CUESTICK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <common/model.h>

class Cuestick {
public:
    Cuestick(glm::vec3 cueBallPos);
    ~Cuestick();

    float dotX = 0.0f;
    float dotY = 0.0f; 
    bool chargingMode = false;
    double pressStartTime = 0.0f;
    double pressEndTime = 0.0f;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    void update(glm::vec3 cueBallPos, float horizontalAngle);
    void draw();
    glm::mat4 calculateModelMatrix(glm::vec3 cueBallPos, float horizontalAngle);

private:
    Drawable* cuestick;

    glm::vec3 cuestick_position_worldspace;
    glm::vec3 targetPosition;
    glm::vec3 direction;
};

#endif
