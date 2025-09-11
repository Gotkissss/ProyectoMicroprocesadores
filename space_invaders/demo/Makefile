# Makefile para el demo de Space Invaders
CXX = g++
CXXFLAGS = -std=c++11 -I../src
SRCDIR = ../src/graphics
SOURCES = $(SRCDIR)/sprites.cpp $(SRCDIR)/screen.cpp test_demo.cpp
TARGET = test_demo

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) $(TARGET).exe

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run