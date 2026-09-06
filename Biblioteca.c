#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "Biblioteca.h"

#define ARCHIVO_CATALOGO "catalogo.json"


//E: Numero ingresado por el usuario.
//S: Menu de opcion que ingreso.
//R: Debe ingresar un numero entero.
//F: Mostrar el menu de gestion de catalogo.
void menuCatalogo(void)
{
    int numero;

    do
    {
        printf("\n-----Gestion de Catalogo-----\n");
        printf("1. Agregar producciones\n");
        printf("2. Ver el catalogo completo\n");
        printf("3. Volver\n");
        printf("Que desea realizar: ");

        // Revisar si ingreso un entero
        if (scanf("%d", &numero) == 1)
        {
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
        else
        {
            printf("El caracter ingresado no es un entero\n");

            // Limpiar lo que quedo escrito
            while (getchar() != '\n')
            {
            }

            numero = 0;
        }

    } while (numero != 3);
}


//E: Ruta del archivo que ingresa el usuario.
//S: Abre el archivo y envia su contenido para ser leido.
//R: Debe ingresar una ruta de un archivo existente.
//F: Solicitar la ruta e intentar abrir el archivo.
void rutaarchivo(void)
{
    char *ruta;
    int op;

    // Reservar espacio para la ruta
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

        // Abrir archivo en modo lectura
        archivo = fopen(ruta, "r");

        if (archivo == NULL)
        {
            printf("No se pudo abrir el archivo\n");
        }
        else
        {
            printf("El archivo se abrio correctamente\n");

            // Leer las producciones
            leerArchivo(archivo);

            fclose(archivo);
        }
    }

    // Liberar la memoria de la ruta
    free(ruta);
}


//E: Cadena de texto que se desea copiar.
//S: Nueva cadena con el mismo contenido.
//R: El texto no debe ser NULL.
//F: Crear una copia del texto.
char *copiarTexto(char *texto)
{
    char *copia;

    // Reservar memoria para la copia
    copia = malloc((strlen(texto) + 1) * sizeof(char));

    if (copia == NULL)
    {
        return NULL;
    }

    // Copiar el texto
    strcpy(copia, texto);

    return copia;
}


//E: Linea de texto con los datos de una produccion.
//S: Retorna 1 si la produccion es valida o 0 si no es valida.
//R: La linea debe contener los datos separados por #.
//F: Validar que la produccion tenga la cantidad correcta de campos.
int validarProduccion(char *linea)
{
    int cantidadSeparadores = 0;
    int i = 0;

    // Recorrer toda la linea
    while (linea[i] != '\0')
    {
        // Contar los #
        if (linea[i] == '#')
        {
            cantidadSeparadores++;
        }

        i++;
    }

    // Se necesitan 5 # para formar 6 campos
    if (cantidadSeparadores == 5)
    {
        return 1;
    }

    return 0;
}


//E: Una linea y una estructura Produccion.
//S: Datos guardados en la estructura.
//R: La linea debe tener los 6 campos separados por #.
//F: Guardar los datos de una produccion.
void guardarProduccion(char *linea, Produccion *produccion)
{
    char *parte;

    // Guardar nombre
    parte = strtok(linea, "#");
    (*produccion).nombre = copiarTexto(parte);

    // Guardar autor
    parte = strtok(NULL, "#");
    (*produccion).autor = copiarTexto(parte);

    // Guardar año
    parte = strtok(NULL, "#");
    (*produccion).anPubli = atoi(parte);

    // Guardar genero
    parte = strtok(NULL, "#");
    (*produccion).genero = copiarTexto(parte);

    // Guardar resumen
    parte = strtok(NULL, "#");
    (*produccion).resumen = copiarTexto(parte);

    // Guardar cantidad de ejemplares
    parte = strtok(NULL, "#");
    (*produccion).cantidadEjemplares = atoi(parte);
}


//E: Una Produccion.
//S: Libera la memoria de los textos.
//R: La produccion debe estar creada.
//F: Liberar la memoria utilizada por una produccion.
void liberarProduccion(Produccion *produccion)
{
    // Liberar los textos
    free((*produccion).nombre);
    free((*produccion).autor);
    free((*produccion).genero);
    free((*produccion).resumen);
}


//E: Catalogo, cantidad y nueva produccion.
//S: Retorna 1 si se agrega o 0 si ocurre un error.
//R: La produccion debe ser valida.
//F: Agregar una produccion al catalogo.
int agregarProduccion(
    Produccion **catalogo,
    int *cantidad,
    Produccion *nueva
)
{
    Produccion *temporal;

    // Aumentar espacio para una produccion mas
    temporal = realloc(
        *catalogo,
        ((*cantidad) + 1) * sizeof(Produccion)
    );

    if (temporal == NULL)
    {
        return 0;
    }

    // Actualizar el catalogo
    *catalogo = temporal;

    // Guardar la nueva produccion
    (*catalogo)[*cantidad] = *nueva;

    // Aumentar la cantidad
    (*cantidad)++;

    return 1;
}


//E: Catalogo y cantidad de producciones.
//S: Retorna 1 si se guarda correctamente o 0 si ocurre un error.
//R: El catalogo debe contener producciones validas.
//F: Guardar el catalogo en un archivo JSON.
int guardarCatalogoJSON(
    Produccion *catalogo,
    int cantidad
)
{
    FILE *archivo;
    cJSON *arreglo;
    cJSON *objeto;
    char *textoJSON;

    int i;

    // Crear el arreglo JSON
    arreglo = cJSON_CreateArray();

    if (arreglo == NULL)
    {
        return 0;
    }

    // Recorrer todas las producciones
    for (i = 0; i < cantidad; i++)
    {
        // Crear una produccion en JSON
        objeto = cJSON_CreateObject();

        if (objeto == NULL)
        {
            cJSON_Delete(arreglo);
            return 0;
        }

        // Agregar nombre
        cJSON_AddStringToObject(
            objeto,
            "nombre",
            catalogo[i].nombre
        );

        // Agregar autor
        cJSON_AddStringToObject(
            objeto,
            "autor",
            catalogo[i].autor
        );

        // Agregar año
        cJSON_AddNumberToObject(
            objeto,
            "anioPublicacion",
            catalogo[i].anPubli
        );

        // Agregar genero
        cJSON_AddStringToObject(
            objeto,
            "genero",
            catalogo[i].genero
        );

        // Agregar resumen
        cJSON_AddStringToObject(
            objeto,
            "resumen",
            catalogo[i].resumen
        );

        // Agregar cantidad
        cJSON_AddNumberToObject(
            objeto,
            "cantidad",
            catalogo[i].cantidadEjemplares
        );

        // Agregar la produccion al arreglo
        cJSON_AddItemToArray(arreglo, objeto);
    }

    // Convertir el JSON a texto
    textoJSON = cJSON_Print(arreglo);

    if (textoJSON == NULL)
    {
        cJSON_Delete(arreglo);
        return 0;
    }

    // Abrir catalogo.json
    archivo = fopen(ARCHIVO_CATALOGO, "w");

    if (archivo == NULL)
    {
        cJSON_free(textoJSON);
        cJSON_Delete(arreglo);
        return 0;
    }

    // Guardar el JSON
    fprintf(archivo, "%s", textoJSON);

    fclose(archivo);

    // Liberar memoria de cJSON
    cJSON_free(textoJSON);
    cJSON_Delete(arreglo);

    return 1;
}


//E: Archivo con el lote de producciones.
//S: Producciones validas almacenadas y guardadas en JSON.
//R: El archivo debe estar abierto correctamente.
//F: Leer, validar y guardar las producciones.
void leerArchivo(FILE *archivo)
{
    char *linea;

    // Catalogo dinamico
    Produccion *catalogo = NULL;

    // Produccion que se esta leyendo
    Produccion nueva;

    int cantidad = 0;
    int i;

    // Reservar espacio para leer una linea
    linea = malloc(500 * sizeof(char));

    if (linea == NULL)
    {
        printf("No se pudo reservar memoria\n");
        return;
    }

    // Leer linea por linea
    while (fgets(linea, 500, archivo) != NULL)
    {
        // Revisar si tiene el formato correcto
        if (validarProduccion(linea) == 1)
        {
            // Guardar los datos en la estructura
            guardarProduccion(linea, &nueva);

            // Agregar al catalogo usando realloc
            if (
                agregarProduccion(
                    &catalogo,
                    &cantidad,
                    &nueva
                ) == 1
            )
            {
                printf("Produccion agregada correctamente\n");
            }
            else
            {
                printf("No se pudo agregar la produccion\n");

                // Liberar si no se pudo agregar
                liberarProduccion(&nueva);
            }
        }
        else
        {
            printf("Produccion no valida\n");
        }
    }

    // Mostrar total agregado
    printf(
        "\nCantidad de producciones agregadas: %d\n",
        cantidad
    );

    // Guardar todo en JSON
    if (guardarCatalogoJSON(catalogo, cantidad) == 1)
    {
        printf("Catalogo guardado correctamente en JSON\n");
    }
    else
    {
        printf("No se pudo guardar el catalogo en JSON\n");
    }

    // Liberar cada produccion
    for (i = 0; i < cantidad; i++)
    {
        liberarProduccion(&catalogo[i]);
    }

    // Liberar el catalogo
    free(catalogo);

    // Liberar la linea
    free(linea);
}