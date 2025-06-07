#ifndef APUESTAS_H
#define APUESTAS_H

#include "estructuras.h"
#include "usuarios.h"
#include "partidos.h"

// Funciones principales de apuestas
void realizar_apuesta(const Usuario *usuario);
void procesar_apuestas_partido(int id_partido);
void listar_apuestas_usuario(const char *usuario);
void listar_todas_apuestas(void);
void calcular_ganancias_apuesta(Apuesta *apuesta, const Partido *partido);
bool guardar_apuesta(const Apuesta *apuesta);
bool cargar_apuesta(int id, Apuesta *resultado);
void mostrar_apuesta(const Apuesta *apuesta);
void cancelar_apuesta(int id_apuesta);
void mostrar_historial_apuestas(const char *usuario);

#endif // APUESTAS_H