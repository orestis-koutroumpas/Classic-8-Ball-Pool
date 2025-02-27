#include "Ball.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>

using namespace glm;

Ball::Ball(vec3 pos, vec3 vel, vec3 omega, float radius, float mass, int number)
    : RigidBody() {
    ball = new Drawable("models/ball.obj");

    r = radius;
    m = mass;
    x = pos;
    v = vel;
    om = omega;
    P = m * v;

    float I_sphere = (2.0f / 5.0f) * m * r * r;
    L = I_sphere * om;

    num = number;
    if (num == 15) {
        isCueBall = true;
    }
    else {
        isCueBall = false;
    }

    if (radius == 0) throw std::logic_error("Ball: radius must not be zero");

    I = mat3(
        I_sphere, 0, 0,
        0, I_sphere, 0,
        0, 0, I_sphere);

    I_inv = inverse(I);
}

Ball::~Ball() {
    delete ball;
}

void Ball::draw(unsigned int drawable) {
    ball->bind();
    ball->draw();
} 

void Ball::launch(double factor, float angle, vec3 spin) {
    // Compute linear velocity based on angle
    v = vec3(
        sin(angle) * factor,
        0.0f,
        cos(angle) * factor
    );

    // Set initial angular velocity based on spin input
    om = spin; 

    // Update momentum and angular momentum
    P = m * v;
    L = I * om;
    wasHitted = true; // ball hit with cuestick
    std::cout << "Cue Ball launched with velocity = (" << v.x << "," << v.y << "," << v.z << ") and spin = (" << om.x << "," << om.y << "," << om.z << ")." << std::endl;
}

void Ball::update(float t, float dt) {
    // Integration: update position and velocity
    advanceState(t, dt);

    // Compute velocity at the contact point
    vec3 v_contact = v + cross(om, vec3(0, -r, 0));

    // Apply linear friction 
    if (glm::length(v) > 0.1f) {
        vec3 F = -0.15f * v; 
        P += F * dt;  
        v = P / m; 
    } else {
        v = vec3(0.0f);
        P = vec3(0.0f);  
    }

    // Apply spin friction 
    if (glm::length(om) > 0.2f) {
        vec3 T = -0.3f * L;  
        L += T * dt;  
        om = I_inv * L;  
    }
    else {
        om = vec3(0.0f);
        L = vec3(0.0f);  
    }

    if (length(v_contact) > 0.01f) {
        vec3 friction_dir = -normalize(v_contact);

        vec3 rollingFrictionForce = 0.05f * m * 9.81f * friction_dir;
        vec3 rollingTorque = cross(vec3(0, -r, 0), rollingFrictionForce);

        // Apply rolling friction effects
        v += (rollingFrictionForce / m) * dt;
        L += rollingTorque * dt;
        om = I_inv * L;
    }

    mat4 scale = glm::scale(mat4(1.0f), vec3(r, r, r));
    mat4 translation = translate(mat4(1.0f), x);

#ifdef USE_QUATERNIONS
    q += 0.5f * quat(0, om) * q * dt;
    q = normalize(q);
    mat4 rotation = mat4_cast(q);
#else
    mat4 rotation = mat4(R);
#endif

    modelMatrix = translation * rotation * scale;
}


void Ball::resetPosition(glm::vec3 pos) {
    x = pos;
    v = vec3(0.0f, 0.0f, 0.0f);
    P = vec3(0.0f, 0.0f, 0.0f);
    om = vec3(0.0f, 0.0f, 0.0f);
    L = vec3(0.0f, 0.0f, 0.0f);

}

void Ball::resetState() {
    wasHitted = false;
    hitCushion = false;
}