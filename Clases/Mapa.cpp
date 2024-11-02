#include "Mapa.h"
#include <cstdlib>
#include <queue>
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>
#include "Tank.h"

Mapa::Mapa(int filas, int columnas) : filas(filas), columnas(columnas) {
    // Inicializar la matriz de adyacencia
    int totalNodos = filas * columnas;
    matrizAdyacencia.resize(totalNodos, std::vector<int>(totalNodos, 0));

    // Inicializar el vector de obstáculos
    obstaculos.resize(filas, std::vector<bool>(columnas, false));
}

void Mapa::generarMapa(const std::vector<Tank>& todosLosTanques) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Inicializar conexiones en la matriz de adyacencia
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            int nodoActual = i * columnas + j;

            // Conectar con vecinos (arriba, abajo, izquierda, derecha)
            if (i > 0) conectarNodos(nodoActual, (i - 1) * columnas + j);
            if (i < filas - 1) conectarNodos(nodoActual, (i + 1) * columnas + j);
            if (j > 0) conectarNodos(nodoActual, i * columnas + (j - 1));
            if (j < columnas - 1) conectarNodos(nodoActual, i * columnas + (j + 1));
        }
    }

    // Generar obstáculos aleatorios
    int cantidadObstaculos = static_cast<int>((filas * columnas) * 0.1);
    std::cout << "Generated " << cantidadObstaculos << " obstacles." << std::endl;

    for (int i = 0; i < cantidadObstaculos; ++i) {
        int fila = std::rand() % filas;
        int columna = std::rand() % columnas;
        int nodo = fila * columnas + columna;

        // Evitar colocar obstáculos en posiciones de tanques
        if (!esPosicionTanque(fila, columna, todosLosTanques)) {
            desconectarNodo(nodo);
            obstaculos[fila][columna] = true;
        }
    }

    // Asegurar que el mapa sea accesible
    asegurarAccesibilidad();
}

void Mapa::conectarNodos(int nodo1, int nodo2) {
    matrizAdyacencia[nodo1][nodo2] = 1;
    matrizAdyacencia[nodo2][nodo1] = 1;
}

void Mapa::desconectarNodo(int nodo) {
    for (int i = 0; i < matrizAdyacencia.size(); ++i) {
        matrizAdyacencia[nodo][i] = 0;
        matrizAdyacencia[i][nodo] = 0;
    }
}

bool Mapa::esPosicionTanque(int fila, int columna, const std::vector<Tank>& tanques) {
    return std::any_of(tanques.begin(), tanques.end(),
        [fila, columna](const Tank& tanque) {
            return tanque.getX() == fila && tanque.getY() == columna;
        });
}

void Mapa::asegurarAccesibilidad() {
    std::vector<bool> visitado(filas * columnas, false);
    std::queue<int> cola;

    // Comenzar desde el nodo (0,0)
    cola.push(0);
    visitado[0] = true;

    while (!cola.empty()) {
        int nodoActual = cola.front();
        cola.pop();

        for (int i = 0; i < matrizAdyacencia.size(); ++i) {
            if (matrizAdyacencia[nodoActual][i] == 1 && !visitado[i]) {
                visitado[i] = true;
                cola.push(i);
            }
        }
    }

    // Conectar nodos no visitados al grafo
    for (int i = 0; i < visitado.size(); ++i) {
        if (!visitado[i]) {
            int fila = i / columnas;
            int columna = i % columnas;

            // Conectar con un vecino accesible
            if (fila > 0) conectarNodos(i, (fila-1) * columnas + columna);
            else if (fila < filas-1) conectarNodos(i, (fila+1) * columnas + columna);
            else if (columna > 0) conectarNodos(i, fila * columnas + (columna-1));
            else if (columna < columnas-1) conectarNodos(i, fila * columnas + (columna+1));
        }
    }
}

bool Mapa::esAccesible(int fila, int columna) const {
    int nodo = fila * columnas + columna;
    for (int i = 0; i < matrizAdyacencia.size(); ++i) {
        if (matrizAdyacencia[nodo][i] == 1) return true;
    }
    return false;
}

bool Mapa::hayObstaculo(int fila, int columna) const {
    return obstaculos[fila][columna];
}

int Mapa::getFilas() const {
    return filas;
}

int Mapa::getColumnas() const {
    return columnas;
}

std::vector<std::pair<int, int>> Mapa::obtenerVecinos(int fila, int columna) const {
    std::vector<std::pair<int, int>> vecinos;
    int nodo = fila * columnas + columna;

    for (int i = 0; i < matrizAdyacencia.size(); ++i) {
        if (matrizAdyacencia[nodo][i] == 1) {
            int vecinoFila = i / columnas;
            int vecinoColumna = i % columnas;
            vecinos.emplace_back(vecinoFila, vecinoColumna);
        }
    }

    return vecinos;
}