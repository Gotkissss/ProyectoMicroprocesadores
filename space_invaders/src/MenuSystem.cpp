#include "MenuSystem.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>

MenuSystem::MenuSystem() {
    currentState = MAIN_MENU;
    selectedOption = 0;
    maxOptions = 4; 
    highScoreFile = "high_scores.txt";
    playerName = "";
    
    // Inicializar ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Cargar puntajes
    loadHighScores();
}

MenuSystem::~MenuSystem() {
    endwin();
}

MenuState MenuSystem::run() {
    int key;
    
    while (currentState != GAME_RUNNING && currentState != EXIT_PROGRAM) {
        clearScreen();
        
        switch (currentState) {
            case MAIN_MENU:
                drawMainMenu();
                break;
            case INSTRUCTIONS:
                drawInstructions();
                break;
            case HIGH_SCORES:
                drawHighScores();
                break;
            case ENTER_NAME:
                drawEnterName();
                break;
            default:
                break;
        }
        
        refresh();
        key = getch();
        
        // Manejar entrada según el estado actual
        switch (currentState) {
            case MAIN_MENU:
                handleMainMenuInput(key);
                break;
            case INSTRUCTIONS:
                handleInstructionsInput(key);
                break;
            case HIGH_SCORES:
                handleHighScoresInput(key);
                break;
            case ENTER_NAME:
                handleEnterNameInput(key);
                break;
            default:
                break;
        }
    }
    
    return currentState;
}

void MenuSystem::drawMainMenu() {
    drawBorder();
    drawTitle();
    
    int startY = 10;
    const char* options[] = {
        "1. Iniciar Juego",
        "2. Instrucciones",
        "3. Puntajes Altos",
        "4. Salir"
    };
    
    // Dibujar opciones del menú
    for (int i = 0; i < 4; i++) {
        int y = startY + i * 2;
        if (i == selectedOption) {
            attron(A_REVERSE);
            drawCenteredText(y, std::string(">>> ") + options[i] + " <<<");
            attroff(A_REVERSE);
        } else {
            drawCenteredText(y, options[i]);
        }
    }
    
    // Instrucciones de navegación
    drawCenteredText(20, "Usa las flechas ARRIBA/ABAJO para navegar");
    drawCenteredText(21, "Presiona ENTER para seleccionar");
}

void MenuSystem::drawInstructions() {
    drawBorder();
    
    // Título
    attron(A_BOLD);
    drawCenteredText(3, "=== SPACE INVADERS - INSTRUCCIONES ===");
    attroff(A_BOLD);
    
    // Objetivo del juego
    drawCenteredText(6, "OBJETIVO:");
    drawCenteredText(7, "Destruye todos los invasores espaciales antes de que lleguen al suelo");
    drawCenteredText(8, "o te eliminen. Sobrevive el mayor tiempo posible y obten la mayor puntuacion.");
    
    // Controles
    drawCenteredText(10, "CONTROLES:");
    drawCenteredText(11, "A / Flecha Izquierda  - Mover nave a la izquierda");
    drawCenteredText(12, "D / Flecha Derecha    - Mover nave a la derecha");
    drawCenteredText(13, "ESPACIO               - Disparar");
    drawCenteredText(14, "P                     - Pausar juego");
    drawCenteredText(15, "Q / ESC               - Salir del juego");
    
    // Elementos del juego
    drawCenteredText(17, "ELEMENTOS DEL JUEGO:");
    drawCenteredText(18, "[^^^]  - Tu nave espacial (3 vidas)");
    drawCenteredText(19, "<@@@>  - Invasores tipo 1 (30 puntos)");
    drawCenteredText(20, "<%#%>  - Invasores tipo 2 (20 puntos)");
    drawCenteredText(21, "<*+*>  - Invasores tipo 3 (10 puntos)");
    drawCenteredText(22, "  |    - Proyectiles");
    
    // Instrucciones de salida
    drawCenteredText(24, "Presiona cualquier tecla para volver al menu principal");
}

void MenuSystem::drawHighScores() {
    drawBorder();
    
    // Título
    attron(A_BOLD);
    drawCenteredText(3, "=== TABLA DE PUNTAJES ALTOS ===");
    attroff(A_BOLD);
    
    if (highScores.empty()) {
        drawCenteredText(12, "No hay puntajes registrados aun");
        drawCenteredText(13, "Se el primero en jugar!");
    } else {
        // Encabezados
        drawCenteredText(6, "POS  JUGADOR         PUNTOS  NIVEL    FECHA");
        drawCenteredText(7, "===  ==============  ======  =====  ==========");
        
        // Mostrar hasta 10 mejores puntajes
        int maxDisplay = std::min(10, (int)highScores.size());
        for (int i = 0; i < maxDisplay; i++) {
            std::stringstream ss;
            ss << std::setw(3) << (i + 1) << "  "
               << std::setw(14) << std::left << highScores[i].playerName.substr(0, 14) << "  "
               << std::setw(6) << std::right << highScores[i].score << "  "
               << std::setw(5) << highScores[i].level << "  "
               << highScores[i].date;
            
            drawCenteredText(9 + i, ss.str());
        }
    }
    
    drawCenteredText(22, "Presiona cualquier tecla para volver al menu principal");
}

void MenuSystem::drawEnterName() {
    drawBorder();
    
    attron(A_BOLD);
    drawCenteredText(8, "=== INGRESA TU NOMBRE ===");
    attroff(A_BOLD);
    
    drawCenteredText(12, "Escribe tu nombre (maximo 12 caracteres):");
    
    // Mostrar nombre actual
    std::string displayName = playerName;
    if (displayName.length() < 12) {
        displayName += "_";
    }
    
    attron(A_REVERSE);
    drawCenteredText(14, "[" + displayName + "]");
    attroff(A_REVERSE);
    
    drawCenteredText(18, "Presiona ENTER para continuar");
    drawCenteredText(19, "Presiona ESC para cancelar");
}

void MenuSystem::handleMainMenuInput(int key) {
    switch (key) {
        case KEY_UP:
            selectedOption = (selectedOption - 1 + 4) % 4;
            break;
        case KEY_DOWN:
            selectedOption = (selectedOption + 1) % 4;
            break;
        case 10: 
        case 13: 
            switch (selectedOption) {
                case 0: // Iniciar Juego
                    currentState = ENTER_NAME;
                    break;
                case 1: // Instrucciones
                    currentState = INSTRUCTIONS;
                    break;
                case 2: // Puntajes Altos
                    currentState = HIGH_SCORES;
                    break;
                case 3: // Salir
                    currentState = EXIT_PROGRAM;
                    break;
            }
            break;
        case 'q':
        case 'Q':
        case 27:
            currentState = EXIT_PROGRAM;
            break;
    }
}

void MenuSystem::handleInstructionsInput(int key) {
    currentState = MAIN_MENU;
    selectedOption = 0;
}

void MenuSystem::handleHighScoresInput(int key) {
    currentState = MAIN_MENU;
    selectedOption = 0;
}

void MenuSystem::handleEnterNameInput(int key) {
    switch (key) {
        case 10: 
        case 13: 
            if (playerName.length() > 0) {
                currentState = GAME_RUNNING;
            }
            break;
        case 27:
            currentState = MAIN_MENU;
            selectedOption = 0;
            playerName = "";
            break;
        case KEY_BACKSPACE:
        case 127:
        case 8:
            if (playerName.length() > 0) {
                playerName.pop_back();
            }
            break;
        default:
            // Agregar caracter si es válido
            if (key >= 32 && key <= 126 && playerName.length() < 12) {
                playerName += (char)key;
            }
            break;
    }
}

void MenuSystem::loadHighScores() {
    highScores.clear();
    std::ifstream file(highScoreFile);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, scoreStr, levelStr, date;
            
            if (std::getline(ss, name, '|') &&
                std::getline(ss, scoreStr, '|') &&
                std::getline(ss, levelStr, '|') &&
                std::getline(ss, date)) {
                
                HighScore score;
                score.playerName = name;
                score.score = std::stoi(scoreStr);
                score.level = std::stoi(levelStr);
                score.date = date;
                
                highScores.push_back(score);
            }
        }
        file.close();
        sortHighScores();
    }
}

void MenuSystem::saveHighScores() {
    std::ofstream file(highScoreFile);
    
    if (file.is_open()) {
        for (const auto& score : highScores) {
            file << score.playerName << "|" 
                 << score.score << "|"
                 << score.level << "|"
                 << score.date << std::endl;
        }
        file.close();
    }
}

void MenuSystem::addHighScore(const std::string& name, int score, int level) {
    // Obtener fecha actual
    time_t now = time(0);
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", localtime(&now));
    
    HighScore newScore;
    newScore.playerName = name;
    newScore.score = score;
    newScore.level = level;
    newScore.date = std::string(dateStr);
    
    highScores.push_back(newScore);
    sortHighScores();
    
    // Mantener solo los 50 mejores
    if (highScores.size() > 50) {
        highScores.resize(50);
    }
    
    saveHighScores();
}

void MenuSystem::sortHighScores() {
    std::sort(highScores.begin(), highScores.end(), 
              [](const HighScore& a, const HighScore& b) {
                  if (a.score == b.score) {
                      return a.level > b.level;
                  }
                  return a.score > b.score;
              });
}

void MenuSystem::drawBorder() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    // Dibujar bordes
    for (int i = 0; i < maxX; i++) {
        mvaddch(0, i, '=');
        mvaddch(maxY - 1, i, '=');
    }
    
    for (int i = 0; i < maxY; i++) {
        mvaddch(i, 0, '|');
        mvaddch(i, maxX - 1, '|');
    }
    
    // Esquinas
    mvaddch(0, 0, '+');
    mvaddch(0, maxX - 1, '+');
    mvaddch(maxY - 1, 0, '+');
    mvaddch(maxY - 1, maxX - 1, '+');
}

void MenuSystem::drawCenteredText(int y, const std::string& text) {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    int x = (maxX - text.length()) / 2;
    if (x < 1) x = 1;
    if (y < maxY - 1) {
        mvprintw(y, x, "%s", text.c_str());
    }
}

void MenuSystem::drawTitle() {
    attron(A_BOLD);
    drawCenteredText(2, "   _____ _____  ___  _____ _____   _____ _   _ _   _  ___  ____  _____ _____   _____ ");
    drawCenteredText(3, "  /  ___/  _  |/ _ \\|  ___/  ___| |_   _| \\ | | | | |/ _ \\|  _ \\| ____|  _  \\ /  ___/");
    drawCenteredText(4, "  \\ `--|. (_| / /_\\ \\ |__ \\ `--.    | | |  \\| | | | / /_\\ \\ | | |  __|| |_| | \\ `--. ");
    drawCenteredText(5, "   `--. \\  _  ||  _  |  __| `--. \\   | | | . ` | | | |  _  | | | | |___ |    /  `--. \\");
    drawCenteredText(6, "  /\\__/ / | | || | | | |___/\\__/ /  _| |_| |\\  \\ \\_/ / | | | |/ /|  ___|| |\\ \\ /\\__/ /");
    drawCenteredText(7, "  \\____/\\_| |_|\\_| |_\\____/\\____/   \\___/\\_| \\_/\\___/\\_| |_|___/ |_____\\_| \\_|\\____/ ");
    attroff(A_BOLD);
}

void MenuSystem::clearScreen() {
    clear();
}