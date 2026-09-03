#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Usuario.h"

void menuUsuario(void) {
    int opcion;
    int continuar = 1;
    
    do {
        printf("\n-----Gestion de Usuario-----\n");
        printf("1. Crear usuario\n");
        printf("2. Visualizar usuario en especifico\n");
        printf("3. Visualizar todos los usuarios\n");
        printf("4. Modificar usuario\n");
        printf("5. Eliminar Usuario\n");
        printf("6. Volver al menu principal\n");
        printf("Que desea realizar: ");

        if (scanf("%d", &opcion) == 1) {
            switch (opcion) {
                case 1:
                    printf("Crear usuario\n");
                    break;

                case 2:
                    printf("Visualizar usuario en especifico\n");
                    break;

                case 3:
                    printf("Visualizar todos los usuarios\n");
                    break;

                case 4:
                    printf("Modificar usuario\n");
                    break;
                
                case 5:
                    printf("Eliminar Usuario\n");
                    break;
                
                case 6:
                    printf("Volver al menu principal\n");
                    continuar = 0;
                    break;

                default:
                    printf("Opcion no valida\n");
                    break;
            }
        } else {
            printf("El caracter ingresado no es un entero\n");
            while (getchar() != '\n') {
                // eliminar caracteres de texto
            }
        }
    } while (continuar == 1);
}

int main(void) {    
    menuUsuario();
    
    return 0;
}