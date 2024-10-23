#include "Juego.h"
#include <SFML/Graphics.hpp>
#include <random>
#include "Interfaz.h"

Juego::Juego() : mapa(20, 20) {  // Tamaño del mapa ajustable
    // Inicialización del generador de números aleatorios
    std::random_device rd;
    gen = std::mt19937(rd());
}

void Juego::iniciar() {
    // Crear ventana para la pantalla inicial
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tank Attack! - Ingresar Nombres");

    sf::Font font;
    if (!font.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/PixelifySans-VariableFont_wght.ttf")) {
        throw std::runtime_error("No se pudo cargar la fuente");
    }

    // Textos para pedir nombres
    sf::Text texto1("Jugador 1: ", font, 30);
    texto1.setPosition(100, 200);
    sf::Text texto2("Jugador 2: ", font, 30);
    texto2.setPosition(100, 300);

    // Textos que se actualizarán con los nombres
    sf::Text nombre1("", font, 30);
    nombre1.setPosition(300, 200);
    sf::Text nombre2("", font, 30);
    nombre2.setPosition(300, 300);

    bool esJugador1 = true;
    std::string entradaNombre;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {  // Retroceso
                    if (!entradaNombre.empty()) entradaNombre.pop_back();
                } else if (event.text.unicode == '\r') {  // Enter
                    if (esJugador1) {
                        nombreJugador1 = entradaNombre;
                        nombre1.setString(nombreJugador1);
                        entradaNombre.clear();
                        esJugador1 = false;
                    } else {
                        nombreJugador2 = entradaNombre;
                        nombre2.setString(nombreJugador2);
                        window.close();  // Cerrar la ventana al completar ambos nombres
                    }
                } else {
                    entradaNombre += static_cast<char>(event.text.unicode);
                }
            }
        }

        // Actualizar texto de entrada según el jugador
        if (esJugador1) {
            nombre1.setString(entradaNombre);
        } else {
            nombre2.setString(entradaNombre);
        }

        window.clear(sf::Color::Black);
        window.draw(texto1);
        window.draw(nombre1);
        window.draw(texto2);
        window.draw(nombre2);
        window.display();
    }

    // Una vez obtenidos los nombres, iniciar el juego
    jugar();
}

void Juego::jugar() {
    // Jugador 1 (izquierda)
    tanquesJugador1.emplace_back(1, 1, Tank::AZUL);
    tanquesJugador1.emplace_back(5, 3, Tank::ROJO);
    tanquesJugador1.emplace_back(9, 3, Tank::AZUL);
    tanquesJugador1.emplace_back(14, 1, Tank::ROJO);

    // Jugador 2 (derecha)
    tanquesJugador2.emplace_back(1, 18, Tank::AMARILLO);
    tanquesJugador2.emplace_back(5, 16, Tank::CELESTE);
    tanquesJugador2.emplace_back(9, 16, Tank::AMARILLO);
    tanquesJugador2.emplace_back(14, 18, Tank::CELESTE);

    // Generar el mapa con obstáculos, asegurándose de que no interfiera con los tanques
    std::vector<Tank> todosLosTanques = tanquesJugador1;
    todosLosTanques.insert(todosLosTanques.end(), tanquesJugador2.begin(), tanquesJugador2.end());
    mapa.generarMapa(todosLosTanques);

    // Crear la instancia de Interfaz con el mapa y los tanques
    Interfaz interfaz(mapa, nombreJugador1, nombreJugador2, tanquesJugador1, tanquesJugador2);
    interfaz.iniciarVentana();
}


std::vector<std::pair<int, int>> Juego::generarPosicionesAleatorias() {
    std::vector<std::pair<int, int>> posiciones;
    std::uniform_int_distribution<> disX(0, mapa.getFilas() - 1);
    std::uniform_int_distribution<> disY(0, mapa.getColumnas() - 1);

    while (posiciones.size() < 8) {
        int x = disX(gen);
        int y = disY(gen);
        bool posicionValida = true;

        // Verificar que la posición no esté ya ocupada
        for (const auto& pos : posiciones) {
            if (pos.first == x && pos.second == y) {
                posicionValida = false;
                break;
            }
        }

        if (posicionValida) {
            posiciones.emplace_back(x, y);
        }
    }

    return posiciones;
}