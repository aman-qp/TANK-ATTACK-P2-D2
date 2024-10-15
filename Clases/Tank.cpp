#include "Tank.h"
#include <cstdlib>

Tank::Tank(int x, int y, Color color) : x(x), y(y), color(color), salud(100) {}

int Tank::getX() const {
    return x;
}

int Tank::getY() const {
    return y;
}

Tank::Color Tank::getColor() const {
    return color;
}

void Tank::mover(const Mapa& mapa) {
    if (color == AZUL || color == CELESTE) {
        // 50% probabilidad de usar BFS o movimiento aleatorio
        if (rand() % 2 == 0) {
            moverConBFS(mapa);
        } else {
            movimientoAleatorio(mapa);
        }
    } else if (color == ROJO || color == AMARILLO) {
        // 80% probabilidad de usar Dijkstra o movimiento aleatorio
        if (rand() % 10 < 8) {
            moverConDijkstra(mapa);
        } else {
            movimientoAleatorio(mapa);
        }
    }
}

void Tank::moverConBFS(const Mapa& mapa) {
    // Implementar algoritmo BFS para mover el tanque
}

void Tank::moverConDijkstra(const Mapa& mapa) {
    // Implementar algoritmo Dijkstra para mover el tanque
}

void Tank::movimientoAleatorio(const Mapa& mapa) {
    // Implementar un movimiento aleatorio en el mapa
}

void Tank::recibirDanio(int cantidad) {
    salud -= cantidad;
    if (salud < 0) salud = 0;
}
