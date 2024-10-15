#include "Interfaz.h"
#include "Mapa.h"
#include "Tank.h"
#include <iostream>

Interfaz::Interfaz(Mapa& mapa, Tank& tank1, Tank& tank2, Tank& tank3, Tank& tank4)
    : window(sf::VideoMode(800, 600), "Tank Attack!"),
      mapa(mapa),
      tank1(tank1),
      tank2(tank2),
      tank3(tank3),
      tank4(tank4) {
    // Cargar la textura de fondo
    if (!fondoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/fondo.png")) {  // Asegúrate de que el path es correcto
        throw std::runtime_error("No se pudo cargar la textura del fondo");
    }
    fondoSprite.setTexture(fondoTexture);

    // Cargar la textura del obstáculo
    if (!obstaculoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/muro.png")) {  // Cambia a tu ruta de imagen
        throw std::runtime_error("No se pudo cargar la textura del obstáculo");
    }
    obstaculoSprite.setTexture(obstaculoTexture);

    float tileSize = 50.0f;  // Tamaño de cada celda
    obstaculoSprite.setScale(tileSize / obstaculoSprite.getLocalBounds().width,
                              tileSize / obstaculoSprite.getLocalBounds().height);

    if (!tanqueAzulTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueCeleste.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque azul");
    }
    if (!tanqueRojoTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueAzul.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque rojo");
    }
    if (!tanqueCelesteTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueRojo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque celeste");
    }
    if (!tanqueAmarilloTexture.loadFromFile("/home/amanda/CLionProjects/Proyecto_2_D2/recursos/tanqueAmarillo.png")) {
        throw std::runtime_error("No se pudo cargar la textura del tanque amarillo");
    }
}

void Interfaz::iniciarVentana() {
    while (window.isOpen()) {
        procesarEventos();  // Procesar eventos del mouse/teclado

        // Limpiar la ventana con un color de fondo
        window.clear(sf::Color::Black);

        // Dibujar el fondo
        dibujarFondo();
        // Llamar a dibujar el mapa y los tanques
        dibujarMapa(mapa);  // Asumiendo que tienes un objeto mapa
        dibujarTank(tank1);  // Asumiendo que tienes un objeto tank1
        dibujarTank(tank2);  // Asumiendo que tienes un objeto tank2

        // Mostrar
        window.display();
    }
}

void Interfaz::dibujarFondo() {
    // Dibujar el sprite del fondo
    window.draw(fondoSprite);
}

void Interfaz::procesarEventos() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        // Puedes agregar eventos de clic de mouse o teclado aquí
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                std::cout << "Clic izquierdo en (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
            }
        }
    }
}

void Interfaz::dibujarMapa(const Mapa& mapa) {
    int tileSize = 50;  // Tamaño de cada celda

    for (int x = 0; x < mapa.getFilas(); ++x) {
        for (int y = 0; y < mapa.getColumnas(); ++y) {
            if (mapa.getValor(x, y) == 1) {
                // Si hay un obstáculo, dibujar el sprite del obstáculo
                obstaculoSprite.setPosition(x * tileSize, y * tileSize);
                window.draw(obstaculoSprite);
            }
            // Si es terreno libre, no dibujamos nada
        }
    }
}


void Interfaz::dibujarTank(const Tank& tank) {
    sf::Sprite tanqueSprite;

    // Configurar el sprite del tanque según su color
    switch (tank.getColor()) {
        case Tank::AZUL:
            tanqueSprite.setTexture(tanqueAzulTexture);
        break;
        case Tank::ROJO:
            tanqueSprite.setTexture(tanqueRojoTexture);
        break;
        case Tank::CELESTE:
            tanqueSprite.setTexture(tanqueCelesteTexture);
        break;
        case Tank::AMARILLO:
            tanqueSprite.setTexture(tanqueAmarilloTexture);
        break;
    }

    // Ajustar la posición del tanque
    tanqueSprite.setPosition(tank.getX() * 50, tank.getY() * 50);  // Suponiendo que cada celda es de 50x50

    // Escalar el sprite para que coincida con el tamaño de las celdas (50x50)
    float tileSize = 50.0f;  // Tamaño de cada celda
    tanqueSprite.setScale(tileSize / tanqueSprite.getLocalBounds().width,
                           tileSize / tanqueSprite.getLocalBounds().height);

    // Dibujar el sprite del tanque
    window.draw(tanqueSprite);
}



