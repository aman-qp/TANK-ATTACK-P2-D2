cmake_minimum_required(VERSION 3.29)
project(Proyecto_2_D2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Find SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)


# Incluir directorios .h
include_directories(include)

add_executable(Proyecto_2_D2 Clases/main.cpp
        Clases/Juego.cpp
        include/Juego.h
        Clases/Mapa.cpp
        include/Mapa.h
        Clases/Tank.cpp
        include/Tank.h
        Clases/Interfaz.cpp
        include/Interfaz.h
        include/PowerUp.h
        Clases/PowerUp.cpp
)

# Link SFML libraries
target_link_libraries(Proyecto_2_D2 sfml-system sfml-window sfml-graphics sfml-network sfml-audio)