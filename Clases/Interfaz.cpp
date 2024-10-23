#include "Interfaz.h"
#include "Mapa.h"
#include "Tank.h"
#include <cmath>
#include <algorithm> // Para sort, shuffle, etc.
#include <random>    // Para sample, shuffle, etc.
#include <iterator>  // Para std::move (si es necesario)
#include <iostream>
#include <utility>
#include "Juego.h"

Interfaz::Interfaz(Mapa& mapa, std::string  nombre1, std::string  nombre2,
                   const std::vector<Tank>& tanques1, const std::vector<Tank>& tanques2)
    : window(sf::VideoMode(1000, 800), "Tank Attack!"),
      mapa(mapa),
      nombreJugador1(std::move(nombre1)),
      nombreJugador2(std::move(nombre2)),
      tanquesJugador1(tanques1),
      tanquesJugador2(tanques2),
      turnoJugador1(true) {

    shootButton.setSize(sf::Vector2f(100, 40));
    shootButton.setFillColor(sf::Color::White);
    shootButton.setPosition(static_cast<float>(window.getSize().x) - 120.0f, static_cast<float>(window.getSize().y) - 70.0f);
    shootButtonText.setFont(font);
    shootButtonText.setString("Disparar");

    // Configuración del botón "Mover"
    moveButton.setSize(sf::Vector2f(100, 40)); // Tamaño del botón
    moveButton.setFillColor(sf::Color::White); // Color del botón
    moveButton.setPosition(static_cast<float>(window.getSize().x) - 240.0f, static_cast<float>(window.getSize().y) - 70.0f); // Posición del botón
    moveButtonText.setFont(font);
    moveButtonText.setString("Mover");
    moveButtonText.setCharacterSize(20);
    moveButtonText.setFillColor(sf::Color::Black);
    moveButtonText.setPosition(static_cast<float>(window.getSize().x) - 210, static_cast<float>(window.getSize().y) - 65);


    shootButtonText.setCharacterSize(20);
    shootButtonText.setFillColor(sf::Color::Black);
    shootButtonText.setPosition(static_cast<float>(window.getSize().x)- 110, static_cast<float>(window.getSize().y) - 65);

    currentTurnText.setFont(font);
    currentTurnText.setCharacterSize(20);
    currentTurnText.setPosition(static_cast<float>(window.getSize().x) / 2 - 50, 70);

    selectedTank = nullptr;
    isShooting = false;

    if (!fondoTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/fondo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del fondo");
    }
    fondoSprite.setTexture(fondoTexture);
    fondoSprite.setScale(
    static_cast<float>(window.getSize().x) / static_cast<float>(fondoTexture.getSize().x),
    static_cast<float>(window.getSize().y) / static_cast<float>(fondoTexture.getSize().y)
    );

    if (!font.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/PixelifySans-VariableFont_wght.ttf")) {
        throw std::runtime_error("No se pudo cargar la fuente");
    }

    if (!obstaculoTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/muro.png")) {
        throw std::runtime_error("No se pudo cargar la textura del obstáculo");
    }
    obstaculoSprite.setTexture(obstaculoTexture);

    if (!tanqueAzulTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/tanqueAzul.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque azul");
    }
    if (!tanqueRojoTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/tanqueRojo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque rojo");
    }
    if (!tanqueCelesteTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/tanqueCeleste.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque celeste");
    }
    if (!tanqueAmarilloTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/tanqueAmarillo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque amarillo");
    }
    if (!tanqueDestruidoTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/explosion.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque destruido");
    }
    if (!balaTexture.loadFromFile("/home/mau/Escritorio/Proyecto ii Datos ii/TANK-ATTACK-P2-D2/recursos/bala.png")) {
        throw std::runtime_error("No se pudo cargar la textura de la bala");
    }
    balaSprite.setTexture(balaTexture);
    balaSprite.setScale(0.05f, 0.05f);
    lineaTrayectoria.setPrimitiveType(sf::LineStrip);
    indiceAnimacionBala = 0;

    configurarHUD();
}

void Interfaz::iniciarVentana() {
    juegoTerminado = false;
    reloj.restart();

    while (window.isOpen() && !juegoTerminado) {
        sf::Time tiempoTranscurrido = reloj.getElapsedTime();

        procesarEventos();
        actualizar(tiempoTranscurrido);
        dibujar();
        verificarCondicionesVictoria();
    }
}

void Interfaz::procesarEventos() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                manejarClicIzquierdo(event.mouseButton.x, event.mouseButton.y);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                manejarClickDerecho(event.mouseButton.x, event.mouseButton.y);
            }
        }

    }
}



void Interfaz::actualizar(sf::Time tiempoTranscurrido) {
    if (!trayectoriaBala.empty() && indiceAnimacionBala < trayectoriaBala.size()) {
        if (relojAnimacion.getElapsedTime().asMilliseconds() > 50) { // Ajusta la velocidad de la animación
            balaSprite.setPosition(trayectoriaBala[indiceAnimacionBala]);
            indiceAnimacionBala++;
            relojAnimacion.restart();
        }
    }
}

void Interfaz::actualizarInterfaz() {
    // Update turn text
    currentTurnText.setString("Turno: " + (turnoJugador1 ? nombreJugador1 : nombreJugador2));

    // Update game timer
    sf::Time tiempoRestante = sf::seconds(300) - gameTimer.getElapsedTime();
    int minutos = static_cast<int>(tiempoRestante.asSeconds()) / 60;
    int segundos = static_cast<int>(tiempoRestante.asSeconds()) % 60;
    textoTiempo.setString("Tiempo: " + std::to_string(minutos) + ":" +
                          (segundos < 10 ? "0" : "") + std::to_string(segundos));
}

void Interfaz::manejarSeleccionTanque(int x, int y) {
    float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());
    int fila = static_cast<int>(static_cast<float>(y) / tileSize);
    int columna = static_cast<int>(static_cast<float>(x) / tileSize);

    for (auto& tanque : (turnoJugador1 ? tanquesJugador1 : tanquesJugador2)) {
        if (tanque.getX() == fila && tanque.getY() == columna && tanque.getSalud() > 0) {
            selectedTank = &tanque;
            return;
        }
    }
    selectedTank = nullptr;
}
void Interfaz::manejarMovimiento(int destinoX, int destinoY) {
    if (selectedTank == nullptr || selectedTank->getSalud() <= 0) {
        std::cout << "No se puede mover: tanque no seleccionado o destruido" << std::endl;
        return;
    }

    // Verifica si la posición de destino está dentro del mapa y no es un obstáculo
    if (destinoX < 0 || destinoX >= mapa.getFilas() ||
        destinoY < 0 || destinoY >= mapa.getColumnas() ||
        mapa.hayObstaculo(destinoX, destinoY)) {
        std::cout << "Movimiento inválido: fuera de límites o posición con obstáculo" << std::endl;
        return;

        }

    // Establecer el destino y mover el tanque
    selectedTank->setPos(destinoX, destinoY);
    selectedTank->mover(mapa);
}

void Interfaz::manejarDisparo(int destinoX, int destinoY) {
        if (selectedTank == nullptr || selectedTank->getSalud() <= 0) {
            std::cout << "No se puede disparar: tanque no seleccionado o destruido" << std::endl;
            return;
        }

        std::cout << "Disparo realizado a (" << destinoX << ", " << destinoY << ")" << std::endl;
        float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());
        sf::Vector2f inicio(static_cast<float>(selectedTank->getY()) * tileSize + tileSize / 2.0f, static_cast<float>(selectedTank->getX()) * tileSize + tileSize / 2.0f);
        sf::Vector2f destino(static_cast<float>(destinoX), static_cast<float>(destinoY));

        std::vector<sf::Vector2f> trayectoria = calcularTrayectoria(inicio, destino);
        dibujarTrayectoriaBala(trayectoria);

        // Aplicar daño si la bala golpea a un tanque
        for (auto& tanque : (turnoJugador1 ? tanquesJugador2 : tanquesJugador1)) {
            sf::Vector2f posTanque(static_cast<float>(tanque.getY()) * tileSize + tileSize / 2.0f, static_cast<float>(tanque.getX()) * tileSize + tileSize / 2.0f);
            if (balaGolpeaTanque(trayectoria, posTanque)) {
                int danio = calcularDanio(selectedTank->getColor(), tanque.getColor());
                tanque.recibirDanio(danio);
                break; // La bala solo golpea al primer tanque que encuentra
            }
        }

        // La bala puede dañar al tanque que la disparó
        sf::Vector2f posTanqueDisparo(static_cast<float>(selectedTank->getY()) * tileSize + tileSize / 2.0f, static_cast<float>(selectedTank->getX()) * tileSize + tileSize / 2.0f);
        if (balaGolpeaTanque(trayectoria, posTanqueDisparo)) {
            int danio = calcularDanio(selectedTank->getColor(), selectedTank->getColor());
            selectedTank->recibirDanio(danio);
        }

        verificarCondicionesVictoria();
        if (!juegoTerminado) {
            cambiarTurno();
        }
    }

std::vector<sf::Vector2f> Interfaz::calcularTrayectoria(sf::Vector2f inicio, sf::Vector2f destino) {
    std::vector<sf::Vector2f> trayectoria;
    trayectoria.push_back(inicio);

    sf::Vector2f direccion = destino - inicio;
    float longitud = std::sqrt(direccion.x * direccion.x + direccion.y * direccion.y);
    direccion /= longitud;

    sf::Vector2f posActual = inicio;
    int maxRebotes = 3; // Maximum number of rebounds
    int rebotesActuales = 0;

    while (rebotesActuales <= maxRebotes) {
        sf::Vector2f siguientePosicion = posActual + direccion * 10.0f; // Move in small steps

        float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());

        // Check collision with obstacles
        int filaObstaculo = static_cast<int>(siguientePosicion.y / tileSize);
        int columnaObstaculo = static_cast<int>(siguientePosicion.x / tileSize);

        if (mapa.hayObstaculo(filaObstaculo, columnaObstaculo)) {
            // Calculate rebound
            sf::Vector2f normal;
            if (std::abs(siguientePosicion.x - posActual.x) > std::abs(siguientePosicion.y - posActual.y)) {
                normal = sf::Vector2f(1, 0);
            } else {
                normal = sf::Vector2f(0, 1);
            }
            direccion = direccion - 2.0f * (direccion.x * normal.x + direccion.y * normal.y) * normal;
            rebotesActuales++;
            if (rebotesActuales > maxRebotes) break;
        }

        posActual = siguientePosicion;
        trayectoria.push_back(posActual);

        // Check if we've reached the destination
        if (longitud <= 5.0f) break;
        longitud -= 10.0f;
    }

    return trayectoria;
}


bool Interfaz::balaGolpeaTanque(const std::vector<sf::Vector2f>& trayectoria, sf::Vector2f posTanque) {
    float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());
    return std::ranges::any_of(trayectoria, [&](const sf::Vector2f& punto) {
        return distancia(punto, posTanque) < tileSize / 2;
    });
}


float Interfaz::distancia(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}


int Interfaz::calcularDanio(Tank::Color colorAtacante, Tank::Color colorDefensor) {
    if (colorDefensor == Tank::CELESTE || colorDefensor == Tank::AZUL) {
        return 25;
    } else if (colorDefensor == Tank::AMARILLO || colorDefensor == Tank::ROJO) {
        return 50;
    }
    return 0; // No debería llegar aquí
}

void Interfaz::cambiarTurno() {
    turnoJugador1 = !turnoJugador1;
    limpiarTrazos();
    selectedTank = nullptr;
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

void Interfaz::limpiarTrazos() {
    trazosDisparos.clear();
}

void Interfaz::dibujar() {
    window.clear(sf::Color::Black);

    window.draw(fondoSprite);
    dibujarMapa();
    dibujarTanques();
    dibujarHUD();

    // Draw shoot button
    window.draw(shootButton);
    window.draw(shootButtonText);
    window.draw(moveButton);         // Dibuja el botón de mover
    window.draw(moveButtonText);


    // Draw selected tank indicator
    if (selectedTank != nullptr) {
        // Definir tileSize aquí
        float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());

        sf::CircleShape selector(10);
        selector.setFillColor(sf::Color::Green);
        selector.setPosition(static_cast<float>(selectedTank->getY()) * tileSize, static_cast<float>(selectedTank->getX()) * tileSize);
        window.draw(selector);
    }

    window.draw(lineaTrayectoria);
    if (!trayectoriaBala.empty() && indiceAnimacionBala < trayectoriaBala.size()) {
        window.draw(balaSprite);
    }

    for (const auto& trazo : trazosDisparos) {
        window.draw(trazo);
    }

    for (const auto& bala : balas) {
        window.draw(bala);
    }

    dibujarInfoDanio();
    window.display();
}

void Interfaz::dibujarInfoDanio() {
    float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());
    sf::Text textoSalud;
    textoSalud.setFont(font);
    textoSalud.setCharacterSize(12);

    auto dibujarSaludTanque = [&](const Tank& tanque) {
        textoSalud.setPosition(static_cast<float>(tanque.getY()) * tileSize, static_cast<float>(tanque.getX()) * tileSize - 20.0f);
        textoSalud.setString(std::to_string(tanque.getSalud()) + "%");
        window.draw(textoSalud);
    };

    for (const auto& tanque : tanquesJugador1) dibujarSaludTanque(tanque);
    for (const auto& tanque : tanquesJugador2) dibujarSaludTanque(tanque);
}

void Interfaz::dibujarMapa() {
    float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());

    for (int i = 0; i < mapa.getFilas(); ++i) {
        for (int j = 0; j < mapa.getColumnas(); ++j) {

            if (mapa.hayObstaculo(i, j)) {

                obstaculoSprite.setPosition(static_cast<float>(j) * tileSize, static_cast<float>(i) * tileSize);
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
    float tileSize = static_cast<float>(window.getSize().x) / static_cast<float>(mapa.getColumnas());

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
        sprite.setPosition(static_cast<float>(tanque.getY()) * tileSize, static_cast<float>(tanque.getX()) * tileSize);
        sprite.setScale(
            tileSize / static_cast<float>(sprite.getTexture()->getSize().x),
            tileSize / static_cast<float>(sprite.getTexture()->getSize().y)
        );
        window.draw(sprite);
    };

    for (const auto& tanque : tanquesJugador1) dibujarTanque(tanque);
    for (const auto& tanque : tanquesJugador2) dibujarTanque(tanque);
}

void Interfaz::configurarHUD() {
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
}

void Interfaz::dibujarHUD() {
    textoJugador1.setString("Jugador 1:"+ nombreJugador1);
    window.draw(textoJugador1);
    textoJugador2.setString("Jugador 2:"+ nombreJugador2);
    window.draw(textoJugador2);

    textoTurno.setString("Turno: " + (turnoJugador1 ? nombreJugador1 : nombreJugador2));
    window.draw(textoTurno);

    // Actualizar y dibujar el tiempo restante
    sf::Time tiempoRestante = sf::seconds(300) - reloj.getElapsedTime();
    int minutos = static_cast<int>(tiempoRestante.asSeconds()) / 60;
    int segundos = static_cast<int>(tiempoRestante.asSeconds()) % 60;
    textoTiempo.setString("Tiempo: " + std::to_string(minutos) + ":" +
                          (segundos < 10 ? "0" : "") + std::to_string(segundos));
    window.draw(textoTiempo);
}

void Interfaz::manejarClicIzquierdo(int x, int y) {
    // Verificar si se presiona el botón de disparo
    if (shootButton.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y))) {
        isShooting = true;  // Indica que se está en modo de disparo
    }
    // Verificar si se presiona el botón de mover
    else if (moveButton.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y))) {
        // Si el botón de mover fue presionado, maneja el movimiento
        manejarMovimiento(x, y); // Llama al método para mover el tanque
    }
    // Manejar el disparo si se estaba en modo de disparo
    else if (isShooting) {
        manejarDisparo(x, y);
        isShooting = false; // Restablecer el estado de disparo
    }
    // Si no se presionaron los botones, manejar la selección del tanque
    else {
        manejarSeleccionTanque(x, y);
    }
}

void Interfaz::manejarClickDerecho(int x, int y) {
    // Si estamos esperando el destino, procesar el movimiento
    if (esperandoDestino) {
        // Convertir las coordenadas del click a coordenadas del mapa
        int filasMapa = mapa.getFilas();
        int columnasMapa = mapa.getColumnas();
        float tamañoCelda = static_cast<float>(window.getSize().x) / columnasMapa;

        int mapaX = static_cast<int>(x / tamañoCelda);
        int mapaY = static_cast<int>(y / tamañoCelda);

        manejarMovimiento(mapaX, mapaY);
        esperandoDestino = false;
        return;
    }

    // Si se presiona el botón de mover
    if (moveButton.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y))) {
        if (selectedTank != nullptr && selectedTank->getSalud() > 0) {
            esperandoDestino = true;
            std::cout << "Seleccione el destino para el tanque" << std::endl;
        } else {
            std::cout << "Seleccione un tanque válido primero" << std::endl;
        }
    } else {
        manejarSeleccionTanque(x, y);
    }
}


void Interfaz::verificarCondicionesVictoria() {
    if (todosLosTanquesDestruidos(tanquesJugador1)) {
        mostrarPantallaVictoria(nombreJugador2);
        juegoTerminado = true;
    } else if (todosLosTanquesDestruidos(tanquesJugador2)) {
        mostrarPantallaVictoria(nombreJugador1);
        juegoTerminado = true;
    } else if (reloj.getElapsedTime().asSeconds() >= 300) { // 5 minutos
        int tanquesVivosJ1 = contarTanquesVivos(tanquesJugador1);
        int tanquesVivosJ2 = contarTanquesVivos(tanquesJugador2);
        if (tanquesVivosJ1 > tanquesVivosJ2) {
            mostrarPantallaVictoria(nombreJugador1);
        } else if (tanquesVivosJ2 > tanquesVivosJ1) {
            mostrarPantallaVictoria(nombreJugador2);
        } else {
            mostrarPantallaVictoria("Empate");
        }
        juegoTerminado = true;
    }
}

bool Interfaz::todosLosTanquesDestruidos(const std::vector<Tank>& tanques) {
    return std::all_of(tanques.begin(), tanques.end(), [](const Tank& t) { return t.getSalud() <= 0; });

}

int Interfaz::contarTanquesVivos(const std::vector<Tank>& tanques) {
    int count = 0;
    for (const auto& tanque : tanques) {
        if (tanque.getSalud() > 0) {
            count++;
        }
    }
    return count;
}

void Interfaz::mostrarPantallaVictoria(const std::string& ganador) {
    sf::Text textoVictoria;
    textoVictoria.setFont(font);
    textoVictoria.setCharacterSize(40);
    textoVictoria.setFillColor(sf::Color::Yellow);

    if (ganador == "Empate") {
        textoVictoria.setString("¡Empate!");
    } else {
        textoVictoria.setString("¡" + ganador + " ha ganado!");
    }

    textoVictoria.setPosition(
    (static_cast<float>(window.getSize().x) - textoVictoria.getLocalBounds().width) / 2.0f,
    (static_cast<float>(window.getSize().y) - textoVictoria.getLocalBounds().height) / 2.0f
    );

    window.clear();
    window.draw(textoVictoria);
    window.display();

    sf::sleep(sf::seconds(5)); // Mostrar la pantalla de victoria por 5 segundos
}



