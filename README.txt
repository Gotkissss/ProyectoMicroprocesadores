// =============================================================================
// SPACE INVADERS - ESTRUCTURA DEL PROYECTO
// =============================================================================

ESTRUCTURA DE CARPETAS PROPUESTA:
space_invaders/
├── src/
│   ├── main.cpp
│   ├── game/
│   │   ├── game.h
│   │   ├── game.cpp
│   │   └── entities.h
│   ├── graphics/
│   │   ├── screen.h
│   │   ├── screen.cpp
│   │   ├── sprites.h
│   │   ├── sprites.cpp
│   │   └── animations.h
│   ├── menu/
│   │   ├── menu.h
│   │   └── menu.cpp
│   └── utils/
│       ├── console.h
│       └── console.cpp
├── assets/
│   ├── sprites/
│   │   ├── player.txt
│   │   ├── invaders.txt
│   │   ├── bullets.txt
│   │   └── ui_elements.txt
│   └── animations/
│       ├── explosions.txt
│       └── invader_movement.txt
└── Makefile