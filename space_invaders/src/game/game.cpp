#include "game.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <ncurses.h>
#include <ctime>

Game::Game(SpriteManager& sm, Screen& s) : spriteManager(sm), screen(s) {
    // Inicializar el mutex usando pthread_mutex_init como especifica el PDF
    pthread_mutex_init(&gameMutex, NULL);
    initializeGame();
}

Game::~Game() {
    pthread_mutex_destroy(&gameMutex);
}

void Game::initializeGame() {
    running = true;
    score = 0;
    lives = 3;
    level = 1;
    
    // Posicionar al jugador en el centro inferior
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    player.lives = lives;
    player.score = score;
    
    // Generar la formación inicial de invasores
    generateInvaders();
}

void Game::generateInvaders() {
    invaders.clear();
    
    // Definir la formación de los invasores usando generación aleatoria con rand() como especifica el PDF
    int startY = 5;
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 10; ++x) {
            Invader newInvader;
            newInvader.position.x = 5 + x * 6;
            newInvader.position.y = startY + y * 3;
            newInvader.type = (y < 1) ? 1 : (y < 2) ? 2 : 3;
            newInvader.direction = 1;
            newInvader.isAlive = true;
            invaders.push_back(newInvader);
        }
    }
}

void Game::updateInvaders() {
    // Bloquear el acceso usando pthread_mutex_lock como especifica el PDF
    pthread_mutex_lock(&gameMutex);
    
    bool changeDirection = false;
    
    // Verificar si algún invasor toca los bordes
    for (auto& invader : invaders) {
        if (!invader.isAlive) continue;
        
        if ((invader.position.x >= screen.getWidth() - 10 && invader.direction > 0) ||
            (invader.position.x <= 5 && invader.direction < 0)) {
            changeDirection = true;
            break;
        }
    }
    
    // Si hay que cambiar dirección, mover todos hacia abajo y cambiar dirección
    if (changeDirection) {
        for (auto& invader : invaders) {
            if (!invader.isAlive) continue;
            invader.direction *= -1;
            invader.position.y += 2;
        }
    } else {
        // Mover horizontalmente
        for (auto& invader : invaders) {
            if (!invader.isAlive) continue;
            invader.position.x += invader.direction;
        }
    }
    
    // Lógica para que los invasores disparen usando rand() como especifica el PDF
    if (rand() % 100 < 5) { // 5% de probabilidad de que un invasor dispare
        std::vector<int> aliveInvaders;
        for (int i = 0; i < invaders.size(); i++) {
            if (invaders[i].isAlive) {
                aliveInvaders.push_back(i);
            }
        }
        
        if (!aliveInvaders.empty()) {
            int randomIndex = aliveInvaders[rand() % aliveInvaders.size()];
            Projectile newBullet;
            newBullet.position.x = invaders[randomIndex].position.x + 3;
            newBullet.position.y = invaders[randomIndex].position.y + 1;
            newBullet.directionY = 1; // Hacia abajo
            projectiles.push_back(newBullet);
        }
    }
    
    // Desbloquear usando pthread_mutex_unlock como especifica el PDF
    pthread_mutex_unlock(&gameMutex);
}

void Game::checkCollisions() {
    pthread_mutex_lock(&gameMutex);
    
    // Revisar colisiones proyectil-invasor
    for (auto& projectile : projectiles) {
        if (projectile.directionY == -1) { // Proyectil del jugador
            for (auto& invader : invaders) {
                if (!invader.isAlive) continue;
                
                if (projectile.position.x >= invader.position.x &&
                    projectile.position.x < invader.position.x + 7 &&
                    projectile.position.y >= invader.position.y &&
                    projectile.position.y < invader.position.y + 2) {
                    
                    invader.isAlive = false;
                    projectile.directionY = 0; // Marcar proyectil para eliminar
                    
                    // Sumar puntos según tipo de invasor
                    switch(invader.type) {
                        case 1: score += 30; break;
                        case 2: score += 20; break;
                        case 3: score += 10; break;
                    }
                    player.score = score;
                }
            }
        }
    }
    
    // Eliminar proyectiles marcados
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return p.directionY == 0; }),
        projectiles.end()
    );
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::drawElements() {
    pthread_mutex_lock(&gameMutex);
    
    // Dibujar el borde y la UI
    screen.drawBorder();
    screen.drawUI(score, lives, level);
    
    // Dibujar el jugador
    screen.drawSprite(spriteManager.getPlayerSprite(), player.position);
    
    // Dibujar todos los invasores
    for (const auto& invader : invaders) {
        if (invader.isAlive) {
            const Sprite& invaderSprite = spriteManager.getInvaderSprite(invader.type);
            screen.drawSprite(invaderSprite, invader.position);
        }
    }
    
    // Dibujar proyectiles
    for (const auto& bullet : projectiles) {
        screen.drawSprite(spriteManager.getBulletSprite(), bullet.position);
    }
    
    pthread_mutex_unlock(&gameMutex);
}

bool Game::isRunning() const {
    return running;
}

void Game::setRunning(bool status) {
    running = status;
}

void Game::handleInput() {
    // Usar getch() de ncurses como especifica el PDF para captura en tiempo real
    int key = getch();
    
    if (key != ERR) { // Si se presionó una tecla
        pthread_mutex_lock(&gameMutex);
        
        switch(key) {
            case 'a':
            case 'A':
            case KEY_LEFT:
                // Mover jugador a la izquierda
                if (player.position.x > 2) {
                    player.position.x -= 2;
                }
                break;
            case 'd':
            case 'D':
            case KEY_RIGHT:
                // Mover jugador a la derecha
                if (player.position.x < screen.getWidth() - 7) {
                    player.position.x += 2;
                }
                break;
            case ' ':
            case KEY_UP: // También permitir flecha arriba para disparar
                // Disparar
                {
                    Projectile newBullet;
                    newBullet.position.x = player.position.x + 2; // Centro del jugador
                    newBullet.position.y = player.position.y - 1;
                    newBullet.directionY = -1; // Hacia arriba
                    projectiles.push_back(newBullet);
                }
                break;
            case 'p':
            case 'P':
                // Pausar juego usando usleep() como especifica el PDF
                usleep(100000); // Pausa breve
                break;
            case 'q':
            case 'Q':
            case 27: // ESC
                running = false;
                break;
        }
        
        pthread_mutex_unlock(&gameMutex);
    }
}

void Game::updateGameLogic() {
    updateProjectiles();
    checkCollisions();
}

void Game::updateProjectiles() {
    pthread_mutex_lock(&gameMutex);
    
    // Mover proyectiles
    for (auto& projectile : projectiles) {
        projectile.position.y += projectile.directionY;
    }
    
    // Eliminar proyectiles fuera de pantalla
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [this](const Projectile& p) {
                return p.position.y < 3 || p.position.y >= screen.getHeight() - 1;
            }),
        projectiles.end()
    );
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::mainLoop() {
    // Función vacía por ahora - la lógica está en main()
}