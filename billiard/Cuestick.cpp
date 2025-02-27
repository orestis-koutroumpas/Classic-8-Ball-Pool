#include "Cuestick.h"
#include <GLFW/glfw3.h>  // Required for glfwGetTime
#include <algorithm>      // Required for std::min
#include <iostream>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Cuestick::Cuestick(glm::vec3 cueBallPos) {
    cuestick = new Drawable("models/cue_stick.obj");

    vec3 forward = normalize(vec3(0.0f, 0.0f, 1.0f));  

    float initialDistance = 0.75f;  
    cuestick_position_worldspace = cueBallPos - forward * initialDistance;

    // Target is always the cue ball
    targetPosition = cueBallPos;

    direction = normalize(targetPosition - cuestick_position_worldspace);

    projectionMatrix = perspective(
        radians(20.0f), // Field of View (angle of light coverage)
        1.0f,           // Aspect Ratio (square shadow map)
        0.01f,          // Near Clipping Plane
        1.0f            // Far Clipping Plane
    );
}

void Cuestick::update(glm::vec3 cueBallPos, float horizontalAngle) {
    targetPosition = cueBallPos;

    // Compute shot direction based on aim angle
    glm::vec3 shotDirection = glm::normalize(glm::vec3(sin(horizontalAngle), 0.0f, cos(horizontalAngle)));

    // Compute right and up vectors
    glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));
    glm::vec3 up = glm::cross(right, shotDirection);

    glm::vec3 spinEffect = right * dotX + up * dotY;

    float spinMagnitude = glm::length(spinEffect);
    if (spinMagnitude > 0.025f) {
        spinEffect = glm::normalize(spinEffect) * 0.025f;
    }

    // Position cuestick behind cue ball with spin effect applied
    cuestick_position_worldspace = cueBallPos - shotDirection * 0.75f + spinEffect;

    // Recalculate direction
    direction = glm::normalize(targetPosition - cuestick_position_worldspace);

    // Update view matrix to keep the cue stick aimed at the cue ball
    viewMatrix = glm::lookAt(
        cuestick_position_worldspace,
        targetPosition,
        up
    );
}


glm::mat4 Cuestick::calculateModelMatrix(glm::vec3 cueBallPos, float horizontalAngle) {
    // Calculate angle
    float angle = horizontalAngle - (3.14f / 2.0f);

    // Compute shot direction
    vec3 shotDirection = normalize(vec3(sin(horizontalAngle), 0.0f, cos(horizontalAngle)));

    // Compute right and up vectors
    vec3 right = vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));
    vec3 up = cross(right, shotDirection);

    // Spin effect position adjustment
    glm::vec3 spinEffect = right * dotX + up * dotY;

    // Ensure spinEffect magnitude does not exceed ball radius
    float spinMagnitude = glm::length(spinEffect);
    if (spinMagnitude > 0.025f) {
        spinEffect = glm::normalize(spinEffect) * 0.025f;
    }
    // Cue stick length offset
    vec3 cueLengthOffset = -shotDirection * 0.05f;

    // Charge distance calculation
    float chargeDistance = 0.0f;
    if (chargingMode) {
        chargeDistance = std::min<float>((static_cast<float>(glfwGetTime() - pressStartTime) / 10.0f), 0.125f);
    }

    // Cue power offset when charging
    vec3 cuePowerOffset = -shotDirection * chargeDistance;

    // Final cue stick position with spin and charge effects
    vec3 cuestickPos = cueBallPos + cueLengthOffset + cuePowerOffset + spinEffect;

    // Apply transformations
    return translate(mat4(1.0f), cuestickPos) * rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f));
}


void Cuestick::draw() {
    cuestick->bind();
    cuestick->draw();
}

Cuestick::~Cuestick() {
    delete cuestick;
}