#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Biblioteca.h"
#include "Usuario.h"
#include "prestamosDevoluciones.h"

#include "menu.h"

void operativas(int opcion) {

    printf("-----Opciones Operativas-----\n");
    printf("1. Gestion del catalogo.\n");
    printf("2. Gestion de usuario.\n");
    printf("3. Historial de préstamos\n");
    printf("4. Estadisticas\n");
    printf("5. Volver\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Gestión del catalogo seleccionada.\n");
            menuCatalogo(); // Llamar a la función correspondiente para la gestión del catálogo
            break;
        case 2:
            printf("Gestian de usuario seleccionada.\n");
            menuUsuario(); // Llamar a la función correspondiente para la gestión de usuario
            break;
        case 3:
            printf("Historial de prestamos seleccionado.\n");
            menuPrestamos();
            break;
        case 4:
            printf("Estadisticas seleccionadas.\n");
            //menuEstadisticas(); // Llamar a la función correspondiente para las estadísticas
            break;
        case 5:
            printf("Volviendo al menu principal.\n");
            mainPrincipal(); // Llamar a la función main para volver al menú principal
            break;
        default:
            printf("Opcion no valida. Intente nuevamente.\n");
            break;
    }
}

void generales(int opcion) {

    printf("-----Opciones Generales-----\n");
    printf("1. Busqueda simple\n");
    printf("2. Búsqueda avanzada\n");
    printf("3. Préstamo de ejemplares\n");
    printf("4. Devolucion de ejemplares\n");
    printf("5. Volver\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Busqueda simple seleccionada.\n");
            //menuBusquedaSimple(); // Llamar a la función correspondiente para la búsqueda simple
            break;
        case 2:
            printf("Búsqueda avanzada seleccionada.\n");
            //menuBusquedaAvanzada(); // Llamar a la función correspondiente para la búsqueda avanzada
            break;
        case 3:
            printf("Prestamo de ejemplares seleccionado.\n");
            menuPrestamos();
            break;
        case 4:
            printf("Devolucion de ejemplares seleccionada.\n");
            //menuDevolucion(); // Llamar a la función correspondiente para la devolución de ejemplares
            break;
        case 5:
            printf("Volviendo al menú principal.\n");
            mainPrincipal(); // Llamar a la función main para volver al menú principal
            break;
        default:
            printf("Opción no valida. Intente nuevamente.\n");
            break;
    }
}
void mainPrincipal(void) {
    int opcion;

    printf("Bienvenido al sistema de gestión de biblioteca\n");
    printf("1. Opciones Operativas.\n");
    printf("2. Opciones Generales.\n");
    printf("3. Salir\n");
    printf("Seleccione una opcion:\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            operativas(opcion);
            // Llamar a la función correspondiente para las opciones operativas
            break;
        case 2:
            generales(opcion);
            // Llamar a la función correspondiente para las opciones generales
            break;
        case 3:
            printf("Saliendo del programa.\n");
            exit(0);
        default:
            printf("Opcion no valida. Intente nuevamente.\n");
            break;
    }
}