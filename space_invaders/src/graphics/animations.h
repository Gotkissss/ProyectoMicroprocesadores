#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "sprites.h"
#include "screen.h"
#include <chrono>

class AnimationManager {
private:
    std::chrono::steady_clock::time_point lastUpdate;
    int animationSpeed; // milisegundos entre frames
    
public:
    AnimationManager(int speed = 500);
    
    bool shouldUpdate();
    void updateInvaderAnimations(SpriteManager& sm);
    void playExplosion(Screen& screen, Position pos, SpriteManager& sm);
};

#endif