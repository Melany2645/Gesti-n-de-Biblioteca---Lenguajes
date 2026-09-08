#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu.h"

char operativas() {

    int opcion;

    printf("-----Opciones Operativas-----\n");
    printf("1. Gestión del catálogo.\n");
    printf("2. Gestión de usuario.\n");
    printf("3. Historial de préstamos\n");
    printf("4. Estadísticas\n");
    printf("5. Volver\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Gestión del catálogo seleccionada.\n");
            // Llamar a la función correspondiente para la gestión del catálogo
            break;
        case 2:
            printf("Gestión de usuario seleccionada.\n");
            // Llamar a la función correspondiente para la gestión de usuario
            break;
        case 3:
            printf("Historial de préstamos seleccionado.\n");
            // Llamar a la función correspondiente para el historial de préstamos
            break;
        case 4:
            printf("Estadísticas seleccionadas.\n");
            // Llamar a la función correspondiente para las estadísticas
            break;
        case 5:
            printf("Volviendo al menú principal.\n");
            // Volver al menú principal
            break;
        default:
            printf("Opción no válida. Intente nuevamente.\n");
            break;
    }
}

char generales() {
    printf("-----Opciones Generales-----\n");
    printf("1. Búsqueda simple\n");
    printf("2. Búsqueda avanzada\n");
    printf("3. Préstamo de ejemplares\n");
    printf("4. Devolución de ejemplares\n");
    printf("5. Volver\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Búsqueda simple seleccionada.\n");
            // Llamar a la función correspondiente para la búsqueda simple
            break;
        case 2:
            printf("Búsqueda avanzada seleccionada.\n");
            // Llamar a la función correspondiente para la búsqueda avanzada
            break;
        case 3:
            printf("Préstamo de ejemplares seleccionado.\n");
            // Llamar a la función correspondiente para el préstamo de ejemplares
            break;
        case 4:
            printf("Devolución de ejemplares seleccionada.\n");
            // Llamar a la función correspondiente para la devolución de ejemplares
            break;
        case 5:
            printf("Volviendo al menú principal.\n");
            // Volver al menú principal
            break;
        default:
            printf("Opción no válida. Intente nuevamente.\n");
            break;
    }
}

int main() {
    printf("Bienvenido al sistema de gestión de biblioteca\n");
    printf("1. Opciones Operativas.\n");
    printf("2. Opciones Generales.\n");
    printf("3. Salir\n");
    printf("Seleccione una opción:\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            operativas();
            // Llamar a la función correspondiente para las opciones operativas
            break;
        case 2:
            generales();
            // Llamar a la función correspondiente para las opciones generales
            break;
        case 3:
            printf("Saliendo del programa.\n");
            exit(0);
        default:
            printf("Opción no válida. Intente nuevamente.\n");
            break;
    }
}