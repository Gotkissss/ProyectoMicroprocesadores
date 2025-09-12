#ifndef ENTITIES_H
#define ENTITIES_H

#include "graphics/screen.h"

// Estructura para la nave del jugador
struct Player {
    Position position;
    int lives;
    int score;
};

// Estructura para los invasores
struct Invader {
    Position position;
    int type; // Para diferenciar entre los 3 tipos de invasores
    int direction; // Para controlar el movimiento horizontal
    bool isAlive;
};

// Estructura para los proyectiles
struct Projectile {
    Position position;
    int directionY; // 1 para abajo (invasor), -1 para arriba (jugador)
};

#endif