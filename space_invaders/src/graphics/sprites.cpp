#include "sprites.h"

SpriteManager::SpriteManager() {
    initializeSprites();
}

void SpriteManager::initializeSprites() {
    // ===================
    // SPRITE DEL JUGADOR
    // ===================
    playerSprite.width = 5;
    playerSprite.height = 3;
    
    // Frame 1: Nave normal
    playerSprite.addFrame(
        "  ^  \n"
        " |O| \n"
        " /_\\ "
    );
    
    // Frame 2: Nave disparando (opcional para animación)
    playerSprite.addFrame(
        "  ^  \n"
        " |O| \n"
        " /_\\ "
    );
    
    // ===================
    // SPRITES DE INVASORES
    // ===================
    
    // Invasor Tipo 1 (Fila superior - 30 puntos)
    invaderSprite1.width = 7;
    invaderSprite1.height = 2;
    
    invaderSprite1.addFrame(
        " .-\"-. \n"
        " o-o-o "
    );
    
    invaderSprite1.addFrame(
        " '-\"-' \n"
        " o-o-o "
    );
    
    // Invasor Tipo 2 (Fila media - 20 puntos)
    invaderSprite2.width = 7;
    invaderSprite2.height = 2;
    
    invaderSprite2.addFrame(
        " >===< \n"
        "  \\_/  "
    );
    
    invaderSprite2.addFrame(
        " <===> \n"
        "  /_\\  "
    );
    
    // Invasor Tipo 3 (Fila inferior - 10 puntos)
    invaderSprite3.width = 5;
    invaderSprite3.height = 2;
    
    invaderSprite3.addFrame(
        " \\o/ \n"
        "  V  "
    );
    
    invaderSprite3.addFrame(
        " /o\\ \n"
        "  ^  "
    );
    
    // ===================
    // SPRITE DE PROYECTIL
    // ===================
    bulletSprite.width = 1;
    bulletSprite.height = 1;
    
    bulletSprite.addFrame("|");  // Proyectil del jugador
    bulletSprite.addFrame("*");  // Proyectil de invasores
    
    // ===================
    // SPRITE DE EXPLOSIÓN
    // ===================
    explosionSprite.width = 5;
    explosionSprite.height = 3;
    
    explosionSprite.addFrame(
        "  *  \n"
        " *** \n"
        "  *  "
    );
    
    explosionSprite.addFrame(
        " \\|/ \n"
        "--*--\n"
        " /|\\ "
    );
    
    explosionSprite.addFrame(
        "     \n"
        "  .  \n"
        "     "
    );
}

const Sprite& SpriteManager::getInvaderSprite(int type) const {
    switch(type) {
        case 1: return invaderSprite1;
        case 2: return invaderSprite2;
        case 3: return invaderSprite3;
        default: return invaderSprite1;
    }
}

void SpriteManager::updateAnimations() {
    invaderSprite1.nextFrame();
    invaderSprite2.nextFrame();
    invaderSprite3.nextFrame();
    explosionSprite.nextFrame();
}
