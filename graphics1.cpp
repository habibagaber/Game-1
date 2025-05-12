#include <cstdlib>
#include <glut.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <string>
#include <cmath> 


bool isImmune = false;
float immunityDuration = 0.0f; // Duration of immunity
const float M_PI = 3.14;
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;
const float CHAR_SIZE = 50.0f;
const float BARRIER_WIDTH = 60.0f;
const float BARRIER_HEIGHT = 50.0f;
const float ITEM_RADIUS = 17.5f; // Circle radius for collectibles
const float BOOST_WIDTH = 35.0f;
const float BOOST_HEIGHT = 35.0f;
const int MAX_HEALTH = 5;
const float GRAVITY_FORCE = -0.6f;
const float JUMP_FORCE = 15.0f;
const float INITIAL_VELOCITY = 5.0f;
const float VELOCITY_INCREMENT = 0.1f;

class GameInfo {
public:
    int score;
    int health;
    int lives; 
    float characterY;
    float timeLeft;
    float velocity;

    GameInfo() : score(0), health(MAX_HEALTH), lives(3), characterY(0.0f), timeLeft(60.0f), velocity(INITIAL_VELOCITY) {}
    //Game currentGame = { MAX_HEALTH, 0, 60.0f, 3, INITIAL_VELOCITY }; // Initialize lives to 3

};

GameInfo currentGame;

bool isJumping = false;
bool isDucking = false;
float verticalVelocity = 0.0f;
bool isGameOver = false;
bool isPlayerLost = false;

struct Barrier {
    float posX, posY;
    bool active;
};


struct Collectible {
    float posX, posY;
    bool active;
};

struct Boost {
    float posX, posY;
    bool active;
};
struct ImmunityPowerUp {
    float posX, posY;
    bool active;
    float rotationAngle; 
};
struct FlyPowerUp {
    float posX, posY;
    bool active;
    float rotationAngle; 
};



std::vector<FlyPowerUp> flyPowerUps;
std::vector<ImmunityPowerUp> immunityPowerUps;
std::vector<Barrier> barriers;
std::vector<Collectible> collectibles;
std::vector<Boost> boosts;

void renderGameScene();
void updateGameLoop(int);
void resetGame();
void createBarrier();
void createCollectible();
void createFlyPowerUp();
void createBoost();
void renderCharacter();
void renderBarrier(const Barrier& bar);
void renderCollectible(const Collectible& item);
void renderImmunityPowerUp(const ImmunityPowerUp& pUp);
void renderFlyPowerUp(const FlyPowerUp& fUp);
void renderBoost(const Boost& b);
void detectCollisions();
void showGameOverScreen();
void displayHealthBar();
void renderGround();
void renderTrees();
void renderSun();
void handleKeyPress(unsigned char key, int x, int y);
void handleKeyRelease(unsigned char key, int x, int y);
void renderBackground();
void renderUI();

void initializeGame() {
    glClearColor(0.95f, 0.9f, 1.0f, 1.0f); // Light pink background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    srand(static_cast<unsigned int>(time(nullptr)));
}

float skyColor[3] = { 0.0f, 0.5f, 0.8f }; // Initial sky color
float colorChangeSpeed = 0.001f; // Speed of color transition

void updateSkyColor() {
    // Change colors smoothly
    skyColor[0] += colorChangeSpeed; // Red component
    skyColor[1] -= colorChangeSpeed * 0.5f; // Green component
    skyColor[2] += colorChangeSpeed; // Blue component

    // Loop color values to create a gradient effect
    if (skyColor[0] > 1.0f) skyColor[0] = 0.0f;
    if (skyColor[1] < 0.0f) skyColor[1] = 1.0f;
    if (skyColor[2] > 1.0f) skyColor[2] = 0.0f;
}


void renderCloud(float x, float y) {
    glBegin(GL_POLYGON);
    for (int j = 0; j < 6; j++) {
        float theta = j * 2.0f * M_PI / 6;
        glVertex2f(x + 20 * cos(theta), y + 10 * sin(theta));
    }
    glEnd();
}

float sandColor[3] = { 1.0f, 0.9f, 0.6f }; // Sand color

void renderPalmTree(float x, float y) {
    // Draw trunk
    glColor3f(0.55f, 0.27f, 0.07f); // Brown trunk
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y);
    glVertex2f(x + 5, y);
    glVertex2f(x + 5, y + 30);
    glVertex2f(x - 5, y + 30);
    glEnd();

    // Draw leaves
    glColor3f(0.0f, 0.5f, 0.0f); // Green leaves
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 30); // Top point
    glVertex2f(x - 20, y + 10); // Left point
    glVertex2f(x + 20, y + 10); // Right point
    glEnd();
}

void renderWaves(float y) {
    glColor3f(0.0f, 0.5f, 1.0f); // Ocean color
    glBegin(GL_QUADS);
    glVertex2f(0, y);
    glVertex2f(WIN_WIDTH, y);
    glVertex2f(WIN_WIDTH, y + 50);
    glVertex2f(0, y + 50);
    glEnd();
}

void renderBackground() {
    updateSkyColor(); // Update the sky color

    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

    // Render your background with the updated sky color
    glColor3f(skyColor[0], skyColor[1], skyColor[2]);
    glBegin(GL_QUADS);
    glVertex2f(0, WIN_HEIGHT);
    glVertex2f(WIN_WIDTH, WIN_HEIGHT);
    glVertex2f(WIN_WIDTH, 0);
    glVertex2f(0, 0);
    glEnd();

    // Draw the ocean waves
    renderWaves(WIN_HEIGHT / 2);

    // Draw the sand
    glColor3f(sandColor[0], sandColor[1], sandColor[2]);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIN_WIDTH, 0);
    glVertex2f(WIN_WIDTH, WIN_HEIGHT / 2);
    glVertex2f(0, WIN_HEIGHT / 2);
    glEnd();

    // Draw palm trees
    for (int i = 0; i < 3; i++) {
        float treeX = (i + 1) * (WIN_WIDTH / 4); // Space trees evenly
        renderPalmTree(treeX, WIN_HEIGHT / 2);
    }

    // Draw the sun
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for sun
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(WIN_WIDTH - 100, WIN_HEIGHT - 100); // Center of sun
    for (int i = 0; i <= 20; i++) {
        float theta = i * 2.0f * M_PI / 20;
        glVertex2f(WIN_WIDTH - 100 + 40 * cos(theta), WIN_HEIGHT - 100 + 40 * sin(theta));
    }
    glEnd();

    // Draw sun rays
    glColor3f(1.0f, 0.8f, 0.0f); // Lighter yellow for rays
    for (int i = 0; i < 12; i++) {
        float theta = i * 2.0f * M_PI / 12;
        glBegin(GL_LINES);
        glVertex2f(WIN_WIDTH - 100 + 40 * cos(theta), WIN_HEIGHT - 100 + 40 * sin(theta));
        glVertex2f(WIN_WIDTH - 100 + 60 * cos(theta), WIN_HEIGHT - 100 + 60 * sin(theta));
        glEnd();
    }
}



void renderGameScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    renderBackground();

    renderTrees(); // Draw trees in the background

    if (isGameOver) {
        showGameOverScreen();
    }
    else {
        renderGround();
        renderCharacter();

        for (const auto& bar : barriers) {
            if (bar.active) {
                renderBarrier(bar);
            }
        }

        for (const auto& item : collectibles) {
            if (item.active) {
                renderCollectible(item);
            }
        }

        for (const auto& b : boosts) {
            if (b.active) {
                renderBoost(b);
            }
        }

        for (const auto& pUp : immunityPowerUps) { // Render immunity power-ups
            if (pUp.active) {
                renderImmunityPowerUp(pUp);
            }
        }

        for (const auto& fUp : flyPowerUps) { // Render flying power-ups
            if (fUp.active) {
                renderFlyPowerUp(fUp);
            }
        }

        displayHealthBar();
        renderUI();
    }

    glutSwapBuffers();
}


void updateGameLoop(int) {
    resetGame();
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, updateGameLoop, 0);
    // Update rotation angles
    for (auto& pUp : immunityPowerUps) {
        if (pUp.active) {
            pUp.rotationAngle += 1.0f; // Adjust speed as needed
            if (pUp.rotationAngle >= 360.0f) {
                pUp.rotationAngle -= 360.0f; // Reset after a full rotation
            }
        }
    }

    for (auto& fUp : flyPowerUps) {
        if (fUp.active) {
            fUp.rotationAngle += 1.0f; // Adjust speed as needed
            if (fUp.rotationAngle >= 360.0f) {
                fUp.rotationAngle -= 360.0f; // Reset after a full rotation
            }
        }
    }
}

void resetGame() {
    if (!isGameOver) {
        if (currentGame.timeLeft > 0) {
            currentGame.timeLeft -= 1.0f / 60.0f;

            if (isImmune) {
                immunityDuration -= 1.0f / 60.0f;
                if (immunityDuration <= 0) {
                    isImmune = false; // End immunity
                }
            }

            // Increase the velocity based on remaining time
            currentGame.velocity = INITIAL_VELOCITY * pow(1.05f, (60.0f - currentGame.timeLeft));

            // Handle jumping
            if (isJumping) {
                verticalVelocity += GRAVITY_FORCE;
                currentGame.characterY += verticalVelocity;

                // Check if character has landed
                if (currentGame.characterY <= 0) {
                    currentGame.characterY = 0; // Reset to ground level
                    isJumping = false; // Reset jump state
                    verticalVelocity = 0; // Reset vertical velocity
                }
            }

            // Randomly create barriers, collectibles, boosts, and flying power-ups
            if (rand() % 100 < 10) createBarrier();
            if (rand() % 100 < 10) createCollectible();
            if (rand() % 100 < 3) createBoost();
            if (rand() % 100 < 1) createFlyPowerUp(); // Reduced chance for fly power-ups

            // Move all objects
            for (auto& bar : barriers) {
                if (bar.active) {
                    bar.posX -= currentGame.velocity;
                    if (bar.posX < -BARRIER_WIDTH) bar.active = false;
                }
            }

            for (auto& item : collectibles) {
                if (item.active) {
                    item.posX -= currentGame.velocity;
                    if (item.posX < -ITEM_RADIUS * 2) item.active = false;
                }
            }

            for (auto& b : boosts) {
                if (b.active) {
                    b.posX -= currentGame.velocity;
                    if (b.posX < -BOOST_WIDTH) b.active = false;
                }
            }

            for (auto& fUp : flyPowerUps) {
                if (fUp.active) {
                    fUp.posX -= currentGame.velocity; // Move the fly power-up
                    if (fUp.posX < -BOOST_WIDTH) fUp.active = false; // Deactivate if off-screen
                }
            }

            for (auto& pUp : immunityPowerUps) {
                if (pUp.active) {
                    pUp.posX -= currentGame.velocity; // Move the immunity power-up
                    if (pUp.posX < -BOOST_WIDTH) pUp.active = false; // Deactivate if off-screen
                }
            }

            detectCollisions();
        }
        else {
            isGameOver = true;
        }
    }
}


const float GROUND_HEIGHT = 0.0f; // Height of the ground level

void createBarrier() {
    float height;
    // Randomly decide to place it on the ground or at a random height
    if (rand() % 2 == 0) { // 50% chance to place on the ground
        height = WIN_HEIGHT - GROUND_HEIGHT; // This should position it at ground level
    }
    else {
        // Calculate a random height above the ground but below the top of the window
        height = (rand() % (static_cast<int>(WIN_HEIGHT - GROUND_HEIGHT - 100))) + GROUND_HEIGHT;
        // Ensure it stays above the ground height
    }

    Barrier bar = { WIN_WIDTH, height, true };
    barriers.push_back(bar);
}



void createCollectible() {
    Collectible item = { WIN_WIDTH, static_cast<float>(rand() % (WIN_HEIGHT - 100) + 100), true };
    collectibles.push_back(item);
}
void createImmunityPowerUp() {
    // Set a vertical position within the window bounds
    ImmunityPowerUp pUp = { WIN_WIDTH, static_cast<float>(rand() % (WIN_HEIGHT - 100) + 50), true };
    immunityPowerUps.push_back(pUp);
}

void createFlyPowerUp() {
    // Set a vertical position within the window bounds
    FlyPowerUp fUp = { WIN_WIDTH, static_cast<float>(rand() % (WIN_HEIGHT - 100) + 50), true };
    flyPowerUps.push_back(fUp);

}


void createBoost() {
    Boost b = { WIN_WIDTH, static_cast<float>(rand() % (WIN_HEIGHT - 100) + 100), true };
    boosts.push_back(b);

    // 10% chance to create an immunity power-up (adjust if needed)
    if (rand() % 100 < 20) createImmunityPowerUp();
}

void detectCollisions() {
    // Check for collisions with barriers
    
    for (auto& bar : barriers) {
        if (bar.active && !isImmune && bar.posX < 50 + CHAR_SIZE && bar.posX + BARRIER_WIDTH > 50 &&
            bar.posY < currentGame.characterY + CHAR_SIZE && bar.posY + BARRIER_HEIGHT > currentGame.characterY) {
            if (!isDucking) {
                currentGame.health--;
            }
            bar.active = false;

            if (currentGame.health <= 0) {
                if (currentGame.lives > 0) {
                    currentGame.lives--; // Decrease lives
                    currentGame.health = MAX_HEALTH; // Reset health
                }
                else {
                    isGameOver = true;
                    isPlayerLost = true;
                }
            }
        }
    }
    for (auto& bar : barriers) {
        if (bar.active && !isImmune && bar.posX < 50 + CHAR_SIZE && bar.posX + BARRIER_WIDTH > 50 &&
            bar.posY < currentGame.characterY + CHAR_SIZE && bar.posY + BARRIER_HEIGHT > currentGame.characterY) {
            if (!isDucking) {
                currentGame.health--;
            }
            bar.active = false;

            if (currentGame.health <= 0) {
                isGameOver = true;
                isPlayerLost = true;
            }
        }
    }

    // Check for collisions with collectibles
    for (auto& item : collectibles) {
        if (item.active && item.posX < 50 + CHAR_SIZE && item.posX + ITEM_RADIUS * 2 > 50 &&
            item.posY < currentGame.characterY + CHAR_SIZE && item.posY + ITEM_RADIUS * 2 > currentGame.characterY) {
            currentGame.score += 10;
            item.active = false;
        }
    }

    // Check for collisions with boosts
    for (auto& b : boosts) {
        if (b.active && b.posX < 50 + CHAR_SIZE && b.posX + BOOST_WIDTH > 50 &&
            b.posY < currentGame.characterY + CHAR_SIZE && b.posY + BOOST_HEIGHT > currentGame.characterY) {
            currentGame.health++;
            if (currentGame.health > MAX_HEALTH) currentGame.health = MAX_HEALTH;
            b.active = false;
        }
    }

    // Check for collisions with immunity power-ups
    for (auto& pUp : immunityPowerUps) {
        if (pUp.active && pUp.posX < 50 + CHAR_SIZE && pUp.posX + BOOST_WIDTH > 50 &&
            pUp.posY < currentGame.characterY + CHAR_SIZE && pUp.posY + BOOST_WIDTH > currentGame.characterY) {
            isImmune = true;
            immunityDuration = 5.0f; // Set immunity for 5 seconds
            pUp.active = false;
        }
    }

    // Check for collisions with fly power-ups
    for (auto& fUp : flyPowerUps) {
        if (fUp.active && fUp.posX < 50 + CHAR_SIZE && fUp.posX + BOOST_WIDTH > 50 &&
            fUp.posY < currentGame.characterY + CHAR_SIZE && fUp.posY + BOOST_WIDTH > currentGame.characterY) {
            // Set flying logic
            isJumping = true; // Start flying
            verticalVelocity = JUMP_FORCE; // Optional: adjust vertical force
            fUp.active = false;

            // Reset jumping state after 5 seconds
            glutTimerFunc(5000, [](int) {
                isJumping = false; // Stop flying after 5 seconds
                verticalVelocity = 0; // Reset vertical velocity
                }, 0);
        }
    }
}




void showGameOverScreen() {
    glColor3f(0.0f, 0.0f, 0.0f);
    std::string message = isPlayerLost ? "Game Over! You have no lives left!" : "Game over!";
    glRasterPos2f(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2);
    for (char c : message) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}


void displayHealthBar() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red for health bar
    glBegin(GL_QUADS);
    glVertex2f(10, WIN_HEIGHT - 30);
    glVertex2f(10 + currentGame.health * 20, WIN_HEIGHT - 30);
    glVertex2f(10 + currentGame.health * 20, WIN_HEIGHT - 20);
    glVertex2f(10, WIN_HEIGHT - 20);
    glEnd();
}

void renderGround() {
    glColor3f(0.0f, 0.8f, 0.0f); // Green for ground
    glBegin(GL_QUADS);
    glVertex2f(0, 50);
    glVertex2f(WIN_WIDTH, 50);
    glVertex2f(WIN_WIDTH, 0);
    glVertex2f(0, 0);
    glEnd();
}

void renderTrees() {
    glColor3f(0.5f, 0.3f, 0.1f); // Brown for tree trunks
    for (int i = 0; i < 5; i++) {
        glBegin(GL_QUADS);
        glVertex2f(i * 150 + 30, 50);
        glVertex2f(i * 150 + 50, 50);
        glVertex2f(i * 150 + 50, 100);
        glVertex2f(i * 150 + 30, 100);
        glEnd();

        glColor3f(0.0f, 0.8f, 0.0f); // Green for leaves
        glBegin(GL_TRIANGLES);
        glVertex2f(i * 150 + 10, 100);
        glVertex2f(i * 150 + 40, 150);
        glVertex2f(i * 150 + 70, 100);
        glEnd();
    }
}

void renderCharacter() {
    // Set color to purple for the character
    if (isDucking) {
        glColor3f(0.5f, 0.0f, 0.5f); // Darker purple for ducking
    }
    else {
        glColor3f(0.75f, 0.0f, 0.75f); // Lighter purple for normal state
    }

    float characterHeight = isDucking ? CHAR_SIZE / 2 : CHAR_SIZE;

    // Draw body
    glBegin(GL_QUADS);
    glVertex2f(50, currentGame.characterY);
    glVertex2f(50 + CHAR_SIZE, currentGame.characterY);
    glVertex2f(50 + CHAR_SIZE, currentGame.characterY + characterHeight);
    glVertex2f(50, currentGame.characterY + characterHeight);
    glEnd();

    // Draw lines on the body
    glColor3f(1.0f, 1.0f, 1.0f); // White color for lines

    // Vertical line on the left
    glBegin(GL_LINES);
    glVertex2f(50, currentGame.characterY);
    glVertex2f(50, currentGame.characterY + characterHeight);
    glEnd();

    // Vertical line on the right
    glBegin(GL_LINES);
    glVertex2f(50 + CHAR_SIZE, currentGame.characterY);
    glVertex2f(50 + CHAR_SIZE, currentGame.characterY + characterHeight);
    glEnd();

    // Draw head
    glColor3f(1.0f, 1.0f, 1.0f); // White for head
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(50 + CHAR_SIZE / 2, currentGame.characterY + characterHeight); // Center of the head
    for (int i = 0; i <= 20; i++) {
        float theta = i * 2.0f * M_PI / 20;
        glVertex2f(50 + CHAR_SIZE / 2 + 15 * cos(theta), currentGame.characterY + characterHeight + 15 + 15 * sin(theta));
    }
    glEnd();

    // Draw eyes
    glColor3f(0.0f, 0.0f, 0.0f); // Black for eyes
    glBegin(GL_QUADS);
    // Left eye
    glVertex2f(50 + CHAR_SIZE / 2 - 5, currentGame.characterY + characterHeight + 10);
    glVertex2f(50 + CHAR_SIZE / 2 - 5 + 5, currentGame.characterY + characterHeight + 10);
    glVertex2f(50 + CHAR_SIZE / 2 - 5 + 5, currentGame.characterY + characterHeight + 15);
    glVertex2f(50 + CHAR_SIZE / 2 - 5, currentGame.characterY + characterHeight + 15);
    // Right eye
    glVertex2f(50 + CHAR_SIZE / 2 + 5, currentGame.characterY + characterHeight + 10);
    glVertex2f(50 + CHAR_SIZE / 2 + 5 + 5, currentGame.characterY + characterHeight + 10);
    glVertex2f(50 + CHAR_SIZE / 2 + 5 + 5, currentGame.characterY + characterHeight + 15);
    glVertex2f(50 + CHAR_SIZE / 2 + 5, currentGame.characterY + characterHeight + 15);
    glEnd();

    // Draw arms
    glColor3f(0.75f, 0.0f, 0.0f); // Lighter purple for arms
    float armHeight = 10.0f; // Height of the arms
    // Left arm
    glBegin(GL_QUADS);
    glVertex2f(50 - 10, currentGame.characterY + characterHeight / 2);
    glVertex2f(50, currentGame.characterY + characterHeight / 2);
    glVertex2f(50, currentGame.characterY + characterHeight / 2 + armHeight);
    glVertex2f(50 - 10, currentGame.characterY + characterHeight / 2 + armHeight);
    glEnd();

    // Right arm
    glBegin(GL_QUADS);
    glVertex2f(50 + CHAR_SIZE, currentGame.characterY + characterHeight / 2);
    glVertex2f(50 + CHAR_SIZE + 10, currentGame.characterY + characterHeight / 2);
    glVertex2f(50 + CHAR_SIZE + 10, currentGame.characterY + characterHeight / 2 + armHeight);
    glVertex2f(50 + CHAR_SIZE, currentGame.characterY + characterHeight / 2 + armHeight);
    glEnd();

    // Optional: Draw a shadow
    if (isDucking) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // Shadow color
        glBegin(GL_QUADS);
        glVertex2f(50, currentGame.characterY - 5); // Shadow
        glVertex2f(50 + CHAR_SIZE, currentGame.characterY - 5);
        glVertex2f(50 + CHAR_SIZE, currentGame.characterY + characterHeight - 5);
        glVertex2f(50, currentGame.characterY + characterHeight - 5);
        glEnd();
    }
}

void renderBarrier(const Barrier& bar) {
    // Render the barrier as a red rectangle
    glColor3f(1.0f, 0.0f, 0.0f); // Red for barriers
    glBegin(GL_QUADS);
    glVertex2f(bar.posX, bar.posY);
    glVertex2f(bar.posX + BARRIER_WIDTH, bar.posY);
    glVertex2f(bar.posX + BARRIER_WIDTH, bar.posY + BARRIER_HEIGHT);
    glVertex2f(bar.posX, bar.posY + BARRIER_HEIGHT);
    glEnd();

    // Draw a thicker white line on top of the barrier
    glColor3f(1.0f, 1.0f, 1.0f); // White for the line
    glLineWidth(5.0f); // Set line width
    glBegin(GL_LINES);
    glVertex2f(bar.posX, bar.posY + BARRIER_HEIGHT); // Start of the line
    glVertex2f(bar.posX + BARRIER_WIDTH, bar.posY + BARRIER_HEIGHT); // End of the line
    glEnd();
}





void renderCollectible(const Collectible& item) {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for collectibles
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(item.posX, item.posY); // Center of the collectible
    for (int i = 0; i <= 20; i++) {
        float theta = i * 2.0f * M_PI / 20;
        glVertex2f(item.posX + ITEM_RADIUS * cos(theta), item.posY + ITEM_RADIUS * sin(theta));
    }
    glEnd();
}

void renderBoost(const Boost& b) {
    glColor3f(0.0f, 1.0f, 0.0f); // Green for boosts
    glBegin(GL_QUADS);
    glVertex2f(b.posX, b.posY);
    glVertex2f(b.posX + BOOST_WIDTH, b.posY);
    glVertex2f(b.posX + BOOST_WIDTH, b.posY + BOOST_HEIGHT);
    glVertex2f(b.posX, b.posY + BOOST_HEIGHT);
    glEnd();
}

void renderImmunityPowerUp(const ImmunityPowerUp& pUp) {
    glPushMatrix(); // Save the current state
    glTranslatef(pUp.posX, pUp.posY, 0.0f); // Move to the power-up position
    glRotatef(pUp.rotationAngle, 0.0f, 0.0f, 1.0f); // Apply rotation

    const float stripeHeight = ITEM_RADIUS / 5; // Height of each stripe
    const float squareSize = ITEM_RADIUS / 10; // Size of the small squares
    const float dotRadius = ITEM_RADIUS / 15; // Radius of the dots

    // Draw striped triangle
    for (int i = 0; i < 5; i++) {
        glBegin(GL_TRIANGLES);
        glColor3f(i % 2 == 0 ? 0.0f : 0.2f, i % 2 == 0 ? 0.0f : 0.2f, 1.0f); // Alternate colors

        glVertex2f(0, ITEM_RADIUS - i * stripeHeight); // Top vertex
        glVertex2f(-ITEM_RADIUS, -ITEM_RADIUS + (i + 1) * stripeHeight); // Bottom left vertex
        glVertex2f(ITEM_RADIUS, -ITEM_RADIUS + (i + 1) * stripeHeight); // Bottom right vertex

        glEnd();
    }

    // Draw dots on the triangle
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for dots
    for (int i = -1; i <= 1; i++) {
        for (int j = 0; j <= 2; j++) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(i * (ITEM_RADIUS / 2), j * (ITEM_RADIUS / 5)); // Position of the dot
            for (int k = 0; k <= 20; k++) {
                float theta = k * 2.0f * M_PI / 20; // Circle for the dot
                glVertex2f(i * (ITEM_RADIUS / 2) + dotRadius * cos(theta),
                    j * (ITEM_RADIUS / 5) + dotRadius * sin(theta));
            }
            glEnd();
        }
    }

    // Draw small squares for decoration
    glColor3f(0.0f, 1.0f, 0.0f); // Green for squares
    for (int i = -1; i <= 1; i++) {
        glBegin(GL_QUADS);
        glVertex2f(i * (ITEM_RADIUS / 3), -ITEM_RADIUS / 2); // Bottom left
        glVertex2f(i * (ITEM_RADIUS / 3) + squareSize, -ITEM_RADIUS / 2); // Bottom right
        glVertex2f(i * (ITEM_RADIUS / 3) + squareSize, -ITEM_RADIUS / 2 + squareSize); // Top right
        glVertex2f(i * (ITEM_RADIUS / 3), -ITEM_RADIUS / 2 + squareSize); // Top left
        glEnd();
    }

    glPopMatrix(); // Restore the previous state
}

void renderFlyPowerUp(const FlyPowerUp& fUp) {
    glPushMatrix(); // Save the current state
    glTranslatef(fUp.posX, fUp.posY, 0.0f); // Move to the power-up position
    glRotatef(fUp.rotationAngle, 0.0f, 0.0f, 1.0f); // Apply rotation

    const float smallRadius = ITEM_RADIUS * 0.7f; // Make it 70% of original size
    const int stripes = 6; // Number of stripes
    const float stripeWidth = smallRadius / stripes;

    // Draw striped hexagon
    for (int i = 0; i < stripes; i++) {
        glBegin(GL_POLYGON);
        // Set color for the current stripe
        if (i % 2 == 0) {
            glColor3f(1.0f, 0.0f, 1.0f); // Magenta for even stripes
        }
        else {
            glColor3f(0.5f, 0.0f, 0.5f); // Darker magenta for odd stripes
        }

        for (int j = 0; j < 6; j++) {
            float theta = j * 2.0f * M_PI / 6 + (i * stripeWidth / smallRadius); // Offset for stripes
            glVertex2f((smallRadius + stripeWidth * i) * cos(theta),
                (smallRadius + stripeWidth * i) * sin(theta));
        }
        glEnd();
    }

    // Draw dot in the center
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for the dot
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0); // Center of the dot
    for (int i = 0; i <= 20; i++) {
        float theta = i * 2.0f * M_PI / 20; // Circle for the dot
        glVertex2f((smallRadius / 4) * cos(theta), (smallRadius / 4) * sin(theta));
    }
    glEnd();

    glPopMatrix(); // Restore the previous state
}







void handleKeyPress(unsigned char key, int x, int y) {
    if (key == ' ') {
        if (!isJumping) {
            isJumping = true;
            verticalVelocity = JUMP_FORCE;
        }
    }
    if (key == 's') {
        isDucking = true;
    }
}

void handleKeyRelease(unsigned char key, int x, int y) {
    if (key == 's') {
        isDucking = false;
    }
}

void renderUI() {
    glColor3f(0.0f, 0.0f, 0.0f);

    std::string timeLeftString = "Time Left: " + std::to_string(static_cast<int>(currentGame.timeLeft));
    glRasterPos2f(WIN_WIDTH - 200, WIN_HEIGHT - 20);
    for (char c : timeLeftString) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    std::string scoreString = "Score: " + std::to_string(currentGame.score);
    glRasterPos2f(WIN_WIDTH - 200, WIN_HEIGHT - 40);
    for (char c : scoreString) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    // Display Lives
    std::string livesString = "Lives: " + std::to_string(currentGame.lives);
    glRasterPos2f(WIN_WIDTH - 200, WIN_HEIGHT - 60);
    for (char c : livesString) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("Speed Increasing Game");
    initializeGame();

    glutDisplayFunc(renderGameScene);
    glutKeyboardFunc(handleKeyPress);
    glutKeyboardUpFunc(handleKeyRelease);
    glutTimerFunc(0, updateGameLoop, 0);

    resetGame();

    glutMainLoop();
    return 0;
}