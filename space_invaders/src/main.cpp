#include <iostream>
#include <unistd.h>
#include "game/game.h"
#include "graphics/screen.h"
#include "graphics/sprites.h"
#include <pthread.h>

// Función que se ejecuta por el hilo de los invasores
void* invaderThreadFunction(void* arg) {
    Game* game = static_cast<Game*>(arg);
    while (game->isRunning()) {
        game->updateInvaders();
        // Controlar la velocidad del movimiento
        usleep(500000); // Mueve los invasores cada 0.5 segundos
    }
    return NULL;
}

int main() {
    // Inicialización del juego
    SpriteManager spriteManager;
    Screen screen(spriteManager);
    Game game(spriteManager, screen);
    
    // Declaración del hilo
    pthread_t invaderThread;
    
    // Crear el hilo de los invasores
    if (pthread_create(&invaderThread, NULL, invaderThreadFunction, &game) != 0) {
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
        
        // Pequeño retraso para controlar el bucle
        usleep(16000);
    }
    
    // Espera a que el hilo de los invasores termine antes de salir
    pthread_join(invaderThread, NULL);
    
    return 0;
}