#ifndef TANK_H
#define TANK_H

#include <SFML/Graphics.hpp>  // Incluir SFML para usar sprites

class Mapa;

class Tank {
    int damage{};

public:
    enum Color { AZUL, CELESTE, ROJO, AMARILLO };  // Colores del tanque

    // Constructor que inicializa la posición, el color y carga la textura del tanque
    Tank(int x, int y, Color color);

    int getX() const; // Devuelve la posición en X
    int getY() const; // Devuelve la posición en Y
    Color getColor() const; // Devuelve el color del tanque



    // Mueve el tanque según las reglas (BFS, Dijkstra o movimiento aleatorio)
    void mover(const Mapa& mapa);
    void moverPasoAPaso(const std::vector<std::pair<int, int>>& camino);

    // Aplica daño al tanque
    void recibirDanio(int cantidad);

    // Devuelve la salud del tanque
    int getSalud() const { return salud; }

    // Sobrecarga del operador ==
    bool operator==(const Tank& other) const {
        return x == other.x && y == other.y && color == other.color;
    }

    // Método para establecer la posición del tanque
    void setPos(int nuevaX, int nuevaY) {
        x = nuevaX;  // Actualiza la posición en X
        y = nuevaY;  // Actualiza la posición en Y
        sprite.setPosition(static_cast<float>(y) * sprite.getLocalBounds().width,
                           static_cast<float>(x) * sprite.getLocalBounds().height);
    }

    // Devuelve una referencia al sprite del tanque
    sf::Sprite& getSprite() {
        return sprite;
    }

private:
    int x, y;  // Posición del tanque en la cuadrícula
    Color color;  // Color del tanque
    int salud = 100;  // Salud del tanque (inicialmente 100)

    // Textura y sprite del tanque
    sf::Texture textura;  // Textura del tanque
    sf::Sprite sprite;  // Sprite del tanque

    // Métodos privados para el movimiento
    void moverConBFS(const Mapa& mapa);      // Movimiento con BFS
    void moverConDijkstra(const Mapa& mapa); // Movimiento con Dijkstra
    void movimientoAleatorio(const Mapa& mapa); // Movimiento aleatorio
};

#endif // TANK_H