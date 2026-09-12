#ifndef BUSQUEDA_SIMPLE_AVANZADA_H
#define BUSQUEDA_SIMPLE_AVANZADA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"

// Estructura para producción
typedef struct {
    char *nombre;
    char *autor;
    int anPubli;
    char *genero;
    char *resumen;
    int cantidadEjemplares;
} ProduccionBusqueda;

// Funciones
void busquedaSimple(void);
void busquedaAvanzada(void);

#endif