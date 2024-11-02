#ifndef POWERUP_H
#define POWERUP_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Mapa.h"

class PowerUp {
public:
    enum Type {
        DOBLE_TURNO,
        PRECISION_MOVIMIENTO,
        PRECISION_ATAQUE,
        PODER_ATAQUE,
        NONE
    };

    // Constructores
    PowerUp();
    explicit PowerUp(Type tipo);

    // Métodos de gestión
    void activar();
    void reducirTurno();
    static void generarPowerUpAleatorio(std::vector<PowerUp>& powerUps, float tiempoTranscurrido, float& tiempoUltimoPowerUp);
    static void actualizarPowerUps(std::vector<PowerUp>& powerUps);
    static void limpiarPowerUpsExpirados(std::vector<PowerUp>& powerUps);
    bool estaEnCola() const { return enCola; }
    void marcarEnCola(bool estado) { enCola = estado; }
    bool gastado() const { return usado && !activo; }

    // Métodos de efectos
    static bool tienePowerUpActivo(const std::vector<PowerUp>& powerUps, Type tipo);
    static int calcularDanioConPowerUp(int danioBase, const std::vector<PowerUp>& powerUps);
    static bool debeSaltarTurno(const std::vector<PowerUp>& powerUps);
    static std::vector<sf::Vector2f> calcularTrayectoriaAStar(sf::Vector2f inicio, sf::Vector2f destino,
                                                             const Mapa& mapa, float tileSize,
                                                             sf::Vector2f posicionMapa);

    // Métodos de renderizado
    void dibujar(sf::RenderWindow& window, const sf::Font& font, float x, float y, bool seleccionado = false, const std::string& nombreJugador = "") const;
    bool contienePunto(float x, float y, float posX, float posY) const;

    // Getters
    std::string getNombre() const;
    bool estaActivo() const { return activo; }
    Type getTipo() const { return tipo; }
    int getTurnosRestantes() const { return turnosRestantes; }

    // Constantes
    static constexpr float TIEMPO_ENTRE_POWERUPS = 30.0f;
    static constexpr int MAX_POWERUPS = 3;
    static constexpr int DURACION_DOBLE_TURNO = 2;
    static constexpr int DURACION_OTROS_POWERUPS = 1;
    static constexpr float POWERUP_WIDTH = 150.0f;
    static constexpr float POWERUP_HEIGHT = 40.0f;

private:
    Type tipo;
    bool activo;
    int turnosRestantes;
    bool usado = false;
    bool enCola = false;
    struct AStarNode {
        sf::Vector2f pos;
        float g;
        float h;
        float f;
        AStarNode* parent;

        AStarNode(sf::Vector2f pos, float g, float h, AStarNode* parent = nullptr)
            : pos(pos), g(g), h(h), f(g + h), parent(parent) {}

        bool operator<(const AStarNode& other) const {
            return f > other.f;
        }
    };
};

#endif