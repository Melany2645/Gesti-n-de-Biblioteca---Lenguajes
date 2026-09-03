#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu.h"

#ifndef MENUFLUJO_H
#define MENUFLUJO_H

int main() {
    printf("Bienvenido al sistema de gestión de biblioteca\n");
    printf("1. Opciones Operativas.\n");
    printf("2. Opciones Generales.\n");
    printf("3. Salir\n");
    printf("Seleccione una opción:\n");
    scanf("%d", &opcion);
}

#endif // MENUFLUJO_H