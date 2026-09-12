#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

// Estructura para contar préstamos por producción
typedef struct {
    char *nombreProduccion;
    int cantidadPrestamos;
} ProduccionConteo;

// Estructura para contar préstamos por usuario
typedef struct {
    int idUsuario;
    char *nombreUsuario;
    int cantidadPrestamos;
} UsuarioConteo;

// Estructura para conteo por mes/año
typedef struct {
    int mes;
    int anio;
    double totalDinero;
} MesAnioConteo;

// Estructura para conteo por género
typedef struct {
    char *genero;
    int cantidadPrestamos;
} GeneroConteo;

// Función principal para mostrar menú de estadísticas
void menuEstadisticas(void);

// Funciones de estadísticas individuales
void estadisticasProduccionesMasPrestadas(void);
void estadisticasUsuarioMasPrestamos(void);
void estadisticasMesesAnioMasDinero(void);
void estadisticasGeneros(void);

// Funciones auxiliares
static void limpiarBuffer(void);

#endif