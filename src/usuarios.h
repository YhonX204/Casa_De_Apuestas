#ifndef USUARIOS_H
#define USUARIOS_H

#include "estructuras.h"

// Funciones de gestión de usuarios
void registrar_usuario();
bool iniciar_sesion(Usuario *usuario_actual);
void guardar_usuario(Usuario nuevo);
bool cargar_usuario(const char *usuario, Usuario *resultado);
bool actualizar_saldo(const char *usuario, float nuevo_saldo);
void mostrar_perfil(const Usuario *usuario);
void cambiar_contrasena(Usuario *usuario);
void agregar_fondos(Usuario *usuario);
void retirar_fondos(Usuario *usuario);
void listar_usuarios();
bool es_administrador(const char *usuario);

#endif // USUARIOS_H 