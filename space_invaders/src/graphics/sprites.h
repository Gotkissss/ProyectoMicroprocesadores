#ifndef SPRITES_H
#define SPRITES_H

#include <vector>
#include <string>

// Dimensiones estándar de consola
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 24;
const int GAME_AREA_WIDTH = 78;
const int GAME_AREA_HEIGHT = 22;

// Estructura para representar un sprite
struct Sprite {
    std::vector<std::string> frames;
    int width;
    int height;
    int current_frame;
    
    Sprite() : width(0), height(0), current_frame(0) {}
    
    void addFrame(const std::string& frame) {
        frames.push_back(frame);
    }
    
    std::string getCurrentFrame() const {
        if (frames.empty()) return " ";
        return frames[current_frame];
    }
    
    void nextFrame() {
        if (!frames.empty()) {
            current_frame = (current_frame + 1) % frames.size();
        }
    }
};

// Clase para manejar todos los sprites del juego
class SpriteManager {
private:
    Sprite playerSprite;
    Sprite invaderSprite1;
    Sprite invaderSprite2;
    Sprite invaderSprite3;
    Sprite bulletSprite;
    Sprite explosionSprite;
    
public:
    SpriteManager();
    void initializeSprites();
    
    // Getters para los sprites
    const Sprite& getPlayerSprite() const { return playerSprite; }
    const Sprite& getInvaderSprite(int type) const;
    const Sprite& getBulletSprite() const { return bulletSprite; }
    const Sprite& getExplosionSprite() const { return explosionSprite; }
    
    // Actualizar animaciones
    void updateAnimations();
};

#endif