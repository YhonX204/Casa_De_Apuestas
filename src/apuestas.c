#include "estructuras.h"
#include "usuarios.h"
#include "partidos.h"
#include "apuestas.h"
#include "equipos.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARCHIVO_APUESTAS "apuestas.txt"

// Declaraciones de funciones estáticas
static int generar_id_apuesta(void);
static float calcular_cuota(const Partido* partido, TipoApuesta tipo, int prediccion);

// Función auxiliar para generar un ID único para la apuesta
static int generar_id_apuesta() {
    static int ultimo_id = 0;
    return ++ultimo_id;
}

// Función para calcular la cuota según el tipo de apuesta y predicción
static float calcular_cuota(const Partido* partido, TipoApuesta tipo, int prediccion) {
    float cuota_base = 1.0;
    
    switch(tipo) {
        case RESULTADO_FINAL:
            if (prediccion == 0) // Victoria local
                cuota_base = partido->stats.cuotaEquipo1;
            else if (prediccion == 1) // Empate
                cuota_base = partido->stats.cuotaEmpate;
            else // Victoria visitante
                cuota_base = partido->stats.cuotaEquipo2;
            break;
            
        case GOLES_TOTALES:
            cuota_base = 2.0; // Cuota base para over/under
            break;
            
        case PRIMER_GOLEADOR:
            cuota_base = 3.0; // Cuota base para goleador
            break;
    }
    
    return cuota_base;
}

// Implementación de realizar_apuesta según la declaración en partidos.h
void realizar_apuesta(const Usuario *usuario) {
    int id_partido;
    TipoApuesta tipo;
    float monto;
    int prediccion;
    
    printf("\n=== REALIZAR APUESTA ===\n");
    
    // Mostrar partidos disponibles
    listar_partidos();
    
    printf("\nIngrese ID del partido: ");
    scanf("%d", &id_partido);
    
    Partido partido;
    if (!cargar_partido(id_partido, &partido)) {
        printf("Partido no encontrado o no disponible para apuestas.\n");
        return;
    }
    
    printf("\nTipos de apuesta disponibles:\n");
    printf("1. Resultado final (0=Local, 1=Empate, 2=Visitante)\n");
    printf("2. Goles totales\n");
    printf("3. Primer goleador\n");
    printf("Seleccione tipo de apuesta: ");
    int tipo_seleccionado;
    scanf("%d", &tipo_seleccionado);
    tipo = (TipoApuesta)(tipo_seleccionado - 1);
    
    printf("Ingrese monto a apostar: $");
    scanf("%f", &monto);
    
    if (monto <= 0 || monto > usuario->saldo) {
        printf("Monto invalido o saldo insuficiente.\n");
        return;
    }
    
    printf("Ingrese su prediccion: ");
    scanf("%d", &prediccion);
    
    float cuota = calcular_cuota(&partido, tipo, prediccion);
    float ganancia_potencial = monto * cuota;
    
    Apuesta nueva_apuesta = {
        .id = generar_id_apuesta(),
        .idPartido = id_partido,
        .tipo = tipo,
        .montoApostado = monto,
        .cuota = cuota,
        .prediccion = prediccion,
        .estado = -1, // Pendiente
        .ganancia_potencial = ganancia_potencial
    };
    strcpy(nueva_apuesta.usuario, usuario->usuario);
    
    // Obtener fecha actual
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(nueva_apuesta.fecha_apuesta, sizeof(nueva_apuesta.fecha_apuesta), "%Y-%m-%d %H:%M", tm);
    
    // Guardar la apuesta
    if (guardar_apuesta(&nueva_apuesta)) {
        // Actualizar saldo del usuario
        if (actualizar_saldo(usuario->usuario, usuario->saldo - monto)) {
            printf("Apuesta realizada con exito. ID: %d\n", nueva_apuesta.id);
            printf("Ganancia potencial: $%.2f\n", ganancia_potencial);
        } else {
            printf("Error al actualizar el saldo.\n");
        }
    } else {
        printf("Error al guardar la apuesta.\n");
    }
}

// Implementación de procesar_apuestas_partido según la declaración en partidos.h
void procesar_apuestas_partido(int id_partido) {
    Partido partido;
    if (!cargar_partido(id_partido, &partido)) {
        printf("Partido no encontrado.\n");
        return;
    }
    
    if (strcmp(partido.estado, "Finalizado") != 0) {
        printf("El partido no está finalizado.\n");
        return;
    }
    
    FILE* archivo = fopen(ARCHIVO_APUESTAS, "rb+");
    if (!archivo) {
        printf("No hay apuestas para procesar.\n");
        return;
    }
    
    Apuesta apuesta;
    while (fread(&apuesta, sizeof(Apuesta), 1, archivo)) {
        if (apuesta.idPartido == id_partido && apuesta.estado == -1) {
            calcular_ganancias_apuesta(&apuesta, &partido);
            
            // Actualizar la apuesta en el archivo
            fseek(archivo, -sizeof(Apuesta), SEEK_CUR);
            fwrite(&apuesta, sizeof(Apuesta), 1, archivo);
            
            // Si ganó, actualizar saldo del usuario
            if (apuesta.estado == 1) {
                Usuario usuario;
                if (cargar_usuario(apuesta.usuario, &usuario)) {
                    actualizar_saldo(usuario.usuario, usuario.saldo + apuesta.ganancia_potencial);
                }
            }
        }
    }
    
    fclose(archivo);
    printf("Apuestas procesadas con exito.\n");
}

// Las funciones listar_apuestas_usuario y listar_todas_apuestas ya están declaradas en partidos.h
// y su implementación actual es compatible 

// Función para calcular las ganancias de una apuesta
void calcular_ganancias_apuesta(Apuesta *apuesta, const Partido *partido) {
    bool es_ganadora = false;
    int total_goles;

    switch(apuesta->tipo) {
        case RESULTADO_FINAL:
            if (partido->stats.golesEquipo1 > partido->stats.golesEquipo2)
                es_ganadora = (apuesta->prediccion == 0); // Victoria local
            else if (partido->stats.golesEquipo1 < partido->stats.golesEquipo2)
                es_ganadora = (apuesta->prediccion == 2); // Victoria visitante
            else
                es_ganadora = (apuesta->prediccion == 1); // Empate
            break;

        case GOLES_TOTALES:
            total_goles = partido->stats.golesEquipo1 + partido->stats.golesEquipo2;
            es_ganadora = (apuesta->prediccion == total_goles);
            break;

        case PRIMER_GOLEADOR:
            if (partido->stats.goleadores[0] > 0) {
                es_ganadora = (apuesta->jugadorID == partido->stats.goleadores[0]);
            }
            break;
    }

    apuesta->estado = es_ganadora ? 1 : 0;
}

// Función para guardar una apuesta en el archivo
bool guardar_apuesta(const Apuesta *apuesta) {
    crear_archivo_si_no_existe(ARCHIVO_APUESTAS);
    
    FILE *archivo = fopen(ARCHIVO_APUESTAS, "a");
    if (!archivo) {
        return false;
    }

    fprintf(archivo, "%d,%s,%d,%d,%.2f,%.2f,%d,%d,%d,%.2f,%s\n",
            apuesta->id,
            apuesta->usuario,
            apuesta->idPartido,
            apuesta->tipo,
            apuesta->montoApostado,
            apuesta->cuota,
            apuesta->prediccion,
            apuesta->jugadorID,
            apuesta->estado,
            apuesta->ganancia_potencial,
            apuesta->fecha_apuesta);

    fclose(archivo);
    return true;
}

// Función para cargar una apuesta específica
bool cargar_apuesta(int id, Apuesta *resultado) {
    FILE *archivo = fopen(ARCHIVO_APUESTAS, "r");
    if (!archivo) {
        return false;
    }

    char linea[512];
    bool encontrado = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Apuesta temp;
        if (sscanf(linea, "%d,%[^,],%d,%d,%f,%f,%d,%d,%d,%f,%[^\n]",
                   &temp.id,
                   temp.usuario,
                   &temp.idPartido,
                   &temp.tipo,
                   &temp.montoApostado,
                   &temp.cuota,
                   &temp.prediccion,
                   &temp.jugadorID,
                   &temp.estado,
                   &temp.ganancia_potencial,
                   temp.fecha_apuesta) == 11) {
            
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

// Función para mostrar una apuesta
void mostrar_apuesta(const Apuesta *apuesta) {
    printf("\n=== DETALLES DE LA APUESTA ===\n");
    printf("ID: %d\n", apuesta->id);
    printf("Usuario: %s\n", apuesta->usuario);
    printf("Partido ID: %d\n", apuesta->idPartido);
    printf("Monto: $%.2f\n", apuesta->montoApostado);
    printf("Cuota: %.2f\n", apuesta->cuota);
    printf("Ganancia potencial: $%.2f\n", apuesta->ganancia_potencial);
    
    printf("Tipo: ");
    switch(apuesta->tipo) {
        case RESULTADO_FINAL:
            printf("Resultado Final (0=Local, 1=Empate, 2=Visitante)\n");
            printf("Predicción: %s\n", 
                apuesta->prediccion == 0 ? "Victoria Local" :
                apuesta->prediccion == 1 ? "Empate" : "Victoria Visitante");
            break;
        case GOLES_TOTALES:
            printf("Goles Totales\n");
            printf("Predicción: %d goles\n", apuesta->prediccion);
            break;
        case PRIMER_GOLEADOR:
            printf("Primer Goleador\n");
            printf("ID Jugador: %d\n", apuesta->jugadorID);
            break;
    }
    
    printf("Estado: %s\n", 
        apuesta->estado == -1 ? "Pendiente" :
        apuesta->estado == 1 ? "Ganada" : "Perdida");
    printf("Fecha: %s\n", apuesta->fecha_apuesta);
}

// Función para cancelar una apuesta
void cancelar_apuesta(int id_apuesta) {
    FILE *archivo = fopen(ARCHIVO_APUESTAS, "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        printf("Error al abrir el archivo de apuestas.\n");
        return;
    }

    char linea[512];
    bool encontrada = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Apuesta apuesta;
        if (sscanf(linea, "%d,%[^,],%d,%d,%f,%f,%d,%d,%d,%f,%[^\n]",
                   &apuesta.id,
                   apuesta.usuario,
                   &apuesta.idPartido,
                   &apuesta.tipo,
                   &apuesta.montoApostado,
                   &apuesta.cuota,
                   &apuesta.prediccion,
                   &apuesta.jugadorID,
                   &apuesta.estado,
                   &apuesta.ganancia_potencial,
                   apuesta.fecha_apuesta) == 11) {
            
            if (apuesta.id == id_apuesta && apuesta.estado == -1) {
                encontrada = true;
                // Devolver el dinero al usuario
                Usuario usuario;
                if (cargar_usuario(apuesta.usuario, &usuario)) {
                    actualizar_saldo(usuario.usuario, usuario.saldo + apuesta.montoApostado);
                    printf("Saldo devuelto al usuario.\n");
                }
                
                // Marcar la apuesta como cancelada
                apuesta.estado = 0;
                fprintf(temp, "%d,%s,%d,%d,%.2f,%.2f,%d,%d,%d,%.2f,%s\n",
                        apuesta.id,
                        apuesta.usuario,
                        apuesta.idPartido,
                        apuesta.tipo,
                        apuesta.montoApostado,
                        apuesta.cuota,
                        apuesta.prediccion,
                        apuesta.jugadorID,
                        apuesta.estado,
                        apuesta.ganancia_potencial,
                        apuesta.fecha_apuesta);
                printf("Apuesta cancelada con éxito.\n");
            } else {
                fputs(linea, temp);
            }
        }
    }

    if (!encontrada) {
        printf("No se encontró la apuesta o no está pendiente.\n");
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_APUESTAS);
    rename("temp.txt", ARCHIVO_APUESTAS);
}

// Función para mostrar el historial de apuestas de un usuario
void mostrar_historial_apuestas(const char *usuario) {
    FILE* archivo = fopen(ARCHIVO_APUESTAS, "rb");
    if (!archivo) {
        printf("No hay historial de apuestas.\n");
        return;
    }

    printf("\n=== HISTORIAL DE APUESTAS DE %s ===\n\n", usuario);
    
    Apuesta apuesta;
    bool encontro = false;
    float total_ganado = 0;
    float total_perdido = 0;
    int apuestas_ganadas = 0;
    int apuestas_perdidas = 0;
    int apuestas_pendientes = 0;

    while (fread(&apuesta, sizeof(Apuesta), 1, archivo)) {
        if (strcmp(apuesta.usuario, usuario) == 0) {
            encontro = true;
            mostrar_apuesta(&apuesta);
            printf("------------------------\n");

            if (apuesta.estado == 1) {
                total_ganado += apuesta.ganancia_potencial;
                apuestas_ganadas++;
            } else if (apuesta.estado == 0) {
                total_perdido += apuesta.montoApostado;
                apuestas_perdidas++;
            } else {
                apuestas_pendientes++;
            }
        }
    }

    if (encontro) {
        printf("\nRESUMEN:\n");
        printf("Total apostado: $%.2f\n", total_ganado + total_perdido);
        printf("Total ganado: $%.2f\n", total_ganado);
        printf("Total perdido: $%.2f\n", total_perdido);
        printf("Balance: $%.2f\n", total_ganado - total_perdido);
        printf("Apuestas ganadas: %d\n", apuestas_ganadas);
        printf("Apuestas perdidas: %d\n", apuestas_perdidas);
        printf("Apuestas pendientes: %d\n", apuestas_pendientes);
    } else {
        printf("No se encontraron apuestas para este usuario.\n");
    }

    fclose(archivo);
}

void listar_apuestas_usuario(const char *usuario) {
    FILE *archivo = fopen(ARCHIVO_APUESTAS, "r");
    if (!archivo) {
        printf("No hay apuestas registradas.\n");
        return;
    }

    printf("\n=== APUESTAS DE %s ===\n", usuario);
    printf("ID  | Partido               | Tipo          | Monto    | Estado\n");
    printf("----+----------------------+---------------+----------+--------\n");

    char linea[512];
    while (fgets(linea, sizeof(linea), archivo)) {
        Apuesta apuesta;
        if (sscanf(linea, "%d,%[^,],%d,%d,%f,%f,%d,%d,%d,%f,%[^\n]",
                   &apuesta.id,
                   apuesta.usuario,
                   &apuesta.idPartido,
                   &apuesta.tipo,
                   &apuesta.montoApostado,
                   &apuesta.cuota,
                   &apuesta.prediccion,
                   &apuesta.jugadorID,
                   &apuesta.estado,
                   &apuesta.ganancia_potencial,
                   apuesta.fecha_apuesta) == 11) {
            
            if (strcmp(apuesta.usuario, usuario) == 0) {
                Partido partido;
                char descripcion_partido[50] = "Partido no encontrado";
                
                if (cargar_partido(apuesta.idPartido, &partido)) {
                    Equipo equipo1, equipo2;
                    if (cargar_equipo(partido.idEquipo1, &equipo1) && 
                        cargar_equipo(partido.idEquipo2, &equipo2)) {
                        sprintf(descripcion_partido, "%s vs %s", 
                                equipo1.nombre, equipo2.nombre);
                    }
                }
                
                const char *tipos[] = {"Resultado", "Goles Totales", "Primer Goleador"};
                const char *estados[] = {"Perdida", "Ganada", "Pendiente"};
                
                printf("%-3d | %-20s | %-13s | $%-7.2f | %s\n",
                       apuesta.id,
                       descripcion_partido,
                       tipos[apuesta.tipo],
                       apuesta.montoApostado,
                       estados[apuesta.estado + 1]);
            }
        }
    }

    fclose(archivo);
}

void listar_todas_apuestas(void) {
    FILE *archivo = fopen(ARCHIVO_APUESTAS, "r");
    if (!archivo) {
        printf("No hay apuestas registradas.\n");
        return;
    }

    printf("\n=== LISTA DE TODAS LAS APUESTAS ===\n");
    printf("ID  | Usuario      | Partido               | Tipo          | Monto    | Estado\n");
    printf("----+-------------+----------------------+---------------+----------+--------\n");

    char linea[512];
    while (fgets(linea, sizeof(linea), archivo)) {
        Apuesta apuesta;
        if (sscanf(linea, "%d,%[^,],%d,%d,%f,%f,%d,%d,%d,%f,%[^\n]",
                   &apuesta.id,
                   apuesta.usuario,
                   &apuesta.idPartido,
                   &apuesta.tipo,
                   &apuesta.montoApostado,
                   &apuesta.cuota,
                   &apuesta.prediccion,
                   &apuesta.jugadorID,
                   &apuesta.estado,
                   &apuesta.ganancia_potencial,
                   apuesta.fecha_apuesta) == 11) {
            
            Partido partido;
            char descripcion_partido[50] = "Partido no encontrado";
            
            if (cargar_partido(apuesta.idPartido, &partido)) {
                Equipo equipo1, equipo2;
                if (cargar_equipo(partido.idEquipo1, &equipo1) && 
                    cargar_equipo(partido.idEquipo2, &equipo2)) {
                    sprintf(descripcion_partido, "%s vs %s", 
                            equipo1.nombre, equipo2.nombre);
                }
            }
            
            const char *tipos[] = {"Resultado", "Goles Totales", "Primer Goleador"};
            const char *estados[] = {"Perdida", "Ganada", "Pendiente"};
            
            printf("%-3d | %-11s | %-20s | %-13s | $%-7.2f | %s\n",
                   apuesta.id,
                   apuesta.usuario,
                   descripcion_partido,
                   tipos[apuesta.tipo],
                   apuesta.montoApostado,
                   estados[apuesta.estado + 1]);
        }
    }

    fclose(archivo);
} 