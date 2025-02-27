# 3D Billiard Game (C++ & OpenGL)

## Overview
This project is a 3D billiard game built using C++ and OpenGL. The game simulates realistic billiard mechanics, including aiming, shooting, ball collisions, and spin effects. It also features a dynamic lighting system with toggleable lights and free camera movement.

## Features & Capabilities
### Game Mechanics
- **Realistic Ball Physics**: The game implements collision detection and friction for accurate ball movement.
- **Cue Stick Mechanics**:
  - Right-click to enter aiming mode when all balls are stationary.
  - Left-click to charge the shot, with power determined by charge duration.
  - Cue ball spin can be adjusted by targeting specific areas.
- **Turn-Based Gameplay**: The game alternates turns between two players.
- **Game Rules Implementation** Following standard billiard rules.

### Controls
- **Mouse Controls**:
  - `Right-click`: Enter aiming mode (when balls are stationary).
  - `Left-click` (hold & release): Charge and release the shot.
- **Keyboard Controls**:
  - `W, A, S, D`: Move the camera.
  - `R`: Reset the game.
  - `Q`: Exit aiming mode.
  - `P`: Toggle 2D cue ball projection (for spin adjustment).
  - `I, K, J, L`: Adjust cue stick targeting position (for spin effects).
  - `0-9`: Toggle individual lights around the table.

### Lighting & Shadows
- 10 evenly spaced lights around the pool table.
- Each light can be toggled on/off using number keys.
- Dynamic lighting and shadow effects enhance realism.

### Camera System
- Free camera movement to explore the table.
- Overhead aiming mode for precise shots.

## Build and Run Instructions

### Prerequisites
Ensure you have the following installed:
- CMake (version 3.x or later)
- Visual Studio

### Build Steps
1. **Open CMake GUI**
   - Launch CMake GUI.
2. **Select Source Code Directory**
   - Click "Browse Source" and select your project's root directory.
3. **Select Build Directory**
   - Click "Browse Build" and create/select a `build` directory.
4. **Configure the Project**
   - Click "Configure" and "Finish".
5. **Generate Build Files**
   - Click "Generate".
6. **Build the Project**
   - Open the generated `.sln` file in Visual Studio and build the project.

## Future Enhancements
- Enhanced AI for single-player mode.
- Improved graphics and visual effects.
- Multiplayer support via network play.

---
This README provides a structured overview of the project, detailing both the development process and gameplay mechanics. Let me know if you'd like any modifications!

