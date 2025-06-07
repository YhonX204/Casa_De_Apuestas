#include "utils.h"
#include <time.h>
#include <stdlib.h>

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool validar_texto(const char *texto) {
    if (texto == NULL || strlen(texto) == 0) return false;
    
    for (int i = 0; texto[i]; i++) {
        if (!isalpha(texto[i]) && texto[i] != ' ') return false;
    }
    return true;
}

bool validar_usuario(const char *usuario) {
    if (usuario == NULL || strlen(usuario) < 4) return false;
    
    for (int i = 0; usuario[i]; i++) {
        if (!isalnum(usuario[i])) return false;
    }
    return true;
}

bool validar_saldo(const char *input) {
    if (input == NULL || strlen(input) == 0) return false;
    
    int puntos = 0;
    for (int i = 0; input[i]; i++) {
        if (input[i] == '.') {
            puntos++;
            if (puntos > 1) return false;
        } else if (!isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

void leer_cadena(char *destino, int tamano) {
    if (!fgets(destino, tamano, stdin)) {
        destino[0] = '\0';
        return;
    }
    size_t len = strlen(destino);
    if (len > 0 && destino[len-1] == '\n') {
        destino[len-1] = '\0';
    }
}

bool validar_fecha(const char *fecha) {
    if (strlen(fecha) != 10) return false;
    
    int dia, mes, anio;
    if (sscanf(fecha, "%d/%d/%d", &dia, &mes, &anio) != 3) return false;
    
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;
    if (anio < 2024 || anio > 2100) return false;
    
    return true;
}

bool validar_hora(const char *hora) {
    if (strlen(hora) != 5) return false;
    
    int hh, mm;
    if (sscanf(hora, "%d:%d", &hh, &mm) != 2) return false;
    
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    
    return true;
}

bool validar_correo(const char *correo) {
    if (correo == NULL) return false;
    
    const char *arroba = strchr(correo, '@');
    if (!arroba) return false;
    
    if (strstr(correo, "@gmail.com") == NULL) return false;
    
    return strlen(correo) >= 10;
}

bool validar_cedula(const char *cedula) {
    if (cedula == NULL || strlen(cedula) < 6) return false;
    
    for (int i = 0; cedula[i]; i++) {
        if (!isdigit(cedula[i])) return false;
    }
    return true;
}

bool archivo_existe(const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo) {
        fclose(archivo);
        return true;
    }
    return false;
}

void crear_archivo_si_no_existe(const char *nombre_archivo) {
    if (!archivo_existe(nombre_archivo)) {
        FILE *archivo = fopen(nombre_archivo, "w");
        if (archivo) {
            fclose(archivo);
        }
    }
}

int obtener_siguiente_id(const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) return 1;
    
    int max_id = 0;
    char linea[256];
    
    while (fgets(linea, sizeof(linea), archivo)) {
        int id;
        if (sscanf(linea, "%d,", &id) == 1) {
            if (id > max_id) max_id = id;
        }
    }
    
    fclose(archivo);
    return max_id + 1;
}

void encriptar_contrasena(const char *contrasena, char *contrasena_encriptada) {
    // Implementación básica de encriptación (en producción usar una biblioteca de criptografía)
    strcpy(contrasena_encriptada, contrasena);
    for (int i = 0; contrasena_encriptada[i]; i++) {
        contrasena_encriptada[i] = contrasena_encriptada[i] + 1;
    }
}

bool verificar_contrasena(const char *contrasena_ingresada, const char *contrasena_almacenada) {
    char contrasena_encriptada[20];
    encriptar_contrasena(contrasena_ingresada, contrasena_encriptada);
    return strcmp(contrasena_encriptada, contrasena_almacenada) == 0;
}

void obtener_fecha_actual(char *fecha) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(fecha, 20, "%d/%m/%Y", tm);
}

void obtener_hora_actual(char *hora) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(hora, 10, "%H:%M", tm);
}

void formatear_dinero(float cantidad, char *resultado) {
    sprintf(resultado, "%.2f", cantidad);
} 