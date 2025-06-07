#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "estructuras.h"

// Funciones de validación
void limpiar_buffer();
bool validar_texto(const char *texto);
bool validar_usuario(const char *usuario);
bool validar_saldo(const char *input);
void leer_cadena(char *destino, int tamano);
bool validar_fecha(const char *fecha);
bool validar_hora(const char *hora);
bool validar_correo(const char *correo);
bool validar_cedula(const char *cedula);

// Funciones de archivo
bool archivo_existe(const char *nombre_archivo);
void crear_archivo_si_no_existe(const char *nombre_archivo);
int obtener_siguiente_id(const char *nombre_archivo);

// Funciones de encriptación
void encriptar_contrasena(const char *contrasena, char *contrasena_encriptada);
bool verificar_contrasena(const char *contrasena_ingresada, const char *contrasena_almacenada);

// Funciones de formato
void obtener_fecha_actual(char *fecha);
void obtener_hora_actual(char *hora);
void formatear_dinero(float cantidad, char *resultado);

#endif // UTILS_H 