#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Biblioteca.h"

//E:Numero ingresado por el usuario
//S:Menu de opcion que ingreso
//R: Debe ingresar un numero entero
//F: Mostrar un menu en el que el usuario ingresa un opcion para ingresar a algun menu
void menuCatalogo(void){
    do{
    int numero;
    printf("-----Gestion de Catalogo-----\n");
    printf("1. Agregar producciones\n");
    printf("2. Ver el catalogo completo\n");
    printf("3. Volver\n");
    printf("Que desea realizar:");

    if(scanf("%d", &numero) == 1){// revisar si el numero es un entero a que el scanf si es un numero retorna 1
        switch (numero)
        {
            case 1:
                printf("Agregar producciones\n");
                void rutaarchivo();
                break;

            case 2:
                printf("Ver el catalogo completo\n");
                break;

            case 3:
                printf("Volver\n");
                break;

            default:
                printf("Opcion no valida\n");
                 break;
        }
            }
    else{
        printf("El caracter ingresado no es un entero");
        while (getchar() != '\n')
        {
            // eliminar caracteres de texto
        }
        }
    }while (numero!=3); //hasta q no se ponga 3 no se sale de esta parte
    
}
//E: La ruta del archivo que el usuario ingreso
//S: 
//R: Debe ingresar una cadena de texto con la ruta.
//F: 
void rutaarchivo(void){
    char *ruta;
    int op;


    ruta = malloc(300 * sizeof(ruta))

    if (ruta == null){
        printf("No se pudo reservar la memoria")
    }
    
    printf("Ingrese la ruta del archivo con el lote de producciones:");
    scanf("%199[^\n]", ruta);

    printf("La ruta ingresada es: %199[^\n]", ruta);
    printf("Desea continuar:");
    printf("1. SI");
    printf("2. NO");
    printf("Indique su respuesta:")
    scanf("%d, op");

    if (op == 1){
        FILE *archivo;
        archivo = fopen(ruta, "r")


    }
    free(ruta);

}