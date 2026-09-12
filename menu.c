#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Biblioteca.h"
#include "Usuario.h"
#include "prestamosDevoluciones.h"
#include "BusquedaSimpleAvanzada.h"
#include "Estadisticas.h"

#include "menu.h"

void operativas(int opcion) {

    printf("-----Opciones Operativas-----\n");
    printf("1. Gestion del catalogo.\n");
    printf("2. Gestion de usuario.\n");
    printf("3. Historial de prestamos\n");
    printf("4. Estadisticas\n");
    printf("5. Volver\n");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Gestion del catalogo seleccionada.\n");
            menuCatalogo(); 
            break;
        case 2:
            printf("Gestian de usuario seleccionada.\n");
            menuUsuario(); 
            break;
        case 3:
            printf("Historial de prestamos seleccionado.\n");
            mostrarTodosLosPrestamos();  
            break;
        case 4:
            printf("Estadisticas seleccionadas.\n");
            menuEstadisticas();
            break;
        case 5:
            printf("Volviendo al menu principal.\n");
            mainPrincipal(); 
            break;
        default:
            printf("Opcion no valida. Intente nuevamente.\n");
            break;
    }
}

void generales(int opcion) {
    int continuar = 1;

    do {
        printf("\n-----Opciones Generales-----\n");
        printf("1. Busqueda simple\n");
        printf("2. Busqueda avanzada\n");
        printf("3. Prestamo de ejemplares\n");
        printf("4. Devolucion de ejemplares\n");
        printf("5. Volver\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Busqueda simple seleccionada.\n");
                busquedaSimple();
                break;

            case 2:
                printf("Busqueda avanzada seleccionada.\n");
                busquedaAvanzada();
                break;

            case 3:
                printf("Prestamo de ejemplares seleccionado.\n");
                menuPrestamos();
                break;

            case 4:
                printf("Devolucion de ejemplares seleccionada.\n");
                menuDevoluciones();
                break;

            case 5:
                printf("Volviendo al menu principal.\n");
                continuar = 0;
                break;

            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }

    } while (continuar);
}
void mainPrincipal(void) {
    int opcion;
    int continuar = 1;

    do {
        printf("\nBienvenido al sistema de gestion de biblioteca\n");
        printf("1. Opciones Operativas.\n");
        printf("2. Opciones Generales.\n");
        printf("3. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                operativas(opcion);
                break;

            case 2:
                generales(opcion);
                break;

            case 3:
                printf("Saliendo del programa.\n");
                continuar = 0;
                break;

            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }

    } while (continuar);
}