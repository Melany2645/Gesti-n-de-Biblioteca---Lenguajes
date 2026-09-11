#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <stdio.h>


typedef struct
{
    char *nombre;
    char *autor;
    int anPubli;
    char *genero;
    char *resumen;
    int cantidadEjemplares;

} Produccion;


typedef struct
{
    int id;
    char *nombreProduccion;
    int disponible;
} Ejemplar;


// Menu de catalogo
void menuCatalogo(void);

void mostrarCatalogo(void);

// Manejo del archivo de entrada
void rutaarchivo(void);

void leerArchivo(FILE *archivo);


// Manejo de textos
char *copiarTexto(char *texto);


// Validaciones
int validarProduccion(char *linea);

int esNumero(char *texto);

int existeProduccion(
    Produccion *catalogo,
    int cantidad,
    char *nombre
);


// Producciones
void guardarProduccion(
    char *linea,
    Produccion *produccion
);

void liberarProduccion(
    Produccion *produccion
);

int agregarProduccion(
    Produccion **catalogo,
    int *cantidad,
    Produccion *nueva
);


// JSON del catalogo
int guardarCatalogoJSON(
    Produccion *catalogo,
    int cantidad
);

int cargarCatalogoJSON(
    Produccion **catalogo,
    int *cantidad
);


// Ejemplares
int agregarEjemplar(
    Ejemplar **ejemplares,
    int *cantidad,
    Ejemplar *nuevo
);

void liberarEjemplar(
    Ejemplar *ejemplar
);

void generarEjemplares(
    Produccion *produccion,
    Ejemplar **ejemplares,
    int *cantidadEjemplares
);

int siguienteId(
    Ejemplar *ejemplares,
    int cantidad
);


// JSON de ejemplares
int guardarEjemplaresJSON(
    Ejemplar *ejemplares,
    int cantidad
);

int cargarEjemplaresJSON(
    Ejemplar **ejemplares,
    int *cantidad
);

int cantidadDisponibles(
    Ejemplar *ejemplares,
    int cantidadEjemplares,
    char *nombre
);
#endif