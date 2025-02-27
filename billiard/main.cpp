// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <map>

// Include GLEW
#include <GL/glew.h>
#include <cmath>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h> 
#include <SOIL.h>

// Include header files for classes
#include "Ball.h"
#include "Collision.h"
#include "Cuestick.h"
#include "Game.h"

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Classic 8-Ball Pool"
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

using namespace std;
using namespace glm;

// Constants
const int numBalls = 16;
const int numLights = 10;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram, depthProgram;

// Lights
Light* light[numLights];
bool lightOn[numLights] = { true, true, true, true, true, true, true, true, true, true };
Drawable* lightbulb;
GLuint depthFBO[numLights], depthTexture[numLights];

// Table
Drawable* table;
Cuestick* cuestick;
Drawable* felt;
vector<Drawable*> holes;

// Room
Drawable* bottom;
Drawable* rightWall;
Drawable* leftWall;
Drawable* frontWall;
Drawable* backWall;
Drawable* ceiling;

// Balls
vector<Ball*> balls;
vector<GLuint> ballTextures;
vector<vec3> ballPositions;

// Game
Game game;

// locations for shaderProgram
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation;
GLuint textureSampler;
GLuint depthMapSampler[numLights];
GLuint tableTexture, feltTexture, holeTexture, cueStickTexturePlayer1, cueStickTexturePlayer2, floorTexture, wallTexture, ceilingTexture;
GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation[numLights], LdLocation[numLights], LsLocation[numLights];
GLuint lightPositionLocation[numLights], lightPowerLocation[numLights];
GLuint lightVPLocation[numLights];
GLuint useTextureLocation;
GLuint calcShadowLocation;
GLuint invertNormalsLocation;

// locations for depthProgram
GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;

// ------- Create Context functions ------- //

// Set up ball positions for the start of the game
void setupBallPositions(float ballRadius) {
    ballPositions.clear();  // Ensure the positions vector is empty

    float xOffset = 3.25f * ballRadius;  // Distance between balls along the x-axis
    float zOffset = sqrt(3.25f) * ballRadius;  // Vertical spacing between rows (height of an equilateral triangle)

    float yOffset = 0.0f;  // Y-position for all balls on the table surface

    // Properly rack balls in a triangle formation
    // Apex ball (row 1)
    ballPositions.push_back(vec3(0.0f, yOffset, 0.0f)); // Ball 1 (apex)

    // Row 2
    ballPositions.push_back(vec3(4 * xOffset, yOffset, -4 * zOffset));  // Ball 2
    ballPositions.push_back(vec3(3 * xOffset, yOffset, -zOffset));   // Ball 3

    // Row 3
    ballPositions.push_back(vec3(xOffset, yOffset, zOffset)); // Ball 4
    ballPositions.push_back(vec3(2 * xOffset, yOffset, -2 * zOffset)); // Ball 5
    ballPositions.push_back(vec3(3 * xOffset, yOffset, 3 * zOffset)); // Ball 6

    ballPositions.push_back(vec3(4 * xOffset, yOffset, 2 * zOffset)); // Ball 7
    ballPositions.push_back(vec3(2 * xOffset, yOffset, 0.0)); // Ball 8
    ballPositions.push_back(vec3(4 * xOffset, yOffset, 4 * zOffset)); // Ball 9
    ballPositions.push_back(vec3(xOffset, yOffset, -zOffset)); // Ball 10

    ballPositions.push_back(vec3(3 * xOffset, yOffset, -3 * zOffset)); // Ball 11
    ballPositions.push_back(vec3(4 * xOffset, yOffset, -2 * zOffset)); // Ball 12
    ballPositions.push_back(vec3(2 * xOffset, yOffset, 2 * zOffset)); // Ball 13
    ballPositions.push_back(vec3(3 * xOffset, yOffset, zOffset)); // Ball 14
    ballPositions.push_back(vec3(4 * xOffset, yOffset, 0.0)); // Ball 15

    // Cue ball position
    ballPositions.push_back(vec3(-55.0f * ballRadius, yOffset, 0.0f)); // Cue ball 
}

// Create the 6 holes
void loadHoles() {
    vector<vec3> holePositions = {
        vec3(-2.025f, -0.04f,  0.675f), // Left - Bottom
        vec3(-0.7f,   -0.04f,  0.715f), // Middle - Bottom
        vec3(-2.025f, -0.04f, -0.675f), // Left - Top
        vec3(-0.7f,   -0.04f, -0.715f), // Middle - Top
        vec3(0.625f, -0.04f,  0.675f), // Right - Bottom
        vec3(0.625f, -0.04f, -0.675f)  // Right - Top
    };

    int numSides = 50;
    float radius = 0.11f;
    float angleStep = 2.0f * 3.14f / numSides;

    holes.reserve(holePositions.size());

    for (const auto& center : holePositions) {
        vector<vec3> polygonVertices;
        vector<vec3> polygonNormals(numSides * 3, vec3(0.0f, 1.0f, 0.0f));
        vector<vec2> polygonUVs;

        polygonVertices.reserve(numSides * 3);
        polygonUVs.reserve(numSides * 3);

        for (int i = 0; i < numSides; ++i) {
            float angle = i * angleStep;
            float nextAngle = (i + 1) * angleStep;

            float x1 = radius * cos(angle);
            float z1 = radius * sin(angle);
            float x2 = radius * cos(nextAngle);
            float z2 = radius * sin(nextAngle);

            polygonVertices.push_back(vec3(center.x + x1, center.y, center.z + z1));
            polygonVertices.push_back(center);
            polygonVertices.push_back(vec3(center.x + x2, center.y, center.z + z2));

            float u1 = (x1 + radius) / (2 * radius);
            float v1 = (z1 + radius) / (2 * radius);
            float u2 = (x2 + radius) / (2 * radius);
            float v2 = (z2 + radius) / (2 * radius);

            polygonUVs.push_back(vec2(u1, v1));
            polygonUVs.push_back(vec2(0.5f, 0.5f));
            polygonUVs.push_back(vec2(u2, v2));
        }

        holes.push_back(new Drawable(polygonVertices, polygonUVs, polygonNormals));
    }
}

// Create plane for felt + room
void createPlane(float x1, float x2, float y1, float y2, float z1, float z2, vec3 normals, vector<vec2> planeUVs, Drawable*& plane) {
    vector<vec3> planeVertices;

    // Flip the order of vertices to ensure inward-facing walls
    if (x1 == x2) {
        // YZ Plane (Left/Right Walls)
        planeVertices = {
            vec3(x1, y1, z1),
            vec3(x1, y2, z1),
            vec3(x1, y2, z2),
            vec3(x1, y2, z2),
            vec3(x1, y1, z2),
            vec3(x1, y1, z1)
        };
    }
    else if (z1 == z2) {
        // XZ Plane (Front/Back Walls)
        planeVertices = {
            vec3(x1, y1, z1),
            vec3(x1, y2, z1),
            vec3(x2, y2, z1),
            vec3(x2, y2, z1),
            vec3(x2, y1, z1),
            vec3(x1, y1, z1)
        };
    }
    else {
        // XY Plane (Floor & Ceiling)
        planeVertices = {
            vec3(x1, y1, z1),
            vec3(x1, y1, z2),
            vec3(x2, y1, z2),
            vec3(x2, y1, z2),
            vec3(x2, y1, z1),
            vec3(x1, y1, z1)
        };
    }

    // Set correct normals (flip them to face inward)
    vector<vec3> planeNormals(6, normals);

    // Create the Drawable object
    plane = new Drawable(planeVertices, planeUVs, planeNormals);
}

// Set up lights positions around the table
void createLights() {

    float x1 = -2.1f, x4 = 0.7f;
    float x2 = (abs(x1) + abs(x4)) * 0.3333f + x1;
    float x3 = (abs(x1) + abs(x4)) * 0.6667f + x1;
    float z1 = 0.77f, z3 = -0.77f;
    float z2 = (abs(z1) + abs(z3)) * 0.5f + z3;
    float y = 0.1f; // Height above the table

    // Define light positions based on table layout
    vec3 lightPositions[numLights] = {
        vec3(x1, y, z1), // Bottom left corner    | Light0 
        vec3(x1, y, z2), // Mid left side         | Light1   
        vec3(x1, y, z3), // Top left corner       | Light2 
        vec3(x2, y, z3), // Top side left mid     | Light3  
        vec3(x3, y, z3), // Top side right mid    | Light4
        vec3(x4, y, z3), // Top right corner      | Light5
        vec3(x4, y, z2), // Mid right side        | Light6
        vec3(x4, y, z1), // Bottom right corner   | Light7
        vec3(x3, y, z1), // Bottom side right mid | Light8
        vec3(x2, y, z1)  // Bottom side left mid  | Light9
    };

    // Create lights based on the positions
    for (int i = 0; i < numLights; ++i) {
        light[i] = new Light(window,
            vec4(1.0, 1.0, 1.0, 1.0), // Ambient light
            vec4(1.0, 1.0, 1.0, 1.0), // Diffuse light
            vec4(1.0, 1.0, 1.0, 1.0), // Specular light
            lightPositions[i],        // Position of the light
            2.5f                      // Power
        );
    }
}

// Create depth framebuffers and textures to store the depthmap
void createDepthBuffers() {
    for (int i = 0; i < numLights; ++i) {
        glGenFramebuffers(1, &depthFBO[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO[i]);

        glGenTextures(1, &depthTexture[i]);
        glBindTexture(GL_TEXTURE_2D, depthTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture[i], 0);

        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO[i]);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
    }
}

void createContext() {

    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("ShadowMapping.vertexshader", "ShadowMapping.fragmentshader");
    // Create and load the shader program for the depth buffer construction
    depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader");

    // Get pointers to the uniform variables
    projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
    viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
    modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");

    // Phong lighting
    KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
    KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
    KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
    NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
    for (int i = 0; i < numLights; ++i) {
        string index = std::to_string(i);
        LaLocation[i] = glGetUniformLocation(shaderProgram, ("light[" + index + "].La").c_str());
        LdLocation[i] = glGetUniformLocation(shaderProgram, ("light[" + index + "].Ld").c_str());
        LsLocation[i] = glGetUniformLocation(shaderProgram, ("light[" + index + "].Ls").c_str());
        lightPositionLocation[i] = glGetUniformLocation(shaderProgram, ("light[" + index + "].lightPosition_worldspace").c_str());
        lightPowerLocation[i] = glGetUniformLocation(shaderProgram, ("light[" + index + "].power").c_str());
        lightVPLocation[i] = glGetUniformLocation(shaderProgram, ("lightVP[" + index + "]").c_str());
        depthMapSampler[i] = glGetUniformLocation(shaderProgram, ("shadowMapSampler[" + index + "]").c_str());
    }

    // --- shaderProgram --- //
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");
    useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
    calcShadowLocation = glGetUniformLocation(shaderProgram, "calculateShadow");
    invertNormalsLocation = glGetUniformLocation(shaderProgram, "invertNormals");

    // --- depthProgram --- //
    shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
    shadowModelLocation = glGetUniformLocation(depthProgram, "M");

    // Create depth framebuffers for shadow mapping
    createDepthBuffers();

    // Textures
    feltTexture = loadSOIL("textures/felt.png");
    tableTexture = loadSOIL("textures/table.jpg");
    holeTexture = loadSOIL("textures/hole.jpg");
    cueStickTexturePlayer1 = loadSOIL("textures/cuestick1.jpg");
    cueStickTexturePlayer2 = loadSOIL("textures/cuestick2.jpg");
    floorTexture = loadSOIL("textures/floor.jpg");
    wallTexture = loadSOIL("textures/wall.jpg");
    ceilingTexture = loadSOIL("textures/ceiling.jpg");

    // Define cube boundaries
    float xMin = -5.0f, xMax = 5.0f;
    float zMin = -5.0f, zMax = 5.0f;
    float yMin = -0.8f, yMax = 5.0f;

    vector<vec2> floorUVs = {vec2(0.0f, 0.0f),vec2(0.0f, 1.0f),vec2(1.0f, 1.0f),vec2(1.0f, 1.0f),vec2(1.0f, 0.0f),vec2(0.0f, 0.0f),};

    vector<vec2> topUVs = {vec2(0.0f, 0.0f),vec2(0.0f,-1.0f),vec2(-1.0f,-1.0f),vec2(-1.0f, -1.0f),vec2(-1.0f, 0.0f),vec2(0.0f, 0.0f),};

    vector<vec2> wallUVs = {
        vec2(0.0f, 0.0f), // Bottom-left
        vec2(0.0f, 1.0f), // Top-left
        vec2(1.0f, 1.0f), // Top-right
        vec2(1.0f, 1.0f), // Top-right
        vec2(1.0f, 0.0f), // Bottom-right
        vec2(0.0f, 0.0f)  // Bottom-left
    };

    // Load Room
       
    // Floor (y = -0.8, facing up)
    createPlane(xMin, xMax, yMin, yMin, zMin, zMax, vec3(0, 1, 0), floorUVs, bottom);

    // Ceiling (y = 3.0, facing down)
    createPlane(xMin, xMax, yMax, yMax, zMin, zMax, vec3(0, -1, 0), topUVs, ceiling);

    // Left wall (x = -5, facing inward)
    createPlane(xMin, xMin, yMin, yMax, zMin, zMax, vec3(1, 0, 0), wallUVs, leftWall);

    // Right wall (x = 5, facing inward)
    createPlane(xMax, xMax, yMin, yMax, zMin, zMax, vec3(-1, 0, 0), wallUVs, rightWall);

    // Front wall (z = 5, facing inward)
    createPlane(xMin, xMax, yMin, yMax, zMax, zMax, vec3(0, 0, -1), wallUVs, frontWall);

    // Back wall (z = -5, facing inward)
    createPlane(xMin, xMax, yMin, yMax, zMin, zMin, vec3(0, 0, 1), wallUVs, backWall);

    // Load balls
    float ballRadius = 0.025f;
    setupBallPositions(ballRadius);
    vec3 inVelocity = vec3(0, 0, 0);
    vec3 inOmega = vec3(0, 0, 0);
    float mass = 0.1f;
    for (int i = 0; i < numBalls; ++i) {
        balls.push_back(new Ball(ballPositions[i], inVelocity, inOmega, ballRadius, mass, i + 1));
        std::string textureFile = "textures/Ball" + std::to_string(i + 1) + ".jpg";
        GLuint ballTexture = loadSOIL(textureFile.c_str());
        ballTextures.push_back(ballTexture);
        if (!ballTexture) {
            std::cerr << "Failed to load texture: " << textureFile << std::endl;
        }
        if (i == 15) {
            balls[i]->isCueBall = true;
        }
        else {
            balls[i]->isCueBall = false;
        }
    }

    // Load table
    table = new Drawable("models/pool_table.obj"); // table model
    loadHoles(); // craete and load 6 circles as holes 
    cuestick = new Cuestick(balls[15]->x); // create curstick
    createPlane(-3.35f, -0.5f, 0.0f, 0.0f, -1.3f, 0.3f, vec3(0.0f, 1.0f, 0.0f), floorUVs, felt); // create plane for felt

    // Set up lights positions
    createLights();

    // Load lightbulbs
    lightbulb = new Drawable("models/lightbulb.obj");
}

void free()
{
    glDeleteProgram(shaderProgram);
    glDeleteProgram(depthProgram);
    glfwTerminate();
}

// ------- Main Loop functions ------- //

// Depth Pass
void depth_pass(mat4 viewMatrix, mat4 projectionMatrix, GLuint depthFBO) {

    // Setting viewport to shadow map size
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    // Binding the depth framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    // Cleaning the framebuffer depth information (stored from the last render)
    glClear(GL_DEPTH_BUFFER_BIT);

    // Selecting the new shader program that will output the depth component
    glUseProgram(depthProgram);

    // sending the view and projection matrix to the shader
    mat4 view_projection = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);

    // ---- rendering the scene ---- //
    
    //mat4 tableModelMatrix = translate(mat4(1.0f), vec3(1.25f, -0.80f, 0.50f));
    //glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &tableModelMatrix[0][0]);
    //table->bind();
    //table->draw();

    // Balls
    for (int i = 0; i < numBalls; i++) {
        mat4 ballModelMatrix = balls[i]->modelMatrix;
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &ballModelMatrix[0][0]);
        balls[i]->draw();
    }

    // Cuestick
    if (game.aimMode) {
        mat4 cueStickModelMatrix = cuestick->calculateModelMatrix(balls[15]->x, camera->horizontalAngle);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &cueStickModelMatrix[0][0]);
        cuestick->draw();
    }

    // Plane
    mat4 feltModelMatrix = translate(mat4(1.0f), vec3(1.25f, -0.045f, 0.50f));
    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &feltModelMatrix[0][0]);
    felt->bind();
    felt->draw();

    // binding the default framebuffer again
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

// ---- Drawing functions for lighting pass ---- //
// Draw table, plane, cuestick
void drawTable() {
    // --- Table --- //
    mat4 tableModelMatrix = translate(mat4(1.0f), vec3(1.25f, -0.80f, 0.50f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &tableModelMatrix[0][0]);
    glUniform1i(calcShadowLocation, 0);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, tableTexture);
    glUniform1i(textureSampler, 10);
    glUniform1i(useTextureLocation, 1);
    table->bind();
    table->draw();

    // --- Felt --- //
    mat4 feltModelMatrix = translate(mat4(1.0f), vec3(1.25f, -0.045f, 0.50f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &feltModelMatrix[0][0]);
    glUniform1i(calcShadowLocation, 1);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, feltTexture);
    glUniform1i(textureSampler, 11);
    glUniform1i(useTextureLocation, 1);
    felt->bind();
    felt->draw();

    // --- Holes --- //
    for (int i = 0; i < holes.size(); ++i) {
        mat4 holeModelMatrix = mat4(1.0f);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &holeModelMatrix[0][0]);
        glUniform1i(calcShadowLocation, 1);
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_2D, holeTexture);
        glUniform1i(textureSampler, 13);
        glUniform1i(useTextureLocation, 1);

        holes[i]->bind();
        holes[i]->draw();
    }

    // --- Cuestick --- //
    if (game.aimMode) {
        mat4 cueStickModelMatrix = cuestick->calculateModelMatrix(balls[15]->x, camera->horizontalAngle);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cueStickModelMatrix[0][0]);
        glUniform1i(calcShadowLocation, 1);
        glActiveTexture(GL_TEXTURE14);
        glBindTexture(GL_TEXTURE_2D, game.isPlayer1Turn ? cueStickTexturePlayer1 : cueStickTexturePlayer2);
        glUniform1i(textureSampler, 14);
        glUniform1i(useTextureLocation, 1);
        cuestick->draw();
    }
}
// Draw balls
void drawBall(Ball* ball, GLuint texture) {
    mat4 ballModelMatrix = ball->modelMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &ballModelMatrix[0][0]);
    glUniform1i(calcShadowLocation, 1);
    glUniform1i(useTextureLocation, 1);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureSampler, 12);
    ball->draw();
}
// Draw lightbulbs in each light position
void drawLightBulbs() {
    vec4 Ka = vec4{ 0.05, 0.05, 0.0, 1.0 };
    vec4 Kd = vec4{ 0.5, 0.5, 0.4, 1.0 };
    vec4 Ks = vec4{ 0.7,0.7,0.04,1.0 };
    float Ns = 0.078125;

    glUniform4f(KaLocation, Ka.r, Ka.g, Ka.b, Ka.a);
    glUniform4f(KdLocation, Kd.r, Kd.g, Kd.b, Kd.a);
    glUniform4f(KsLocation, Ks.r, Ks.g, Ks.b, Ks.a);
    glUniform1f(NsLocation, Ns);
    glUniform1i(useTextureLocation, 0); // Disable textures for lights
    glUniform1i(calcShadowLocation, 0);
    glUniform1i(invertNormalsLocation, GL_TRUE); // Enable inverted normals

    for (int i = 0; i < numLights; ++i) {
        mat4 lightModelMatrix = translate(mat4(1.0f), light[i]->lightPosition_worldspace) * scale(mat4(1.0f), vec3(0.02f));
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &lightModelMatrix[0][0]);

        lightbulb->bind();
        lightbulb->draw();
    }
    // Restore invert normals to false
    glUniform1i(invertNormalsLocation, GL_FALSE);
}
// Draw walls, floor, ceiling
void drawRoom() {
    glDisable(GL_CULL_FACE);

    // --- Floor --- //
    mat4 bottomModelMatrix = mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &bottomModelMatrix[0][0]);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(calcShadowLocation, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glUniform1i(textureSampler, 0);
    bottom->bind();
    bottom->draw();

    // --- Ceiling --- //
    mat4 ceilingModelMatrix = mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &ceilingModelMatrix[0][0]);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(calcShadowLocation, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ceilingTexture);
    glUniform1i(textureSampler, 1);
    ceiling->bind();
    ceiling->draw();

    // --- Front --- //
    mat4 frontModelMatrix = mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &frontModelMatrix[0][0]);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(calcShadowLocation, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glUniform1i(textureSampler, 2);
    frontWall->bind();
    frontWall->draw();

    // --- Back --- //
    mat4 backModelMatrix = mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &backModelMatrix[0][0]);    
    glUniform1i(useTextureLocation, 1);
    glUniform1i(calcShadowLocation, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glUniform1i(textureSampler, 3);
    backWall->bind();
    backWall->draw();

    // --- Left --- //
    mat4 leftModelMatrix = mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &leftModelMatrix[0][0]);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(calcShadowLocation, 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glUniform1i(textureSampler, 4);
    leftWall->bind();
    leftWall->draw();

    // --- Right --- //
    mat4 rightModelMatrix = mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &rightModelMatrix[0][0]);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(calcShadowLocation, 0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glUniform1i(textureSampler, 5);
    rightWall->bind();
    rightWall->draw();

    glEnable(GL_CULL_FACE);

}

// Lighting Pass
void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix) {
    // Bind the default framebuffer and set up the viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, W_WIDTH, W_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the lighting shader program
    glUseProgram(shaderProgram);

    // Set view and projection matrices once
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Upload light properties (e.g., using a UBO or direct uniforms)
    for (int i = 0; i < numLights; ++i) {
        glUniform4f(LaLocation[i], light[i]->La.r, light[i]->La.g, light[i]->La.b, light[i]->La.a);
        glUniform4f(LdLocation[i], light[i]->Ld.r, light[i]->Ld.g, light[i]->Ld.b, light[i]->Ld.a);
        glUniform4f(LsLocation[i], light[i]->Ls.r, light[i]->Ls.g, light[i]->Ls.b, light[i]->Ls.a);
        glUniform3f(lightPositionLocation[i], light[i]->lightPosition_worldspace.x, light[i]->lightPosition_worldspace.y, light[i]->lightPosition_worldspace.z);
        glUniform1f(lightPowerLocation[i], light[i]->power);
    }

    // Upload depth textures and light view-projection matrices
    for (int i = 0; i < numLights; ++i) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, depthTexture[i]);
        glUniform1i(depthMapSampler[i], i); // Set the sampler location

        mat4 lightVP = light[i]->lightVP();
        glUniformMatrix4fv(lightVPLocation[i], 1, GL_FALSE, &lightVP[0][0]);
    }

    // Drawing scene objects
    drawTable(); // Draw the table
    for (int i = 0; i < numBalls; i++) {
        drawBall(balls[i], ballTextures[i]);// Draw the balls
    }
    drawLightBulbs(); // Draw the light bulbs
    drawRoom();
}

// Handles collisions between balls, table, holes
void handleBallsCollisions(float currentTime, float lastTime) {
    float dt = currentTime - lastTime;
    if (dt > 0.016f) dt = 0.016f;  // Clamp delta time

    for (int i = 0; i < numBalls; ++i) {
        handleBallTableCollision(*balls[i]);
        handleBallHoleCollision(*balls[i]);

        for (int j = i + 1; j < numBalls; ++j) {
            handleBallBallCollision(*balls[i], *balls[j]);
        }

        // Update the ball's state using absolute time
        balls[i]->update(currentTime, dt);
    }
}

void mainLoop() {
    float lastTime = static_cast<float>(glfwGetTime());  
    do {
        float currentTime = static_cast<float>(glfwGetTime());

        handleBallsCollisions(currentTime, lastTime);

        // When balls stop and shot is in progress check game conditions
        if (game.areBallsStopped(balls) && game.shotInProgress) {
            game.update(balls, ballPositions);
        }

        // Depth pass for each light
        for (int i = 0; i < numLights; ++i) {
            light[i]->update();
            depth_pass(light[i]->viewMatrix, light[i]->projectionMatrix, depthFBO[i]);
        }

        camera->update();
        cuestick->update(balls[15]->x, camera->horizontalAngle);

        if (game.spinMode) {
            // For 2D projection
            // Render the scene from cuestick's perspective
            lighting_pass(cuestick->viewMatrix, cuestick->projectionMatrix);
        }
        else {
            // Render the scene from camera's perspective
            lighting_pass(camera->viewMatrix, camera->projectionMatrix);
        }

        lastTime = currentTime;  // Store current time for next frame

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Enable shooting mode when right-click is pressed and all balls are stationary
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && game.areBallsStopped(balls)) {
        std::cout << "Now it's " << (game.isPlayer1Turn ? "Player 1" : "Player 2") << "'s turn.\n";
        std::cout << "Entering aim mode." << std::endl;

        // Set camera to overhead view
        camera->setAbove();
        // Aim mode
        game.aimMode = true;
        // Previous shot has finished
        game.shotInProgress = false;
        // Reset time for charge
        cuestick->pressStartTime = 0.0f;
    }

    // Handle left-click for charging and shooting (only in aim mode)
    if (game.aimMode) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                // Start charging
                std::cout << "Aim Mode. Charging power..." << std::endl;
                cuestick->chargingMode = true;

                // Reset pressStartTime when the player starts charging
                cuestick->pressStartTime = glfwGetTime();
            }
            else if (action == GLFW_RELEASE) {
                // Stop charging and launch the cue ball
                cuestick->pressEndTime = glfwGetTime();
                // Clalculate how much time user charged the cue stick
                double holdDuration = cuestick->pressEndTime - cuestick->pressStartTime;

                // Compute cue ball's initial velocity
                double velocityFactor = std::min(holdDuration, 1.5) * 5.0f;

                float angle = camera->horizontalAngle;
                vec3 spin = vec3(cuestick->dotX, cuestick->dotY, 0.0f) * 5.0f;

                // Launch ball
                balls[15]->launch(velocityFactor, angle, spin);

                // Exit shooting mode
                camera->lock = false;
                game.aimMode = false;
                game.shotInProgress = true;
                cuestick->chargingMode = false;
                cuestick->dotX = 0.0f;
                cuestick->dotY = 0.0f;
            }
        }
    }
}

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // Toggle lights using number keys (0-9)
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            int lightIndex = key - GLFW_KEY_0;

            // Toggle light state
            lightOn[lightIndex] = !lightOn[lightIndex];

            if (lightOn[lightIndex]) {
                // Turn light on
                light[lightIndex]->power = 5.0f;
                light[lightIndex]->Ld = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                light[lightIndex]->La = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                light[lightIndex]->Ls = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                glUniform1i(invertNormalsLocation, GL_TRUE);
            }
            else {
                // Turn light off
                light[lightIndex]->power = 0.0f;
                light[lightIndex]->Ld = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                light[lightIndex]->La = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                light[lightIndex]->Ls = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                glUniform1i(invertNormalsLocation, GL_FALSE);
            }

            std::cout << "Light " << lightIndex << " is " << (lightOn[lightIndex] ? "ON" : "OFF") << std::endl;
        }

        // Reset game when 'R' is pressed (works in both modes)
        if (key == GLFW_KEY_R) {
            std::cout << "R key pressed. Resetting balls." << std::endl;
            game.resetGame(balls, ballPositions);
            std::cout << "Balls reset to starting position." << std::endl;
        }

        // Exit aiming mode when 'Q' is pressed
        if (key == GLFW_KEY_Q && game.aimMode) {
            std::cout << "Q key pressed. Exiting aim mode." << std::endl;
            camera->lock = false;
            cuestick->chargingMode = false;
            game.aimMode = false;
        }

        if (key == GLFW_KEY_P && game.aimMode) {
            game.spinMode = !game.spinMode;
            std::cout << "Toggled 2D Cue Ball Projection: "
                << (game.spinMode ? "ON" : "OFF")
                << std::endl;
        }

        if (key == GLFW_KEY_Q && game.spinMode) {
            std::cout << "Q key pressed. Exiting aim mode." << std::endl;
            camera->lock = false;
            game.aimMode = false;
            cuestick->chargingMode = false;
            game.spinMode = false;
        }

        if (key == GLFW_KEY_I) cuestick->dotY += 0.001f;  // Move up
        if (key == GLFW_KEY_K) cuestick->dotY -= 0.001f;  // Move down
        if (key == GLFW_KEY_J) cuestick->dotX -= 0.001f;  // Move left
        if (key == GLFW_KEY_L) cuestick->dotX += 0.001f;  // Move right

        // Keep dot inside the ball (clamp to a circle)
        float dist = sqrt(cuestick->dotX * cuestick->dotX + cuestick->dotY * cuestick->dotY);
        if (dist > 1.0f) {
            cuestick->dotX /= dist;
            cuestick->dotY /= dist;
        }
    }
}

void initialize() {

    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
            " If you have an Intel GPU, they are not 3.3 compatible." +
            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // enable texturing and bind the depth texture
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);

    glfwSetKeyCallback(window, pollKeyboard);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);
}

int main(void)
{
    try
    {
        initialize();
        createContext();
        mainLoop();
        free();
    }
    catch (exception& ex)
    {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}