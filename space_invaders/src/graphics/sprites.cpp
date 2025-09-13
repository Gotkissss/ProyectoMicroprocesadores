#include "sprites.h"

SpriteManager::SpriteManager() {
    // Inicializar sprites del jugador
    playerSprite.width = 5;
    playerSprite.height = 3;
    playerSprite.frames.push_back(
        "  ^  \n"
        " |O| \n"
        " /_\\ "
    );
    
    // Inicializar sprites de invasores (3 tipos)
    invaderSprites.resize(3);
    
    // Invasor Tipo 1 (Fila superior - 30 puntos)
    invaderSprites[0].width = 7;
    invaderSprites[0].height = 2;
    invaderSprites[0].frames.push_back(
        " .-\"-. \n"
        " o-o-o "
    );
    invaderSprites[0].frames.push_back(
        " '-\"-' \n"
        " o-o-o "
    );
    
    // Invasor Tipo 2 (Fila media - 20 puntos)
    invaderSprites[1].width = 7;
    invaderSprites[1].height = 2;
    invaderSprites[1].frames.push_back(
        " >===< \n"
        "  \\_/  "
    );
    invaderSprites[1].frames.push_back(
        " <===> \n"
        "  /_\\  "
    );
    
    // Invasor Tipo 3 (Fila inferior - 10 puntos)
    invaderSprites[2].width = 5;
    invaderSprites[2].height = 2;
    invaderSprites[2].frames.push_back(
        " \\o/ \n"
        "  V  "
    );
    invaderSprites[2].frames.push_back(
        " /o\\ \n"
        "  ^  "
    );
    
    // Sprite de proyectil
    bulletSprite.width = 1;
    bulletSprite.height = 1;
    bulletSprite.frames.push_back("|");  // Proyectil del jugador
    bulletSprite.frames.push_back("*");  // Proyectil de invasores
}

void SpriteManager::updateAnimations() {
    for (auto& invader : invaderSprites) {
        invader.nextFrame();
    }
}