#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "prestamosDevoluciones.h"
#include "menu.h"
#include "menu.h"

#define ARCHIVO_PRESTAMOS "prestamos.json"

// Manejo y creación del archivo que contiene la información de las prestamos
int crearArchivoPrestamos(void)
{
    FILE *archivo = fopen(ARCHIVO_PRESTAMOS, "ab");

    if (archivo == NULL)
    {
        printf("Error al crear el archivo de prestamos.\n");
        return 0;
    }

    fclose(archivo);
    return 1;
}

// E: Puntero al objeto JSON, nombre de la fecha, estructura de fecha
// S: Nada, solo se agrega el objeto de fecha al json
// R: Nada
// F: Agregar la fecha al json
static void agregarFechaJSON(cJSON *objeto, const char *nombre, Fecha fecha)
{
    cJSON *fechaJSON = cJSON_CreateObject();

    if (fechaJSON == NULL)
    {
        return;
    }

    cJSON_AddNumberToObject(fechaJSON, "dia", fecha.dia);
    cJSON_AddNumberToObject(fechaJSON, "mes", fecha.mes);
    cJSON_AddNumberToObject(fechaJSON, "anio", fecha.anio);
    cJSON_AddItemToObject(objeto, nombre, fechaJSON);
}

// E: Puntero al sistema de prestamos
// S: Nos retorna 1 si se guardó y 0 se hubo un error
// R: Valores válidos en el sistema de prestamos
// F: Guardar los prestamos en un archivo json
int guardarPrestamosJSON(SistemaPrestamos *sistema)
{
    if (sistema == NULL || !crearArchivoPrestamos())
    {
        return 0;
    }

    // Creamos un arreglo json para almacenar los prestamos
    cJSON *arreglo = cJSON_CreateArray();
    if (arreglo == NULL)
    {
        return 0;
    }

    // Recorremos todos los prestamos y los agregamos al arreglo json para guardarlos en el archivo
    for (int i = 0; i < sistema->cantidadPrestamos; i++)
    {
        Prestamo *prestamo = sistema->prestamos[i];
        cJSON *objeto = cJSON_CreateObject();
        cJSON *ejemplares = cJSON_CreateArray();

        if (prestamo == NULL || objeto == NULL || ejemplares == NULL)
        {
            cJSON_Delete(objeto);
            cJSON_Delete(ejemplares);
            cJSON_Delete(arreglo);
            return 0;
        }

        // Agregamos los datos del prestamo al json 
        cJSON_AddNumberToObject(objeto, "idPrestamo", prestamo->idPrestamo);
        cJSON_AddNumberToObject(objeto, "idUsuario", prestamo->idUsuario);
        cJSON_AddNumberToObject(objeto, "estado", prestamo->estado);
        cJSON_AddBoolToObject(objeto, "entregaTardia", prestamo->entregaTardia);
        cJSON_AddNumberToObject(objeto, "multa", prestamo->multa);
        agregarFechaJSON(objeto, "fechaInicio", prestamo->fechaInicio);
        agregarFechaJSON(objeto, "fechaFin", prestamo->fechaFin);

        if (prestamo->devolucion.anio != 0)
        {
            agregarFechaJSON(objeto, "devolucion", prestamo->devolucion);
        }

        for (int j = 0; j < prestamo->cantidadEjemplares; j++)
        {
            cJSON *ejemplar = cJSON_CreateObject();

            if (ejemplar == NULL)
            {
                cJSON_Delete(objeto);
                cJSON_Delete(ejemplares);
                cJSON_Delete(arreglo);
                return 0;
            }

            cJSON_AddNumberToObject(
                ejemplar,
                "id",
                prestamo->idEjemplares[j]
            );
            cJSON_AddStringToObject(
                ejemplar,
                "produccion",
                prestamo->nombreProducciones[j]
            );
            cJSON_AddItemToArray(ejemplares, ejemplar);
        }

        cJSON_AddItemToObject(objeto, "ejemplares", ejemplares);
        cJSON_AddItemToArray(arreglo, objeto);
    }

    char *textoJSON = cJSON_Print(arreglo);
    cJSON_Delete(arreglo);

    if (textoJSON == NULL)
    {
        return 0;
    }

    FILE *archivo = fopen(ARCHIVO_PRESTAMOS, "w");
    if (archivo == NULL)
    {
        cJSON_free(textoJSON);
        return 0;
    }

    fputs(textoJSON, archivo);
    fclose(archivo);
    cJSON_free(textoJSON);
    return 1;
}

// E: Lista de los usuarios, id del usuario a buscar
// S: Nos retorna la dirección de memoria del usuario si se encuentra, NULL si no se encuentra
// R: Valores válidos en la lista de usuarios
// F: Buscar un usuario por su ID en la lista de usuarios
Usuario *buscarUsuarioPorID(ListaUsuarios *lista, int id)
{
    if (lista == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < lista->cantidad; i++)
    {
        if (lista->usuarios[i].id == id)
        {
            return &lista->usuarios[i];
        }
    }

    return NULL;
}

// E: Lista de los usuarios, nombre del usuario a buscar
// S: Nos retorna la dirección de memoria del usuario si se encuentra, NULL si no se encuentra
// R: Valores válidos en la lista de usuarios
// F: Buscar un usuario por su nombre en la lista de usuarios
char buscarUsuarioPorNombre(ListaUsuarios *lista, char nombre)
{
    if (lista == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < lista->cantidad; i++)
    {
        if (lista->usuarios[i].id == nombre)
        {
            return &lista->usuarios[i];
        }
    }

    return NULL;
}

// E: Puntero al sistema de prestamos, id del ejemplar a buscar
// S: Nos retorna la dirección de memoria del ejemplar si se encuentra, NULL si no se encuentra
// R: Valores válidos en el sistema de prestamos
// F: Buscar un ejemplar por su ID en el sistema de prestamos
static Ejemplar *buscarEjemplarPorID(SistemaPrestamos *sistema, int id)
{
    if (sistema == NULL || sistema->ejemplares == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < sistema->cantidadEjemplares; i++)
    {
        if (sistema->ejemplares[i].id == id)
        {
            return &sistema->ejemplares[i];
        }
    }

    return NULL;
}

// E: Un puntero al sistema de prestamos y el id del ejemplar a verificar
// S: Retorna 1 si el ejemplar está disponible, 0 si no lo está
// R: Valores válidos en el sistema de prestamos
// F: Verificar la disponibilidad de un ejemplar en el sistema de prestamos
static int verificarDisponibilidadEjemplar(
    SistemaPrestamos *sistema,
    int idEjemplar
)
{
    if (buscarEjemplarPorID(sistema, idEjemplar) == NULL)
    {
        return 0;
    }

    for (int i = 0; i < sistema->cantidadPrestamos; i++)
    {
        Prestamo *prestamo = sistema->prestamos[i];

        if (prestamo == NULL || prestamo->estado != ACTIVO)
        {
            continue;
        }

        for (int j = 0; j < prestamo->cantidadEjemplares; j++)
        {
            if (prestamo->idEjemplares[j] == idEjemplar)
            {
                return 0;
            }
        }
    }

    return 1;
}

// E: Un puntero al prestamo
// S: Nada, solo libera la memoria utilizada por el prestamo
// R: Valores válidos en el prestamo
// F: Liberar la memoria utilizada por un prestamo

// Ya que almacenamos información del prestamo de forma local y luego la almacenamos en el json.
static void liberarPrestamoLocal(Prestamo *prestamo)
{
    if (prestamo == NULL)
    {
        return;
    }

    for (int i = 0; i < prestamo->cantidadEjemplares; i++)
    {
        free(prestamo->nombreProducciones[i]);
    }

    free(prestamo->nombreProducciones);
    free(prestamo->idEjemplares);
    free(prestamo);
}

// Funciones auxiliares para manejar el sistema de prestamos

// E: Dos fechas a comparar.
// S: Un valor entero que indica la relación entre las fechas.
// R: Las fechas deben estar en formato válido.
// F: Compara dos fechas y devuelve un valor que indica si la primera fecha es anterior, igual o posterior a la segunda.
int compararFechas(Fecha fecha1, Fecha fecha2)
{
    if (fecha1.anio != fecha2.anio)
        return fecha1.anio - fecha2.anio;
    if (fecha1.mes != fecha2.mes)
        return fecha1.mes - fecha2.mes;
    return fecha1.dia - fecha2.dia;
}


// E: Dos fechas a comparar.
// S: Un valor entero que indica la diferencia en días entre las fechas.
// R: Las fechas deben estar en formato válido.
// F: Calcula la diferencia en días entre dos fechas.
int diferenciaEntreDias(Fecha fecha1, Fecha fecha2)
{
    // Esta función calcula la diferencia en días entre dos fechas.
    // Para que sea más simple, utilizamos el metodo de Año exacto pero tomamos los 30 de cada mes
    int dias1 = fecha1.anio * 365 + fecha1.mes * 30 + fecha1.dia;
    int dias2 = fecha2.anio * 365 + fecha2.mes * 30 + fecha2.dia;
    return dias2 - dias1;
}

// E: Un puntero al sistema de prestamos, id del usuario, fecha de inicio y fecha final, arreglo de ids de ejemplares solicitados y la cantidad de ejemplares solicitados
// S: Si no hubo ningún error, nos retorna el id del prestamo, y si hubo algún error nos retorna 0
// R: Valores válidos en el sistema de prestamos
// F: Realizar un préstamo de ejemplares a un usuario
int realizarPrestamo(SistemaPrestamos *sistema, int idUsuario, Fecha fechaInicio, Fecha fechaFin, int *idEjemplaresSolicitados, int cantidad)
{
    // Esta función realiza un préstamo de ejemplares a un usuario.
    // Se verifica que el usuario exista y que los ejemplares estén disponibles.
    // Luego se crea un nuevo préstamo y se guarda en el sistema.

    if (sistema == NULL || idEjemplaresSolicitados == NULL || cantidad <= 0)
    {
        printf("Datos invalidos para realizar el prestamo.\n");
        return 0;
    }

    if (compararFechas(fechaInicio, fechaFin) > 0)
    {
        printf("La fecha de inicio no puede ser posterior a la fecha final.\n");
        return 0;
    }

    // Verificar si el usuario existe
    Usuario *usuario = buscarUsuarioPorID(sistema->usuarios, idUsuario);
    if (usuario == NULL)
    {
        printf("Usuario con ID %d no encontrado.\n", idUsuario);
        return 0;
    }

    // Verificar disponibilidad de los ejemplares
    for (int i = 0; i < cantidad; i++)
    {
        int idEjemplar = idEjemplaresSolicitados[i];

        for (int j = 0; j < i; j++)
        {
            if (idEjemplaresSolicitados[j] == idEjemplar)
            {
                printf("El ejemplar con ID %d fue solicitado mas de una vez.\n",
                       idEjemplar);
                return 0;
            }
        }

        if (!verificarDisponibilidadEjemplar(sistema, idEjemplar))
        {
            printf("El ejemplar con ID %d no existe o no esta disponible.\n", idEjemplar);
            return 0;
        }
    }

    // Crear un nuevo préstamo
    Prestamo *nuevoPrestamo = malloc(sizeof(Prestamo));
    if (nuevoPrestamo == NULL)
    {
        printf("Error de memoria al crear el préstamo.\n");
        return 0;
    }

    // Almacenamos la información del préstamo de forma local y luego la almacenamos en el json.

    nuevoPrestamo->idUsuario = idUsuario;
    nuevoPrestamo->idPrestamo = sistema->siguienteIDPrestamo++;
    nuevoPrestamo->fechaInicio = fechaInicio;
    nuevoPrestamo->fechaFin = fechaFin;
    nuevoPrestamo->estado = ACTIVO;
    nuevoPrestamo->entregaTardia = 0;
    nuevoPrestamo->devolucion = (Fecha){0, 0, 0};
    nuevoPrestamo->multa = 0.0;
    nuevoPrestamo->cantidadEjemplares = cantidad;

    nuevoPrestamo->idEjemplares = (int *)malloc(cantidad * sizeof(int));
    nuevoPrestamo->nombreProducciones = (char **)calloc(
        cantidad,
        sizeof(char *)
    );

    if (nuevoPrestamo->idEjemplares == NULL ||
        nuevoPrestamo->nombreProducciones == NULL)
    {
        liberarPrestamoLocal(nuevoPrestamo);
        printf("Error de memoria al crear los ejemplares del prestamo.\n");
        return 0;
    }

    for (int i = 0; i < cantidad; i++)
    {
        Ejemplar *ejemplar = buscarEjemplarPorID(
            sistema,
            idEjemplaresSolicitados[i]
        );

        nuevoPrestamo->idEjemplares[i] = idEjemplaresSolicitados[i];
        nuevoPrestamo->nombreProducciones[i] = copiarTexto(
            ejemplar->nombreProduccion
        );

        if (nuevoPrestamo->nombreProducciones[i] == NULL)
        {
            liberarPrestamoLocal(nuevoPrestamo);
            printf("Error de memoria al guardar la produccion del ejemplar.\n");
            return 0;
        }
    }

    if (sistema->cantidadPrestamos >= sistema->capacidadPrestamos)
    {
        int nuevaCapacidad = sistema->capacidadPrestamos == 0
            ? 4
            : sistema->capacidadPrestamos * 2;
        Prestamo **nuevosPrestamos = realloc(
            sistema->prestamos,
            nuevaCapacidad * sizeof(Prestamo *)
        );

        if (nuevosPrestamos == NULL)
        {
            liberarPrestamoLocal(nuevoPrestamo);
            printf("Error de memoria al ampliar los prestamos.\n");
            return 0;
        }

        sistema->prestamos = nuevosPrestamos;
        sistema->capacidadPrestamos = nuevaCapacidad;
    }

    sistema->prestamos[sistema->cantidadPrestamos++] = nuevoPrestamo;

    if (guardarPrestamosJSON(sistema) == 0)
    {
        sistema->cantidadPrestamos--;
        liberarPrestamoLocal(nuevoPrestamo);
        printf("No se pudo guardar el prestamo en %s.\n", ARCHIVO_PRESTAMOS);
        return 0;
    }

    return nuevoPrestamo->idPrestamo;
}

// E: Sistema de préstamos, fecha de inicio, fecha de fin
// S: Lista de préstamos en el rango de fechas
// R: Devuelve los préstamos que se realizaron entre las fechas especificadas
// F: Consulta el historial de préstamos realizados entre dos fechas dadas.
void consultarHistorialPrestamos(SistemaPrestamos *sistema, Fecha desde, Fecha hasta)
{
    if (sistema == NULL)
    {
        return;
    }

    for (int i = 0; i < sistema->cantidadPrestamos; i++)
    {
        Prestamo *prestamo = sistema->prestamos[i];

        if (prestamo != NULL &&
            compararFechas(prestamo->fechaInicio, desde) >= 0 &&
            compararFechas(prestamo->fechaInicio, hasta) <= 0)
        {
            printf("Prestamo %d - Usuario %d\n",
                   prestamo->idPrestamo,
                   prestamo->idUsuario);
        }
    }
}

void datosPrestamo(void) 
{
    char nombreUsuario[50];

    printf("Ingrese los siguientes datos: ");

}

void menuPrestamos(void) 
{
    printf("-----Menu de Prestamos------\n");
    printf("1. Realizar prestamo\n");
    printf("2. Consultar historial de prestamos\n");
    printf("3. Consultar vencimientos\n");
    printf("4. Consultar prestamos por usuario\n");
    printf("5. volver\n");

    int opcion;

    printf("Ingrese una opción: ");
    scanf("%d", &opcion);

    switch (opcion)
    {
    case 1:
        // Lógica para realizar préstamo
        break;
    case 2:
        // Lógica para consultar historial de préstamos
        break;
    case 3:
        // Lógica para consultar vencimientos
        break;
    case 4:
        // Lógica para consultar préstamos por usuario
        break;
    case 5:
        mainPrincipal(); // Llamar a la función main para volver al menú principal
        break;
    default:
        printf("Opción no válida.\n");
        break;
    }

}