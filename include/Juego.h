#ifndef JUEGO_H
#define JUEGO_H
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
#include "Mapa.h"
#include "Tank.h"

class Juego {
    bool turnoJugador1{};
    sf::Clock gameTimer;
public:
    Juego();
    void iniciar();

private:
    void jugar();
    std::vector<std::pair<int, int>> generarPosicionesAleatorias();

    Mapa mapa;
    std::string nombreJugador1;
    std::string nombreJugador2;
    std::vector<Tank> tanquesJugador1;
    std::vector<Tank> tanquesJugador2;
    std::mt19937 gen;  // Generador de números aleatorios
};

#endif // JUEGO_H