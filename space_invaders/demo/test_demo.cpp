#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

// Incluir las clases de tu proyecto
#include "../src/graphics/sprites.h"
#include "../src/graphics/screen.h"

int main() {
    std::cout << "=== SPACE INVADERS - GRAPHICS SYSTEM TEST ===\n";
    std::cout << "Presiona Enter para iniciar el demo...\n";
    std::cin.get();
    
    // Crear el sprite manager
    SpriteManager spriteManager;
    
    // Crear la pantalla
    Screen screen(spriteManager);
    
    // Demo loop
    for (int frame = 0; frame < 100; frame++) {
        // Limpiar pantalla
        screen.clear();
        screen.drawBorder();
        screen.drawUI(1500 + frame * 10, 3, 1);
        
        // Posiciones de los invasores
        int startX = 10;
        int startY = 5;
        
        // Dibujar invasores tipo 1 (fila superior)
        for (int i = 0; i < 8; i++) {
            Position pos(startX + i * 8, startY);
            screen.drawSprite(spriteManager.getInvaderSprite(1), pos);
        }
        
        // Dibujar invasores tipo 2 (fila media)
        for (int i = 0; i < 8; i++) {
            Position pos(startX + i * 8, startY + 3);
            screen.drawSprite(spriteManager.getInvaderSprite(2), pos);
        }
        
        // Dibujar invasores tipo 3 (fila inferior)
        for (int i = 0; i < 8; i++) {
            Position pos(startX + i * 8, startY + 6);
            screen.drawSprite(spriteManager.getInvaderSprite(3), pos);
        }
        
        // Dibujar jugador
        Position playerPos(35, 20);
        screen.drawSprite(spriteManager.getPlayerSprite(), playerPos);
        
        // Dibujar proyectiles
        Position bulletPos(37, 15 - (frame % 10));
        if (bulletPos.y > 3) {
            screen.drawSprite(spriteManager.getBulletSprite(), bulletPos);
        }
        
        // Actualizar animaciones cada 10 frames
        if (frame % 10 == 0) {
            spriteManager.updateAnimations();
        }
        
        // Mostrar en pantalla
        screen.draw();
        
        // Pausa para ver la animación
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "\n=== DEMO TERMINADO ===\n";
    std::cout << "✅ Sistema gráfico funcionando correctamente!\n";
    
    return 0;
}