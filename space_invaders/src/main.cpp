#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include "game/game.h"
#include "graphics/screen.h"
#include "graphics/sprites.h"
#include <pthread.h>

// Función que se ejecuta por el hilo de los invasores
void* invaderThreadFunction(void* arg) {
    Game* game = static_cast<Game*>(arg);
    while (game->isRunning()) {
        game->updateInvaders();
        // Controlar la velocidad del movimiento usando usleep() como especifica el PDF
        usleep(500000); // Mueve los invasores cada 0.5 segundos
    }
    return NULL;
}

int main() {
    // Inicializar semilla para aleatorios usando time() como especifica el PDF
    srand(time(NULL));
    
    // Inicializar ncurses usando initscr() como especifica el PDF
    initscr();
    nodelay(stdscr, TRUE); // Permitir lectura no bloqueante como especifica el PDF
    noecho(); // No mostrar teclas presionadas
    cbreak(); // Deshabilitar buffering de línea
    keypad(stdscr, TRUE); // Habilitar teclas especiales
    curs_set(0); // Ocultar cursor
    
    // Inicialización del juego
    SpriteManager spriteManager;
    Screen screen(spriteManager);
    Game game(spriteManager, screen);
    
    // Declaración del hilo
    pthread_t invaderThread;
    
    // Crear el hilo de los invasores usando pthread_create() como especifica el PDF
    if (pthread_create(&invaderThread, NULL, invaderThreadFunction, &game) != 0) {
        endwin(); // Limpiar ncurses antes de salir
        std::cerr << "Error al crear el hilo de los invasores." << std::endl;
        return 1;
    }
    
    // Bucle principal del juego (para la entrada del jugador)
    while (game.isRunning()) {
        // Manejo de la entrada del jugador
        game.handleInput();
        
        // Actualizar la lógica del juego (proyectiles, colisiones)
        game.updateGameLogic();
        
        // Dibujar en pantalla
        screen.clear();
        game.drawElements();
        screen.draw();
        
        // Pequeño retraso para controlar el bucle usando usleep() como especifica el PDF
        usleep(16000); // ~60 FPS
    }
    
    // Espera a que el hilo de los invasores termine usando pthread_join() como especifica el PDF
    pthread_join(invaderThread, NULL);
    
    // Limpiar ncurses
    endwin();
    
    return 0;
}