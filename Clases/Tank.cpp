#include "Tank.h"
#include "Mapa.h"
#include <cstdlib>
#include <queue>
#include <thread> // Para usar std::this_thread::sleep_for
#include <chrono> // Para manejar el tiempo
#include <vector>
#include <SFML/Graphics.hpp>  // Incluir SFML para usar sprites
#include <iostream>
#include <algorithm> // Para sort, shuffle, etc.
#include <random>
#include <climits> // Para INT_MAX

Tank::Tank(int x, int y, Color color) : x(x), y(y), color(color), salud(100){}


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
void Tank::moverPasoAPaso(const std::vector<std::pair<int, int>>& camino) {
    for (size_t i = 1; i < camino.size(); ++i) {
        setPos(camino[i].first, camino[i].second); // Mueve el tanque a la siguiente posición
        std::cout << "Moviendo el tanque a: (" << camino[i].first << ", " << camino[i].second << ")" << std::endl;

        // Espera un tiempo para que el movimiento sea visible (ajusta el tiempo como prefieras)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Definir las direcciones de movimiento (arriba, abajo, izquierda, derecha)
const int movimientos[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

void Tank::moverConBFS(const Mapa& mapa) {
    int filas = mapa.getFilas();
    int columnas = mapa.getColumnas();

    // Variables de inicio y destino
    int inicioX = x, inicioY = y;
    int destinoX = rand() % filas;
    int destinoY = rand() % columnas;

    // Asegurarse de que no esté seleccionando una celda con obstáculo
    while (mapa.hayObstaculo(destinoX, destinoY)) {
        destinoX = rand() % filas;
        destinoY = rand() % columnas;
    }

    std::cout << "Inicio: (" << inicioX << ", " << inicioY << "), Destino: (" << destinoX << ", " << destinoY << ")" << std::endl;

    std::vector<std::vector<bool>> visitado(filas, std::vector<bool>(columnas, false));
    std::queue<std::pair<int, int>> cola;
    cola.push({inicioX, inicioY});
    visitado[inicioX][inicioY] = true;

    std::vector<std::vector<std::pair<int, int>>> padre(filas, std::vector<std::pair<int, int>>(columnas, {-1, -1}));

    // BFS Loop
    bool encontrado = false;
    while (!cola.empty() && !encontrado) {
        int actualX = cola.front().first;
        int actualY = cola.front().second;
        cola.pop();

        std::cout << "Procesando nodo: (" << actualX << ", " << actualY << ")" << std::endl;

        // Explorar las 4 direcciones posibles
        for (int i = 0; i < 4; ++i) {
            int nuevoX = actualX + movimientos[i][0];
            int nuevoY = actualY + movimientos[i][1];

            // Asegurarse de que el nuevo movimiento esté dentro del mapa y no sea un obstáculo
            if (nuevoX >= 0 && nuevoX < filas && nuevoY >= 0 && nuevoY < columnas &&
                !mapa.hayObstaculo(nuevoX, nuevoY) && !visitado[nuevoX][nuevoY]) {

                std::cout << "Moviendo a: (" << nuevoX << ", " << nuevoY << ")" << std::endl;

                cola.push({nuevoX, nuevoY});
                visitado[nuevoX][nuevoY] = true;
                padre[nuevoX][nuevoY] = {actualX, actualY};

                // Si encontramos el destino
                if (nuevoX == destinoX && nuevoY == destinoY) {
                    encontrado = true;
                    std::cout << "Destino alcanzado: (" << nuevoX << ", " << nuevoY << ")" << std::endl;
                    break;
                }
            }
        }
    }

    if (encontrado) {
        // Reconstruir el camino
        std::vector<std::pair<int, int>> camino;
        for (std::pair<int, int> at = {destinoX, destinoY}; at != std::make_pair(inicioX, inicioY); at = padre[at.first][at.second]) {
            camino.push_back(at);
        }
        camino.push_back({inicioX, inicioY});
        std::reverse(camino.begin(), camino.end());

        // Mueve el tanque paso a paso
        moverPasoAPaso(camino);
    } else {
        std::cout << "No se encontró un camino válido." << std::endl;
    }
}

void Tank::moverConDijkstra(const Mapa& mapa) {
    int filas = mapa.getFilas();
    int columnas = mapa.getColumnas();

    // Variables de inicio y destino
    int inicioX = x, inicioY = y;
    int destinoX = rand() % filas;
    int destinoY = rand() % columnas;

    // Asegurarse de que no esté seleccionando una celda con obstáculo
    while (mapa.hayObstaculo(destinoX, destinoY)) {
        destinoX = rand() % filas;
        destinoY = rand() % columnas;
    }

    std::cout << "Inicio: (" << inicioX << ", " << inicioY << "), Destino: (" << destinoX << ", " << destinoY << ")" << std::endl;

    // Matriz de distancias y visitados
    std::vector<std::vector<int>> distancias(filas, std::vector<int>(columnas, INT_MAX));
    std::vector<std::vector<bool>> visitado(filas, std::vector<bool>(columnas, false));
    std::vector<std::vector<std::pair<int, int>>> padre(filas, std::vector<std::pair<int, int>>(columnas, {-1, -1}));

    // Inicializar la distancia al nodo de inicio
    distancias[inicioX][inicioY] = 0;

    // Priority queue para los nodos a procesar
    using Nodo = std::pair<int, std::pair<int, int>>; // (distancia, (x, y))
    std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> cola;
    cola.push({0, {inicioX, inicioY}});

    // Dijkstra Loop
    while (!cola.empty()) {
        int actualX = cola.top().second.first;
        int actualY = cola.top().second.second;
        cola.pop();

        // Si el nodo ya fue visitado, continuamos
        if (visitado[actualX][actualY]) continue;
        visitado[actualX][actualY] = true;

        // Explorar las 4 direcciones posibles
        for (int i = 0; i < 4; ++i) {
            int nuevoX = actualX + movimientos[i][0];
            int nuevoY = actualY + movimientos[i][1];

            // Asegurarse de que el nuevo movimiento esté dentro del mapa y no sea un obstáculo
            if (nuevoX >= 0 && nuevoX < filas && nuevoY >= 0 && nuevoY < columnas &&
                !mapa.hayObstaculo(nuevoX, nuevoY) && !visitado[nuevoX][nuevoY]) {

                // Calcular la nueva distancia
                int nuevaDistancia = distancias[actualX][actualY] + 1; // Cada movimiento tiene un costo de 1

                // Si encontramos una distancia más corta, actualizamos
                if (nuevaDistancia < distancias[nuevoX][nuevoY]) {
                    distancias[nuevoX][nuevoY] = nuevaDistancia;
                    padre[nuevoX][nuevoY] = {actualX, actualY};
                    cola.push({nuevaDistancia, {nuevoX, nuevoY}});
                }
            }
        }
    }

    // Reconstruir el camino
    std::vector<std::pair<int, int>> camino;
    for (std::pair<int, int> at = {destinoX, destinoY}; at != std::make_pair(-1, -1); at = padre[at.first][at.second]) {
        camino.push_back(at);
    }
    std::reverse(camino.begin(), camino.end());

    if (camino.size() > 1) {
        moverPasoAPaso(camino); // Mueve el tanque paso a paso
    } else {
        std::cout << "No se encontró un camino válido." << std::endl;
    }
}



void Tank::movimientoAleatorio(const Mapa& mapa) {
    // Generar un movimiento aleatorio
    int direccion = rand() % 4;
    int nuevoX = x + movimientos[direccion][0];
    int nuevoY = y + movimientos[direccion][1];

    // Verificar si la nueva posición es válida
    if (nuevoX >= 0 && nuevoX < mapa.getFilas() && nuevoY >= 0 && nuevoY < mapa.getColumnas() && !mapa.hayObstaculo(nuevoX, nuevoY)) {
        setPos(nuevoX, nuevoY);
        std::cout << "Movido aleatoriamente a: (" << nuevoX << ", " << nuevoY << ")" << std::endl;
    }
}

void Tank::recibirDanio(int cantidad) {
    salud -= cantidad;
    if (salud < 0) salud = 0;
}