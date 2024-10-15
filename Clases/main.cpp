#include "Interfaz.h"
#include "Mapa.h"
#include "Tank.h"

int main() {
    // Crear un mapa de 10x10
    Mapa mapa(10, 10);
    mapa.generarMapa();  // Generar el mapa con obstáculos

    // Crear tanques
    Tank tank1(4, 3, Tank::AZUL);     // Posición inicial (2, 3) color azul
    Tank tank2(4, 6, Tank::ROJO);     // Posición inicial (5, 6) color rojo
    Tank tank3(7, 2, Tank::CELESTE);  // Posición inicial (7, 2) color celeste
    Tank tank4(7, 8, Tank::AMARILLO); // Posición inicial (1, 8) color amarillo

    Interfaz interfaz(mapa, tank1, tank2, tank3, tank4);  // Pasar mapa y tanques a la interfaz

    // Iniciar la ventana del juego
    interfaz.iniciarVentana();

    return 0;
}
