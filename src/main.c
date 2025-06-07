#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include "usuarios.h"
#include "equipos.h"
#include "partidos.h"
#include "apuestas.h"
#include "utils.h"

void mostrar_menu_principal();
void mostrar_menu_usuario(Usuario *usuario);
void mostrar_menu_admin(Usuario *usuario);

int main() {
    int opcion;
    Usuario usuario_actual;
    bool sesion_iniciada = false;

    do {
        if (!sesion_iniciada) {
            mostrar_menu_principal();
            printf("\nSeleccione una opcion: ");
            scanf("%d", &opcion);
            limpiar_buffer();

            switch (opcion) {
                case 1:
                    registrar_usuario();
                    break;
                case 2:
                    if (iniciar_sesion(&usuario_actual)) {
                        sesion_iniciada = true;
                    }
                    break;
                case 3:
                    printf("\n¡Hasta pronto!\n");
                    break;
                default:
                    printf("Opcion no valida.\n");
            }
        } else {
            if (usuario_actual.esAdministrador) {
                mostrar_menu_admin(&usuario_actual);
            } else {
                mostrar_menu_usuario(&usuario_actual);
            }
            printf("\nSeleccione una opcion: ");
            scanf("%d", &opcion);
            limpiar_buffer();

            if (usuario_actual.esAdministrador) {
                switch (opcion) {
                    case 1: // Gestión de equipos
                        printf("\n=== GESTION DE EQUIPOS ===\n");
                        printf("1. Registrar equipo\n");
                        printf("2. Listar equipos\n");
                        printf("3. Editar equipo\n");
                        printf("4. Eliminar equipo\n");
                        printf("Seleccione: ");
                        int op_equipo;
                        scanf("%d", &op_equipo);
                        limpiar_buffer();
                        
                        switch (op_equipo) {
                            case 1:
                                registrar_equipo();
                                break;
                            case 2:
                                listar_equipos();
                                break;
                            case 3:
                                printf("ID del equipo a editar: ");
                                int id_equipo;
                                scanf("%d", &id_equipo);
                                limpiar_buffer();
                                editar_equipo(id_equipo);
                                break;
                            case 4:
                                printf("ID del equipo a eliminar: ");
                                scanf("%d", &id_equipo);
                                limpiar_buffer();
                                eliminar_equipo(id_equipo);
                                break;
                        }
                        break;

                    case 2: // GestiOn de jugadores
                        printf("\n=== GESTION DE JUGADORES ===\n");
                        printf("1. Registrar jugador\n");
                        printf("2. Listar jugadores\n");
                        printf("3. Editar jugador\n");
                        printf("4. Eliminar jugador\n");
                        printf("5. Transferir jugador\n");
                        printf("Seleccione: ");
                        int op_jugador;
                        scanf("%d", &op_jugador);
                        limpiar_buffer();
                        
                        switch (op_jugador) {
                            case 1:
                                printf("ID del equipo: ");
                                int id_equipo;
                                scanf("%d", &id_equipo);
                                limpiar_buffer();
                                registrar_jugador(id_equipo);
                                break;
                            case 2:
                                listar_jugadores();
                                break;
                            case 3:
                                printf("ID del jugador a editar: ");
                                int id_jugador;
                                scanf("%d", &id_jugador);
                                limpiar_buffer();
                                editar_jugador(id_jugador);
                                break;
                            case 4:
                                printf("ID del jugador a eliminar: ");
                                scanf("%d", &id_jugador);
                                limpiar_buffer();
                                eliminar_jugador(id_jugador);
                                break;
                            case 5:
                                printf("ID del jugador a transferir: ");
                                scanf("%d", &id_jugador);
                                limpiar_buffer();
                                printf("ID del nuevo equipo: ");
                                scanf("%d", &id_equipo);
                                limpiar_buffer();
                                transferir_jugador(id_jugador, id_equipo);
                                break;
                        }
                        break;

                    case 3: // GestiOn de partidos
                        printf("\n=== GESTION DE PARTIDOS ===\n");
                        printf("1. Registrar partido\n");
                        printf("2. Listar partidos\n");
                        printf("3. Actualizar estadísticas\n");
                        printf("4. Finalizar partido\n");
                        printf("Seleccione: ");
                        int op_partido;
                        scanf("%d", &op_partido);
                        limpiar_buffer();
                        
                        switch (op_partido) {
                            case 1:
                                registrar_partido();
                                break;
                            case 2:
                                listar_partidos();
                                break;
                            case 3:
                                printf("ID del partido: ");
                                int id_partido;
                                scanf("%d", &id_partido);
                                limpiar_buffer();
                                actualizar_estadisticas_partido(id_partido);
                                break;
                            case 4:
                                printf("ID del partido: ");
                                scanf("%d", &id_partido);
                                limpiar_buffer();
                                finalizar_partido(id_partido);
                                procesar_apuestas_partido(id_partido);
                                break;
                        }
                        break;

                    case 4: // Ver todas las apuestas
                        listar_todas_apuestas();
                        break;

                    case 5: // Ver usuarios
                        listar_usuarios();
                        break;

                    case 6: // Cerrar sesion
                        sesion_iniciada = false;
                        printf("Sesion cerrada.\n");
                        break;

                    default:
                        printf("Opcion no valida.\n");
                }
            } else { // MenU de usuario normal
                switch (opcion) {
                    case 1: // Ver perfil
                        mostrar_perfil(&usuario_actual);
                        break;

                    case 2: // Realizar apuesta
                        realizar_apuesta(&usuario_actual);
                        break;

                    case 3: // Ver mis apuestas
                        listar_apuestas_usuario(usuario_actual.usuario);
                        break;

                    case 4: // Ver partidos
                        listar_partidos();
                        break;

                    case 5: // Agregar fondos
                        agregar_fondos(&usuario_actual);
                        break;

                    case 6: // Retirar fondos
                        retirar_fondos(&usuario_actual);
                        break;

                    case 7: // Cambiar contraseña
                        cambiar_contrasena(&usuario_actual);
                        break;

                    case 8: // Cerrar sesion
                        sesion_iniciada = false;
                        printf("Sesion cerrada.\n");
                        break;

                    default:
                        printf("Opcion no valida.\n");
                }
            }
        }
    } while (opcion != 3 || sesion_iniciada);

    return 0;
}

void mostrar_menu_principal() {
    printf("\n=== CASA DE APUESTAS ===\n");
    printf("1. Registrar usuario\n");
    printf("2. Iniciar sesion\n");
    printf("3. Salir\n");
}

void mostrar_menu_usuario(Usuario *usuario) {
    printf("\n=== MENU DE USUARIO ===\n");
    printf("Usuario: %s\n", usuario->nombre);
    printf("Saldo: $%.2f\n", usuario->saldo);
    printf("\n1. Ver perfil\n");
    printf("2. Realizar apuesta\n");
    printf("3. Ver mis apuestas\n");
    printf("4. Ver partidos\n");
    printf("5. Agregar fondos\n");
    printf("6. Retirar fondos\n");
    printf("7. Cambiar contraseña\n");
    printf("8. Cerrar sesion\n");
}

void mostrar_menu_admin(Usuario *usuario) {
    printf("\n=== MENU DE ADMINISTRADOR ===\n");
    printf("Administrador: %s\n", usuario->nombre);
    printf("\n1. GestiOn de equipos\n");
    printf("2. GestiOn de jugadores\n");
    printf("3. GestiOn de partidos\n");
    printf("4. Ver todas las apuestas\n");
    printf("5. Ver usuarios\n");
    printf("6. Cerrar sesion\n");
} 