#ifndef SPRITES_H
#define SPRITES_H

#include <vector>
#include <string>

// Constantes de pantalla
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 24;

// Estructura para los sprites
struct Sprite {
    std::vector<std::string> frames;
    int width, height;
    int currentFrame;
    
    Sprite() : width(0), height(0), currentFrame(0) {}
    
    std::string getCurrentFrame() const {
        if (frames.empty()) return "";
        return frames[currentFrame % frames.size()];
    }
    
    void nextFrame() {
        if (!frames.empty()) {
            currentFrame = (currentFrame + 1) % frames.size();
        }
    }
};

class SpriteManager {
private:
    Sprite playerSprite;
    std::vector<Sprite> invaderSprites;  // 3 tipos
    Sprite bulletSprite;
    
public:
    SpriteManager();
    
    // Getters para los sprites
    const Sprite& getPlayerSprite() const { return playerSprite; }
    const Sprite& getInvaderSprite(int type) const { 
        int index = std::max(0, std::min(type - 1, (int)invaderSprites.size() - 1));
        return invaderSprites[index]; 
    }
    const Sprite& getBulletSprite() const { return bulletSprite; }
    
    // Funciones de animación
    void updateAnimations();
};

#endif