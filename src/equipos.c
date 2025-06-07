#include "equipos.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARCHIVO_EQUIPOS "equipos.txt"
#define ARCHIVO_JUGADORES "jugadores.txt"

// =================== FUNCIONES DE EQUIPOS ===================

void registrar_equipo() {
    Equipo nuevo;
    bool valido;

    printf("\n--- REGISTRO DE EQUIPO ---\n");

    // Nombre del equipo
    do {
        printf("Nombre del equipo: ");
        leer_cadena(nuevo.nombre, sizeof(nuevo.nombre));
        valido = strlen(nuevo.nombre) >= 3;
        if (!valido) {
            printf("Error: El nombre debe tener al menos 3 caracteres.\n");
        }
    } while (!valido);

    // Liga
    do {
        printf("Liga: ");
        leer_cadena(nuevo.liga, sizeof(nuevo.liga));
        valido = strlen(nuevo.liga) >= 3;
        if (!valido) {
            printf("Error: El nombre de la liga debe tener al menos 3 caracteres.\n");
        }
    } while (!valido);

    // Ranking
    char input[50];
    do {
        printf("Ranking (1.0 - 10.0): ");
        leer_cadena(input, sizeof(input));
        valido = validar_saldo(input); // Reutilizamos esta función para validar numeros decimales
        if (valido) {
            nuevo.ranking = atof(input);
            if (nuevo.ranking < 1.0 || nuevo.ranking > 10.0) {
                printf("Error: El ranking debe estar entre 1.0 y 10.0.\n");
                valido = false;
            }
        } else {
            printf("Error: Ingrese un numero valido.\n");
        }
    } while (!valido);

    nuevo.id = obtener_siguiente_id(ARCHIVO_EQUIPOS);
    nuevo.num_jugadores = 0;

    if (guardar_equipo(nuevo)) {
        printf("\nEquipo registrado exitosamente. ID: %d\n", nuevo.id);
    } else {
        printf("Error al registrar el equipo.\n");
    }
}

void listar_equipos() {
    FILE *archivo = fopen(ARCHIVO_EQUIPOS, "r");
    if (!archivo) {
        printf("No hay equipos registrados.\n");
        return;
    }

    printf("\n=== LISTA DE EQUIPOS ===\n");
    printf("ID  | Nombre                | Liga                 | Ranking | Jugadores\n");
    printf("----+----------------------+---------------------+---------+----------\n");

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Equipo temp;
        if (sscanf(linea, "%d,%[^,],%[^,],%f,%d\n",
                   &temp.id,
                   temp.nombre,
                   temp.liga,
                   &temp.ranking,
                   &temp.num_jugadores) == 5) {
            
            printf("%-3d | %-20s | %-19s | %6.1f  | %d\n",
                   temp.id,
                   temp.nombre,
                   temp.liga,
                   temp.ranking,
                   temp.num_jugadores);
        }
    }

    fclose(archivo);
}

bool cargar_equipo(int id, Equipo *resultado) {
    FILE *archivo = fopen(ARCHIVO_EQUIPOS, "r");
    if (!archivo) return false;

    char linea[256];
    bool encontrado = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Equipo temp;
        if (sscanf(linea, "%d,%[^,],%[^,],%f,%d\n",
                   &temp.id,
                   temp.nombre,
                   temp.liga,
                   &temp.ranking,
                   &temp.num_jugadores) == 5) {
            
            if (temp.id == id) {
                *resultado = temp;
                encontrado = true;
                break;
            }
        }
    }

    fclose(archivo);
    
    // Si encontramos el equipo, cargamos sus jugadores
    if (encontrado) {
        FILE *arch_jugadores = fopen(ARCHIVO_JUGADORES, "r");
        if (arch_jugadores) {
            resultado->num_jugadores = 0;
            while (fgets(linea, sizeof(linea), arch_jugadores)) {
                Jugador temp;
                if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]\n",
                          &temp.id,
                          temp.nombre,
                          &temp.idEquipo,
                          &temp.numeroCamiseta,
                          temp.posicion) == 5) {
                    
                    if (temp.idEquipo == id && resultado->num_jugadores < MAX_JUGADORES) {
                        resultado->jugadores[resultado->num_jugadores++] = temp;
                    }
                }
            }
            fclose(arch_jugadores);
        }
    }

    return encontrado;
}

void mostrar_equipo(const Equipo *equipo) {
    printf("\n=== DETALLES DEL EQUIPO ===\n");
    printf("ID: %d\n", equipo->id);
    printf("Nombre: %s\n", equipo->nombre);
    printf("Liga: %s\n", equipo->liga);
    printf("Ranking: %.1f\n", equipo->ranking);
    printf("Numero de jugadores: %d\n", equipo->num_jugadores);

    if (equipo->num_jugadores > 0) {
        printf("\nJugadores:\n");
        printf("ID  | Nombre                | Numero | Posición\n");
        printf("----+----------------------+--------+----------\n");
        
        for (int i = 0; i < equipo->num_jugadores; i++) {
            printf("%-3d | %-20s | %-6d | %s\n",
                   equipo->jugadores[i].id,
                   equipo->jugadores[i].nombre,
                   equipo->jugadores[i].numeroCamiseta,
                   equipo->jugadores[i].posicion);
        }
    }
}

bool guardar_equipo(Equipo equipo) {
    crear_archivo_si_no_existe(ARCHIVO_EQUIPOS);
    
    // Si es un equipo nuevo
    if (equipo.id == 0) {
        equipo.id = obtener_siguiente_id(ARCHIVO_EQUIPOS);
    }
    
    FILE *archivo = fopen(ARCHIVO_EQUIPOS, "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        return false;
    }

    bool actualizado = false;
    char linea[256];

    // Copiar todos los equipos, actualizando o agregando el nuevo
    while (fgets(linea, sizeof(linea), archivo)) {
        Equipo temp_equipo;
        if (sscanf(linea, "%d,%[^,],%[^,],%f,%d\n",
                   &temp_equipo.id,
                   temp_equipo.nombre,
                   temp_equipo.liga,
                   &temp_equipo.ranking,
                   &temp_equipo.num_jugadores) == 5) {
            
            if (temp_equipo.id == equipo.id) {
                fprintf(temp, "%d,%s,%s,%.1f,%d\n",
                        equipo.id,
                        equipo.nombre,
                        equipo.liga,
                        equipo.ranking,
                        equipo.num_jugadores);
                actualizado = true;
            } else {
                fputs(linea, temp);
            }
        }
    }

    // Si no se actualizó ningún equipo existente, agregar el nuevo
    if (!actualizado) {
        fprintf(temp, "%d,%s,%s,%.1f,%d\n",
                equipo.id,
                equipo.nombre,
                equipo.liga,
                equipo.ranking,
                equipo.num_jugadores);
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_EQUIPOS);
    rename("temp.txt", ARCHIVO_EQUIPOS);

    return true;
}

void editar_equipo(int id_equipo) {
    Equipo equipo;
    if (!cargar_equipo(id_equipo, &equipo)) {
        printf("Error: Equipo no encontrado.\n");
        return;
    }

    printf("\n--- EDITAR EQUIPO ---\n");
    mostrar_equipo(&equipo);

    char input[50];
    bool valido;

    printf("\nDeje en blanco para mantener el valor actual\n");

    // Nombre
    printf("Nombre actual: %s\nNuevo nombre: ", equipo.nombre);
    leer_cadena(input, sizeof(input));
    if (strlen(input) > 0) {
        strcpy(equipo.nombre, input);
    }

    // Liga
    printf("Liga actual: %s\nNueva liga: ", equipo.liga);
    leer_cadena(input, sizeof(input));
    if (strlen(input) > 0) {
        strcpy(equipo.liga, input);
    }

    // Ranking
    do {
        printf("Ranking actual: %.1f\nNuevo ranking (1.0 - 10.0, Enter para mantener): ", equipo.ranking);
        leer_cadena(input, sizeof(input));
        
        if (strlen(input) == 0) {
            break;
        }
        
        valido = validar_saldo(input);
        if (valido) {
            float nuevo_ranking = atof(input);
            if (nuevo_ranking >= 1.0 && nuevo_ranking <= 10.0) {
                equipo.ranking = nuevo_ranking;
                break;
            }
        }
        printf("Error: Ingrese un ranking valido entre 1.0 y 10.0\n");
    } while (true);

    if (guardar_equipo(equipo)) {
        printf("Equipo actualizado exitosamente.\n");
    } else {
        printf("Error al actualizar el equipo.\n");
    }
}

void eliminar_equipo(int id_equipo) {
    Equipo equipo;
    if (!cargar_equipo(id_equipo, &equipo)) {
        printf("Error: Equipo no encontrado.\n");
        return;
    }

    printf("\n--- ELIMINAR EQUIPO ---\n");
    mostrar_equipo(&equipo);

    char confirmacion[3];
    printf("\n¿Está seguro de eliminar este equipo? (s/n): ");
    leer_cadena(confirmacion, sizeof(confirmacion));

    if (confirmacion[0] != 's' && confirmacion[0] != 'S') {
        printf("Operación cancelada.\n");
        return;
    }

    FILE *archivo = fopen(ARCHIVO_EQUIPOS, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        printf("Error al acceder a los archivos.\n");
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Equipo temp_equipo;
        if (sscanf(linea, "%d,%[^,],%[^,],%f,%d",
                   &temp_equipo.id,
                   temp_equipo.nombre,
                   temp_equipo.liga,
                   &temp_equipo.ranking,
                   &temp_equipo.num_jugadores) == 5) {
            
            if (temp_equipo.id != id_equipo) {
                fputs(linea, temp);
            }
        }
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_EQUIPOS);
    rename("temp.txt", ARCHIVO_EQUIPOS);

    // También eliminar los jugadores del equipo
    archivo = fopen(ARCHIVO_JUGADORES, "r");
    temp = fopen("temp.txt", "w");

    if (archivo && temp) {
        while (fgets(linea, sizeof(linea), archivo)) {
            Jugador temp_jugador;
            if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]",
                      &temp_jugador.id,
                      temp_jugador.nombre,
                      &temp_jugador.idEquipo,
                      &temp_jugador.numeroCamiseta,
                      temp_jugador.posicion) == 5) {
                
                if (temp_jugador.idEquipo != id_equipo) {
                    fputs(linea, temp);
                }
            }
        }
        fclose(archivo);
        fclose(temp);
        remove(ARCHIVO_JUGADORES);
        rename("temp.txt", ARCHIVO_JUGADORES);
    }

    printf("Equipo eliminado exitosamente.\n");
}

// =================== FUNCIONES DE JUGADORES ===================

void registrar_jugador(int id_equipo) {
    Equipo equipo;
    if (!cargar_equipo(id_equipo, &equipo)) {
        printf("Error: Equipo no encontrado.\n");
        return;
    }

    if (equipo.num_jugadores >= MAX_JUGADORES) {
        printf("Error: El equipo ya tiene el máximo de jugadores permitido.\n");
        return;
    }

    Jugador nuevo;
    bool valido;

    printf("\n--- REGISTRO DE JUGADOR ---\n");
    printf("Equipo: %s\n", equipo.nombre);

    // Nombre del jugador
    do {
        printf("Nombre del jugador: ");
        leer_cadena(nuevo.nombre, sizeof(nuevo.nombre));
        valido = validar_texto(nuevo.nombre);
        if (!valido) {
            printf("Error: El nombre debe contener solo letras y espacios.\n");
        }
    } while (!valido);

    // Numero de camiseta
    char input[10];
    do {
        printf("Numero de camiseta (1-99): ");
        leer_cadena(input, sizeof(input));
        valido = true;
        for (int i = 0; input[i]; i++) {
            if (!isdigit(input[i])) {
                valido = false;
                break;
            }
        }
        if (valido) {
            nuevo.numeroCamiseta = atoi(input);
            if (nuevo.numeroCamiseta < 1 || nuevo.numeroCamiseta > 99) {
                printf("Error: El numero debe estar entre 1 y 99.\n");
                valido = false;
            }
        }
    } while (!valido);

    // Posición
    printf("Posición (Portero/Defensa/Mediocampista/Delantero): ");
    leer_cadena(nuevo.posicion, sizeof(nuevo.posicion));

    nuevo.id = obtener_siguiente_id(ARCHIVO_JUGADORES);
    nuevo.idEquipo = id_equipo;

    // Guardar jugador
    crear_archivo_si_no_existe(ARCHIVO_JUGADORES);
    FILE *archivo = fopen(ARCHIVO_JUGADORES, "a");
    if (archivo) {
        fprintf(archivo, "%d,%s,%d,%d,%s\n",
                nuevo.id,
                nuevo.nombre,
                nuevo.idEquipo,
                nuevo.numeroCamiseta,
                nuevo.posicion);
        fclose(archivo);

        // Actualizar numero de jugadores en el equipo
        equipo.num_jugadores++;
        guardar_equipo(equipo);

        printf("\nJugador registrado exitosamente. ID: %d\n", nuevo.id);
    } else {
        printf("Error al registrar el jugador.\n");
    }
}

void listar_jugadores() {
    FILE *archivo = fopen(ARCHIVO_JUGADORES, "r");
    if (!archivo) {
        printf("No hay jugadores registrados.\n");
        return;
    }

    printf("\n=== LISTA DE JUGADORES ===\n");
    printf("ID  | Nombre                | Equipo                | Numero | Posición\n");
    printf("----+----------------------+----------------------+--------+----------\n");

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Jugador jugador;
        if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]",
                   &jugador.id,
                   jugador.nombre,
                   &jugador.idEquipo,
                   &jugador.numeroCamiseta,
                   jugador.posicion) == 5) {
            
            Equipo equipo;
            char nombre_equipo[MAX_NOMBRE] = "Equipo no encontrado";
            if (cargar_equipo(jugador.idEquipo, &equipo)) {
                strcpy(nombre_equipo, equipo.nombre);
            }
            
            printf("%-3d | %-20s | %-20s | %-6d | %s\n",
                   jugador.id,
                   jugador.nombre,
                   nombre_equipo,
                   jugador.numeroCamiseta,
                   jugador.posicion);
        }
    }

    fclose(archivo);
}

bool cargar_jugador(int id, Jugador *resultado) {
    FILE *archivo = fopen(ARCHIVO_JUGADORES, "r");
    if (!archivo) return false;

    char linea[256];
    bool encontrado = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Jugador temp;
        if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]",
                   &temp.id,
                   temp.nombre,
                   &temp.idEquipo,
                   &temp.numeroCamiseta,
                   temp.posicion) == 5) {
            
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

void mostrar_jugador(const Jugador *jugador) {
    Equipo equipo;
    char nombre_equipo[MAX_NOMBRE] = "Equipo no encontrado";
    if (cargar_equipo(jugador->idEquipo, &equipo)) {
        strcpy(nombre_equipo, equipo.nombre);
    }

    printf("\n=== DETALLES DEL JUGADOR ===\n");
    printf("ID: %d\n", jugador->id);
    printf("Nombre: %s\n", jugador->nombre);
    printf("Equipo: %s\n", nombre_equipo);
    printf("Numero: %d\n", jugador->numeroCamiseta);
    printf("Posición: %s\n", jugador->posicion);
}

void editar_jugador(int id_jugador) {
    Jugador jugador;
    if (!cargar_jugador(id_jugador, &jugador)) {
        printf("Error: Jugador no encontrado.\n");
        return;
    }

    printf("\n--- EDITAR JUGADOR ---\n");
    mostrar_jugador(&jugador);

    char input[50];
    bool valido;

    printf("\nDeje en blanco para mantener el valor actual\n");

    // Nombre
    do {
        printf("Nombre actual: %s\nNuevo nombre: ", jugador.nombre);
        leer_cadena(input, sizeof(input));
        
        if (strlen(input) == 0) break;
        
        valido = validar_texto(input);
        if (valido) {
            strcpy(jugador.nombre, input);
            break;
        }
        printf("Error: El nombre debe contener solo letras y espacios.\n");
    } while (true);

    // Numero de camiseta
    do {
        printf("Numero actual: %d\nNuevo numero (1-99, Enter para mantener): ", jugador.numeroCamiseta);
        leer_cadena(input, sizeof(input));
        
        if (strlen(input) == 0) break;
        
        valido = true;
        for (int i = 0; input[i]; i++) {
            if (!isdigit(input[i])) {
                valido = false;
                break;
            }
        }
        if (valido) {
            int nuevo_numero = atoi(input);
            if (nuevo_numero >= 1 && nuevo_numero <= 99) {
                jugador.numeroCamiseta = nuevo_numero;
                break;
            }
        }
        printf("Error: El numero debe estar entre 1 y 99.\n");
    } while (true);

    // Posición
    printf("Posición actual: %s\nNueva posición: ", jugador.posicion);
    leer_cadena(input, sizeof(input));
    if (strlen(input) > 0) {
        strcpy(jugador.posicion, input);
    }

    // Actualizar jugador
    FILE *archivo = fopen(ARCHIVO_JUGADORES, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        printf("Error al acceder a los archivos.\n");
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Jugador temp_jugador;
        if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]",
                   &temp_jugador.id,
                   temp_jugador.nombre,
                   &temp_jugador.idEquipo,
                   &temp_jugador.numeroCamiseta,
                   temp_jugador.posicion) == 5) {
            
            if (temp_jugador.id == id_jugador) {
                fprintf(temp, "%d,%s,%d,%d,%s\n",
                        jugador.id,
                        jugador.nombre,
                        jugador.idEquipo,
                        jugador.numeroCamiseta,
                        jugador.posicion);
            } else {
                fputs(linea, temp);
            }
        }
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_JUGADORES);
    rename("temp.txt", ARCHIVO_JUGADORES);

    printf("Jugador actualizado exitosamente.\n");
}

void eliminar_jugador(int id_jugador) {
    Jugador jugador;
    if (!cargar_jugador(id_jugador, &jugador)) {
        printf("Error: Jugador no encontrado.\n");
        return;
    }

    printf("\n--- ELIMINAR JUGADOR ---\n");
    mostrar_jugador(&jugador);

    char confirmacion[3];
    printf("\n¿Está seguro de eliminar este jugador? (s/n): ");
    leer_cadena(confirmacion, sizeof(confirmacion));

    if (confirmacion[0] != 's' && confirmacion[0] != 'S') {
        printf("Operación cancelada.\n");
        return;
    }

    // Actualizar el numero de jugadores en el equipo
    Equipo equipo;
    if (cargar_equipo(jugador.idEquipo, &equipo)) {
        equipo.num_jugadores--;
        guardar_equipo(equipo);
    }

    // Eliminar jugador
    FILE *archivo = fopen(ARCHIVO_JUGADORES, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        printf("Error al acceder a los archivos.\n");
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Jugador temp_jugador;
        if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]",
                   &temp_jugador.id,
                   temp_jugador.nombre,
                   &temp_jugador.idEquipo,
                   &temp_jugador.numeroCamiseta,
                   temp_jugador.posicion) == 5) {
            
            if (temp_jugador.id != id_jugador) {
                fputs(linea, temp);
            }
        }
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_JUGADORES);
    rename("temp.txt", ARCHIVO_JUGADORES);

    printf("Jugador eliminado exitosamente.\n");
}

void transferir_jugador(int id_jugador, int nuevo_equipo) {
    Jugador jugador;
    if (!cargar_jugador(id_jugador, &jugador)) {
        printf("Error: Jugador no encontrado.\n");
        return;
    }

    Equipo equipo_nuevo;
    if (!cargar_equipo(nuevo_equipo, &equipo_nuevo)) {
        printf("Error: Equipo destino no encontrado.\n");
        return;
    }

    if (equipo_nuevo.num_jugadores >= MAX_JUGADORES) {
        printf("Error: El equipo destino ya tiene el máximo de jugadores permitido.\n");
        return;
    }

    // Actualizar el numero de jugadores en ambos equipos
    Equipo equipo_actual;
    if (cargar_equipo(jugador.idEquipo, &equipo_actual)) {
        equipo_actual.num_jugadores--;
        guardar_equipo(equipo_actual);
    }

    equipo_nuevo.num_jugadores++;
    guardar_equipo(equipo_nuevo);

    // Actualizar el equipo del jugador
    jugador.idEquipo = nuevo_equipo;

    // Guardar cambios del jugador
    FILE *archivo = fopen(ARCHIVO_JUGADORES, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        printf("Error al acceder a los archivos.\n");
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Jugador temp_jugador;
        if (sscanf(linea, "%d,%[^,],%d,%d,%[^\n]",
                   &temp_jugador.id,
                   temp_jugador.nombre,
                   &temp_jugador.idEquipo,
                   &temp_jugador.numeroCamiseta,
                   temp_jugador.posicion) == 5) {
            
            if (temp_jugador.id == id_jugador) {
                fprintf(temp, "%d,%s,%d,%d,%s\n",
                        jugador.id,
                        jugador.nombre,
                        jugador.idEquipo,
                        jugador.numeroCamiseta,
                        jugador.posicion);
            } else {
                fputs(linea, temp);
            }
        }
    }

    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_JUGADORES);
    rename("temp.txt", ARCHIVO_JUGADORES);

    printf("Jugador transferido exitosamente al equipo %s.\n", equipo_nuevo.nombre);
} 