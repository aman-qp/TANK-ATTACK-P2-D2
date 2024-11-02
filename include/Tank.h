#ifndef TANK_H
#define TANK_H

#include <SFML/Graphics.hpp>
#include "PowerUp.h"
#include "Mapa.h"
#include <vector>

class Tank {
public:
    enum Color { AZUL, CELESTE, ROJO, AMARILLO };
    enum MovementType { BFS, DIJKSTRA, RANDOM };

    // Constructor y métodos principales
    Tank(int x, int y, Color color);

    // Métodos de movimiento
    std::vector<std::pair<int, int>> calcularRuta(const Mapa& mapa, int destinoX, int destinoY,
                                                 const std::vector<PowerUp>& powerUps);
    void setPos(int nuevaX, int nuevaY);

    // Métodos de estado
    void recibirDanio(int cantidad);

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    Color getColor() const { return color; }
    int getSalud() const { return salud; }
    const std::vector<std::pair<int, int>>& getRutaActual() const { return rutaActual; }
    sf::Sprite& getSprite() { return sprite; }

private:
    // Atributos
    int x, y;
    Color color;
    int salud;
    sf::Sprite sprite;
    std::vector<std::pair<int, int>> rutaActual;

    // Métodos de pathfinding
    std::vector<std::pair<int, int>> calcularRutaBFS(const Mapa& mapa, int inicioX, int inicioY,
                                                    int destinoX, int destinoY);
    std::vector<std::pair<int, int>> calcularRutaDijkstra(const Mapa& mapa, int inicioX, int inicioY,
                                                         int destinoX, int destinoY);
    std::vector<std::pair<int, int>> calcularRutaAleatoria(const Mapa& mapa, int inicioX, int inicioY,
                                                          int destinoX, int destinoY) const;
    MovementType determinarTipoMovimiento(const std::vector<PowerUp>& powerUps) const;

    // Direcciones de movimiento
    static const int movimientos[4][2];
};

#endif