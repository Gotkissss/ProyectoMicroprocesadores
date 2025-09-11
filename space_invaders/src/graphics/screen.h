#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include <string>
#include "sprites.h"

// Posición en la pantalla
struct Position {
    int x, y;
    Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

// Clase para manejar la pantalla del juego
class Screen {
private:
    std::vector<std::vector<char>> buffer;
    int width, height;
    SpriteManager& spriteManager;
    
public:
    Screen(SpriteManager& sm);
    
    // Funciones básicas de pantalla
    void clear();
    void draw();
    void clearConsole();
    
    // Funciones de renderizado
    void drawSprite(const Sprite& sprite, Position pos, int frame = 0);
    void drawText(const std::string& text, Position pos);
    void drawBorder();
    void drawUI(int score, int lives, int level);
    
    // Funciones de utilidad
    bool isValidPosition(Position pos, int width, int height);
    void setPixel(int x, int y, char c);
    char getPixel(int x, int y);
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif