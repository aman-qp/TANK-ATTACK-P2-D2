#include "Tank.h"
#include "Mapa.h"
#include <algorithm>
#include <queue>
#include <climits>
#include <iostream>
#include <random>

const int Tank::movimientos[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Arriba, abajo, izquierda, derecha

Tank::Tank(int x, int y, Color color) : x(x), y(y), color(color), salud(100) {
    std::cout << "Creando tanque en posición (" << x << "," << y << ") con color " << color << std::endl;
}

Tank::MovementType Tank::determinarTipoMovimiento(const std::vector<PowerUp>& powerUps) const {
    bool tienePrecisionMovimiento = std::any_of(powerUps.begin(), powerUps.end(),
        [](const PowerUp& p) { return p.estaActivo() && p.getTipo() == PowerUp::PRECISION_MOVIMIENTO; });

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    int probabilidad = dis(gen);

    if (color == AZUL || color == CELESTE) {
        if (tienePrecisionMovimiento) {
            return probabilidad <= 90 ? BFS : RANDOM;
        }
        return probabilidad <= 50 ? BFS : RANDOM;
    } else { // ROJO o AMARILLO
        if (tienePrecisionMovimiento) {
            return probabilidad <= 90 ? DIJKSTRA : RANDOM;
        }
        return probabilidad <= 80 ? DIJKSTRA : RANDOM;
    }
}

std::vector<std::pair<int, int>> Tank::calcularRuta(const Mapa& mapa, int destinoX, int destinoY,
                                                   const std::vector<PowerUp>& powerUps) {
    MovementType tipoMovimiento = determinarTipoMovimiento(powerUps);

    switch (tipoMovimiento) {
        case BFS:
            return calcularRutaBFS(mapa, x, y, destinoX, destinoY);
        case DIJKSTRA:
            return calcularRutaDijkstra(mapa, x, y, destinoX, destinoY);
        case RANDOM:
            return calcularRutaAleatoria(mapa, x, y, destinoX, destinoY);
    }

    return std::vector<std::pair<int, int>>();
}

std::vector<std::pair<int, int>> Tank::calcularRutaBFS(const Mapa& mapa, int inicioX, int inicioY,
                                                      int destinoX, int destinoY) {
    int filas = mapa.getFilas();
    int columnas = mapa.getColumnas();

    std::vector<std::vector<bool>> visitado(filas, std::vector<bool>(columnas, false));
    std::vector<std::vector<std::pair<int, int>>> padre(filas,
        std::vector<std::pair<int, int>>(columnas, {-1, -1}));

    std::queue<std::pair<int, int>> cola;
    cola.push({inicioX, inicioY});
    visitado[inicioX][inicioY] = true;

    bool encontrado = false;
    while (!cola.empty() && !encontrado) {
        int actualX = cola.front().first;
        int actualY = cola.front().second;
        cola.pop();

        for (const auto& mov : movimientos) {
            int nuevoX = actualX + mov[0];
            int nuevoY = actualY + mov[1];

            if (nuevoX >= 0 && nuevoX < filas && nuevoY >= 0 && nuevoY < columnas &&
                !mapa.hayObstaculo(nuevoX, nuevoY) && !visitado[nuevoX][nuevoY]) {

                visitado[nuevoX][nuevoY] = true;
                padre[nuevoX][nuevoY] = {actualX, actualY};
                cola.push({nuevoX, nuevoY});

                if (nuevoX == destinoX && nuevoY == destinoY) {
                    encontrado = true;
                    break;
                }
            }
        }
    }

    std::vector<std::pair<int, int>> ruta;
    if (encontrado) {
        auto actual = std::make_pair(destinoX, destinoY);
        while (actual.first != -1 && actual.second != -1) {
            ruta.push_back(actual);
            actual = padre[actual.first][actual.second];
        }
        std::reverse(ruta.begin(), ruta.end());
    }

    return ruta;
}

std::vector<std::pair<int, int>> Tank::calcularRutaDijkstra(const Mapa& mapa, int inicioX, int inicioY,
                                                           int destinoX, int destinoY) {
    int filas = mapa.getFilas();
    int columnas = mapa.getColumnas();

    std::vector<std::vector<int>> distancias(filas, std::vector<int>(columnas, INT_MAX));
    std::vector<std::vector<bool>> visitado(filas, std::vector<bool>(columnas, false));
    std::vector<std::vector<std::pair<int, int>>> padre(filas,
        std::vector<std::pair<int, int>>(columnas, {-1, -1}));

    distancias[inicioX][inicioY] = 0;

    using Nodo = std::pair<int, std::pair<int, int>>;
    std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> cola;
    cola.push({0, {inicioX, inicioY}});

    while (!cola.empty()) {
        int actualX = cola.top().second.first;
        int actualY = cola.top().second.second;
        cola.pop();

        if (visitado[actualX][actualY]) continue;
        visitado[actualX][actualY] = true;

        for (const auto& mov : movimientos) {
            int nuevoX = actualX + mov[0];
            int nuevoY = actualY + mov[1];

            if (nuevoX >= 0 && nuevoX < filas && nuevoY >= 0 && nuevoY < columnas &&
                !mapa.hayObstaculo(nuevoX, nuevoY) && !visitado[nuevoX][nuevoY]) {

                int nuevaDistancia = distancias[actualX][actualY] + 1;

                if (nuevaDistancia < distancias[nuevoX][nuevoY]) {
                    distancias[nuevoX][nuevoY] = nuevaDistancia;
                    padre[nuevoX][nuevoY] = {actualX, actualY};
                    cola.push({nuevaDistancia, {nuevoX, nuevoY}});
                }
            }
        }
    }

    std::vector<std::pair<int, int>> ruta;
    if (distancias[destinoX][destinoY] != INT_MAX) {
        auto actual = std::make_pair(destinoX, destinoY);
        while (actual.first != -1 && actual.second != -1) {
            ruta.push_back(actual);
            actual = padre[actual.first][actual.second];
        }
        std::reverse(ruta.begin(), ruta.end());
    }

    return ruta;
}

std::vector<std::pair<int, int>> Tank::calcularRutaAleatoria(const Mapa& mapa, int inicioX, int inicioY,
                                                            int destinoX, int destinoY) {
    std::vector<std::pair<int, int>> ruta;
    ruta.push_back({inicioX, inicioY});
    ruta.push_back({destinoX, destinoY});
    return ruta;
}

void Tank::setPos(int nuevaX, int nuevaY) {
    x = nuevaX;
    y = nuevaY;
    sprite.setPosition(
        static_cast<float>(y) * sprite.getLocalBounds().width,
        static_cast<float>(x) * sprite.getLocalBounds().height
    );
}

void Tank::recibirDanio(int cantidad) {
    salud -= cantidad;
    if (salud < 0) salud = 0;
}