#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <string>
#include <vector>
#include <fstream>
#include <ncurses.h>

struct HighScore {
    std::string playerName;
    int score;
    int level;
    std::string date;
};

enum MenuState {
    MAIN_MENU,
    INSTRUCTIONS,
    HIGH_SCORES,
    ENTER_NAME,
    GAME_RUNNING,
    EXIT_PROGRAM
};

class MenuSystem {
private:
    MenuState currentState;
    std::vector<HighScore> highScores;
    std::string highScoreFile;
    int selectedOption;
    int maxOptions;
    std::string playerName;
    
public:
    MenuSystem();
    ~MenuSystem();
    
    // Funciones principales de menú
    MenuState run();
    void drawMainMenu();
    void drawInstructions();
    void drawHighScores();
    void drawEnterName();
    
    // Manejo de entrada
    void handleMainMenuInput(int key);
    void handleInstructionsInput(int key);
    void handleHighScoresInput(int key);
    void handleEnterNameInput(int key);
    
    // Sistema de puntajes
    void loadHighScores();
    void saveHighScores();
    void addHighScore(const std::string& name, int score, int level);
    void sortHighScores();
    
    // Utilidades de dibujo
    void drawBorder();
    void drawCenteredText(int y, const std::string& text);
    void drawTitle();
    void clearScreen();
    
    // Getters
    MenuState getCurrentState() const { return currentState; }
    std::string getPlayerName() const { return playerName; }
};

#endif