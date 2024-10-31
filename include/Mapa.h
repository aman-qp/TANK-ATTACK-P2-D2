#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <utility>

class Tank;

class Mapa {
public:
    // Constructor y métodos principales
    Mapa(int filas, int columnas);
    void generarMapa(const std::vector<Tank>& todosLosTanques);

    // Métodos de consulta
    bool esAccesible(int fila, int columna) const;
    bool hayObstaculo(int fila, int columna) const;
    int getFilas() const;
    int getColumnas() const;
    std::vector<std::pair<int, int>> obtenerVecinos(int fila, int columna) const;

private:
    // Atributos
    int filas, columnas;
    std::vector<std::vector<int>> matrizAdyacencia;
    std::vector<std::vector<bool>> obstaculos;

    // Métodos auxiliares para la generación del mapa
    void conectarNodos(int nodo1, int nodo2);
    void desconectarNodo(int nodo);
    static bool esPosicionTanque(int fila, int columna, const std::vector<Tank>& tanques);
    void asegurarAccesibilidad();
};

#endif