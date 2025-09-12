#include "game.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

Game::Game(SpriteManager& sm, Screen& s) : spriteManager(sm), screen(s) {
    // Inicializar el mutex
    pthread_mutex_init(&gameMutex, NULL);
    initializeGame();
}

void Game::initializeGame() {
    running = true;
    score = 0;
    lives = 3;
    level = 1;
    
    // Posicionar al jugador en el centro inferior
    player.position.x = screen.getWidth() / 2;
    player.position.y = screen.getHeight() - 5;
    
    // Generar la formación inicial de invasores
    generateInvaders();
}

void Game::generateInvaders() {
    // Definir la formación de los invasores (Ejemplo)
    int startY = 5;
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 10; ++x) {
            Invader newInvader;
            newInvader.position.x = 5 + x * 6;
            newInvader.position.y = startY + y * 3;
            newInvader.type = (y < 1) ? 1 : (y < 2) ? 2 : 3;
            invaders.push_back(newInvader);
        }
    }
}

void Game::updateInvaders() {
    // Bloquear el acceso al vector de invasores para evitar condiciones de carrera
    pthread_mutex_lock(&gameMutex);
    
    // Lógica de movimiento de los invasores
    for (auto& invader : invaders) {
        // Mover horizontalmente
        invader.position.x += invader.direction;
        
        // Lógica de cambio de dirección y descenso
        if (invader.position.x >= screen.getWidth() - 10) {
            invader.direction = -1;
            invader.position.y += 1;
        } else if (invader.position.x <= 5) {
            invader.direction = 1;
            invader.position.y += 1;
        }
    }
    
    // Lógica para que los invasores disparen aleatoriamente
    if (rand() % 100 < 5) { // 5% de probabilidad de que un invasor dispare
        int invaderIndex = rand() % invaders.size();
        Projectile newBullet;
        newBullet.position.x = invaders[invaderIndex].position.x;
        newBullet.position.y = invaders[invaderIndex].position.y + 1;
        newBullet.directionY = 1; // Hacia abajo
        projectiles.push_back(newBullet);
    }
    
    // Desbloquear el mutex
    pthread_mutex_unlock(&gameMutex);
}

void Game::checkCollisions() {
    // Bloquear el acceso para evitar condiciones de carrera
    pthread_mutex_lock(&gameMutex);
    
    // Lógica de detección de colisiones
    // (proyectiles vs invasores, proyectiles vs jugador)
    
    pthread_mutex_unlock(&gameMutex);
}

void Game::drawElements() {
    // Bloquear el acceso al dibujar
    pthread_mutex_lock(&gameMutex);
    
    // Dibujar el borde y la UI
    screen.drawBorder();
    screen.drawUI(score, lives, level);
    
    // Dibujar el jugador
    screen.drawSprite(spriteManager.getPlayerSprite(), player.position);
    
    // Dibujar todos los invasores y proyectiles
    for (const auto& invader : invaders) {
        const Sprite& invaderSprite = spriteManager.getInvaderSprite(invader.type);
        screen.drawSprite(invaderSprite, invader.position);
    }
    
    for (const auto& bullet : projectiles) {
        screen.drawSprite(spriteManager.getBulletSprite(), bullet.position);
    }
    
    // Desbloquear el mutex
    pthread_mutex_unlock(&gameMutex);
}
