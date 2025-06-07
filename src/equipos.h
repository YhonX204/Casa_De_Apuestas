#ifndef EQUIPOS_H
#define EQUIPOS_H

#include "estructuras.h"

// Funciones de gestión de equipos
void registrar_equipo();
void listar_equipos();
bool cargar_equipo(int id, Equipo *resultado);
void mostrar_equipo(const Equipo *equipo);
void agregar_jugador(int id_equipo);
void listar_jugadores_equipo(int id_equipo);
bool guardar_equipo(Equipo equipo);
void editar_equipo(int id_equipo);
void eliminar_equipo(int id_equipo);
void mostrar_estadisticas_equipo(int id_equipo);

// Funciones de gestión de jugadores
void registrar_jugador(int id_equipo);
void listar_jugadores();
bool cargar_jugador(int id, Jugador *resultado);
void mostrar_jugador(const Jugador *jugador);
void editar_jugador(int id_jugador);
void eliminar_jugador(int id_jugador);
void transferir_jugador(int id_jugador, int nuevo_equipo);

#endif // EQUIPOS_H 