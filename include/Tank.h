#ifndef TANK_H
#define TANK_H

#include "Mapa.h"

class Tank {
public:
    enum Color { AZUL, CELESTE, ROJO, AMARILLO };  // Colores del tanque

    Tank(int x, int y, Color color);  // Constructor que inicializa la posición y el color
    int getX() const;
    int getY() const;
    Color getColor() const;
    void mover(const Mapa& mapa);  // Mueve el tanque según las reglas (BFS, Dijkstra, o movimiento aleatorio)
    void recibirDanio(int cantidad);  // Aplica daño al tanque

private:
    int x, y;  // Posición del tanque en la cuadrícula
    Color color;  // Color del tanque
    int salud;  // Salud del tanque (inicialmente 100)
    void moverConBFS(const Mapa& mapa);  // Movimiento con BFS
    void moverConDijkstra(const Mapa& mapa);  // Movimiento con Dijkstra
    void movimientoAleatorio(const Mapa& mapa);  // Movimiento aleatorio
};

#endif // TANK_H
