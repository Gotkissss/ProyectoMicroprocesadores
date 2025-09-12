#ifndef GAME_H
#define GAME_H

#include <vector>
#include "entities.h"
#include "graphics/screen.h"
#include "graphics/sprites.h"
#include <pthread.h>

class Game {
private:
    Screen& screen;
    SpriteManager& spriteManager;
    Player player;
    std::vector<Invader> invaders;
    std::vector<Projectile> projectiles;
    
    // Atributos del estado del juego
    bool running;
    int score;
    int lives;
    int level;
    
    // Mutex para proteger el acceso a los datos compartidos
    pthread_mutex_t gameMutex;
    
public:
    Game(SpriteManager& sm, Screen& s);
    
    // Funciones de inicialización y bucle del juego
    void initializeGame();
    void mainLoop();
    void handleInput();
    
    // Lógica del juego
    void generateInvaders();
    void updateGameLogic();
    void updateInvaders();
    void updateProjectiles();
    void checkCollisions();
    
    // Funciones de dibujo
    void drawElements();
    
    // Getters y Setters
    bool isRunning() const;
    void setRunning(bool status);
};

#endif