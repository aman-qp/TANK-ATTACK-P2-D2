#include "Tank.h"
#include "Mapa.h"
#include <algorithm>
#include <queue>
#include <climits>
#include <iostream>
#include <random>

const int Tank::movimientos[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Arriba, abajo, izquierda, derecha

Tank::Tank(int x, int y, Color color) : x(x), y(y), color(color), salud(100) {
}

Tank::MovementType Tank::determinarTipoMovimiento(const std::vector<PowerUp>& powerUps) const {
    bool tienePrecisionMovimiento = std::any_of(powerUps.begin(), powerUps.end(),
        [](const PowerUp& p) { return p.estaActivo() && p.getTipo() == PowerUp::PRECISION_MOVIMIENTO; });

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    int probabilidad = dis(gen);

    MovementType tipoElegido;
    std::string colorStr;

    // Determinar el color en string
    switch(color) {
        case AZUL: colorStr = "AZUL"; break;
        case CELESTE: colorStr = "CELESTE"; break;
        case ROJO: colorStr = "ROJO"; break;
        case AMARILLO: colorStr = "AMARILLO"; break;
    }

    if (color == AZUL || color == CELESTE) {
        if (tienePrecisionMovimiento) {
            tipoElegido = probabilidad <= 90 ? BFS : RANDOM;
        } else {
            tipoElegido = probabilidad <= 50 ? BFS : RANDOM;
        }
    } else { // ROJO o AMARILLO
        if (tienePrecisionMovimiento) {
            tipoElegido = probabilidad <= 90 ? DIJKSTRA : RANDOM;
        } else {
            tipoElegido = probabilidad <= 80 ? DIJKSTRA : RANDOM;
        }
    }

    // Mostrar información en consola
    std::cout << "\nMovimiento de tanque " << colorStr << ":" << std::endl;
    std::cout << "- Tiene power-up de precisión: " << (tienePrecisionMovimiento ? "Sí" : "No") << std::endl;
    std::cout << "- Probabilidad generada: " << probabilidad << std::endl;
    std::cout << "- Algoritmo elegido: ";
    switch(tipoElegido) {
        case BFS: std::cout << "BFS"; break;
        case DIJKSTRA: std::cout << "Dijkstra"; break;
        case RANDOM: std::cout << "Aleatorio"; break;
    }
    std::cout << std::endl;

    return tipoElegido;
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
                                                            int destinoX, int destinoY) const {
    std::cout << "\nEjecutando movimiento aleatorio:" << std::endl;
    std::vector<std::pair<int, int>> ruta;

    // Función para verificar línea vista
    auto hayLineaVista = [&mapa](int x1, int y1, int x2, int y2) -> bool {
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int x = x1;
        int y = y1;
        int n = 1 + dx + dy;
        int x_inc = (x2 > x1) ? 1 : -1;
        int y_inc = (y2 > y1) ? 1 : -1;
        int error = dx - dy;
        dx *= 2;
        dy *= 2;

        std::cout << "Verificando línea vista desde (" << x1 << "," << y1 << ") hasta ("
                  << x2 << "," << y2 << ")" << std::endl;

        for (; n > 0; --n) {
            if (mapa.hayObstaculo(x, y)) {
                std::cout << "Obstáculo encontrado en (" << x << "," << y << ")" << std::endl;
                return false;
            }
            if (error > 0) {
                x += x_inc;
                error -= dy;
            } else {
                y += y_inc;
                error += dx;
            }
        }
        std::cout << "Línea vista disponible" << std::endl;
        return true;
    };

    // Primer intento: línea vista directa
    if (hayLineaVista(inicioX, inicioY, destinoX, destinoY)) {
        ruta.push_back({inicioX, inicioY});
        ruta.push_back({destinoX, destinoY});
        return ruta;
    }

    // Si no hay línea vista, calculamos posición aleatoria en radio
    const int RADIO_MOVIMIENTO = 3; // Puedes ajustar este valor
    std::random_device rd;
    std::mt19937 gen(rd());

    // Generar posición aleatoria en radio
    auto generarPosicionAleatoria = [&]() -> std::pair<int, int> {
        std::uniform_int_distribution<> dis(-RADIO_MOVIMIENTO, RADIO_MOVIMIENTO);
        int intentos = 0;
        const int MAX_INTENTOS = 20;

        while (intentos < MAX_INTENTOS) {
            int dx = dis(gen);
            int dy = dis(gen);
            int nuevaX = inicioX + dx;
            int nuevaY = inicioY + dy;

            // Verificar que la posición es válida
            if (nuevaX >= 0 && nuevaX < mapa.getFilas() &&
                nuevaY >= 0 && nuevaY < mapa.getColumnas() &&
                !mapa.hayObstaculo(nuevaX, nuevaY)) {

                std::cout << "Posición aleatoria generada: (" << nuevaX << "," << nuevaY << ")" << std::endl;
                return {nuevaX, nuevaY};
            }
            intentos++;
        }
        // Si no encontramos posición válida, retornamos la posición actual
        return {inicioX, inicioY};
    };

    // Obtener posición aleatoria intermedia
    auto [posIntermediaX, posIntermediaY] = generarPosicionAleatoria();

    // Agregar primer movimiento a la posición intermedia
    ruta.push_back({inicioX, inicioY});
    ruta.push_back({posIntermediaX, posIntermediaY});

    // Segundo intento: línea vista desde posición intermedia
    if (hayLineaVista(posIntermediaX, posIntermediaY, destinoX, destinoY)) {
        ruta.push_back({destinoX, destinoY});
    } else {
        // Sí falla el segundo intento, encontrar el punto más cercano posible
        std::cout << "Buscando punto más cercano posible al destino" << std::endl;
        int mejorX = posIntermediaX;
        int mejorY = posIntermediaY;
        double mejorDistancia = std::numeric_limits<double>::max();

        // Buscar en un área alrededor de la posición intermedia
        for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
                int testX = posIntermediaX + dx;
                int testY = posIntermediaY + dy;

                if (testX >= 0 && testX < mapa.getFilas() &&
                    testY >= 0 && testY < mapa.getColumnas() &&
                    !mapa.hayObstaculo(testX, testY)) {

                    double distancia = std::sqrt(
                        std::pow(testX - destinoX, 2) +
                        std::pow(testY - destinoY, 2)
                    );

                    if (distancia < mejorDistancia &&
                        hayLineaVista(posIntermediaX, posIntermediaY, testX, testY)) {
                        mejorDistancia = distancia;
                        mejorX = testX;
                        mejorY = testY;
                    }
                }
            }
        }

        if (mejorX != posIntermediaX || mejorY != posIntermediaY) {
            ruta.push_back({mejorX, mejorY});
            std::cout << "Punto más cercano encontrado: (" << mejorX << "," << mejorY << ")" << std::endl;
        }
    }

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