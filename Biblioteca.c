#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Biblioteca.h"

//E:Numero ingresado por el usuario
//S:Menu de opcion que ingreso
//R: Debe ingresar un numero entero
//F: Mostrar un menu en el que el usuario ingresa un opcion para ingresar a algun menu
void menuCatalogo(void){

    int numero;

    do{

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
                rutaarchivo();
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
        while (getchar() != '\n')
        printf("El caracter ingresado no es un entero\n");
        {
            // eliminar caracteres de texto
        }

        numero = 0;
        }

    }while (numero!=3); //hasta q no se ponga 3 no se sale de esta parte

}
//E: La ruta del archivo que el usuario ingreso
//S: 
//R: Debe ingresar una cadena de texto con la ruta.
//F: Solicitar la ruta e intentar abrir el archivo
void rutaarchivo(void)
{
    char *ruta;
    int op;

    //reservar dinámicamente memoria suficiente para guardar 300 caracteres y hacer que ruta apunte a esa memoria.
    ruta = malloc(300 * sizeof(char));

    if (ruta == NULL)
    {
        printf("No se pudo reservar la memoria\n");
        return;
    }

    printf("Ingrese la ruta del archivo con el lote de producciones: ");
    scanf(" %299[^\n]", ruta);

    printf("La ruta ingresada es: %s\n", ruta);

    printf("Desea continuar:\n");
    printf("1. SI\n");
    printf("2. NO\n");
    printf("Indique su respuesta: ");
    scanf("%d", &op);

    if (op == 1)
    {
        FILE *archivo;

        archivo = fopen(ruta, "r");//abrir archivo en modo lectura

        if (archivo == NULL)
        {
            printf("No se pudo abrir el archivo\n");
        }
        else
        {
            printf("El archivo se abrio correctamente\n");
            leerArchivo(archivo);
            fclose(archivo);
        }
    }

    free(ruta);
}
// E: Archivo previamente abierto que contiene el lote de producciones.
// S: Muestra en pantalla la informacion leida del archivo.
// R: El archivo debe estar abierto correctamente en modo lectura.
// F: Leer la informacion contenida en el archivo de producciones.
void leerArchivo(FILE *archivo)
{
    char *linea;

    linea = malloc(500 * sizeof(char));

    if (linea == NULL)
    {
        printf("No se pudo reservar memoria\n");
        return;
    }

    while (fgets(linea, 500, archivo) != NULL)
    {
    separarProduccion(linea);
    }
    free(linea);
}


//E: Una linea de texto con los datos de una produccion separados por #
//S: Muestra cada dato de la produccion por separado
//R: La linea debe contener los datos separados por el caracter #
//F: Separar los campos de una produccion para poder procesarlos
void separarProduccion(char *linea){
    char *parte;

    parte = strtok(linea, "#");

    while (parte != NULL)
    {
        printf("%s\n", parte);

        parte = strtok(NULL, "#");//separa el texto en el caracter indicado
    }
}
//E: Una linea con los datos de una produccion y una estructura Produccion
//S: Guarda los datos separados dentro de la estructura
//R: La linea debe contener los 6 campos separados por #
//F: Guardar los datos de una linea dentro de una Produccion
void guardarProduccion(char *linea, Produccion *produccion)
{
    char *parte;

    parte = strtok(linea, "#");
    produccion->nombre = parte;

    parte = strtok(NULL, "#");
    produccion->autor = parte;

    parte = strtok(NULL, "#");
    produccion->anPubli = atoi(parte);

    parte = strtok(NULL, "#");
    produccion->genero = parte;

    parte = strtok(NULL, "#");
    produccion->resumen = parte;

    parte = strtok(NULL, "#");
    produccion->cantidadEjemplares = atoi(parte);
}

//E: Linea de texto con los datos de una produccion
//S: Retorna 1 si la produccion es valida o 0 si no es valida
//R: La linea debe contener los datos separados por #
//F: Validar que la produccion tenga la cantidad correcta de campos
int validarProduccion(char *linea)
{
    int cantidadSeparadores = 0; // Guarda cuantos # aparecen en la linea
    int i = 0; // Posicion que se esta revisando en la linea

    // Recorre la linea hasta llegar al final
    while (linea[i] != '\0')
    {
        // Revisa si el caracter actual es un #
        if (linea[i] == '#')
        {
            cantidadSeparadores++;
        }

        i++; // Pasa al siguiente caracter
    }

    // Una produccion tiene 6 campos, por lo tanto debe tener 5 #
    if (cantidadSeparadores == 5)
    {
        return 1; // La cantidad de campos es correcta
    }

    return 0; // La cantidad de campos no es correcta
}