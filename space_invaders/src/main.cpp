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
    
    while (threadRunning) {
        // Verificar que el puntero sea válido
        if (game != nullptr && game->isRunning()) {
            game->updateInvaders();
        } else {
            // Si el juego no está corriendo, esperar un poco más
            usleep(50000);
        }
        usleep(100000); // 100ms entre actualizaciones de invasores
    }
    
    return nullptr;
}

int main() {
    // Inicializar semilla aleatoria
    srand(time(nullptr));
    
    bool keepPlaying = true;
    
    while (keepPlaying) {
        // Asegurarse de que la terminal esté limpia
        int clearResult = system("clear");
        (void)clearResult; // Suprimir warning
        
        // Crear sistema de menús
        MenuSystem menu;
        
        // Ejecutar menús hasta que el usuario elija jugar o salir
        MenuState result = menu.run();
        
        if (result == EXIT_PROGRAM) {
            keepPlaying = false;
            break;
        }
        
        if (result == GAME_RUNNING) {
            // Obtener el nombre del jugador antes de terminar el menú
            std::string playerName = menu.getPlayerName();
            
            // El MenuSystem termina ncurses al destruirse aquí
            // (cuando sale del scope al final del if)
            
            // Pequeña pausa para asegurar limpieza del menú
            usleep(150000);
            
            // Verificar tamaño de terminal
            initscr();
            int maxY, maxX;
            getmaxyx(stdscr, maxY, maxX);
            endwin();
            
            if (maxY < 25 || maxX < 80) {
                std::cout << "Error: El terminal debe ser de al menos 25x80 caracteres." << std::endl;
                std::cout << "Tamaño actual: " << maxY << "x" << maxX << std::endl;
                std::cout << "Por favor ajusta el tamaño de tu terminal y vuelve a ejecutar." << std::endl;
                sleep(2);
                continue;
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
                gameInstance = nullptr;
                sleep(2);
                continue;
            }
            
            // Ejecutar el juego
            game.mainLoop();
            
            // Detener hilo de forma segura
            threadRunning = false;
            
            // Esperar a que el hilo termine
            pthread_join(invaderThread, nullptr);
            
            // Guardar puntaje antes de terminar ncurses
            int finalScore = game.getScore();
            int finalLevel = game.getLevel();
            
            // Terminar ncurses COMPLETAMENTE
            endwin();
            
            // Limpiar puntero
            gameInstance = nullptr;
            
            // Pausa más larga para asegurar limpieza completa de ncurses
            usleep(250000);
            
            // Limpiar la terminal completamente
            clearResult = system("clear");
            (void)clearResult; // Suprimir warning
            
            // Limpiar cualquier basura en el buffer de entrada
            fflush(stdin);
            
            // Guardar puntaje si el jugador jugó
            if (finalScore > 0) {
                // Crear un nuevo MenuSystem solo para guardar el puntaje
                MenuSystem scoreMenu;
                scoreMenu.addHighScore(playerName, finalScore, finalLevel);
                // scoreMenu se destruye aquí automáticamente
            }
            
            // Mostrar estadísticas finales
            std::cout << "\n==================================" << std::endl;
            std::cout << "    ESTADISTICAS FINALES" << std::endl;
            std::cout << "==================================" << std::endl;
            std::cout << "Jugador: " << playerName << std::endl;
            std::cout << "Puntaje Final: " << finalScore << std::endl;
            std::cout << "Nivel Alcanzado: " << finalLevel << std::endl;
            std::cout << "==================================" << std::endl;
            std::cout << "\nVolviendo al menu principal..." << std::endl;
            
            // Esperar 2 segundos antes de volver al menú
            sleep(2);
        }
    }
    
    std::cout << "\n¡Gracias por jugar Space Invaders!" << std::endl;
    return 0;
}