#include "game/game.h"
#include "graphics/sprites.h"
#include "graphics/screen.h"
#include "MenuSystem.h"
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>  // Para usleep

// Variables globales para el juego
Game* gameInstance = nullptr;
pthread_t invaderThread;
bool threadRunning = false;

// Función del hilo para mover invasores
void* invaderThreadFunction(void* arg) {
    Game* game = static_cast<Game*>(arg);
    
    while (threadRunning && game->isRunning()) {
        game->updateInvaders();
        usleep(100000); // 100ms entre actualizaciones de invasores
    }
    
    return nullptr;
}

int main() {
    // Inicializar semilla aleatoria
    srand(time(nullptr));
    
    // Crear sistema de menús
    MenuSystem menu;
    
    // Ejecutar menús hasta que el usuario elija jugar o salir
    MenuState result = menu.run();
    
    if (result == EXIT_PROGRAM) {
        std::cout << "¡Gracias por jugar Space Invaders!" << std::endl;
        return 0;
    }
    
    if (result == GAME_RUNNING) {
        // El MenuSystem ya inicializó ncurses, necesitamos reiniciarlo para el juego
        endwin(); // Terminar la sesión de ncurses del menú
        
        // Verificar tamaño de terminal
        initscr();
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        endwin();
        
        if (maxY < 25 || maxX < 80) {
            std::cout << "Error: El terminal debe ser de al menos 25x80 caracteres." << std::endl;
            std::cout << "Tamaño actual: " << maxY << "x" << maxX << std::endl;
            std::cout << "Por favor ajusta el tamaño de tu terminal y vuelve a ejecutar." << std::endl;
            return 1;
        }
        
        // Crear objetos del juego
        SpriteManager spriteManager;
        Screen screen(spriteManager);  // Screen necesita SpriteManager como parámetro
        Game game(spriteManager, screen);
        
        gameInstance = &game;
        
        // Inicializar el juego
        game.initializeGame();
        
        // Crear hilo para invasores
        threadRunning = true;
        if (pthread_create(&invaderThread, nullptr, invaderThreadFunction, &game) != 0) {
            std::cerr << "Error: No se pudo crear el hilo de invasores" << std::endl;
            return 1;
        }
        
        // Ejecutar el juego
        game.mainLoop();
        
        // Detener hilo
        threadRunning = false;
        pthread_join(invaderThread, nullptr);
        
        // Asegurarse de que ncurses esté terminado antes de mostrar mensajes
        endwin();
        
        // Obtener puntaje final y agregarlo a la tabla
        // Solo si el jugador realmente jugó (no salió inmediatamente)
        if (game.getScore() > 0) {
            menu.addHighScore(menu.getPlayerName(), game.getScore(), game.getLevel());
        }
        
        std::cout << "¡Gracias por jugar Space Invaders!" << std::endl;
        std::cout << "Jugador: " << menu.getPlayerName() << std::endl;
        std::cout << "Puntaje Final: " << game.getScore() << std::endl;
        std::cout << "Nivel Alcanzado: " << game.getLevel() << std::endl;
    }
    
    return 0;
}