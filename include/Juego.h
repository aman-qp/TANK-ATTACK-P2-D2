#ifndef JUEGO_H
#define JUEGO_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
#include "Mapa.h"
#include "Tank.h"

class Juego {
public:
    // Constructor y metodo principal
    Juego();
    void iniciar();

private:
    // Atributos principales
    Mapa mapa;
    std::string nombreJugador1;
    std::string nombreJugador2;
    std::vector<Tank> tanquesJugador1;
    std::vector<Tank> tanquesJugador2;

    // Control del juego
    bool turnoJugador1;
    sf::Clock gameTimer;

    // Generador de números aleatorios
    std::mt19937 gen;

    // Métodos privados
    void jugar();
    std::vector<std::pair<int, int>> generarPosicionesAleatorias();
};

#endif