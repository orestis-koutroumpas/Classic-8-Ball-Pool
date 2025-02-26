#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

using namespace glm;

Camera::Camera(GLFWwindow* window) : window(window) {
    position = vec3(-0.75, 3, 4);
    horizontalAngle = 3.14f;
    verticalAngle = - 3.14f / 6.0f;
    FoV = 45.0f;
    speed = 1.5f;
    mouseSpeed = 0.001f;
    lock = false;
}

void Camera::update() {
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Skip mouse movement if spin mode is active
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetCursorPos(window, width / 2, height / 2);

    horizontalAngle += mouseSpeed * float(width / 2 - xPos);
    if (!lock) {
        verticalAngle += mouseSpeed * float(height / 2 - yPos);
    }
    
    vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    vec3 right(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    vec3 up = cross(right, direction);

    if (!lock) {
        // Move forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += direction * deltaTime * speed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= direction * deltaTime * speed;
        }
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position += right * deltaTime * speed;
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position -= right * deltaTime * speed;
        }
    }
 
    // Enforce room boundarie
    position.x = clamp(position.x, -4.8f, 4.8f);
    position.y = clamp(position.y, -0.6f, 4.8f);
    position.z = clamp(position.z, -4.8f, 4.8f);

    // construct projection and view matrices
    projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

void Camera::setAbove() {
    position = vec3(-0.70f, 4.0f, 0.0f);
    horizontalAngle = 3.14f - pi<float>() / 2.0f;
    verticalAngle = -3.14f / 2.0f;
    lock = true;
    update();
}