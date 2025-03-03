# **Space Battle with Particle Simulation**

A simple 2D game where you control a paddle to shoot down enemies. Enjoy visually appealing particle effects and dynamic backgrounds as you play.

## Getting Started

### Prerequisites
Ensure you have the following installed:
- C++ Compiler (e.g., g++)
- [SDL2](https://www.libsdl.org/) 
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) - A library that allows you to render TrueType fonts using SDL.


## Project Structure

*   `space.cc`: Main C++ source file containing the game logic and rendering.
*   `Makefile`: Build configuration file for compiling the project.
*   `src/`
    *   `include/`:  Header files


### Installation
1. Clone the repository:
    ```bash
    git clone git@github.com:Luke23-45/Coding-a-simple-space-Battle-with-particle-simulation.git
    ```
## Building

This project uses `make` for building. To build the project, run the following command in your terminal:

1. Navigate to the project directory:
    ```bash
    cd Coding-a-simple-space-Battle-with-particle-simulation
    ```
3. Compile the code:
    ```bash
    make
    ```
4. Run the executable (Linux/macOS):
    ```bash
    ./main

    ```
5. Run the executable (Windows):
    ```bash
    main.exe
    ```
6. To clean up the build artifacts:
    ```bash
    make clean
    ```

## Features
- **Dynamic Background**: Features a gradient background that subtly shifts colors and a starfield for an immersive space environment.
- **Particle Explosions**: Enemies explode into colorful particles when hit, adding a satisfying visual effect.
- **Score System**: Keeps track of your score, increasing as you destroy enemies.
- **Realistic Enemy Rendering**: Enemies are rendered with a stylized, detailed design.
- **Responsive Player Control**: Smooth player movement to control the paddle and shoot bullets.
- **Enemy Spawning**: Enemies are spawned at intervals, increasing the challenge over time.

## Key Controls

| Action            | Key       |        |
| ----------------- | --------- | ------ |
| Exit simulation   | `ESC` key |        |
| Move Left         | `LEFT` arrow key |        |
| Move Right        | `RIGHT` arrow key|        |
| Shoot             | `SPACE` key|        |

## Code Structure
The project is structured as follows:

- **`SDLINIT()`**: Initializes SDL2, SDL_ttf, window, and renderer. Handles setup for graphics and font rendering.
- **`createStars()`, `updateStars()`, `renderStars()`**: Manages the starfield background, creating a parallax effect.
- **`spawnExplosion()`, `updateParticles()`, `renderParticles()`**: Implements the particle explosion effect when enemies are destroyed.
- **`getDynamicEnemyColor()`, `spawnEnemy()`, `updateEnemies()`**: Controls enemy spawning and movement, with enemies changing color dynamically.
- **`updateBullets()`, `removeInactiveBullets()`**: Manages bullet movement and removes bullets that are off-screen.
- **`checkCollisions()`**: Detects collisions between bullets and enemies, triggering explosions and score updates.
- **`renderEntity()`, `renderRealisticEnemy()`**: Functions for rendering game entities, including the player and enemies with distinct styles.
- **`renderGradientBackground()`**: Renders the dynamic gradient background.
- **`renderScore()`**: Renders the current game score on the screen.
- **`RenderScreen()`**: Orchestrates the rendering of all game elements in each frame.
- **`DestroyScreen()`**: Cleans up all SDL resources upon program termination.
- **`main()`**: The main game loop, handling game initialization, event processing, updates, rendering, and resource management.

## Demo Video
Check out the project demo video on YouTube: https://youtu.be/HDTzYFRzg50
## License

This project is licensed under the MIT License. Feel free to use, modify, and distribute the code.

## Acknowledgements

- SDL2 for graphics rendering.
- SDL2_ttf for font rendering.
