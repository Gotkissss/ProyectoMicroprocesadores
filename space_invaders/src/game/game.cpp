#include "game.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <ncurses.h>

Game::Game(SpriteManager& sm, Screen& s) : spriteManager(sm), screen(s) {
    running = false;
    score = 0;
    lives = 3;
    level = 1;
    gameState = PLAYING;
    invaderSpeed = 500000;
    lastShotTime = 0;
    
    // Inicializar el mutex
    pthread_mutex_init(&gameMutex, NULL);
}

Game::~Game() {
    pthread_mutex_destroy(&gameMutex);
}

void Game::initializeGame() {
    // Configurar ncurses para entrada no-bloqueante
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);  // Esta es la línea clave - hace getch() no-bloqueante
    timeout(0);             // Timeout de 0ms para getch()
    
    // Reinicializar todas las variables del juego
    running = true;
    gameState = PLAYING;
    score = 0;
    lives = 3;
    level = 1;
    invaderSpeed = 500000;
    lastShotTime = 0;
    
    // Limpiar contenedores
    projectiles.clear();
    invaders.clear();
    
    // Inicializar posición del jugador DESPUÉS de establecer lives
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    player.lives = lives;
    player.score = score;
    
    // Generar invasores
    generateInvaders();
}

void Game::generateInvaders() {
    invaders.clear();
    
    // Crear 5 filas de invasores - POSICIONES CORREGIDAS
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 11; col++) {
            Invader invader;
            invader.position.x = 5 + col * 6;
            invader.position.y = 2 + row * 2; // CAMBIO: Empezar en y=2 con separación de 2
            invader.direction = 1;
            invader.isAlive = true;
            
            // Asignar tipos según la fila
            if (row < 1) {
                invader.type = 1; // Tipo 1 (30 puntos)
            } else if (row < 3) {
                invader.type = 2; // Tipo 2 (20 puntos)
            } else {
                invader.type = 3; // Tipo 3 (10 puntos)
            }
            
            invaders.push_back(invader);
        }
    }
}

void Game::mainLoop() {
    while (running) {
        handleInputImproved();
        
        if (gameState == PLAYING) {
            updateGameLogic();
        }
        
        // Limpiar y dibujar
        screen.clear();
        drawElements();
        drawGameStateMessages();
        screen.draw();
        
        // Control de velocidad del juego - reducido para mejor fluidez
        usleep(30000); // 30ms = ~33 FPS (antes era 50ms)
    }
}

void Game::updateGameLogic() {
    // updateInvaders() se ejecuta en el hilo separado
    updateProjectiles();
    checkCollisions();
    checkPlayerCollisions();
    checkInvaderReachBottom();
    checkVictoryConditions();
    checkGameOverConditions();
    
    // Actualizar animaciones cada cierto tiempo
    static time_t lastAnimUpdate = 0;
    time_t currentTime = time(NULL);
    if (currentTime - lastAnimUpdate >= 1) {
        spriteManager.updateAnimations();
        lastAnimUpdate = currentTime;
    }
}

void Game::checkGameOverConditions() {
    if (lives <= 0) {
        gameState = GAME_OVER;
    }
}

void Game::updateInvaders() {
    pthread_mutex_lock(&gameMutex);
    
    static time_t lastMoveTime = 0;
    time_t currentTime = time(NULL);
    
    // Mover invasores según la velocidad del nivel
    if (currentTime - lastMoveTime >= 1) {
        bool hitEdge = false;
        
        // Verificar si algún invasor tocó el borde
        for (auto& invader : invaders) {
            if (!invader.isAlive) continue;
            
            if ((invader.direction == 1 && invader.position.x >= screen.getWidth() - 10) ||
                (invader.direction == -1 && invader.position.x <= 2)) {
                hitEdge = true;
                break;
            }
        }
        
        // Si tocaron el borde, cambiar dirección y bajar
        if (hitEdge) {
            for (auto& invader : invaders) {
                if (invader.isAlive) {
                    invader.direction *= -1;
                    invader.position.y += 2;
                }
            }
        } else {
            // Mover horizontalmente
            for (auto& invader : invaders) {
                if (invader.isAlive) {
                    invader.position.x += invader.direction;
                }
            }
        }
        
        // Los invasores disparan ocasionalmente
        if (rand() % 50 == 0) {
            for (auto& invader : invaders) {
                if (invader.isAlive && rand() % 100 < 5) {
                    Projectile bullet;
                    bullet.position.x = invader.position.x + 3;
                    bullet.position.y = invader.position.y + 2;
                    bullet.directionY = 1;
                    projectiles.push_back(bullet);
                    break;
                }
            }
        }
        
        lastMoveTime = currentTime;
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::updateProjectiles() {
    pthread_mutex_lock(&gameMutex);
    
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        it->position.y += it->directionY;
        
        // Eliminar proyectiles que salen de la pantalla
        if (it->position.y < 0 || it->position.y >= screen.getHeight()) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::checkCollisions() {
    pthread_mutex_lock(&gameMutex);
    
    for (auto projIt = projectiles.begin(); projIt != projectiles.end();) {
        bool hit = false;
        
        if (projIt->directionY == -1) { // Proyectil del jugador
            for (auto& invader : invaders) {
                if (invader.isAlive &&
                    projIt->position.x >= invader.position.x &&
                    projIt->position.x <= invader.position.x + 6 &&
                    projIt->position.y >= invader.position.y &&
                    projIt->position.y <= invader.position.y + 1) {
                    
                    invader.isAlive = false;
                    score += (invader.type == 1) ? 30 : (invader.type == 2) ? 20 : 10;
                    player.score = score;
                    hit = true;
                    break;
                }
            }
        }
        
        if (hit) {
            projIt = projectiles.erase(projIt);
        } else {
            ++projIt;
        }
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::drawElements() {
    // Dibujar borde
    screen.drawBorder();
    
    // Dibujar UI
    screen.drawUI(score, lives, level);
    
    pthread_mutex_lock(&gameMutex);
    
    // Dibujar jugador
    screen.drawSprite(spriteManager.getPlayerSprite(), player.position);
    
    // Dibujar invasores
    for (const auto& invader : invaders) {
        if (invader.isAlive) {
            screen.drawSprite(spriteManager.getInvaderSprite(invader.type), invader.position);
        }
    }
    
    // Dibujar proyectiles
    for (const auto& projectile : projectiles) {
        screen.drawSprite(spriteManager.getBulletSprite(), projectile.position);
    }
    
    pthread_mutex_unlock(&gameMutex);
}

bool Game::isRunning() const {
    return running;
}

void Game::setRunning(bool status) {
    running = status;
}

// Agregar métodos getScore() y getLevel() que faltaban
int Game::getScore() const {
    return score;
}

int Game::getLevel() const {
    return level;
}

void Game::checkPlayerCollisions() {
    pthread_mutex_lock(&gameMutex);
    
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        if (it->directionY == 1) { // Proyectil de invasor (hacia abajo)
            if (it->position.x >= player.position.x &&
                it->position.x <= player.position.x + 4 &&
                it->position.y >= player.position.y &&
                it->position.y <= player.position.y + 2) {
                
                lives--;
                player.lives = lives;
                it = projectiles.erase(it);
                break;
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::checkInvaderReachBottom() {
    pthread_mutex_lock(&gameMutex);
    
    for (const auto& invader : invaders) {
        if (invader.isAlive && invader.position.y >= screen.getHeight() - 8) {
            running = false;
            gameState = GAME_OVER;
            pthread_mutex_unlock(&gameMutex);
            return;
        }
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::checkVictoryConditions() {
    pthread_mutex_lock(&gameMutex);
    
    bool allInvadersDead = true;
    for (const auto& invader : invaders) {
        if (invader.isAlive) {
            allInvadersDead = false;
            break;
        }
    }
    
    if (allInvadersDead) {
        gameState = LEVEL_COMPLETE;
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::nextLevel() {
    pthread_mutex_lock(&gameMutex);
    
    level++;
    projectiles.clear();
    generateInvaders();
    invaderSpeed = std::max(200000, 500000 - (level * 50000));
    
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    
    gameState = PLAYING;
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::resetGame() {
    pthread_mutex_lock(&gameMutex);
    
    score = 0;
    lives = 3;
    level = 1;
    invaderSpeed = 500000;
    
    player.score = score;
    player.lives = lives;
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    
    projectiles.clear();
    generateInvaders();
    
    gameState = PLAYING;
    running = true;
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::drawGameStateMessages() {
    pthread_mutex_lock(&gameMutex);
    
    switch(gameState) {
        case GAME_OVER:
            screen.drawText("GAME OVER!", Position(screen.getWidth()/2 - 5, screen.getHeight()/2));
            screen.drawText("Press R to restart or Q to quit", Position(screen.getWidth()/2 - 15, screen.getHeight()/2 + 2));
            break;
            
        case LEVEL_COMPLETE:
            screen.drawText("LEVEL COMPLETE!", Position(screen.getWidth()/2 - 7, screen.getHeight()/2));
            screen.drawText("Press SPACE to continue", Position(screen.getWidth()/2 - 10, screen.getHeight()/2 + 2));
            break;
            
        case PAUSED:
            screen.drawText("PAUSED", Position(screen.getWidth()/2 - 3, screen.getHeight()/2));
            screen.drawText("Press P to continue", Position(screen.getWidth()/2 - 8, screen.getHeight()/2 + 2));
            break;
        
        case PLAYING:
            break;
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::handleInputImproved() {
    int key = getch();
    
    if (key != ERR) {  // ERR significa que no hay entrada disponible
        pthread_mutex_lock(&gameMutex);
        
        switch(gameState) {
            case PLAYING:
                handlePlayingInput(key);
                break;
                
            case GAME_OVER:
                if (key == 'r' || key == 'R') {
                    resetGame();
                } else if (key == 'q' || key == 'Q' || key == 27) {
                    running = false;
                }
                break;
                
            case LEVEL_COMPLETE:
                if (key == ' ') {
                    nextLevel();
                }
                break;
                
            case PAUSED:
                if (key == 'p' || key == 'P') {
                    gameState = PLAYING;
                }
                break;
        }
        
        pthread_mutex_unlock(&gameMutex);
    }
}

void Game::handlePlayingInput(int key) {
    switch(key) {
        case 'a': case 'A': case KEY_LEFT:
            if (player.position.x > 2) {
                player.position.x -= 2;
            }
            break;
            
        case 'd': case 'D': case KEY_RIGHT:
            if (player.position.x < screen.getWidth() - 7) {
                player.position.x += 2;
            }
            break;
            
        case ' ': case KEY_UP:
            if (canShoot()) {
                Projectile newBullet;
                newBullet.position.x = player.position.x + 2;
                newBullet.position.y = player.position.y - 1;
                newBullet.directionY = -1;
                projectiles.push_back(newBullet);
                lastShotTime = time(NULL);
            }
            break;
            
        case 'p': case 'P':
            gameState = PAUSED;
            break;
            
        case 'q': case 'Q': case 27:
            running = false;
            break;
    }
}

bool Game::canShoot() {
    time_t currentTime = time(NULL);
    return (currentTime - lastShotTime) >= 1;
}