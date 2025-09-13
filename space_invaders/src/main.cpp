#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include <pthread.h>
#include "game/game.h"
#include "graphics/screen.h"
#include "graphics/sprites.h"

// Función que se ejecuta por el hilo de los invasores
void* invaderThreadFunction(void* arg) {
    Game* game = static_cast<Game*>(arg);
    
    while (game->isRunning()) {
        // Controlar la velocidad del movimiento usando usleep() como especifica el PDF
        // Ahora usa velocidad variable según el nivel
        usleep(game->getInvaderSpeed());
    }
    
    return NULL;
}

int main() {
    // Inicializar semilla para aleatorios usando time() como especifica el PDF
    srand(time(NULL));
    
    // Inicializar ncurses usando initscr() como especifica el PDF
    initscr();
    // No mostrar teclas presionadas como especifica el PDF
    noecho();
    // Deshabilitar buffering de línea
    cbreak();
    // Habilitar teclas especiales
    keypad(stdscr, TRUE);
    // Ocultar cursor
    curs_set(0);
    // No bloquear getch()
    nodelay(stdscr, TRUE);
    
    // Inicialización del juego
    SpriteManager spriteManager;
    Screen screen(spriteManager);
    Game game(spriteManager, screen);
    
    game.initializeGame();
    
    // Crear hilo para el movimiento de invasores
    pthread_t invaderThread;
    if (pthread_create(&invaderThread, NULL, invaderThreadFunction, &game) != 0) {
        endwin();
        std::cerr << "Error al crear el hilo de invasores" << std::endl;
        return 1;
    }
    
    // Bucle principal del juego
    while (game.isRunning()) {
        // Manejo de la entrada del jugador - usar versión mejorada
        game.handleInputImproved();
        
        // Actualizar la lógica del juego (proyectiles, colisiones, etc.)
        // NOTA: updateInvaders() se ejecuta en el hilo separado
        game.updateProjectiles();
        game.checkCollisions();
        game.checkPlayerCollisions();
        game.checkInvaderReachBottom();
        game.checkVictoryConditions();
        game.checkGameOverConditions();
        
        // Dibujar en pantalla
        screen.clear();
        game.drawElements();
        game.drawGameStateMessages();
        screen.draw();
        
        // Pequeño retraso para controlar el bucle usando usleep() como especifica el PDF
        usleep(16000); // ~60 FPS
    }
    
    // Esperar a que termine el hilo de invasores
    pthread_join(invaderThread, NULL);
    
    // Finalizar ncurses
    endwin();
    
    std::cout << "¡Gracias por jugar Space Invaders!" << std::endl;
    
    return 0;
}