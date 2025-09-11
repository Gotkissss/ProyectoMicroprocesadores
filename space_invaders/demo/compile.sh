#!/bin/bash
echo "🚀 Compilando demo de Space Invaders..."

# Compilar sprites.cpp, screen.cpp y test_demo.cpp juntos
g++ -o test_demo \
    ../src/graphics/sprites.cpp \
    ../src/graphics/screen.cpp \
    test_demo.cpp \
    -std=c++11 -I../src

if [ $? -eq 0 ]; then
    echo "✅ Compilación exitosa!"
    echo "Ejecuta con: ./test_demo"
else
    echo "❌ Error en la compilación"
fi