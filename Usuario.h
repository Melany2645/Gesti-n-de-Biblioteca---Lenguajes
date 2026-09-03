#ifndef USUARIO_H
#define USUARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    int id;
    char *nombre;
    char *apellido;
    char *email;
    char *telefono;
    int activo;
} Usuario;

typedef struct {
    Usuario *usuarios;
    int cantidad;
    int capacidad;
} ListaUsuarios;

/ Declaración de la función del menú /
void menuUsuario(void);

#endif