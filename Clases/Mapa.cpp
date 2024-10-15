#include "Mapa.h"
#include <cstdlib>  // Para rand()

Mapa::Mapa(int filas, int columnas) : filas(filas), columnas(columnas) {
    // Inicializar la matriz de adyacencia con todas las celdas accesibles
    matrizAdyacencia.resize(filas, std::vector<int>(columnas, 0));  // Aquí se define 'matrizAdyacencia'
}

void Mapa::generarMapa() {
    // Generar obstáculos aleatorios
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            matrizAdyacencia[i][j] = rand() % 2;  // 0: libre, 1: obstáculo
        }
    }
    asegurarAccesibilidad();  // Asegurarse de que el mapa sea accesible
}

bool Mapa::esAccesible(int fila, int columna) const {
    return matrizAdyacencia[fila][columna] == 0;  // Verifica si es libre
}

int Mapa::getFilas() const {
    return filas;  // Retorna el número de filas
}

int Mapa::getColumnas() const {
    return columnas;  // Retorna el número de columnas
}

int Mapa::getValor(int fila, int columna) const {
    return matrizAdyacencia[fila][columna];  // Retorna el valor de la celda
}

void Mapa::asegurarAccesibilidad() {
    // Implementación para garantizar que todas las posiciones son accesibles
}
