#include "screen.h"
#include <iostream>
#include <cstdlib>
#include <ncurses.h>

Screen::Screen(SpriteManager& sm) : spriteManager(sm) {
    width = SCREEN_WIDTH;
    height = SCREEN_HEIGHT;
    buffer.resize(height, std::vector<char>(width, ' '));
}

void Screen::clear() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            buffer[y][x] = ' ';
        }
    }
}

void Screen::clearConsole() {
    // Con ncurses no necesitamos system calls
    ::clear(); // Llamada a la función clear() de ncurses
}

void Screen::draw() {
    // Limpiar pantalla de ncurses
    ::clear();
    
    // Dibujar el buffer usando ncurses
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            mvaddch(y, x, buffer[y][x]);
        }
    }
    
    // Refrescar pantalla
    refresh();
}

void Screen::drawSprite(const Sprite& sprite, Position pos, int frame) {
    if (!isValidPosition(pos, sprite.width, sprite.height)) return;
    
    std::string spriteFrame = sprite.getCurrentFrame();
    
    // Dividir el frame en líneas
    std::vector<std::string> lines;
    std::string currentLine = "";
    
    for (char c : spriteFrame) {
        if (c == '\n') {
            lines.push_back(currentLine);
            currentLine = "";
        } else {
            currentLine += c;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    
    // Dibujar cada línea
    for (int i = 0; i < lines.size() && i < sprite.height; i++) {
        for (int j = 0; j < lines[i].length() && j < sprite.width; j++) {
            if (pos.y + i >= 0 && pos.y + i < height &&
                pos.x + j >= 0 && pos.x + j < width) {
                buffer[pos.y + i][pos.x + j] = lines[i][j];
            }
        }
    }
}

void Screen::drawText(const std::string& text, Position pos) {
    if (pos.y < 0 || pos.y >= height) return;
    
    for (int i = 0; i < text.length(); i++) {
        if (pos.x + i >= 0 && pos.x + i < width) {
            buffer[pos.y][pos.x + i] = text[i];
        }
    }
}

void Screen::drawBorder() {
    // Bordes horizontales
    for (int x = 0; x < width; x++) {
        buffer[0][x] = '=';
        buffer[height - 1][x] = '=';
    }
    
    // Bordes verticales
    for (int y = 0; y < height; y++) {
        buffer[y][0] = '|';
        buffer[y][width - 1] = '|';
    }
    
    // Esquinas
    buffer[0][0] = '+';
    buffer[0][width - 1] = '+';
    buffer[height - 1][0] = '+';
    buffer[height - 1][width - 1] = '+';
}

void Screen::drawUI(int score, int lives, int level) {
    // Información en la parte superior
    std::string scoreText = "SCORE: " + std::to_string(score);
    std::string livesText = "LIVES: " + std::to_string(lives);
    std::string levelText = "LEVEL: " + std::to_string(level);
    
    drawText(scoreText, Position(2, 1));
    drawText(livesText, Position(width - 12, 1));
    drawText(levelText, Position(width / 2 - 4, 1));
    
    // Línea separadora
    for (int x = 1; x < width - 1; x++) {
        buffer[2][x] = '-';
    }
}

bool Screen::isValidPosition(Position pos, int w, int h) {
    return pos.x >= 1 && pos.y >= 3 && 
           pos.x + w <= width - 1 && pos.y + h <= height - 1;
}

void Screen::setPixel(int x, int y, char c) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        buffer[y][x] = c;
    }
}

char Screen::getPixel(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return buffer[y][x];
    }
    return ' ';
}