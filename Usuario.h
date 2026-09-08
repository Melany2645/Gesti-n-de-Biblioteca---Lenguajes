#ifndef USUARIO_H
#define USUARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMBRE 100
#define MAX_DIRECCION 150
#define ARCHIVO_USUARIOS "usuarios.json"

typedef struct {
    int id;
    char *nombre;
    char *direccion;
} Usuario;

typedef struct {
    Usuario *usuarios;
    int cantidad;
    int capacidad;
} ListaUsuarios;

void menuUsuario(void);
void CapturarDatosUsuario(char *nombre, char *direccion);
int ObtenerMaximoID(const char *nombreArchivo);
void AgregarUsuario(ListaUsuarios *lista, const char *nombreArchivo, 
                    const char *nombre, const char *direccion);
void CargarUsuariosDesdeArchivo(ListaUsuarios *lista, const char *nombreArchivo);
void GuardarUsuariosEnArchivo(ListaUsuarios *lista, const char *nombreArchivo);
void VisualizarUsuarioPorID(ListaUsuarios *lista, int id);
void ModificarUsuario(ListaUsuarios *lista, const char *nombreArchivo, int id);
void EliminarUsuario(ListaUsuarios *lista, const char *nombreArchivo, int id);
void LiberarListaUsuarios(ListaUsuarios *lista);

#endif