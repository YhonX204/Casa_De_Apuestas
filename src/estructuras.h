#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOMBRE 50
#define MAX_CORREO 100
#define MAX_JUGADORES 5
#define MAX_EQUIPOS 2
#define MAX_PARTIDOS 100
#define MAX_APUESTAS 100

// Estados posibles para partidos y apuestas
typedef enum {
    PENDIENTE,
    EN_CURSO,
    FINALIZADO,
    CANCELADO
} Estado;

// Tipos de apuestas
typedef enum {
    RESULTADO_FINAL,    // Victoria local (1), Empate (0), Victoria visitante (2)
    GOLES_TOTALES,     // Número total de goles en el partido
    PRIMER_GOLEADOR    // ID del jugador que marcará el primer gol
} TipoApuesta;

// Usuario / Administrador
typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    char usuario[20];
    char contrasena[20];
    float saldo;
    char cedula[20];
    char correo[MAX_CORREO];
    int esAdministrador; // 1 si es admin, 0 si es cliente
} Usuario;

// Jugador
typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    int idEquipo;
    int numeroCamiseta;
    char posicion[20];
} Jugador;

// Equipo
typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    Jugador jugadores[MAX_JUGADORES];
    int num_jugadores;
    char liga[MAX_NOMBRE];
    float ranking;
} Equipo;

// Estadísticas del Partido
typedef struct {
    int golesEquipo1;
    int golesEquipo2;
    int tirosEsquina;
    int tarjetasAmarillas;
    int tarjetasRojas;
    int rematesArco;
    int fuerasJuego;
    int goleadores[10];
    int asistentes[10];
    int posesionEquipo1;
    float cuotaEquipo1;
    float cuotaEquipo2;
    float cuotaEmpate;
} Estadisticas;

// Partido
typedef struct {
    int id;
    int idEquipo1;
    int idEquipo2;
    char fecha[20];
    char hora[10];
    char estado[20]; // "Programado", "En curso", "Finalizado"
    Estadisticas stats;
    bool resultadoFinal;
} Partido;

// Apuesta
typedef struct {
    int id;
    char usuario[MAX_NOMBRE];
    int idPartido;
    TipoApuesta tipo;
    float montoApostado;
    float cuota;
    int prediccion;
    int jugadorID;  // Para apuestas de goleador
    int estado;     // -1=pendiente, 0=perdida, 1=ganada
    float ganancia_potencial;
    char fecha_apuesta[20];
} Apuesta;

// Funciones para crear estructuras
Equipo* crear_equipo(int id, const char* nombre);
Jugador* crear_jugador(int id, const char* nombre, int edad, const char* posicion, int id_equipo);
Partido* crear_partido(int id, int id_equipo_local, int id_equipo_visitante, const char* fecha);
Apuesta* crear_apuesta(int id, const char* usuario, int id_partido, float monto, TipoApuesta tipo, int prediccion);

// Funciones para liberar memoria
void liberar_equipo(Equipo* equipo);
void liberar_jugador(Jugador* jugador);
void liberar_partido(Partido* partido);
void liberar_apuesta(Apuesta* apuesta);

#endif // ESTRUCTURAS_H 