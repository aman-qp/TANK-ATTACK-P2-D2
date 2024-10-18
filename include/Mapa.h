#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <utility>  // Para std::pair

class Tank;

class Mapa {
public:
    Mapa(int filas, int columnas);  // Constructor que define el tamaño del mapa
    void generarMapa(const std::vector<Tank>& todosLosTanques);  // Genera el mapa con obstáculos, asegurando accesibilidad
    bool esAccesible(int fila, int columna) const;  // Verifica si una celda es accesible
    int getFilas() const;
    int getColumnas() const;
    bool hayObstaculo(int fila, int columna) const;  // Verifica si hay un obstáculo en una celda
    std::vector<std::pair<int, int>> obtenerVecinos(int fila, int columna) const;  // Obtiene los vecinos accesibles de una celda

private:
    int filas, columnas;
    std::vector<std::vector<int>> matrizAdyacencia;  // Matriz de adyacencia (0: libre, 1: obstáculo)
    std::vector<std::vector<bool>> obstaculos;

    void conectarNodos(int nodo1, int nodo2);  // Conecta dos nodos en la matriz de adyacencia
    void desconectarNodo(int nodo);  // Desconecta un nodo de todos sus vecinos
    static bool esPosicionTanque(int fila, int columna, const std::vector<Tank>& tanques) ;  // Verifica si una posición tiene un tanque
    void asegurarAccesibilidad();  // Metodo para garantizar que el mapa sea accesible
};

#endif // MAPA_H
