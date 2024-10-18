#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include "Tank.h"
#include <vector>
#include <string>
#include <ranges>

class Interfaz {
    sf::Clock gameTimer;
    sf::Text currentTurnText;
    sf::RectangleShape moveButton, shootButton, powerUpButton;
    Tank* selectedTank;
    bool isShooting;

public:
    Interfaz(Mapa& mapa, std::string  nombre1, std::string  nombre2,
             const std::vector<Tank>& tanques1, const std::vector<Tank>& tanques2);
    void iniciarVentana();
    void actualizarInterfaz();
    void manejarSeleccionTanque(int x, int y);
    void manejarDisparo(int x, int y);
    void dibujarTrayectoriaBala(const std::vector<sf::Vector2f>& trayectoria);
    std::vector<sf::Vector2f> calcularTrayectoria(sf::Vector2f inicio, sf::Vector2f destino);
    void limpiarTrazos();
    void verificarCondicionesVictoria();
    static bool todosLosTanquesDestruidos(const std::vector<Tank>& tanques) ;
    static int contarTanquesVivos(const std::vector<Tank>& tanques) ;
    void mostrarPantallaVictoria(const std::string& ganador);



private:
    void procesarEventos();
    void actualizar(sf::Time tiempoTranscurrido);
    void dibujar();
    void dibujarMapa();
    void dibujarTanques();
    void configurarHUD();
    void dibujarHUD();
    void manejarClicIzquierdo(int x, int y);

    std::vector<sf::VertexArray> trazosDisparos;

    bool balaGolpeaTanque(const std::vector<sf::Vector2f>& trayectoria, sf::Vector2f posTanque);
    static float distancia(sf::Vector2f a, sf::Vector2f b);
    static int calcularDanio(Tank::Color colorAtacante, Tank::Color colorDefensor);
    void cambiarTurno();
    void dibujarInfoDanio();

    sf::RenderWindow window;
    Mapa& mapa;
    std::string nombreJugador1;
    std::string nombreJugador2;
    std::vector<Tank> tanquesJugador1;
    std::vector<Tank> tanquesJugador2;
    bool turnoJugador1;
    bool juegoTerminado{};

    sf::Texture fondoTexture;
    sf::Sprite fondoSprite;
    sf::Texture obstaculoTexture;
    sf::Sprite obstaculoSprite;
    sf::Texture tanqueAzulTexture;
    sf::Texture tanqueRojoTexture;
    sf::Texture tanqueCelesteTexture;
    sf::Texture tanqueAmarilloTexture;
    sf::Texture tanqueDestruidoTexture;
    sf::Text shootButtonText;
    sf::Texture balaTexture;
    sf::Sprite balaSprite;
    std::vector<sf::Sprite> balas;
    sf::VertexArray lineaTrayectoria;
    std::vector<sf::Vector2f> trayectoriaBala;
    int indiceAnimacionBala;
    sf::Clock relojAnimacion;


    sf::Font font;
    sf::Text textoJugador1;
    sf::Text textoJugador2;
    sf::Text textoTurno;
    sf::Text textoTiempo;

    sf::Clock reloj;
};

#endif // INTERFAZ_H