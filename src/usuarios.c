#include "usuarios.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ARCHIVO_USUARIOS "usuarios.txt"

void registrar_usuario() {
    Usuario nuevo;
    char input[50];
    bool valido;

    printf("\n--- REGISTRO DE USUARIO ---\n");

    // Nombre completo
    do {
        printf("Nombre completo (solo letras): ");
        leer_cadena(nuevo.nombre, sizeof(nuevo.nombre));
        valido = validar_texto(nuevo.nombre);
        if (!valido) {
            printf("Error: Solo se permiten letras y espacios.\n");
        }
    } while (!valido || strlen(nuevo.nombre) < 5);

    // Usuario
    do {
        printf("Usuario (sin espacios ni caracteres especiales): ");
        leer_cadena(nuevo.usuario, sizeof(nuevo.usuario));
        valido = validar_usuario(nuevo.usuario);
        if (!valido) {
            printf("Error: Solo use letras y numeros.\n");
        } else {
            // Verificar si el usuario ya existe
            Usuario temp;
            if (cargar_usuario(nuevo.usuario, &temp)) {
                printf("Error: El usuario ya existe.\n");
                valido = false;
            }
        }
    } while (!valido);

    // ContraseÃ±a
    char contrasena_temp[20];
    do {
        printf("Contraseña (minimo 6 caracteres): ");
        leer_cadena(contrasena_temp, sizeof(contrasena_temp));
        if (strlen(contrasena_temp) < 6) {
            printf("Error: La contraseña debe tener al menos 6 caracteres.\n");
        }
    } while (strlen(contrasena_temp) < 6);
    
    encriptar_contrasena(contrasena_temp, nuevo.contrasena);

    // Saldo inicial
    do {
        printf("Saldo inicial (ejemplo: 10000.00): ");
        leer_cadena(input, sizeof(input));
        valido = validar_saldo(input);
        if (valido) {
            nuevo.saldo = atof(input);
            if (nuevo.saldo <= 0) {
                printf("Error: El saldo debe ser positivo.\n");
                valido = false;
            }
        } else {
            printf("Error: Ingrese solo numeros y un punto decimal.\n");
        }
    } while (!valido);

    // Cedula
    do {
        printf("Cedula (solo numeros): ");
        leer_cadena(nuevo.cedula, sizeof(nuevo.cedula));
        valido = validar_cedula(nuevo.cedula);
        if (!valido) {
            printf("Error: Cedula invalida.\n");
        }
    } while (!valido);

    // Correo
    do {
        printf("Correo electronico (debe terminar en @gmail.com): ");
        leer_cadena(nuevo.correo, sizeof(nuevo.correo));
        valido = validar_correo(nuevo.correo);
        if (!valido) {
            printf("Error: Correo invalido.\n");
        }
    } while (!valido);

    nuevo.id = obtener_siguiente_id(ARCHIVO_USUARIOS);
    nuevo.esAdministrador = 0;

    guardar_usuario(nuevo);
    printf("\nRegistro exitoso. ID: %d\n", nuevo.id);
}

bool iniciar_sesion(Usuario *usuario_actual) {
    char usuario_ingresado[20];
    char contrasena_ingresada[20];
    
    printf("\n--- INICIAR SESION ---\n");
    printf("Usuario: ");
    leer_cadena(usuario_ingresado, sizeof(usuario_ingresado));
    
    printf("Contraseña: ");
    leer_cadena(contrasena_ingresada, sizeof(contrasena_ingresada));
    
    if (cargar_usuario(usuario_ingresado, usuario_actual)) {
        if (verificar_contrasena(contrasena_ingresada, usuario_actual->contrasena)) {
            printf("\¡Bienvenido, %s!\n", usuario_actual->nombre);
            printf("Tipo de usuario: %s\n", usuario_actual->esAdministrador ? "Administrador" : "Cliente");
            printf("Saldo actual: $%.2f\n", usuario_actual->saldo);
            return true;
        }
    }
    
    printf("Error: Usuario o contraseña incorrectos.\n");
    return false;
}

void guardar_usuario(Usuario nuevo) {
    crear_archivo_si_no_existe(ARCHIVO_USUARIOS);
    
    FILE *archivo = fopen(ARCHIVO_USUARIOS, "a");
    if (archivo == NULL) {
        printf("Error al abrir el archivo de usuarios.\n");
        return;
    }

    fprintf(archivo, "%d,%s,%s,%s,%.2f,%s,%s,%d\n",
            nuevo.id,
            nuevo.nombre,
            nuevo.usuario,
            nuevo.contrasena,
            nuevo.saldo,
            nuevo.cedula,
            nuevo.correo,
            nuevo.esAdministrador);

    fclose(archivo);
}

bool cargar_usuario(const char *usuario, Usuario *resultado) {
    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");
    if (archivo == NULL) return false;

    char linea[256];
    bool encontrado = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Usuario temp;
        if (sscanf(linea, "%d,%[^,],%[^,],%[^,],%f,%[^,],%[^,],%d\n",
                   &temp.id,
                   temp.nombre,
                   temp.usuario,
                   temp.contrasena,
                   &temp.saldo,
                   temp.cedula,
                   temp.correo,
                   &temp.esAdministrador) == 8) {
            
            if (strcmp(temp.usuario, usuario) == 0) {
                *resultado = temp;
                encontrado = true;
                break;
            }
        }
    }

    fclose(archivo);
    return encontrado;
}

bool actualizar_saldo(const char *usuario, float nuevo_saldo) {
    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (!archivo || !temp) {
        if (archivo) fclose(archivo);
        if (temp) fclose(temp);
        return false;
    }

    char linea[256];
    bool actualizado = false;

    while (fgets(linea, sizeof(linea), archivo)) {
        Usuario temp_usuario;
        char resto_linea[256];
        
        if (sscanf(linea, "%d,%[^,],%[^,],%[^,],%f,%[^\n]",
                   &temp_usuario.id,
                   temp_usuario.nombre,
                   temp_usuario.usuario,
                   temp_usuario.contrasena,
                   &temp_usuario.saldo,
                   resto_linea) == 6) {
            
            if (strcmp(temp_usuario.usuario, usuario) == 0) {
                fprintf(temp, "%d,%s,%s,%s,%.2f,%s\n",
                        temp_usuario.id,
                        temp_usuario.nombre,
                        temp_usuario.usuario,
                        temp_usuario.contrasena,
                        nuevo_saldo,
                        resto_linea);
                actualizado = true;
            } else {
                fputs(linea, temp);
            }
        }
    }

    fclose(archivo);
    fclose(temp);

    if (actualizado) {
        remove(ARCHIVO_USUARIOS);
        rename("temp.txt", ARCHIVO_USUARIOS);
        return true;
    } else {
        remove("temp.txt");
        return false;
    }
}

void mostrar_perfil(const Usuario *usuario) {
    printf("\n=== PERFIL DE USUARIO ===\n");
    printf("ID: %d\n", usuario->id);
    printf("Nombre: %s\n", usuario->nombre);
    printf("Usuario: %s\n", usuario->usuario);
    printf("Saldo: $%.2f\n", usuario->saldo);
    printf("Cedula: %s\n", usuario->cedula);
    printf("Correo: %s\n", usuario->correo);
    printf("Tipo: %s\n", usuario->esAdministrador ? "Administrador" : "Cliente");
}

void cambiar_contrasena(Usuario *usuario) {
    char contrasena_actual[20];
    char nueva_contrasena[20];
    
    printf("\n--- CAMBIAR CONTRASEÃ‘A ---\n");
    printf("Contraseña actual: ");
    leer_cadena(contrasena_actual, sizeof(contrasena_actual));
    
    if (!verificar_contrasena(contrasena_actual, usuario->contrasena)) {
        printf("Error: Contraseña actual incorrecta.\n");
        return;
    }
    
    do {
        printf("Nueva contraseña (mÃ­nimo 6 caracteres): ");
        leer_cadena(nueva_contrasena, sizeof(nueva_contrasena));
        if (strlen(nueva_contrasena) < 6) {
            printf("Error: La contraseña debe tener al menos 6 caracteres.\n");
        }
    } while (strlen(nueva_contrasena) < 6);
    
    encriptar_contrasena(nueva_contrasena, usuario->contrasena);
    
    // Actualizar en archivo
    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");
    FILE *temp = fopen("temp.txt", "w");
    
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Usuario temp_usuario;
        char resto_linea[256];
        
        if (sscanf(linea, "%d,%[^,],%[^,],%[^,],%f,%[^\n]",
                   &temp_usuario.id,
                   temp_usuario.nombre,
                   temp_usuario.usuario,
                   temp_usuario.contrasena,
                   &temp_usuario.saldo,
                   resto_linea) == 6) {
            
            if (temp_usuario.id == usuario->id) {
                fprintf(temp, "%d,%s,%s,%s,%.2f,%s\n",
                        usuario->id,
                        usuario->nombre,
                        usuario->usuario,
                        usuario->contrasena,
                        usuario->saldo,
                        resto_linea);
            } else {
                fputs(linea, temp);
            }
        }
    }
    
    fclose(archivo);
    fclose(temp);
    remove(ARCHIVO_USUARIOS);
    rename("temp.txt", ARCHIVO_USUARIOS);
    
    printf("Contraseña cambiada exitosamente.\n");
}

void agregar_fondos(Usuario *usuario) {
    char input[50];
    float monto;
    bool valido;
    
    printf("\n--- AGREGAR FONDOS ---\n");
    printf("Saldo actual: $%.2f\n", usuario->saldo);
    
    do {
        printf("Monto a agregar: $");
        leer_cadena(input, sizeof(input));
        valido = validar_saldo(input);
        if (valido) {
            monto = atof(input);
            if (monto <= 0) {
                printf("Error: El monto debe ser positivo.\n");
                valido = false;
            }
        } else {
            printf("Error: Ingrese un monto vÃ¡lido.\n");
        }
    } while (!valido);
    
    usuario->saldo += monto;
    if (actualizar_saldo(usuario->usuario, usuario->saldo)) {
        printf("Fondos agregados exitosamente.\n");
        printf("Nuevo saldo: $%.2f\n", usuario->saldo);
    } else {
        usuario->saldo -= monto;
        printf("Error al actualizar el saldo.\n");
    }
}

void retirar_fondos(Usuario *usuario) {
    char input[50];
    float monto;
    bool valido;
    
    printf("\n--- RETIRAR FONDOS ---\n");
    printf("Saldo actual: $%.2f\n", usuario->saldo);
    
    do {
        printf("Monto a retirar: $");
        leer_cadena(input, sizeof(input));
        valido = validar_saldo(input);
        if (valido) {
            monto = atof(input);
            if (monto <= 0) {
                printf("Error: El monto debe ser positivo.\n");
                valido = false;
            } else if (monto > usuario->saldo) {
                printf("Error: Saldo insuficiente.\n");
                valido = false;
            }
        } else {
            printf("Error: Ingrese un monto vÃ¡lido.\n");
        }
    } while (!valido);
    
    usuario->saldo -= monto;
    if (actualizar_saldo(usuario->usuario, usuario->saldo)) {
        printf("Retiro exitoso.\n");
        printf("Nuevo saldo: $%.2f\n", usuario->saldo);
    } else {
        usuario->saldo += monto;
        printf("Error al actualizar el saldo.\n");
    }
}

void listar_usuarios() {
    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");
    if (!archivo) {
        printf("No hay usuarios registrados.\n");
        return;
    }

    printf("\n=== LISTA DE USUARIOS ===\n");
    printf("ID  | Usuario        | Nombre                | Saldo      | Tipo\n");
    printf("----+----------------+----------------------+------------+-------------\n");

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        Usuario temp;
        if (sscanf(linea, "%d,%[^,],%[^,],%[^,],%f,%[^,],%[^,],%d",
                   &temp.id,
                   temp.nombre,
                   temp.usuario,
                   temp.contrasena,
                   &temp.saldo,
                   temp.cedula,
                   temp.correo,
                   &temp.esAdministrador) == 8) {
            
            printf("%-3d | %-14s | %-20s | $%-9.2f | %s\n",
                   temp.id,
                   temp.usuario,
                   temp.nombre,
                   temp.saldo,
                   temp.esAdministrador ? "Admin" : "Cliente");
        }
    }

    fclose(archivo);
}

bool es_administrador(const char *usuario) {
    Usuario temp;
    if (cargar_usuario(usuario, &temp)) {
        return temp.esAdministrador == 1;
    }
    return false;
} 
