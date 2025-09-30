#include "game/game.h"
#include "graphics/sprites.h"
#include "graphics/screen.h"
#include "MenuSystem.h"
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

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
    
    bool keepPlaying = true;
    
    while (keepPlaying) {
        // Crear sistema de menús
        MenuSystem menu;
        
        // Ejecutar menús hasta que el usuario elija jugar o salir
        MenuState result = menu.run();
        
        if (result == EXIT_PROGRAM) {
            std::cout << "¡Gracias por jugar Space Invaders!" << std::endl;
            keepPlaying = false;
            break;
        }
        
        if (result == GAME_RUNNING) {
            // Obtener el nombre del jugador antes de terminar el menú
            std::string playerName = menu.getPlayerName();
            
            // El MenuSystem termina ncurses al destruirse aquí
            // (cuando sale del scope al final del if)
            
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
            Screen screen(spriteManager);
            Game game(spriteManager, screen);
            
            gameInstance = &game;
            
            // Inicializar el juego
            game.initializeGame();
            
            // Crear hilo para invasores
            threadRunning = true;
            if (pthread_create(&invaderThread, nullptr, invaderThreadFunction, &game) != 0) {
                std::cerr << "Error: No se pudo crear el hilo de invasores" << std::endl;
                endwin();
                return 1;
            }
            
            // Ejecutar el juego
            game.mainLoop();
            
            // Detener hilo de forma segura
            threadRunning = false;
            pthread_join(invaderThread, nullptr);
            
            // Terminar ncurses antes de cualquier operación
            endwin();
            
            // Guardar puntaje si el jugador jugó
            if (game.getScore() > 0) {
                // Crear un nuevo MenuSystem solo para guardar el puntaje
                MenuSystem scoreMenu;
                scoreMenu.addHighScore(playerName, game.getScore(), game.getLevel());
                // scoreMenu se destruye aquí automáticamente
            }
            
            // Mostrar estadísticas finales
            std::cout << "\n==================================" << std::endl;
            std::cout << "    ESTADÍSTICAS FINALES" << std::endl;
            std::cout << "==================================" << std::endl;
            std::cout << "Jugador: " << playerName << std::endl;
            std::cout << "Puntaje Final: " << game.getScore() << std::endl;
            std::cout << "Nivel Alcanzado: " << game.getLevel() << std::endl;
            std::cout << "==================================" << std::endl;
            
            // Pequeña pausa antes de volver al menú
            sleep(2);
        }
    }
    
    return 0;
}