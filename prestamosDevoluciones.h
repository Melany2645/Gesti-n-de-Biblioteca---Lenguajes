#ifndef PRESTAMOS_DEVOLUCIONES_H
#define PRESTAMOS_DEVOLUCIONES_H

#include <stdio.h>
#include "Usuario.h"
#include "Biblioteca.h"

// Valores del estado
typedef enum {
    ACTIVO,
    VENCIDO,
    FINALIZADO
} EstadoPrestamo;

// Manejamos la fecha
typedef struct
{
    int dia, mes, anio;
} Fecha;

// Manejo de los datos del prestamo
typedef struct
{
    int idUsuario;
    int idPrestamo;
    Fecha fechaInicio;
    Fecha fechaFin;
    Fecha devolucion;
    EstadoPrestamo estado;
    int entregaTardia;
    double multa;
    int *idEjemplares;
    char **nombreProducciones;
    int cantidadEjemplares;

} Prestamo;

typedef struct 
{
    Prestamo **prestamos;
    int cantidadPrestamos;
    int capacidadPrestamos;
    int totalPrestamos;
    int siguienteIDPrestamo;
    ListaUsuarios *usuarios;
    Ejemplar *ejemplares;
    int cantidadEjemplares;

} SistemaPrestamos;


// Métodos
Usuario *buscarUsuarioPorID(ListaUsuarios *lista, int id);

void inicializarSistemaPrestamos(SistemaPrestamos *sistema);
void liberarPrestamos(SistemaPrestamos *prestamo);
void liberarSistemaPrestamos(SistemaPrestamos *sistema);

int realizarPrestamo(SistemaPrestamos *sistema, int idUsuario, Fecha fechaInicio, Fecha fechaFin, int *idEjemplaresSolicitados, int cantidad);

void consultarHistorialPrestamos(SistemaPrestamos *sistema, Fecha desde, Fecha hasta);
void consultarVencimientos(SistemaPrestamos *sistema, Fecha fechaActual);
void consultarPrestamosPorUsuario(SistemaPrestamos *sistema, int idUsuario);
void devolverPrestamo(SistemaPrestamos *sistema, int idPrestamo, Fecha fechaDevolucion);
void generarReporteEstadisticas(SistemaPrestamos *sistema, Fecha desde, Fecha hasta);
int crearArchivoPrestamos(void);
int guardarPrestamosJSON(SistemaPrestamos *sistema);


#endif // PRESTAMOS_DEVOLUCIONES_H