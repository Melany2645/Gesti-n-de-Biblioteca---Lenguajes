#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H // ya existe una vez no copiar varias.

#include <stdio.h>

#define MAXIMOPRODUCCION 300 //maximo de producciones que tendra nuestra biblioteca.
#define MAXIMOEJEMPLARES 140 //maximo de ejemplares.

typedef struct { //nombro una estructura
    char *nombre;
    char *autor;
    int anPubli; //Año de publicación
    char *genero;
    char *resumen;
    int numEjemplares;
} Produccion; //nombre del struct

typedef struct {
    int id;
} Ejemplar;

//Metodos
void menuCatalogo(void);
void rutaarchivo(void);
void leerArchivo(FILE *archivo);
void separarProduccion(char *linea);
#endif