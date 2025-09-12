#ifndef ENTITIES_H
#define ENTITIES_H

#include "../graphics/screen.h"

// Estructura para la nave del jugador
struct Player {
    Position position;
    int lives;
    int score;
    
    Player() : position(0, 0), lives(3), score(0) {}
};

// Estructura para los invasores
struct Invader {
    Position position;
    int type; // Para diferenciar entre los 3 tipos de invasores
    int direction; // Para controlar el movimiento horizontal
    bool isAlive;
    
    Invader() : position(0, 0), type(1), direction(1), isAlive(true) {}
};

// Estructura para los proyectiles
struct Projectile {
    Position position;
    int directionY; // 1 para abajo (invasor), -1 para arriba (jugador)
    
    Projectile() : position(0, 0), directionY(0) {}
};

#endif