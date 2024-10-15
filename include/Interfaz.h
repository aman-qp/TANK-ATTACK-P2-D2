#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include "Tank.h"

class Interfaz {
public:
    Interfaz(Mapa& mapa, Tank& tank1, Tank& tank2, Tank& tank3, Tank& tank4);  // Constructor modificado
    void iniciarVentana();
    void procesarEventos();
    void dibujarMapa(const Mapa& mapa);
    void dibujarTank(const Tank& tank);
    void dibujarFondo();

private:
    sf::RenderWindow window;
    Mapa& mapa;
    Tank& tank1;
    Tank& tank2;
    Tank& tank3;
    Tank& tank4;

    sf::Texture fondoTexture;  // Textura para el fondo
    sf::Sprite fondoSprite;
    sf::Texture obstaculoTexture;  // Textura para el obstáculo
    sf::Sprite obstaculoSprite;

    sf::Texture tanqueAzulTexture;   // Textura para el tanque azul
    sf::Texture tanqueRojoTexture;    // Textura para el tanque rojo
    sf::Texture tanqueCelesteTexture; // Textura para el tanque celeste
    sf::Texture tanqueAmarilloTexture; // Textura para el tanque amarillo
};


#endif // INTERFAZ_H
