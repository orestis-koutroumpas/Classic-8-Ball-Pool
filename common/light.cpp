#include <glfw3.h>
#include <iostream>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"

using namespace glm;

Light::Light(GLFWwindow* window,
    glm::vec4 init_La,
    glm::vec4 init_Ld,
    glm::vec4 init_Ls,
    glm::vec3 init_position,
    float init_power) : window(window) {
    La = init_La;
    Ld = init_Ld;
    Ls = init_Ls;
    power = init_power;
    lightPosition_worldspace = init_position;

    direction = normalize(targetPosition - lightPosition_worldspace);

    targetPosition = vec3(0.0, 0.0, 0.0);

    projectionMatrix = perspective(
        radians(90.0f), // Field of View (angle of light coverage)
        1.0f,           // Aspect Ratio (square shadow map)
        0.1f,          // Near Clipping Plane
        5.0f            // Far Clipping Plane
    );
    //projectionMatrix = ortho(-4.0f, 2.0f, -0.8f, 1.5f, 0.1f, 5.0f);
}

void Light::update() {

    // We have the direction of the light and the point where the light is looking at
    // We will use this information to calculate the "up" vector, 
    // just like we did with the camera

    direction = normalize(targetPosition - lightPosition_worldspace);

    // converting direction to cylidrical coordinates
    float x = direction.x;
    float y = direction.y;
    float z = direction.z;

    // We don't need to calculate the vertical angle

    float horizontalAngle;
    if (z > 0.0) horizontalAngle = atan(x / z);
    else if (z < 0.0) horizontalAngle = atan(x / z) + 3.1415f;
    else horizontalAngle = 3.1415f / 2.0f;

    // Right vector
    vec3 right(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    vec3 up = cross(right, direction);

    viewMatrix = lookAt(
        lightPosition_worldspace,
        targetPosition,
        up
    );
}

mat4 Light::lightVP() {
    return projectionMatrix * viewMatrix;
}