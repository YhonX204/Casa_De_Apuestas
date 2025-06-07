#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estructuras.h"

// Función para inicializar un nuevo equipo
Equipo* crear_equipo(int id, const char* nombre) {
    Equipo* equipo = (Equipo*)malloc(sizeof(Equipo));
    if (equipo == NULL) {
        return NULL;
    }
    
    equipo->id = id;
    strncpy(equipo->nombre, nombre, sizeof(equipo->nombre) - 1);
    equipo->nombre[sizeof(equipo->nombre) - 1] = '\0';
    equipo->num_jugadores = 0;
    equipo->ranking = 0.0;
    strncpy(equipo->liga, "", sizeof(equipo->liga) - 1);
    equipo->liga[sizeof(equipo->liga) - 1] = '\0';
    
    return equipo;
}

// Función para inicializar un nuevo jugador
Jugador* crear_jugador(int id, const char* nombre, int edad, const char* posicion, int id_equipo) {
    Jugador* jugador = (Jugador*)malloc(sizeof(Jugador));
    if (jugador == NULL) {
        return NULL;
    }
    
    jugador->id = id;
    strncpy(jugador->nombre, nombre, sizeof(jugador->nombre) - 1);
    jugador->nombre[sizeof(jugador->nombre) - 1] = '\0';
    jugador->idEquipo = id_equipo;
    strncpy(jugador->posicion, posicion, sizeof(jugador->posicion) - 1);
    jugador->posicion[sizeof(jugador->posicion) - 1] = '\0';
    jugador->numeroCamiseta = 0;  // Se puede establecer después
    
    return jugador;
}

// Función para inicializar un nuevo partido
Partido* crear_partido(int id, int id_equipo_local, int id_equipo_visitante, const char* fecha) {
    Partido* partido = (Partido*)malloc(sizeof(Partido));
    if (partido == NULL) {
        return NULL;
    }
    
    partido->id = id;
    partido->idEquipo1 = id_equipo_local;
    partido->idEquipo2 = id_equipo_visitante;
    strncpy(partido->fecha, fecha, sizeof(partido->fecha) - 1);
    partido->fecha[sizeof(partido->fecha) - 1] = '\0';
    strncpy(partido->hora, "00:00", sizeof(partido->hora) - 1);
    partido->hora[sizeof(partido->hora) - 1] = '\0';
    strncpy(partido->estado, "Programado", sizeof(partido->estado) - 1);
    partido->estado[sizeof(partido->estado) - 1] = '\0';
    partido->resultadoFinal = false;
    
    // Inicializar estadísticas
    partido->stats.golesEquipo1 = 0;
    partido->stats.golesEquipo2 = 0;
    partido->stats.tirosEsquina = 0;
    partido->stats.tarjetasAmarillas = 0;
    partido->stats.tarjetasRojas = 0;
    partido->stats.rematesArco = 0;
    partido->stats.fuerasJuego = 0;
    partido->stats.posesionEquipo1 = 50;  // 50-50 por defecto
    partido->stats.cuotaEquipo1 = 1.0;
    partido->stats.cuotaEquipo2 = 1.0;
    partido->stats.cuotaEmpate = 1.0;
    
    return partido;
}

// Función para inicializar una nueva apuesta
Apuesta* crear_apuesta(int id, const char* usuario, int id_partido, float monto, TipoApuesta tipo, int prediccion) {
    Apuesta* apuesta = (Apuesta*)malloc(sizeof(Apuesta));
    if (apuesta == NULL) {
        return NULL;
    }
    
    apuesta->id = id;
    strncpy(apuesta->usuario, usuario, sizeof(apuesta->usuario) - 1);
    apuesta->usuario[sizeof(apuesta->usuario) - 1] = '\0';
    apuesta->idPartido = id_partido;
    apuesta->montoApostado = monto;
    apuesta->tipo = tipo;
    apuesta->prediccion = prediccion;
    apuesta->estado = -1;  // -1 = pendiente
    apuesta->ganancia_potencial = 0.0;
    apuesta->jugadorID = 0;  // Se puede establecer después si es necesario
    
    // Obtener fecha actual para la apuesta
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(apuesta->fecha_apuesta, sizeof(apuesta->fecha_apuesta), "%Y-%m-%d", tm);
    
    return apuesta;
}

// Función para liberar la memoria de un equipo
void liberar_equipo(Equipo* equipo) {
    if (equipo != NULL) {
        free(equipo);
    }
}

// Función para liberar la memoria de un jugador
void liberar_jugador(Jugador* jugador) {
    if (jugador != NULL) {
        free(jugador);
    }
}

// Función para liberar la memoria de un partido
void liberar_partido(Partido* partido) {
    if (partido != NULL) {
        free(partido);
    }
}

// Función para liberar la memoria de una apuesta
void liberar_apuesta(Apuesta* apuesta) {
    if (apuesta != NULL) {
        free(apuesta);
    }
} 