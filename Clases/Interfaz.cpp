#include "Interfaz.h"
#include "Mapa.h"
#include "Tank.h"
#include "PowerUp.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <thread>

#include "Juego.h"

Interfaz::Interfaz(Mapa& mapa, std::string nombre1, std::string nombre2,
                   const std::vector<Tank>& tanques1, const std::vector<Tank>& tanques2)
    : window(sf::VideoMode(1000, 800), "Tank Attack!"),
      mapa(mapa),
      nombreJugador1(std::move(nombre1)),
      nombreJugador2(std::move(nombre2)),
      turnoJugador1(true),
      juegoTerminado(false),
      selectedTank(nullptr),
      selectedPowerUp(nullptr),
      isShooting(false),
      esperandoDestino(false),
      tiempoUltimoPowerUp(0.0f) {

    inicializarRecursos();

    // Copiar los tanques después de cargar las texturas
    tanquesJugador1 = tanques1;
    tanquesJugador2 = tanques2;

    configurarHUD();

    lineaTrayectoria.setPrimitiveType(sf::LineStrip);
    indiceAnimacionBala = 0;
}

void Interfaz::inicializarRecursos() {
    if (!font.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/PixelifySans-VariableFont_wght.ttf")) {
        throw std::runtime_error("No se pudo cargar la fuente");
    }

    // Cargar texturas antes de crear los tanques
    if (!tanqueAzulTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueAzul.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque azul");
    }
    if (!tanqueRojoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueRojo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque rojo");
    }
    if (!tanqueCelesteTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueCeleste.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque celeste");
    }
    if (!tanqueAmarilloTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueAmarillo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque amarillo");
    }
    if (!tanqueDestruidoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/explosion.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque destruido");
    }
    if (!balaTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/bala.png")) {
        throw std::runtime_error("No se pudo cargar la textura de la bala");
    }
    if (!fondoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/fondo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del fondo");
    }
    if (!obstaculoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/muro.png")) {
        throw std::runtime_error("No se pudo cargar la textura del obstáculo");
    }

    // Configurar sprites
    fondoSprite.setTexture(fondoTexture);
    fondoSprite.setScale(
        static_cast<float>(window.getSize().x) / static_cast<float>(fondoTexture.getSize().x),
        static_cast<float>(window.getSize().y) / static_cast<float>(fondoTexture.getSize().y)
    );
    obstaculoSprite.setTexture(obstaculoTexture);
    balaSprite.setTexture(balaTexture);
    balaSprite.setScale(0.05f, 0.05f);

    window.setVerticalSyncEnabled(true);
    calcularDimensionesMapa();
}

void Interfaz::iniciarVentana() {
    juegoTerminado = false;
    reloj.restart();
    gameTimer.restart();

    while (window.isOpen() && !juegoTerminado) {
        procesarEventos();
        actualizar(reloj.getElapsedTime());
        dibujar();
        verificarCondicionesVictoria();
    }
}

void Interfaz::procesarEventos() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
            break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    manejarClicIzquierdo(event.mouseButton.x, event.mouseButton.y);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    manejarClicDerecho(event.mouseButton.x, event.mouseButton.y);
                }
            break;
        }
    }
}

void Interfaz::manejarClicIzquierdo(int x, int y) {
    sf::Vector2i coordsMapa = obtenerCoordenadasMapa(static_cast<float>(x), static_cast<float>(y));

    // Verificar si las coordenadas están dentro del mapa
    if (coordsMapa.x >= 0 && coordsMapa.x < mapa.getFilas() &&
        coordsMapa.y >= 0 && coordsMapa.y < mapa.getColumnas()) {

        // Intentar seleccionar un tanque
        bool tanqueSeleccionado = false;
        auto& tanquesActuales = turnoJugador1 ? tanquesJugador1 : tanquesJugador2;

        for (auto& tanque : tanquesActuales) {
            if (tanque.getX() == coordsMapa.x && tanque.getY() == coordsMapa.y && tanque.getSalud() > 0) {
                selectedTank = &tanque;
                tanqueSeleccionado = true;
                std::cout << "Tanque seleccionado en (" << coordsMapa.x << "," << coordsMapa.y << ")" << std::endl;
                break;
            }
        }

        // Si no se seleccionó un tanque y hay uno seleccionado, mover
        if (!tanqueSeleccionado && selectedTank && selectedTank->getSalud() > 0) {
            std::cout << "Moviendo tanque a (" << coordsMapa.x << "," << coordsMapa.y << ")" << std::endl;
            realizarMovimiento(coordsMapa.x, coordsMapa.y);
        }
        }
}

void Interfaz::manejarClicDerecho(int x, int y) {
    if (!selectedTank || selectedTank->getSalud() <= 0) {
        std::cout << "No hay tanque seleccionado para disparar" << std::endl;
        return;
    }
    manejarDisparo(x, y);
}

void Interfaz::realizarMovimiento(int destinoX, int destinoY) {
    if (!selectedTank || selectedTank->getSalud() <= 0) return;

    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;

    std::cout << "\n=== Inicio de movimiento ===" << std::endl;
    std::cout << "Posición inicial: (" << selectedTank->getX() << "," << selectedTank->getY() << ")" << std::endl;
    std::cout << "Destino: (" << destinoX << "," << destinoY << ")" << std::endl;

    std::vector<std::pair<int, int>> ruta = selectedTank->calcularRuta(mapa, destinoX, destinoY, powerUps);

    if (!ruta.empty()) {
        std::cout << "Ruta encontrada con " << ruta.size() << " pasos" << std::endl;
        dibujarRutaTanque(ruta);
        for (const auto& pos : ruta) {
            selectedTank->setPos(pos.first, pos.second);
            dibujar();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        std::cout << "Movimiento completado" << std::endl;
    } else {
        std::cout << "No se pudo encontrar una ruta válida" << std::endl;
    }
    std::cout << "=== Fin de movimiento ===\n" << std::endl;

    cambiarTurno();
}

void Interfaz::actualizar(sf::Time tiempoTranscurrido) {
    // Actualizar animación de bala
    if (!trayectoriaBala.empty() && indiceAnimacionBala < trayectoriaBala.size()) {
        if (relojAnimacion.getElapsedTime().asMilliseconds() > 50) {
            balaSprite.setPosition(trayectoriaBala[indiceAnimacionBala]);
            indiceAnimacionBala++;
            relojAnimacion.restart();
        }
    }

    // Actualizar power-ups
    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;
    PowerUp::actualizarPowerUps(powerUps);
    PowerUp::generarPowerUpAleatorio(powerUps, tiempoTranscurrido.asSeconds(), tiempoUltimoPowerUp);

    actualizarInterfaz();
}

void Interfaz::dibujar() {
    window.clear(sf::Color::Black);

    // Aplicar escala
    float scale = calculateScale();
    sf::View view = window.getDefaultView();
    view.setSize(1000 * scale, 800 * scale);
    view.setCenter(500 * scale, 400 * scale);
    window.setView(view);

    // Dibujar elementos del juego
    window.draw(fondoSprite);
    dibujarMapa();
    dibujarTanques();
    dibujarHUD();

    // Dibujar power-ups
    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;
    float yOffset = 200.0f;
    for (auto& powerUp : powerUps) {
        powerUp.dibujar(window, font, 20.0f, yOffset);
        yOffset += 50.0f;
    }

    // Dibujar elementos de juego activos
    if (selectedTank) {
        dibujarIndicadorSeleccion();
    }

    window.draw(lineaTrayectoria);
    window.draw(trazadoRuta);

    if (!trayectoriaBala.empty() && indiceAnimacionBala < trayectoriaBala.size()) {
        window.draw(balaSprite);
    }

    dibujarInfoDanio();
    window.display();
}

void Interfaz::dibujarIndicadorSeleccion() {
    float tileSize = getTileSize();
    sf::CircleShape selector(tileSize / 4.0f);
    selector.setFillColor(sf::Color(0, 255, 0, 128));
    selector.setPosition(
        posicionMapa.x + (static_cast<float>(selectedTank->getY()) * tileSize) + (tileSize / 2.0f) - selector.getRadius(),
        posicionMapa.y + (static_cast<float>(selectedTank->getX()) * tileSize) + (tileSize / 2.0f) - selector.getRadius()
    );
    window.draw(selector);
}

void Interfaz::cambiarTurno() {
    limpiarEstadoTurno();

    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;
    if (!PowerUp::tienePowerUpActivo(powerUps, PowerUp::DOBLE_TURNO)) {
        turnoJugador1 = !turnoJugador1;
    }
}

void Interfaz::limpiarEstadoTurno() {
    limpiarTrazos();
    trazadoRuta.clear();
    selectedTank = nullptr;
    selectedPowerUp = nullptr;
    isShooting = false;
    esperandoDestino = false;
}

void Interfaz::dibujarMapa() {
    float tileSize = getTileSize();

    // Dibujar el borde del mapa
    sf::RectangleShape borde;
    borde.setSize(sf::Vector2f(
        tileSize * static_cast<float>(mapa.getColumnas()),
        tileSize * static_cast<float>(mapa.getFilas())
    ));
    borde.setPosition(posicionMapa.x, posicionMapa.y);
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineThickness(7.0f);
    borde.setOutlineColor(sf::Color::White);
    window.draw(borde);

    // Dibujar los obstáculos
    for (int i = 0; i < mapa.getFilas(); ++i) {
        for (int j = 0; j < mapa.getColumnas(); ++j) {
            if (mapa.hayObstaculo(i, j)) {
                obstaculoSprite.setPosition(
                    posicionMapa.x + (static_cast<float>(j) * tileSize),
                    posicionMapa.y + (static_cast<float>(i) * tileSize)
                );
                obstaculoSprite.setScale(
                    tileSize / static_cast<float>(obstaculoSprite.getTexture()->getSize().x),
                    tileSize / static_cast<float>(obstaculoSprite.getTexture()->getSize().y)
                );
                window.draw(obstaculoSprite);
            }
        }
    }
}

void Interfaz::dibujarTanques() {
    float tileSize = getTileSize();

    auto dibujarTanque = [&](const Tank& tanque) {
        sf::Sprite sprite;
        if (tanque.getSalud() <= 0) {
            sprite.setTexture(tanqueDestruidoTexture);
        } else {
            switch (tanque.getColor()) {
                case Tank::AZUL: sprite.setTexture(tanqueAzulTexture); break;
                case Tank::ROJO: sprite.setTexture(tanqueRojoTexture); break;
                case Tank::CELESTE: sprite.setTexture(tanqueCelesteTexture); break;
                case Tank::AMARILLO: sprite.setTexture(tanqueAmarilloTexture); break;
            }
        }
        sprite.setPosition(
            posicionMapa.x + (static_cast<float>(tanque.getY()) * tileSize),
            posicionMapa.y + (static_cast<float>(tanque.getX()) * tileSize)
        );
        sprite.setScale(
            tileSize / static_cast<float>(sprite.getTexture()->getSize().x),
            tileSize / static_cast<float>(sprite.getTexture()->getSize().y)
        );
        window.draw(sprite);
    };

    for (const auto& tanque : tanquesJugador1) dibujarTanque(tanque);
    for (const auto& tanque : tanquesJugador2) dibujarTanque(tanque);
}

void Interfaz::dibujarHUD() {
    float scale = calculateScale();

    // Dibujar fondo del HUD
    hudBackground.setSize(sf::Vector2f(1000 * scale, 60 * scale));
    window.draw(hudBackground);

    // Actualizar posiciones de texto
    textoJugador1.setPosition(10 * scale, 10 * scale);
    textoJugador2.setPosition(790 * scale, 10 * scale);
    textoTurno.setPosition(400 * scale, 10 * scale);
    textoTiempo.setPosition(400 * scale, 35 * scale);

    // Actualizar contenido de texto
    textoJugador1.setString("Jugador 1: " + nombreJugador1);
    textoJugador2.setString("Jugador 2: " + nombreJugador2);
    textoTurno.setString("Turno: " + (turnoJugador1 ? nombreJugador1 : nombreJugador2));

    // Actualizar tiempo
    sf::Time tiempoRestante = sf::seconds(300) - reloj.getElapsedTime();
    int minutos = static_cast<int>(tiempoRestante.asSeconds()) / 60;
    int segundos = static_cast<int>(tiempoRestante.asSeconds()) % 60;
    textoTiempo.setString("Tiempo: " + std::to_string(minutos) + ":" +
                         (segundos < 10 ? "0" : "") + std::to_string(segundos));

    // Dibujar textos
    window.draw(textoJugador1);
    window.draw(textoJugador2);
    window.draw(textoTurno);
    window.draw(textoTiempo);
}

void Interfaz::dibujarInfoDanio() {
    float tileSize = getTileSize();
    sf::Text textoSalud;
    textoSalud.setFont(font);
    textoSalud.setCharacterSize(12);
    textoSalud.setFillColor(sf::Color::White);

    auto dibujarSaludTanque = [&](const Tank& tanque) {
        textoSalud.setPosition(
            posicionMapa.x + (static_cast<float>(tanque.getY()) * tileSize),
            posicionMapa.y + (static_cast<float>(tanque.getX()) * tileSize - 20.0f)
        );
        textoSalud.setString(std::to_string(tanque.getSalud()) + "%");

        sf::RectangleShape fondoTexto;
        sf::FloatRect bounds = textoSalud.getLocalBounds();
        fondoTexto.setSize(sf::Vector2f(bounds.width + 4, bounds.height + 4));
        fondoTexto.setPosition(textoSalud.getPosition());
        fondoTexto.setFillColor(sf::Color(0, 0, 0, 150));

        window.draw(fondoTexto);
        window.draw(textoSalud);
    };

    for (const auto& tanque : tanquesJugador1) dibujarSaludTanque(tanque);
    for (const auto& tanque : tanquesJugador2) dibujarSaludTanque(tanque);
}

void Interfaz::verificarCondicionesVictoria() {
    auto todosDestruidos = [](const std::vector<Tank>& tanques) {
        return std::all_of(tanques.begin(), tanques.end(),
                          [](const Tank& t) { return t.getSalud() <= 0; });
    };

    auto contarVivos = [](const std::vector<Tank>& tanques) {
        return std::count_if(tanques.begin(), tanques.end(),
                           [](const Tank& t) { return t.getSalud() > 0; });
    };

    if (todosDestruidos(tanquesJugador1)) {
        mostrarPantallaVictoria(nombreJugador2);
        juegoTerminado = true;
    } else if (todosDestruidos(tanquesJugador2)) {
        mostrarPantallaVictoria(nombreJugador1);
        juegoTerminado = true;
    } else if (reloj.getElapsedTime().asSeconds() >= 300) {
        int vivosJ1 = contarVivos(tanquesJugador1);
        int vivosJ2 = contarVivos(tanquesJugador2);

        if (vivosJ1 > vivosJ2) {
            mostrarPantallaVictoria(nombreJugador1);
        } else if (vivosJ2 > vivosJ1) {
            mostrarPantallaVictoria(nombreJugador2);
        } else {
            mostrarPantallaVictoria("Empate");
        }
        juegoTerminado = true;
    }
}

void Interfaz::mostrarPantallaVictoria(const std::string& ganador) {
    sf::Text textoVictoria;
    textoVictoria.setFont(font);
    textoVictoria.setCharacterSize(40);
    textoVictoria.setFillColor(sf::Color::Yellow);
    textoVictoria.setString(ganador == "Empate" ? "¡Empate!" : "¡" + ganador + " ha ganado!");

    textoVictoria.setPosition(
        (static_cast<float>(window.getSize().x) - textoVictoria.getLocalBounds().width) / 2.0f,
        (static_cast<float>(window.getSize().y) - textoVictoria.getLocalBounds().height) / 2.0f - 50.0f
    );

    sf::RectangleShape botonNuevaPartida;
    botonNuevaPartida.setSize(sf::Vector2f(200, 50));
    botonNuevaPartida.setFillColor(sf::Color::White);
    botonNuevaPartida.setPosition(
        (static_cast<float>(window.getSize().x) - 200.0f) / 2.0f,
        textoVictoria.getPosition().y + 100.0f
    );

    sf::Text textoBoton;
    textoBoton.setFont(font);
    textoBoton.setString("Nueva Partida");
    textoBoton.setCharacterSize(24);
    textoBoton.setFillColor(sf::Color::Black);
    textoBoton.setPosition(
        botonNuevaPartida.getPosition().x + (200.0f - textoBoton.getLocalBounds().width) / 2.0f,
        botonNuevaPartida.getPosition().y + (50.0f - textoBoton.getLocalBounds().height) / 2.0f
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (botonNuevaPartida.getGlobalBounds().contains(
                            static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y))) {
                        window.close();
                        Juego nuevoJuego;
                        nuevoJuego.iniciar();
                        return;
                    }
                }
            }
        }

        window.clear();
        window.draw(textoVictoria);
        window.draw(botonNuevaPartida);
        window.draw(textoBoton);
        window.display();
    }
}

float Interfaz::calculateScale() {
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1000.0f;
    float scaleY = static_cast<float>(windowSize.y) / 800.0f;
    return std::min(scaleX, scaleY);
}

void Interfaz::configurarTexto(sf::Text& texto, const std::string& contenido,
                             unsigned int tamanio, float x, float y) {
    texto.setFont(font);
    texto.setString(contenido);
    texto.setCharacterSize(tamanio);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(x, y);
}


void Interfaz::actualizarInterfaz() {
    currentTurnText.setString("Turno: " + (turnoJugador1 ? nombreJugador1 : nombreJugador2));

    sf::Time tiempoRestante = sf::seconds(300) - gameTimer.getElapsedTime();
    int minutos = static_cast<int>(tiempoRestante.asSeconds()) / 60;
    int segundos = static_cast<int>(tiempoRestante.asSeconds()) % 60;
    textoTiempo.setString("Tiempo: " + std::to_string(minutos) + ":" +
                         (segundos < 10 ? "0" : "") + std::to_string(segundos));
}

void Interfaz::limpiarTrazos() {
    trazosDisparos.clear();
    trazadoRuta.clear();
    rutaActual.clear();
}

void Interfaz::configurarHUD() {
    float scale = calculateScale();
    textoJugador1.setFont(font);
    textoJugador1.setString(nombreJugador1);
    textoJugador1.setCharacterSize(22);
    textoJugador1.setPosition(10, 10);
    textoJugador1.setFillColor(sf::Color::Black);

    textoJugador2.setFont(font);
    textoJugador2.setString(nombreJugador2);
    textoJugador2.setCharacterSize(22);
    textoJugador2.setPosition(static_cast<float>(window.getSize().x) - 200.0f, 10.0f);
    textoJugador2.setFillColor(sf::Color::Black);

    textoTurno.setFont(font);
    textoTurno.setCharacterSize(22);
    textoTurno.setPosition(static_cast<float>(window.getSize().x) / 2 - 50, 10);
    textoTurno.setFillColor(sf::Color::Black);

    textoTiempo.setFont(font);
    textoTiempo.setCharacterSize(22);
    textoTiempo.setPosition(static_cast<float>(window.getSize().x) / 2 - 50, 40);
    textoTiempo.setFillColor(sf::Color::Black);

    hudBackground.setSize(sf::Vector2f(1000 * scale, 60 * scale));
    hudBackground.setFillColor(sf::Color(0, 0, 0, 180));

    configurarTexto(textoJugador1, nombreJugador1, 22 * scale, 10 * scale, 10 * scale);
    configurarTexto(textoJugador2, nombreJugador2, 22 * scale, 790 * scale, 10 * scale);
    configurarTexto(textoTurno, "Turno: ", 22 * scale, 400 * scale, 10 * scale);
    configurarTexto(textoTiempo, "Tiempo: 5:00", 22 * scale, 400 * scale, 35 * scale);
}

void Interfaz::dibujarRutaTanque(const std::vector<std::pair<int, int>>& ruta) {
    if (ruta.empty()) return;

    float tileSize = getTileSize();
    trazadoRuta.clear();
    trazadoRuta.setPrimitiveType(sf::LinesStrip);
    trazadoRuta.resize(ruta.size());

    for (size_t i = 0; i < ruta.size(); ++i) {
        float x = posicionMapa.x + (static_cast<float>(ruta[i].second) + 0.5f) * tileSize;
        float y = posicionMapa.y + (static_cast<float>(ruta[i].first) + 0.5f) * tileSize;
        trazadoRuta.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(255, 255, 0, 128)));
    }

    rutaActual = ruta;
}

PowerUp* Interfaz::obtenerPowerUpEnPosicion(float x, float y) {
    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;
    float yOffset = 200.0f;
    float xOffset = 20.0f;

    for (auto& powerUp : powerUps) {
        if (!powerUp.estaActivo()) {
            sf::FloatRect bounds(xOffset, yOffset, 150, 40);
            if (bounds.contains(x, y)) {
                return &powerUp;
            }
            yOffset += 50.0f;
        }
    }
    return nullptr;
}

void Interfaz::manejarDisparo(int destinoX, int destinoY) {
    if (selectedTank == nullptr || selectedTank->getSalud() <= 0) {
        std::cout << "No se puede disparar: tanque no seleccionado o destruido" << std::endl;
        return;
    }

    float tileSize = getTileSize();
    sf::Vector2f inicio(
        posicionMapa.x + (static_cast<float>(selectedTank->getY()) * tileSize + tileSize / 2.0f),
        posicionMapa.y + (static_cast<float>(selectedTank->getX()) * tileSize + tileSize / 2.0f)
    );
    sf::Vector2f destino(static_cast<float>(destinoX), static_cast<float>(destinoY));

    std::vector<sf::Vector2f> trayectoria = calcularTrayectoria(inicio, destino);
    dibujarTrayectoriaBala(trayectoria);

    bool impactoRegistrado = false;

    // Verificar impacto en tanques enemigos
    std::vector<Tank>& tanquesEnemigos = turnoJugador1 ? tanquesJugador2 : tanquesJugador1;
    for (auto& tanque : tanquesEnemigos) {
        if (tanque.getSalud() <= 0) continue;

        sf::Vector2f posTanque(
            posicionMapa.x + (static_cast<float>(tanque.getY()) * tileSize + tileSize / 2.0f),
            posicionMapa.y + (static_cast<float>(tanque.getX()) * tileSize + tileSize / 2.0f)
        );

        if (balaGolpeaTanque(trayectoria, posTanque)) {
            tanque.recibirDanio(calcularDanio(selectedTank->getColor(), tanque.getColor()));
            impactoRegistrado = true;
            break;
        }
    }

    // Verificar impacto en el tanque que disparó (después de rebote)
    if (!impactoRegistrado && trayectoria.size() > 2) {  // Solo si hay rebotes
        sf::Vector2f posTanqueDisparo(
            posicionMapa.x + (static_cast<float>(selectedTank->getY()) * tileSize + tileSize / 2.0f),
            posicionMapa.y + (static_cast<float>(selectedTank->getX()) * tileSize + tileSize / 2.0f)
        );

        // Crear una trayectoria que excluya el punto de disparo y un pequeño margen
        std::vector<sf::Vector2f> trayectoriaRebote;
        size_t puntoInicio = std::min(size_t(3), trayectoria.size()); // Saltar los primeros puntos
        trayectoriaRebote.insert(trayectoriaRebote.end(),
                                trayectoria.begin() + puntoInicio,
                                trayectoria.end());

        if (!trayectoriaRebote.empty() && balaGolpeaTanque(trayectoriaRebote, posTanqueDisparo)) {
            selectedTank->recibirDanio(calcularDanio(selectedTank->getColor(), selectedTank->getColor()));
            impactoRegistrado = true;
        }
    }

    verificarCondicionesVictoria();
    if (!juegoTerminado) {
        cambiarTurno();
    }
}

std::vector<sf::Vector2f> Interfaz::calcularTrayectoria(sf::Vector2f inicio, sf::Vector2f destino) {
    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;
    bool tienePrecisionAtaque = std::any_of(powerUps.begin(), powerUps.end(),
        [](const PowerUp& p) { return p.estaActivo() && p.getTipo() == PowerUp::PRECISION_ATAQUE; });

    if (tienePrecisionAtaque) {
        return PowerUp::calcularTrayectoriaAStar(inicio, destino, mapa, getTileSize(), posicionMapa);
    }

    std::vector<sf::Vector2f> trayectoria;
    trayectoria.push_back(inicio);

    sf::Vector2f direccion = destino - inicio;
    float longitud = std::sqrt(direccion.x * direccion.x + direccion.y * direccion.y);
    direccion /= longitud;

    sf::Vector2f posActual = inicio;
    int maxRebotes = 3;
    int rebotesActuales = 0;

    while (rebotesActuales <= maxRebotes) {
        sf::Vector2f siguientePosicion = posActual + direccion * 10.0f;
        float tileSize = getTileSize();

        int filaObstaculo = static_cast<int>((siguientePosicion.y - posicionMapa.y) / tileSize);
        int columnaObstaculo = static_cast<int>((siguientePosicion.x - posicionMapa.x) / tileSize);

        bool colisionBorde = false;
        sf::Vector2f normal(0, 0);

        if (siguientePosicion.x < posicionMapa.x) {
            normal = sf::Vector2f(1, 0);
            colisionBorde = true;
        }
        else if (siguientePosicion.x > posicionMapa.x + mapa.getColumnas() * tileSize) {
            normal = sf::Vector2f(-1, 0);
            colisionBorde = true;
        }
        else if (siguientePosicion.y < posicionMapa.y) {
            normal = sf::Vector2f(0, 1);
            colisionBorde = true;
        }
        else if (siguientePosicion.y > posicionMapa.y + mapa.getFilas() * tileSize) {
            normal = sf::Vector2f(0, -1);
            colisionBorde = true;
        }

        bool colisionObstaculo = filaObstaculo >= 0 && filaObstaculo < mapa.getFilas() &&
                                columnaObstaculo >= 0 && columnaObstaculo < mapa.getColumnas() &&
                                mapa.hayObstaculo(filaObstaculo, columnaObstaculo);

        if (colisionBorde || colisionObstaculo) {
            if (colisionObstaculo) {
                if (std::abs(siguientePosicion.x - posActual.x) > std::abs(siguientePosicion.y - posActual.y)) {
                    normal = sf::Vector2f(1, 0);
                } else {
                    normal = sf::Vector2f(0, 1);
                }
            }

            direccion = direccion - 2.0f * (direccion.x * normal.x + direccion.y * normal.y) * normal;
            rebotesActuales++;

            if (rebotesActuales > maxRebotes) break;
        }

        posActual = siguientePosicion;
        trayectoria.push_back(posActual);

        if (longitud <= 5.0f) break;
        longitud -= 10.0f;
    }

    return trayectoria;
}

void Interfaz::manejarSeleccionTanque(int filaClick, int columnaClick) {
    for (auto& tanque : (turnoJugador1 ? tanquesJugador1 : tanquesJugador2)) {
        if (tanque.getX() == filaClick && tanque.getY() == columnaClick && tanque.getSalud() > 0) {
            selectedTank = &tanque;
            return;
        }
    }
    selectedTank = nullptr;
}

void Interfaz::manejarClickDerecho(int x, int y) {
    if (esperandoDestino) {
        int filasMapa = mapa.getFilas();
        int columnasMapa = mapa.getColumnas();
        float tamañoCelda = static_cast<float>(window.getSize().x) / columnasMapa;

        int mapaX = static_cast<int>(x / tamañoCelda);
        int mapaY = static_cast<int>(y / tamañoCelda);

        realizarMovimiento(mapaX, mapaY);
        esperandoDestino = false;
        return;
    }
}

void Interfaz::dibujarTrayectoriaBala(const std::vector<sf::Vector2f>& trayectoria) {
    trayectoriaBala = trayectoria;
    lineaTrayectoria.clear();
    for (const auto& punto : trayectoria) {
        lineaTrayectoria.append(sf::Vertex(punto, sf::Color::Yellow));
    }
    indiceAnimacionBala = 0;
    relojAnimacion.restart();
}

bool Interfaz::balaGolpeaTanque(const std::vector<sf::Vector2f>& trayectoria, sf::Vector2f posTanque) {
    float tileSize = getTileSize();
    float radioColision = tileSize / 3.0f;

    // Para cada segmento de la trayectoria
    for (size_t i = 0; i < trayectoria.size() - 1; ++i) {
        sf::Vector2f puntoActual = trayectoria[i];
        sf::Vector2f puntoSiguiente = trayectoria[i + 1];

        // Calcular el vector dirección del segmento
        sf::Vector2f direccion = puntoSiguiente - puntoActual;
        float longitudSegmento = std::sqrt(direccion.x * direccion.x + direccion.y * direccion.y);

        if (longitudSegmento == 0) continue;

        // Normalizar el vector dirección
        direccion /= longitudSegmento;

        // Vector del punto actual al tanque
        sf::Vector2f alTanque = posTanque - puntoActual;

        // Proyección del vector al tanque sobre la dirección del segmento
        float proyeccion = alTanque.x * direccion.x + alTanque.y * direccion.y;

        // Encontrar el punto más cercano en el segmento
        sf::Vector2f puntoCercano;
        if (proyeccion < 0) {
            puntoCercano = puntoActual;
        } else if (proyeccion > longitudSegmento) {
            puntoCercano = puntoSiguiente;
        } else {
            puntoCercano = puntoActual + direccion * proyeccion;
        }

        // Calcular la distancia entre el punto más cercano y el tanque
        float distancia = std::sqrt(
            std::pow(puntoCercano.x - posTanque.x, 2) +
            std::pow(puntoCercano.y - posTanque.y, 2)
        );

        if (distancia < radioColision) {
            return true;
        }
    }
    return false;
}

int Interfaz::calcularDanio(Tank::Color colorAtacante, Tank::Color colorDefensor) {
    std::vector<PowerUp>& powerUps = turnoJugador1 ? powerUpsJugador1 : powerUpsJugador2;
    bool tienePowerAtaque = std::any_of(powerUps.begin(), powerUps.end(),
        [](const PowerUp& p) { return p.estaActivo() && p.getTipo() == PowerUp::PODER_ATAQUE; });

    if (tienePowerAtaque) {
        return 100;
    }

    if (colorDefensor == Tank::CELESTE || colorDefensor == Tank::AZUL) {
        return 25;
    } else if (colorDefensor == Tank::AMARILLO || colorDefensor == Tank::ROJO) {
        return 50;
    }
    return 0;
}