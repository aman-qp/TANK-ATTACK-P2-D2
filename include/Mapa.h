#ifndef MAPA_H
#define MAPA_H

#include <vector>

class Mapa {
public:
    Mapa(int filas, int columnas);  // Constructor que define el tamaño del mapa
    void generarMapa();  // Genera el mapa con obstáculos, asegurando accesibilidad
    bool esAccesible(int fila, int columna) const;  // Verifica si una celda es accesible
    int getFilas() const;
    int getColumnas() const;
    int getValor(int fila, int columna) const;
    const std::vector<std::vector<int>>& getMatrizAdyacencia() const;  // Retorna la matriz de adyacencia

private:
    int filas, columnas;
    std::vector<std::vector<int>> matrizAdyacencia;  // Matriz de adyacencia (0: libre, 1: obstáculo)
    void asegurarAccesibilidad();  // Metodo para garantizar que el mapa sea accesible
};

#endif // MAPA_H
