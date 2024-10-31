#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include "Tank.h"
#include "PowerUp.h"
#include <vector>
#include <string>

class Interfaz {
public:
    // Constructor
    Interfaz(Mapa& mapa, std::string nombre1, std::string nombre2,
             const std::vector<Tank>& tanques1, const std::vector<Tank>& tanques2);

    // Metodo principal
    void iniciarVentana();

private:
    // Atributos principales
    sf::RenderWindow window;
    Mapa& mapa;
    std::string nombreJugador1;
    std::string nombreJugador2;
    std::vector<Tank> tanquesJugador1;
    std::vector<Tank> tanquesJugador2;
    bool turnoJugador1;
    bool juegoTerminado;

    // Control de tiempo y estado
    sf::Clock reloj;
    sf::Clock gameTimer;
    float tiempoUltimoPowerUp;
    static constexpr float TIEMPO_ENTRE_POWERUPS = 30.0f;

    // Elementos de interfaz
    sf::Text currentTurnText;
    sf::RectangleShape moveButton;
    sf::Text moveButtonText;
    sf::RectangleShape shootButton;
    sf::RectangleShape powerUpButton;
    sf::Text shootButtonText;
    sf::Text powerUpButtonText;
    sf::Font font;
    sf::Text textoJugador1;
    sf::Text textoJugador2;
    sf::Text textoTurno;
    sf::Text textoTiempo;
    sf::RectangleShape hudBackground;

    // Control de selección y estados
    Tank* selectedTank;
    PowerUp* selectedPowerUp;
    bool isShooting;
    bool esperandoDestino;

    // Recursos gráficos
    sf::Texture fondoTexture;
    sf::Sprite fondoSprite;
    sf::Texture obstaculoTexture;
    sf::Sprite obstaculoSprite;
    sf::Texture tanqueAzulTexture;
    sf::Texture tanqueRojoTexture;
    sf::Texture tanqueCelesteTexture;
    sf::Texture tanqueAmarilloTexture;
    sf::Texture tanqueDestruidoTexture;
    sf::Texture balaTexture;
    sf::Sprite balaSprite;

    // Elementos de animación y visualización
    std::vector<sf::Sprite> balas;
    sf::VertexArray lineaTrayectoria;
    std::vector<sf::Vector2f> trayectoriaBala;
    int indiceAnimacionBala;
    sf::Clock relojAnimacion;
    sf::VertexArray trazadoRuta;
    std::vector<std::pair<int, int>> rutaActual;
    std::vector<sf::VertexArray> trazosDisparos;

    // Power-ups
    std::vector<PowerUp> powerUpsJugador1;
    std::vector<PowerUp> powerUpsJugador2;

    // Constantes de layout
    const float MARGEN_LATERAL = 40.0f;
    const float MARGEN_SUPERIOR = 70.0f;
    const float MARGEN_INFERIOR = 60.0f;
    float tamanioMapa;
    sf::Vector2f posicionMapa;

    // Métodos de manejo de eventos
    void procesarEventos();
    void manejarClicIzquierdo(int x, int y);
    void manejarClickDerecho(int x, int y);
    void manejarSeleccionTanque(int x, int y);
    void manejarDisparo(int x, int y);
    void limpiarEstadoTurno();
    void realizarMovimiento(int destinoX, int destinoY);

    // Métodos de actualización y dibujo
    void actualizar(sf::Time tiempoTranscurrido);
    void actualizarInterfaz();
    void dibujar();
    void dibujarMapa();
    void dibujarTanques();
    void dibujarHUD();
    void dibujarInfoDanio();
    void dibujarRutaTanque(const std::vector<std::pair<int, int>>& ruta);
    void dibujarTrayectoriaBala(const std::vector<sf::Vector2f>& trayectoria);
    void limpiarTrazos();
    void dibujarIndicadorSeleccion();

    // Métodos de cálculo y pathfinding
    std::vector<sf::Vector2f> calcularTrayectoria(sf::Vector2f inicio, sf::Vector2f destino);


    // Métodos de configuración
    void configurarHUD();
    void configurarBotones();
    void inicializarRecursos();
    void configurarTexto(sf::Text& texto, const std::string& contenido, unsigned int tamanio, float x, float y);
    float getTileSize() const {
        return tamanioMapa / static_cast<float>(mapa.getColumnas());
    }
    sf::Vector2i obtenerCoordenadasMapa(float screenX, float screenY) {
        float tileSize = getTileSize();
        int fila = static_cast<int>((screenY - posicionMapa.y) / tileSize);
        int columna = static_cast<int>((screenX - posicionMapa.x) / tileSize);
        return {fila, columna};
    }
    auto calculateScale() -> float;
    void calcularDimensionesMapa() {
        float alturaDisponible = window.getSize().y - MARGEN_SUPERIOR - MARGEN_INFERIOR;
        float anchoDisponible = window.getSize().x - (2 * MARGEN_LATERAL);

        // Usar el menor valor para mantener celdas cuadradas
        tamanioMapa = std::min(alturaDisponible, anchoDisponible);

        // Calcular la posición para centrar el mapa
        posicionMapa.x = (window.getSize().x - tamanioMapa) / 2.0f;
        posicionMapa.y = MARGEN_SUPERIOR;
    }

    // Métodos de power-up
    PowerUp* obtenerPowerUpEnPosicion(float x, float y);

    // Métodos de control de juego
    void verificarCondicionesVictoria();
    void mostrarPantallaVictoria(const std::string& ganador);
    void cambiarTurno();

    // Métodos de utilidad
    bool balaGolpeaTanque(const std::vector<sf::Vector2f>& trayectoria, sf::Vector2f posTanque);
    int calcularDanio(Tank::Color colorAtacante, Tank::Color colorDefensor);
};

#endif