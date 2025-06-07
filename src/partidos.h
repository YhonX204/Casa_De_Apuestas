#ifndef PARTIDOS_H
#define PARTIDOS_H

#include "estructuras.h"

// Funciones de gestión de partidos
void registrar_partido();
void listar_partidos();
bool cargar_partido(int id, Partido *resultado);
void mostrar_partido(const Partido *partido);
bool guardar_partido(Partido partido);
void actualizar_estadisticas_partido(int id_partido);
void finalizar_partido(int id_partido);
void mostrar_estadisticas_partido(int id_partido);
void editar_partido(int id_partido);
void eliminar_partido(int id_partido);

#endif // PARTIDOS_H 