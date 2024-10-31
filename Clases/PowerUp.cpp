#include "PowerUp.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>

PowerUp::PowerUp() : tipo(NONE), activo(false), turnosRestantes(0) {}

PowerUp::PowerUp(Type tipo) : tipo(tipo), activo(false), turnosRestantes(0) {}

void PowerUp::activar() {
    activo = true;
    switch(tipo) {
        case DOBLE_TURNO:
            turnosRestantes = DURACION_DOBLE_TURNO;
            break;
        case PRECISION_MOVIMIENTO:
        case PRECISION_ATAQUE:
        case PODER_ATAQUE:
            turnosRestantes = DURACION_OTROS_POWERUPS;
            break;
        case NONE:
            turnosRestantes = 0;
            break;
    }
    std::cout << "Power-up activado: " << getNombre() << " por " << turnosRestantes << " turnos" << std::endl;
}

void PowerUp::reducirTurno() {
    if (turnosRestantes > 0) {
        turnosRestantes--;
        if (turnosRestantes == 0) {
            activo = false;
            std::cout << "Power-up " << getNombre() << " ha expirado" << std::endl;
        }
    }
}

void PowerUp::generarPowerUpAleatorio(std::vector<PowerUp>& powerUps, float tiempoTranscurrido, float& tiempoUltimoPowerUp) {
    if (tiempoTranscurrido - tiempoUltimoPowerUp >= TIEMPO_ENTRE_POWERUPS &&
        powerUps.size() < MAX_POWERUPS) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 3);

        Type nuevoPowerUp = static_cast<Type>(dis(gen));
        powerUps.emplace_back(nuevoPowerUp);

        std::cout << "Nuevo power-up generado: " << powerUps.back().getNombre() << std::endl;
        tiempoUltimoPowerUp = tiempoTranscurrido;
    }
}

void PowerUp::actualizarPowerUps(std::vector<PowerUp>& powerUps) {
    for (auto& powerUp : powerUps) {
        if (powerUp.estaActivo()) {
            powerUp.reducirTurno();
        }
    }
}

void PowerUp::limpiarPowerUpsExpirados(std::vector<PowerUp>& powerUps) {
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [](const PowerUp& p) { return !p.estaActivo() && p.getTurnosRestantes() == 0; }),
        powerUps.end()
    );
}

bool PowerUp::tienePowerUpActivo(const std::vector<PowerUp>& powerUps, Type tipo) {
    return std::any_of(powerUps.begin(), powerUps.end(),
        [tipo](const PowerUp& p) { return p.estaActivo() && p.getTipo() == tipo; });
}

int PowerUp::calcularDanioConPowerUp(int danioBase, const std::vector<PowerUp>& powerUps) {
    if (tienePowerUpActivo(powerUps, PODER_ATAQUE)) {
        return 100;  // Daño máximo con poder de ataque
    }
    return danioBase;
}

bool PowerUp::debeSaltarTurno(const std::vector<PowerUp>& powerUps) {
    return tienePowerUpActivo(powerUps, DOBLE_TURNO);
}

void PowerUp::dibujar(sf::RenderWindow& window, const sf::Font& font, float x, float y, bool seleccionado) const {
    if (!estaActivo()) {
        // Dibujar fondo del power-up
        sf::RectangleShape powerUpBox({POWERUP_WIDTH, POWERUP_HEIGHT});
        powerUpBox.setPosition(x, y);
        powerUpBox.setFillColor(seleccionado ?
            sf::Color(100, 100, 100, 200) : sf::Color(50, 50, 50, 200));
        powerUpBox.setOutlineColor(sf::Color::White);
        powerUpBox.setOutlineThickness(1);
        window.draw(powerUpBox);

        // Dibujar texto del power-up
        sf::Text texto;
        texto.setFont(font);
        texto.setString(getNombre());
        texto.setCharacterSize(14);
        texto.setFillColor(sf::Color::White);
        texto.setPosition(
            x + 10.0f,
            y + (POWERUP_HEIGHT - texto.getLocalBounds().height) / 2.0f
        );
        window.draw(texto);

        // Si está activo, mostrar turnos restantes
        if (turnosRestantes > 0) {
            sf::Text turnosText;
            turnosText.setFont(font);
            turnosText.setString("(" + std::to_string(turnosRestantes) + ")");
            turnosText.setCharacterSize(12);
            turnosText.setFillColor(sf::Color::Yellow);
            turnosText.setPosition(
                x + POWERUP_WIDTH - 30.0f,
                y + (POWERUP_HEIGHT - turnosText.getLocalBounds().height) / 2.0f
            );
            window.draw(turnosText);
        }
    }
}

bool PowerUp::contienePunto(float x, float y, float posX, float posY) const {
    return x >= posX && x <= posX + POWERUP_WIDTH &&
           y >= posY && y <= posY + POWERUP_HEIGHT;
}

std::string PowerUp::getNombre() const {
    switch(tipo) {
        case DOBLE_TURNO: return "Doble Turno";
        case PRECISION_MOVIMIENTO: return "Precision Mov.";
        case PRECISION_ATAQUE: return "Precision Ataque";
        case PODER_ATAQUE: return "Poder Ataque";
        default: return "None";
    }
}

std::vector<sf::Vector2f> PowerUp::calcularTrayectoriaAStar(
    sf::Vector2f inicio, sf::Vector2f destino, const Mapa& mapa,
    float tileSize, sf::Vector2f posicionMapa) {

    std::vector<sf::Vector2f> trayectoria;
    std::priority_queue<AStarNode> openList;
    std::vector<AStarNode*> allNodes;
    std::set<std::pair<int, int>> closedList;

    auto heuristica = [](sf::Vector2f a, sf::Vector2f b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    };

    auto esValido = [&](sf::Vector2f pos) {
        int fila = static_cast<int>((pos.y - posicionMapa.y) / tileSize);
        int columna = static_cast<int>((pos.x - posicionMapa.x) / tileSize);

        return fila >= 0 && fila < mapa.getFilas() &&
               columna >= 0 && columna < mapa.getColumnas() &&
               !mapa.hayObstaculo(fila, columna);
    };

    AStarNode* nodoInicial = new AStarNode(inicio, 0, heuristica(inicio, destino));
    openList.push(*nodoInicial);
    allNodes.push_back(nodoInicial);

    const std::vector<sf::Vector2f> direcciones = {
        sf::Vector2f(0, -1), sf::Vector2f(1, -1),
        sf::Vector2f(1, 0),  sf::Vector2f(1, 1),
        sf::Vector2f(0, 1),  sf::Vector2f(-1, 1),
        sf::Vector2f(-1, 0), sf::Vector2f(-1, -1)
    };

    bool encontrado = false;
    while (!openList.empty()) {
        AStarNode current = openList.top();
        openList.pop();

        int currentX = static_cast<int>((current.pos.x - posicionMapa.x) / tileSize);
        int currentY = static_cast<int>((current.pos.y - posicionMapa.y) / tileSize);

        if (closedList.find({currentX, currentY}) != closedList.end()) {
            continue;
        }
        closedList.insert({currentX, currentY});

        float distActual = std::sqrt(
            std::pow(current.pos.x - destino.x, 2) +
            std::pow(current.pos.y - destino.y, 2)
        );
        if (distActual < tileSize/2) {
            AStarNode* nodo = &current;
            while (nodo) {
                trayectoria.push_back(nodo->pos);
                nodo = nodo->parent;
            }
            std::reverse(trayectoria.begin(), trayectoria.end());
            encontrado = true;
            break;
        }

        for (const auto& dir : direcciones) {
            sf::Vector2f nuevaPos = current.pos + dir * tileSize;
            if (!esValido(nuevaPos)) continue;

            float nuevoG = current.g + std::sqrt(std::pow(dir.x, 2) + std::pow(dir.y, 2)) * tileSize;
            float nuevoH = heuristica(nuevaPos, destino);

            AStarNode* nuevoNodo = new AStarNode(nuevaPos, nuevoG, nuevoH, const_cast<AStarNode*>(&current));
            openList.push(*nuevoNodo);
            allNodes.push_back(nuevoNodo);
        }
    }

    if (!encontrado) {
        trayectoria = {inicio, destino};
    }

    for (auto* node : allNodes) {
        delete node;
    }

    return trayectoria;
}