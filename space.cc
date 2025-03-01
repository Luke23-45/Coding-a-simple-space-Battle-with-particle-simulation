#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <sstream>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Game constants
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 20;
const int BULLET_WIDTH = 5;
const int BULLET_HEIGHT = 10;
const int ENEMY_WIDTH = 40;
const int ENEMY_HEIGHT = 30;
const int ENEMY_SPEED = 2; // vertical speed for enemy
const Uint32 ENEMY_SPAWN_INTERVAL = 2000; // milliseconds
const int MAX_ENEMIES = 10;

// Score system
int score = 0;

// Entity structure for player, bullet, and enemy
struct Entity {
    int x, y, w, h;
    bool active;
    SDL_Color color; 
    Entity(int a, int b, int c, int d, bool f = false)
        : x(a), y(b), w(c), h(d), active(f), color({255,255,255,255}) {}
};

// Global SDL variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

// Create player entity centered near the bottom
Entity player(SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, SCREEN_HEIGHT - 60, PLAYER_WIDTH, PLAYER_HEIGHT);

// Vectors to hold bullets and enemies
std::vector<Entity*> bullets;
std::vector<Entity*> enemies;

// Particle structure for explosion effects
struct Particle {
    float x, y;     // Position
    float vx, vy;    // Velocity
    float life;      // Remaining life 
    float maxLife; // Initial life 
    Uint8 r, g, b;   // Color
};

std::vector<Particle> particles;

// Star structure for the parallax background
struct Star {
    float x, y;
    float speed;
};

std::vector<Star> stars;

// Initialize SDL, TTF, and create window/renderer
bool SDLINIT() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << "\n";
        return false;
    }
    window = SDL_CreateWindow("Advanced Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        return false;
    }
    // Enable blending for alpha in particle effects
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Load font (ensure the font file is in your working directory)
    font = TTF_OpenFont("ARIAL.TTF", 24);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << "\n";
        return false;
    }
    return true;
}

// Create a starfield with a given count of stars
void createStars(int count = 100) {
    stars.clear();
    for (int i = 0; i < count; i++) {
        Star star;
        star.x = rand() % SCREEN_WIDTH;
        star.y = rand() % SCREEN_HEIGHT;
        star.speed = 0.5f + static_cast<float>(rand() % 100) / 200.0f; // Speed between 0.5 and 1.0
        stars.push_back(star);
    }
}

// Update star positions to create a parallax effect
void updateStars() {
    for (auto &star : stars) {
        star.y += star.speed;
        if (star.y > SCREEN_HEIGHT) {
            star.y = 0;
            star.x = rand() % SCREEN_WIDTH;
        }
    }
}

// Render stars as white points on the screen
void renderStars() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (auto &star : stars) {
        SDL_RenderDrawPoint(renderer, static_cast<int>(star.x), static_cast<int>(star.y));
    }
}

// Spawn an explosion of particles at (x, y)
void spawnExplosion(int x, int y, int numParticles = 30) {
    for (int i = 0; i < numParticles; i++) {
        Particle p;
        p.x = x;
        p.y = y;
        // Randomize direction and speed
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float speed = static_cast<float>(rand() % 50) / 10.0f; // Speed between 0 and 5
        p.vx = cos(angle) * speed;
        p.vy = sin(angle) * speed;
        p.life = 50 + rand() % 50; // Life between 50 and 100 frames
        p.maxLife = p.life;
        // Choose a random stunning color from a palette
        int colorChoice = rand() % 5;
        switch (colorChoice) {
            case 0: p.r = 255; p.g = 100; p.b = 100; break;
            case 1: p.r = 100; p.g = 255; p.b = 100; break;
            case 2: p.r = 100; p.g = 100; p.b = 255; break;
            case 3: p.r = 255; p.g = 255; p.b = 100; break;
            case 4: p.r = 255; p.g = 100; p.b = 255; break;
        }
        particles.push_back(p);
    }
}

// Update particle positions and remove dead particles (with alpha fading)
void updateParticles() {
    for (auto &p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.life -= 1;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle &p) { return p.life <= 0; }), particles.end());
}

// Render each particle as a small rectangle with fading alpha
void renderParticles() {
    for (auto &p : particles) {
        float alphaFactor = p.life / p.maxLife;
        Uint8 alpha = static_cast<Uint8>(alphaFactor * 255);
        SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, alpha);
        SDL_Rect rect = { static_cast<int>(p.x), static_cast<int>(p.y), 2, 2 };
        SDL_RenderFillRect(renderer, &rect);
    }
}

// Compute a dynamic enemy color based on the current score and randomness
SDL_Color getDynamicEnemyColor() {

    Uint8 r = (rand() % 256 + score) % 256;
    Uint8 g = (rand() % 256 + score * 2) % 256;
    Uint8 b = (rand() % 256 + score * 3) % 256;
    return { r, g, b, 255 };
}

// Spawn a single enemy at a random horizontal position above the screen
void spawnEnemy() {
    int x = rand() % (SCREEN_WIDTH - ENEMY_WIDTH);
    int y = -ENEMY_HEIGHT;
    Entity* enemy = new Entity(x, y, ENEMY_WIDTH, ENEMY_HEIGHT, true);
    // Set enemy's color dynamically based on the current score
    enemy->color = getDynamicEnemyColor();
    enemies.push_back(enemy);
}

// Update enemy positions (they move downward)
void updateEnemies() {
    for (auto &enemy : enemies) {
        enemy->y += ENEMY_SPEED;
    }
    // Remove enemies that have moved off the bottom
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Entity* enemy) {
        bool remove = enemy->y > SCREEN_HEIGHT;
        if (remove) delete enemy;
        return remove;
    }), enemies.end());
}

// Safely remove inactive bullets and free their memory
void removeInactiveBullets() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Entity* bullet) {
        bool remove = !bullet->active;
        if (remove)
            delete bullet;
        return remove;
    }), bullets.end());
}

// Update bullet positions
void updateBullets() {
    for (auto &bullet : bullets) {
        bullet->y -= 10;
        if (bullet->y + bullet->h <= 0) {
            bullet->active = false;
        }
    }
    removeInactiveBullets();
}

// Check for collisions between bullets and enemies; spawn explosions on hit and update score
void checkCollisions() {
    for (auto &bullet : bullets) {
        for (auto &enemy : enemies) {
            if (bullet->active && enemy->active &&
                bullet->x < enemy->x + enemy->w &&
                bullet->x + bullet->w > enemy->x &&
                bullet->y < enemy->y + enemy->h &&
                bullet->y + bullet->h > enemy->y) {
                bullet->active = false;
                enemy->active = false;
                spawnExplosion(enemy->x + enemy->w / 2, enemy->y + enemy->h / 2);
                score += 10;
            }
        }
    }
    removeInactiveBullets();
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Entity* enemy) {
        bool remove = !enemy->active;
        if (remove)
            delete enemy;
        return remove;
    }), enemies.end());
}

// Render an entity as a filled rectangle 
void renderEntity(const Entity* entity, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect = { entity->x, entity->y, entity->w, entity->h };
    SDL_RenderFillRect(renderer, &rect);
}

void renderRealisticEnemy(const Entity* enemy) {
    SDL_Color color = enemy->color;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    int body_width = enemy->w;
    int body_height = enemy->h / 2;
    int wing_width = body_width / 2;
    int wing_height = body_height / 2;
    int cockpit_radius = body_height / 4;

    // 1. Body (Main Rectangle) - slightly smaller height
    SDL_Rect bodyRect = { enemy->x, enemy->y + wing_height, body_width, body_height };
    SDL_RenderFillRect(renderer, &bodyRect);

    // 2. Wings (Triangles approximated by lines - V shape on sides)
    int wing_y_top = enemy->y + wing_height;
    int wing_y_bottom = enemy->y + wing_height + wing_height; // same as body bottom
    int wing_x_offset = body_width / 4;

    // Left Wing
    SDL_RenderDrawLine(renderer, enemy->x, wing_y_top, enemy->x + wing_x_offset, enemy->y); // Top left line
    SDL_RenderDrawLine(renderer, enemy->x, wing_y_top, enemy->x + wing_x_offset, wing_y_bottom); // Bottom left line

    // Right Wing
    SDL_RenderDrawLine(renderer, enemy->x + body_width, wing_y_top, enemy->x + body_width - wing_x_offset, enemy->y); // Top right line
    SDL_RenderDrawLine(renderer, enemy->x + body_width, wing_y_top, enemy->x + body_width - wing_x_offset, wing_y_bottom); // Bottom right line


    // 3. Cockpit 
    SDL_Rect cockpitRect = { enemy->x + body_width / 4, enemy->y, body_width / 2, wing_height };
    SDL_RenderFillRect(renderer, &cockpitRect);

    // Add a slightly brighter outline to cockpit
    SDL_SetRenderDrawColor(renderer, std::min(255, color.r + 50), std::min(255, color.g + 50), std::min(255, color.b + 50), 255);
    SDL_RenderDrawRect(renderer, &cockpitRect);


    // Add some details - Lines on the body
    SDL_SetRenderDrawColor(renderer, std::min(255, color.r + 30), std::min(255, color.g + 30), std::min(255, color.b + 30), 255);
    SDL_RenderDrawLine(renderer, enemy->x + body_width / 4, bodyRect.y, enemy->x + body_width / 4, bodyRect.y + bodyRect.h);
    SDL_RenderDrawLine(renderer, enemy->x + 3 * body_width / 4, bodyRect.y, enemy->x + 3 * body_width / 4, bodyRect.y + bodyRect.h);
}


// Render a dynamic gradient background for a stunning visual effect
void renderGradientBackground(Uint8 r, Uint8 g, Uint8 b) {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        float factor = static_cast<float>(i) / SCREEN_WIDTH;
        Uint8 rLine = static_cast<Uint8>(r * factor);
        Uint8 gLine = static_cast<Uint8>(g * factor);
        Uint8 bLine = static_cast<Uint8>(b * factor);
        SDL_SetRenderDrawColor(renderer, rLine, gLine, bLine, 255);
        SDL_RenderDrawLine(renderer, i, 0, i, SCREEN_HEIGHT);
    }
}

// Render the score text using SDL_ttf
void renderScore() {
    std::stringstream ss;
    ss << "Score: " << score;
    std::string scoreText = ss.str();
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect destRect = {10, 10, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}


void RenderScreen() {
    // Dynamic background: gradually shifting gradient
    static int colorShift = 0;
    colorShift = (colorShift + 1) % 256;
    renderGradientBackground(0, colorShift, 255 - colorShift);

    // Render the starfield in the background
    renderStars();

    // Render player
    renderEntity(&player, 0, 255, 150);

    // Render enemies as solid rectangles with dynamic colors
    for (const auto &enemy : enemies) {
        renderRealisticEnemy(enemy);
    }

    // Render bullets
    for (const auto &bullet : bullets) {
        renderEntity(bullet, 255, 255, 255);
    }

    // Render particle explosions
    renderParticles();

    // Render score on top
    renderScore();

    SDL_RenderPresent(renderer);
}

// Clean up all resources
void DestroyScreen() {
    for (auto bullet : bullets) {
        delete bullet;
    }
    bullets.clear();
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// Main game loop
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    if (!SDLINIT()) {
        return -1;
    }
    createStars();
    Uint32 lastEnemySpawn = SDL_GetTicks();

    Uint32 startFrame, frameTime;
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        startFrame = SDL_GetTicks();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }
        // Keyboard input for player movement and shooting
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_LEFT] && player.x > 0) {
            player.x -= 10;
        }
        if (keys[SDL_SCANCODE_RIGHT] && player.x + player.w < SCREEN_WIDTH) {
            player.x += 10;
        }
        if (keys[SDL_SCANCODE_SPACE]) {
            // Limit the number of active bullets
            if (bullets.size() < 7) {
                bullets.push_back(new Entity(player.x + player.w / 2, player.y, BULLET_WIDTH, BULLET_HEIGHT, true));
            }
        }

        // Spawn a new enemy at intervals if under limit
        if (SDL_GetTicks() - lastEnemySpawn > ENEMY_SPAWN_INTERVAL && enemies.size() < MAX_ENEMIES) {
            spawnEnemy();
            lastEnemySpawn = SDL_GetTicks();
        }

        // Update game systems
        updateStars();
        updateEnemies();
        updateBullets();
        checkCollisions();
        updateParticles();

        // Clear screen before rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        RenderScreen();

        // Maintain roughly 33 FPS
        frameTime = SDL_GetTicks() - startFrame;
        if (frameTime < 30) {
            SDL_Delay(30 - frameTime);
        }
    }
    DestroyScreen();
    return 0;
}