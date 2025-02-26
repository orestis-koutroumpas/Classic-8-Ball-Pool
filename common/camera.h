#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
    GLFWwindow* window;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 position; // Initial position : on +Z
    float horizontalAngle; // Initial horizontal angle : toward -Z
    float verticalAngle; // Initial vertical angle : none
    float FoV; // Field of View
    float speed; // units / second
    float mouseSpeed;
    bool lock;

    Camera(GLFWwindow* window);
    void update();
    void setAbove();
};
#endif