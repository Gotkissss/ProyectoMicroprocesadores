#ifndef GAME_H
#define GAME_H

#include <vector>
#include <ctime>
#include <unistd.h>
#include "entities.h"
#include "../graphics/screen.h"
#include "../graphics/sprites.h"
#include <pthread.h>

// Estados del juego
enum GameState {
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_COMPLETE
};

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
    GameState gameState;
    
    // Control de velocidad y disparo
    int invaderSpeed;
    time_t lastShotTime;
    
    // Mutex para proteger el acceso a los datos compartidos
    pthread_mutex_t gameMutex;
    
public:
    Game(SpriteManager& sm, Screen& s);
    ~Game();
    
    // Funciones de inicialización y bucle del juego
    void initializeGame();
    void mainLoop();
    void handleInput();
    void handleInputImproved();
    void handlePlayingInput(int key);
    bool canShoot();
    
    // Lógica del juego (públicas para acceso desde hilos)
    void generateInvaders();
    void updateGameLogic();
    void updateInvaders();
    void updateProjectiles();
    void checkCollisions();
    void checkPlayerCollisions();
    void checkInvaderReachBottom();
    void checkVictoryConditions();
    void checkGameOverConditions();
    void nextLevel();
    void resetGame();
    
    // Funciones de dibujo (públicas para acceso desde main)
    void drawElements();
    void drawGameStateMessages();
    
    // Getters y Setters
    bool isRunning() const;
    void setRunning(bool status);
    int getInvaderSpeed() const { return invaderSpeed; }
};

#endif