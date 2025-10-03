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
    nodelay(stdscr, TRUE);
    timeout(0);
    
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
    
    // Inicializar posición del jugador
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    player.lives = lives;
    player.score = score;
    
    // Generar invasores
    generateInvaders();
}

void Game::generateInvaders() {
    invaders.clear();
    
    // Crear 5 filas de invasores
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 11; col++) {
            Invader invader;
            invader.position.x = 5 + col * 6;
            invader.position.y = 2 + row * 2;
            invader.direction = 1;
            invader.isAlive = true;
            
            // Asignar tipos según la fila
            if (row < 1) {
                invader.type = 1;
            } else if (row < 3) {
                invader.type = 2;
            } else {
                invader.type = 3;
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
        
        // Control de velocidad del juego
        usleep(30000); // 30ms = ~33 FPS
    }
}

void Game::updateGameLogic() {
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
    if (gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    if (lives <= 0 && gameState == PLAYING) {
        gameState = GAME_OVER;
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::updateInvaders() {
    // Verificar primero sin mutex
    if (!running || gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    // Verificar de nuevo con mutex
    if (!running || gameState != PLAYING) {
        pthread_mutex_unlock(&gameMutex);
        return;
    }
    
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
    if (!running || gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    // Verificar de nuevo dentro del mutex
    if (!running || gameState != PLAYING) {
        pthread_mutex_unlock(&gameMutex);
        return;
    }
    
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
    if (gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    // Verificar de nuevo dentro del mutex
    if (gameState != PLAYING) {
        pthread_mutex_unlock(&gameMutex);
        return;
    }
    
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

int Game::getScore() const {
    return score;
}

int Game::getLevel() const {
    return level;
}

void Game::checkPlayerCollisions() {
    if (gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    // Verificar de nuevo dentro del mutex
    if (gameState != PLAYING) {
        pthread_mutex_unlock(&gameMutex);
        return;
    }
    
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
    if (gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    // Verificar de nuevo dentro del mutex
    if (gameState != PLAYING) {
        pthread_mutex_unlock(&gameMutex);
        return;
    }
    
    for (const auto& invader : invaders) {
        if (invader.isAlive && invader.position.y >= screen.getHeight() - 8) {
            gameState = GAME_OVER;
            pthread_mutex_unlock(&gameMutex);
            return;
        }
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::checkVictoryConditions() {
    if (gameState != PLAYING) {
        return;
    }
    
    pthread_mutex_lock(&gameMutex);
    
    // Verificar de nuevo dentro del mutex
    if (gameState != PLAYING) {
        pthread_mutex_unlock(&gameMutex);
        return;
    }
    
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
    
    // Reiniciar posición del jugador
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    
    // Bonus de vida cada 3 niveles (opcional)
    if (level % 3 == 0 && lives < 5) {
        lives++;
        player.lives = lives;
    }
    
    // Incrementar dificultad: velocidad de invasores aumenta
    invaderSpeed = std::max(200000, 500000 - (level * 50000));
    
    // Cambiar estado a PLAYING
    gameState = PLAYING;
    
    pthread_mutex_unlock(&gameMutex);
    
    // Pequeña pausa antes de generar los invasores
    usleep(100000);
    
    // Generar invasores con mutex
    pthread_mutex_lock(&gameMutex);
    generateInvaders();
    pthread_mutex_unlock(&gameMutex);
}

void Game::resetGame() {
    pthread_mutex_lock(&gameMutex);
    
    score = 0;
    lives = 3;
    level = 1;
    invaderSpeed = 500000;
    lastShotTime = 0;
    
    player.score = score;
    player.lives = lives;
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    
    projectiles.clear();
    
    // Cambiar estado a PLAYING primero
    gameState = PLAYING;
    
    pthread_mutex_unlock(&gameMutex);
    
    // Pequeña pausa antes de generar los invasores
    usleep(100000);
    
    // Generar invasores con mutex
    pthread_mutex_lock(&gameMutex);
    generateInvaders();
    pthread_mutex_unlock(&gameMutex);
}

void Game::drawGameStateMessages() {
    pthread_mutex_lock(&gameMutex);
    
    switch(gameState) {
        case GAME_OVER: {
            // Crear un cuadro para el mensaje de Game Over
            int centerX = screen.getWidth() / 2;
            int centerY = screen.getHeight() / 2;
            
            // Dibujar cuadro decorativo
            for (int i = -25; i <= 25; i++) {
                screen.setPixel(centerX + i, centerY - 5, '=');
                screen.setPixel(centerX + i, centerY + 6, '=');
            }
            for (int i = -4; i <= 5; i++) {
                screen.setPixel(centerX - 25, centerY + i, '|');
                screen.setPixel(centerX + 25, centerY + i, '|');
            }
            
            // Esquinas
            screen.setPixel(centerX - 25, centerY - 5, '+');
            screen.setPixel(centerX + 25, centerY - 5, '+');
            screen.setPixel(centerX - 25, centerY + 6, '+');
            screen.setPixel(centerX + 25, centerY + 6, '+');
            
            // Mensajes
            screen.drawText("===========================", Position(centerX - 13, centerY - 4));
            screen.drawText("     GAME OVER!!!         ", Position(centerX - 13, centerY - 3));
            screen.drawText("===========================", Position(centerX - 13, centerY - 2));
            
            std::string scoreText = "Puntaje Final: " + std::to_string(score);
            screen.drawText(scoreText, Position(centerX - scoreText.length()/2, centerY));
            
            std::string levelText = "Nivel Alcanzado: " + std::to_string(level);
            screen.drawText(levelText, Position(centerX - levelText.length()/2, centerY + 1));
            
            screen.drawText("=====================================", Position(centerX - 18, centerY + 3));
            screen.drawText("  Presiona R para REINICIAR        ", Position(centerX - 18, centerY + 4));
            screen.drawText("  Presiona Q para SALIR al menu    ", Position(centerX - 18, centerY + 5));
            screen.drawText("=====================================", Position(centerX - 18, centerY + 6));
            break;
        }
            
        case LEVEL_COMPLETE: {
            int centerX = screen.getWidth() / 2;
            int centerY = screen.getHeight() / 2;
            
            // Cuadro decorativo
            for (int i = -22; i <= 22; i++) {
                screen.setPixel(centerX + i, centerY - 4, '=');
                screen.setPixel(centerX + i, centerY + 5, '=');
            }
            for (int i = -3; i <= 4; i++) {
                screen.setPixel(centerX - 22, centerY + i, '|');
                screen.setPixel(centerX + 22, centerY + i, '|');
            }
            
            // Esquinas
            screen.setPixel(centerX - 22, centerY - 4, '+');
            screen.setPixel(centerX + 22, centerY - 4, '+');
            screen.setPixel(centerX - 22, centerY + 5, '+');
            screen.setPixel(centerX + 22, centerY + 5, '+');
            
            screen.drawText("============================", Position(centerX - 14, centerY - 3));
            screen.drawText("  NIVEL COMPLETADO!        ", Position(centerX - 14, centerY - 2));
            screen.drawText("============================", Position(centerX - 14, centerY - 1));
            
            std::string nextLevelText = "Siguiente Nivel: " + std::to_string(level + 1);
            screen.drawText(nextLevelText, Position(centerX - nextLevelText.length()/2, centerY + 1));
            
            std::string currentScore = "Puntaje Actual: " + std::to_string(score);
            screen.drawText(currentScore, Position(centerX - currentScore.length()/2, centerY + 2));
            
            screen.drawText("Presiona ESPACIO para CONTINUAR", Position(centerX - 15, centerY + 4));
            screen.drawText("Presiona Q para SALIR al menu", Position(centerX - 14, centerY + 5));
            break;
        }
            
        case PAUSED: {
            int centerX = screen.getWidth() / 2;
            int centerY = screen.getHeight() / 2;
            
            screen.drawText("====================", Position(centerX - 10, centerY - 1));
            screen.drawText("  JUEGO PAUSADO     ", Position(centerX - 10, centerY));
            screen.drawText("====================", Position(centerX - 10, centerY + 1));
            screen.drawText("Presiona P para continuar", Position(centerX - 12, centerY + 3));
            break;
        }
        
        case PLAYING:
            break;
    }
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::handleInputImproved() {
    int key = getch();
    
    if (key != ERR) {
        pthread_mutex_lock(&gameMutex);
        
        GameState currentGameState = gameState;
        
        pthread_mutex_unlock(&gameMutex);
        
        // Manejar input según el estado (sin mutex para evitar deadlocks)
        switch(currentGameState) {
            case PLAYING:
                pthread_mutex_lock(&gameMutex);
                handlePlayingInput(key);
                pthread_mutex_unlock(&gameMutex);
                break;
                
            case GAME_OVER:
                if (key == 'r' || key == 'R') {
                    resetGame();
                } else if (key == 'm' || key == 'M' || key == 'q' || key == 'Q' || key == 27) {
                    // Volver al menú principal (no salir del programa)
                    pthread_mutex_lock(&gameMutex);
                    running = false;
                    pthread_mutex_unlock(&gameMutex);
                }
                break;
                
            case LEVEL_COMPLETE:
                if (key == ' ') {
                    nextLevel();
                } else if (key == 'm' || key == 'M' || key == 'q' || key == 'Q' || key == 27) {
                    // Volver al menú principal (no salir del programa)
                    pthread_mutex_lock(&gameMutex);
                    running = false;
                    pthread_mutex_unlock(&gameMutex);
                }
                break;
                
            case PAUSED:
                if (key == 'p' || key == 'P') {
                    pthread_mutex_lock(&gameMutex);
                    gameState = PLAYING;
                    pthread_mutex_unlock(&gameMutex);
                } else if (key == 'm' || key == 'M' || key == 'q' || key == 'Q' || key == 27) {
                    // Volver al menú principal (no salir del programa)
                    pthread_mutex_lock(&gameMutex);
                    running = false;
                    pthread_mutex_unlock(&gameMutex);
                }
                break;
        }
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
            
        case 'm': case 'M': case 'q': case 'Q': case 27:
            // Pausar primero, luego permitir salir
            gameState = PAUSED;
            break;
    }
}

bool Game::canShoot() {
    time_t currentTime = time(NULL);
    return (currentTime - lastShotTime) >= 1;
}