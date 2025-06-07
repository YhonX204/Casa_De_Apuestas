#include "partidos.h"
#include "equipos.h"
#include "utils.h"
#include "usuarios.h"
#include "estructuras.h"
#include "apuestas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ARCHIVO_PARTIDOS "partidos.txt"
#define ARCHIVO_APUESTAS "apuestas.txt"

// =================== FUNCIONES DE PARTIDOS ===================

void registrar_partido() {
    Partido nuevo;
    bool valido;
    char input[50];

    printf("\n--- REGISTRO DE PARTIDO ---\n");

    // Mostrar equipos disponibles
    printf("\nEquipos disponibles:\n");
    listar_equipos();

    // Equipo 1
    do {
        printf("\nID del equipo local: ");
        leer_cadena(input, sizeof(input));
        valido = true;
        for (int i = 0; input[i]; i++) {
            if (!isdigit(input[i])) {
                valido = false;
                break;
            }
        }
        if (valido) {
            nuevo.idEquipo1 = atoi(input);
            Equipo equipo;
            if (!cargar_equipo(nuevo.idEquipo1, &equipo)) {
                printf("Error: Equipo no encontrado.\n");
                valido = false;
            }
        }
    } while (!valido);

    // Equipo 2
    do {
        printf("ID del equipo visitante: ");
        leer_cadena(input, sizeof(input));
        valido = true;
        for (int i = 0; input[i]; i++) {
            if (!isdigit(input[i])) {
                valido = false;
                break;
            }
        }
        if (valido) {
            nuevo.idEquipo2 = atoi(input);
            if (nuevo.idEquipo2 == nuevo.idEquipo1) {
                printf("Error: No puede ser el mismo equipo.\n");
                valido = false;
                continue;
            }
            Equipo equipo;
            if (!cargar_equipo(nuevo.idEquipo2, &equipo)) {
                printf("Error: Equipo no encontrado.\n");
                valido = false;
            }
        }
    } while (!valido);

    // Fecha
    do {
        printf("Fecha (dd/mm/yyyy): ");
        leer_cadena(nuevo.fecha, sizeof(nuevo.fecha));
        valido = validar_fecha(nuevo.fecha);
        if (!valido) {
            printf("Error: Formato de fecha inválido.\n");
        }
    } while (!valido);

    // Hora
    do {
        printf("Hora (HH:MM): ");
        leer_cadena(nuevo.hora, sizeof(nuevo.hora));
        valido = validar_hora(nuevo.hora);
        if (!valido) {
            printf("Error: Formato de hora inválido.\n");
        }
    } while (!valido);

    // Inicializar estadísticas
    nuevo.stats.golesEquipo1 = 0;
    nuevo.stats.golesEquipo2 = 0;
    nuevo.stats.tirosEsquina = 0;
    nuevo.stats.tarjetasAmarillas = 0;
    nuevo.stats.tarjetasRojas = 0;
    nuevo.stats.rematesArco = 0;
    nuevo.stats.fuerasJuego = 0;
    nuevo.stats.posesionEquipo1 = 50;
    
    // Establecer cuotas iniciales
    Equipo equipo1, equipo2;
    cargar_equipo(nuevo.idEquipo1, &equipo1);
    cargar_equipo(nuevo.idEquipo2, &equipo2);
    
    // Calcular cuotas basadas en ranking
    float diferencia_ranking = equipo1.ranking - equipo2.ranking;
    nuevo.stats.cuotaEquipo1 = 2.0 + (diferencia_ranking < 0 ? -diferencia_ranking/2 : 0);
    nuevo.stats.cuotaEquipo2 = 2.0 + (diferencia_ranking > 0 ? diferencia_ranking/2 : 0);
    nuevo.stats.cuotaEmpate = 3.0;

    nuevo.id = obtener_siguiente_id(ARCHIVO_PARTIDOS);
    strcpy(nuevo.estado, "Programado");
    nuevo.resultadoFinal = false;

    if (guardar_partido(nuevo)) {
        printf("\nPartido registrado exitosamente. ID: %d\n", nuevo.id);
        printf("Cuotas iniciales:\n");
        printf("Victoria local (%s): %.2f\n", equipo1.nombre, nuevo.stats.cuotaEquipo1);
        printf("Empate: %.2f\n", nuevo.stats.cuotaEmpate);
        printf("Victoria visitante (%s): %.2f\n", equipo2.nombre, nuevo.stats.cuotaEquipo2);
    } else {
        printf("Error al registrar el partido.\n");
    }
}

void listar_partidos() {
    FILE *archivo = fopen(ARCHIVO_PARTIDOS, "r");
    if (!archivo) {
        printf("No hay partidos registrados.\n");
        return;
    }

    printf("\n=== LISTA DE PARTIDOS ===\n");
    printf("ID  | Local vs Visitante           | Fecha     | Hora  | Estado\n");
    printf("----+------------------------------+----------+-------+--------\n");

    char linea[512];
    while (fgets(linea, sizeof(linea), archivo)) {
        Partido partido;
        if (sscanf(linea, "%d,%d,%d,%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f",
                   &partido.id,
                   &partido.idEquipo1,
                   &partido.idEquipo2,
                   partido.fecha,
                   partido.hora,
                   partido.estado,
                   &partido.stats.golesEquipo1,
                   &partido.stats.golesEquipo2,
                   &partido.stats.tirosEsquina,
                   &partido.stats.tarjetasAmarillas,
                   &partido.stats.tarjetasRojas,
                   &partido.stats.rematesArco,
                   &partido.stats.fuerasJuego,
                   &partido.stats.posesionEquipo1,
                   &partido.stats.cuotaEquipo1,
                   &partido.stats.cuotaEquipo2,
                   &partido.stats.cuotaEmpate) == 17) {
            
            Equipo equipo1, equipo2;
            char nombre_equipo1[MAX_NOMBRE] = "Equipo no encontrado";
            char nombre_equipo2[MAX_NOMBRE] = "Equipo no encontrado";
            
            if (cargar_equipo(partido.idEquipo1, &equipo1)) {
                strcpy(nombre_equipo1, equipo1.nombre);
            }
            if (cargar_equipo(partido.idEquipo2, &equipo2)) {
                strcpy(nombre_equipo2, equipo2.nombre);
            }
            
            printf("%-3d | %-12s vs %-12s | %s | %s | %s\n",
                   partido.id,
                   nombre_equipo1,
                   nombre_equipo2,
                   partido.fecha,
                   partido.hora,
                   partido.estado);
        }
    }

    fclose(archivo);
}

bool cargar_partido(int id, Partido *resultado) {
    FILE *archivo = fopen(ARCHIVO_PARTIDOS, "r");
    if (!archivo) return false;

    char linea[512];
    bool encontrado = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Partido temp;
        if (sscanf(linea, "%d,%d,%d,%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f",
                   &temp.id,
                   &temp.idEquipo1,
                   &temp.idEquipo2,
                   temp.fecha,
                   temp.hora,
                   temp.estado,
                   &temp.stats.golesEquipo1,
                   &temp.stats.golesEquipo2,
                   &temp.stats.tirosEsquina,
                   &temp.stats.tarjetasAmarillas,
                   &temp.stats.tarjetasRojas,
                   &temp.stats.rematesArco,
                   &temp.stats.fuerasJuego,
                   &temp.stats.posesionEquipo1,
                   &temp.stats.cuotaEquipo1,
                   &temp.stats.cuotaEquipo2,
                   &temp.stats.cuotaEmpate) == 17) {
            
            if (temp.id == id) {
                *resultado = temp;
                encontrado = true;
                break;
            }
        }
    }

    fclose(archivo);
    return encontrado;
}

void mostrar_partido(const Partido *partido) {
    Equipo equipo1, equipo2;
    char nombre_equipo1[MAX_NOMBRE] = "Equipo no encontrado";
    char nombre_equipo2[MAX_NOMBRE] = "Equipo no encontrado";
    
    if (cargar_equipo(partido->idEquipo1, &equipo1)) {
        strcpy(nombre_equipo1, equipo1.nombre);
    }
    if (cargar_equipo(partido->idEquipo2, &equipo2)) {
        strcpy(nombre_equipo2, equipo2.nombre);
    }

    printf("\n=== DETALLES DEL PARTIDO ===\n");
    printf("ID: %d\n", partido->id);
    printf("Equipos: %s vs %s\n", nombre_equipo1, nombre_equipo2);
    printf("Fecha: %s\n", partido->fecha);
    printf("Hora: %s\n", partido->hora);
    printf("Estado: %s\n", partido->estado);
    
    if (strcmp(partido->estado, "Programado") != 0) {
        printf("\nEstadísticas:\n");
        printf("Marcador: %s %d - %d %s\n", 
               nombre_equipo1, partido->stats.golesEquipo1,
               partido->stats.golesEquipo2, nombre_equipo2);
        printf("Tiros de esquina: %d\n", partido->stats.tirosEsquina);
        printf("Tarjetas amarillas: %d\n", partido->stats.tarjetasAmarillas);
        printf("Tarjetas rojas: %d\n", partido->stats.tarjetasRojas);
        printf("Remates al arco: %d\n", partido->stats.rematesArco);
        printf("Fueras de juego: %d\n", partido->stats.fuerasJuego);
        printf("Posesión equipo local: %d%%\n", partido->stats.posesionEquipo1);
    }

    printf("\nCuotas:\n");
    printf("Victoria %s: %.2f\n", nombre_equipo1, partido->stats.cuotaEquipo1);
    printf("Empate: %.2f\n", partido->stats.cuotaEmpate);
    printf("Victoria %s: %.2f\n", nombre_equipo2, partido->stats.cuotaEquipo2);
}

bool guardar_partido(Partido partido) {
    crear_archivo_si_no_existe(ARCHIVO_PARTIDOS);
    
    // Si es un partido nuevo
    if (partido.id == 0) {
        partido.id = obtener_siguiente_id(ARCHIVO_PARTIDOS);
    }
    
    FILE *archivo = fopen(ARCHIVO_PARTIDOS, "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        return false;
    }

    bool actualizado = false;
    char linea[512];

    // Copiar todos los partidos, actualizando o agregando el nuevo
    while (fgets(linea, sizeof(linea), archivo)) {
        Partido temp_partido;
        if (sscanf(linea, "%d,%d,%d,%[^,],%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f",
                   &temp_partido.id,
                   &temp_partido.idEquipo1,
                   &temp_partido.idEquipo2,
                   temp_partido.fecha,
                   temp_partido.hora,
                   temp_partido.estado,
                   &temp_partido.stats.golesEquipo1,
                   &temp_partido.stats.golesEquipo2,
                   &temp_partido.stats.tirosEsquina,
                   &temp_partido.stats.tarjetasAmarillas,
                   &temp_partido.stats.tarjetasRojas,
                   &temp_partido.stats.rematesArco,
                   &temp_partido.stats.fuerasJuego,
                   &temp_partido.stats.posesionEquipo1,
                   &temp_partido.stats.cuotaEquipo1,
                   &temp_partido.stats.cuotaEquipo2,
                   &temp_partido.stats.cuotaEmpate) == 17) {
            
            if (temp_partido.id == partido.id) {
                fprintf(temp, "%d,%d,%d,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f\n",
                        partido.id,
                        partido.idEquipo1,
                        partido.idEquipo2,
                        partido.fecha,
                        partido.hora,
                        partido.estado,
                        partido.stats.golesEquipo1,
                        partido.stats.golesEquipo2,
                        partido.stats.tirosEsquina,
                        partido.stats.tarjetasAmarillas,
                        partido.stats.tarjetasRojas,
                        partido.stats.rematesArco,
                        partido.stats.fuerasJuego,
                        partido.stats.posesionEquipo1,
                        partido.stats.cuotaEquipo1,
                        partido.stats.cuotaEquipo2,
                        partido.stats.cuotaEmpate);
                actualizado = true;
            } else {
                fputs(linea, temp);
            }
        }
    }

    // Si no se actualizó ningún partido existente, agregar el nuevo
    if (!actualizado) {
        fprintf(temp, "%d,%d,%d,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f\n",
                partido.id,
                partido.idEquipo1,
                partido.idEquipo2,
                partido.fecha,
                partido.hora,
                partido.estado,
                partido.stats.golesEquipo1,
                partido.stats.golesEquipo2,
                partido.stats.tirosEsquina,
                partido.stats.tarjetasAmarillas,
                partido.stats.tarjetasRojas,
                partido.stats.rematesArco,
                partido.stats.fuerasJuego,
                partido.stats.posesionEquipo1,
                partido.stats.cuotaEquipo1,
                partido.stats.cuotaEquipo2,
                partido.stats.cuotaEmpate);
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_PARTIDOS);
    rename("temp.txt", ARCHIVO_PARTIDOS);

    return true;
}

void actualizar_estadisticas_partido(int id_partido) {
    Partido partido;
    if (!cargar_partido(id_partido, &partido)) {
        printf("Error: Partido no encontrado.\n");
        return;
    }

    printf("\n=== ACTUALIZAR ESTADÍSTICAS ===\n");
    printf("1. Goles equipo local\n");
    printf("2. Goles equipo visitante\n");
    printf("3. Tiros de esquina\n");
    printf("4. Tarjetas amarillas\n");
    printf("5. Tarjetas rojas\n");
    printf("6. Remates al arco\n");
    printf("7. Fueras de juego\n");
    printf("8. Posesión equipo local\n");
    
    int opcion;
    printf("\nSeleccione estadística a actualizar: ");
    scanf("%d", &opcion);
    limpiar_buffer();

    int nuevo_valor;
    switch(opcion) {
        case 1:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.golesEquipo1 = nuevo_valor;
            break;
        case 2:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.golesEquipo2 = nuevo_valor;
            break;
        case 3:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.tirosEsquina = nuevo_valor;
            break;
        case 4:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.tarjetasAmarillas = nuevo_valor;
            break;
        case 5:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.tarjetasRojas = nuevo_valor;
            break;
        case 6:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.rematesArco = nuevo_valor;
            break;
        case 7:
            printf("Nuevo valor: ");
            scanf("%d", &nuevo_valor);
            partido.stats.fuerasJuego = nuevo_valor;
            break;
        case 8:
            printf("Nuevo valor (0-100): ");
            scanf("%d", &nuevo_valor);
            if (nuevo_valor >= 0 && nuevo_valor <= 100) {
                partido.stats.posesionEquipo1 = nuevo_valor;
            }
            break;
        default:
            printf("Opción no válida.\n");
            return;
    }
    limpiar_buffer();

    if (guardar_partido(partido)) {
        printf("Estadísticas actualizadas con éxito.\n");
    } else {
        printf("Error al actualizar estadísticas.\n");
    }
}

void finalizar_partido(int id_partido) {
    Partido partido;
    if (!cargar_partido(id_partido, &partido)) {
        printf("Error: Partido no encontrado.\n");
        return;
    }

    if (strcmp(partido.estado, "Finalizado") == 0) {
        printf("Este partido ya está finalizado.\n");
        return;
    }

    strcpy(partido.estado, "Finalizado");
    partido.resultadoFinal = true;

    if (guardar_partido(partido)) {
        printf("Partido finalizado con éxito.\n");
    } else {
        printf("Error al finalizar el partido.\n");
    }

    // Después de finalizar el partido, procesar las apuestas
    procesar_apuestas_partido(id_partido);
}

void mostrar_estadisticas_partido(int id_partido) {
    Partido partido;
    if (!cargar_partido(id_partido, &partido)) {
        printf("Error: Partido no encontrado.\n");
        return;
    }

    mostrar_partido(&partido);
}

void editar_partido(int id_partido) {
    // ... código existente ...
}

void eliminar_partido(int id_partido) {
    // ... código existente ...
} 