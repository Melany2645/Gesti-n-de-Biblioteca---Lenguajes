#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#include "Biblioteca.h"

#define ARCHIVO_CATALOGO "catalogo.json"
#define ARCHIVO_EJEMPLARES "ejemplares.json"


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
                    mostrarCatalogo();
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

//OPCION 1


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
//R: La linea debe contener 6 campos separados por #.
//F: Revisar que la informacion de la produccion tenga el formato correcto.
int validarProduccion(char *linea)
{
    int cantidadSeparadores = 0;
    int i = 0;

    char *copia;
    char *parte;

    int campo = 1;

    // Revisar que la linea tenga contenido
    if (linea == NULL || linea[0] == '\0')
    {
        return 0;
    }

    // No permitir un campo vacio al inicio
    if (linea[0] == '#')
    {
        return 0;
    }

    // Recorrer la linea
    while (linea[i] != '\0')
    {
        // Contar los #
        if (linea[i] == '#')
        {
            cantidadSeparadores++;

            // No permitir campos vacios
            if (linea[i + 1] == '#')
            {
                return 0;
            }
        }

        i++;
    }

    // Debe tener exactamente 5 #
    if (cantidadSeparadores != 5)
    {
        return 0;
    }

    // Crear una copia para no modificar la linea original
    copia = copiarTexto(linea);

    if (copia == NULL)
    {
        return 0;
    }

    // Separar los campos
    parte = strtok(copia, "#");

    while (parte != NULL)
    {
        // Campo 3 = año
        if (campo == 3)
        {
            if (esNumero(parte) == 0)
            {
                free(copia);
                return 0;
            }
        }

        // Campo 6 = cantidad
        if (campo == 6)
        {
            if (esNumero(parte) == 0)
            {
                free(copia);
                return 0;
            }
        }

        campo++;

        parte = strtok(NULL, "#");
    }

    // Liberar la copia
    free(copia);

    // Revisar que realmente encontro 6 campos
    if (campo != 7)
    {
        return 0;
    }

    return 1;
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

//E: Catalogo y cantidad de producciones.
//S: Carga las producciones guardadas anteriormente.
//R: El archivo JSON debe tener el formato del catalogo.
//F: Leer el catalogo guardado en JSON.
int cargarCatalogoJSON(
    Produccion **catalogo,
    int *cantidad
)
{
    FILE *archivo;
    char *texto;
    long tamano;

    cJSON *arreglo;
    cJSON *objeto;

    int i;
    int total;

    // Intentar abrir el catalogo
    archivo = fopen(ARCHIVO_CATALOGO, "r");

    // Si no existe, se empieza con catalogo vacio
    if (archivo == NULL)
    {
        return 1;
    }

    // Buscar el tamaño del archivo
    fseek(archivo, 0, SEEK_END);
    tamano = ftell(archivo);
    rewind(archivo);

    // Reservar espacio para leer el JSON
    texto = malloc((tamano + 1) * sizeof(char));

    if (texto == NULL)
    {
        fclose(archivo);
        return 0;
    }

    // Leer todo el archivo
    fread(texto, sizeof(char), tamano, archivo);

    // Marcar el final del texto
    texto[tamano] = '\0';

    fclose(archivo);

    // Convertir el texto a JSON
    arreglo = cJSON_Parse(texto);

    free(texto);

    if (arreglo == NULL)
    {
        return 0;
    }

    // Cantidad de producciones guardadas
    total = cJSON_GetArraySize(arreglo);

    // Recorrer el JSON
    for (i = 0; i < total; i++)
    {
        Produccion nueva;

        cJSON *nombre;
        cJSON *autor;
        cJSON *anio;
        cJSON *genero;
        cJSON *resumen;
        cJSON *cantidadEjemplares;

        // Obtener una produccion
        objeto = cJSON_GetArrayItem(arreglo, i);

        // Buscar cada dato
        nombre = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "nombre"
        );

        autor = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "autor"
        );

        anio = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "anioPublicacion"
        );

        genero = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "genero"
        );

        resumen = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "resumen"
        );

        cantidadEjemplares =
            cJSON_GetObjectItemCaseSensitive(
                objeto,
                "cantidad"
            );

        // Revisar que los datos existan
        if (
            !cJSON_IsString(nombre) ||
            !cJSON_IsString(autor) ||
            !cJSON_IsNumber(anio) ||
            !cJSON_IsString(genero) ||
            !cJSON_IsString(resumen) ||
            !cJSON_IsNumber(cantidadEjemplares)
        )
        {
            continue;
        }

        // Guardar los datos
        nueva.nombre = copiarTexto(nombre->valuestring);
        nueva.autor = copiarTexto(autor->valuestring);
        nueva.anPubli = anio->valueint;
        nueva.genero = copiarTexto(genero->valuestring);
        nueva.resumen = copiarTexto(resumen->valuestring);
        nueva.cantidadEjemplares =
            cantidadEjemplares->valueint;

        // Agregar al catalogo en memoria
        if (agregarProduccion(
            catalogo,
            cantidad,
            &nueva
        ) == 0)
        {
            liberarProduccion(&nueva);
            cJSON_Delete(arreglo);

            return 0;
        }
    }

    // Liberar el JSON
    cJSON_Delete(arreglo);

    return 1;
}

//E: Archivo con el lote de producciones.
//S: Producciones validas almacenadas y guardadas en JSON.
//R: El archivo debe estar abierto correctamente.
//F: Leer, validar y guardar las producciones.

void leerArchivo(FILE *archivo){
    char *linea;

    Produccion *catalogo = NULL;
    Produccion nueva;

    Ejemplar *ejemplares = NULL;

    int cantidad = 0;
    int cantidadEjemplares = 0;

    int invalidas = 0;
    int repetidas = 0;

    int i;

    // Cargar lo que ya estaba guardado
    if (cargarCatalogoJSON(&catalogo,&cantidad) == 0)
    {
        printf("No se pudo cargar el catalogo anterior\n");

        return;
    }
    // Cargar ejemplares anteriores
    if (cargarEjemplaresJSON( &ejemplares,&cantidadEjemplares) == 0)
    {
        printf("No se pudieron cargar los ejemplares\n");

        return;
    }
    printf(
        "Producciones existentes: %d\n",
        cantidad
    );

    // Reservar espacio para leer una linea
    linea = malloc(500 * sizeof(char));

    if (linea == NULL)
    {
        printf("No se pudo reservar memoria\n");

        // Liberar catalogo cargado
        for (i = 0; i < cantidad; i++)
        {
            liberarProduccion(&catalogo[i]);
        }

        free(catalogo);

        return;
    }

    // Leer el nuevo lote
    while (fgets(linea, 500, archivo) != NULL)
    {
        // Revisar formato
        if (validarProduccion(linea) == 1)
        {
            // Guardar los datos
            guardarProduccion(linea,&nueva);

            // Revisar si ya existe
            if (existeProduccion(catalogo,cantidad,nueva.nombre
            ) == 1)
            {
                printf("No agregada: %s ya existe\n",nueva.nombre);

                    repetidas++;

                    liberarProduccion(&nueva);
                    }
            else
            {
                if (agregarProduccion(&catalogo,&cantidad,&nueva) == 1)
                {
                    printf("Produccion agregada correctamente\n");

                    // Crear ejemplares
                    generarEjemplares(&nueva,&ejemplares,&cantidadEjemplares
                    );
                }
            }
        }
        else
        {
            printf("Produccion no valida: %s", linea);

            invalidas++;
        }
    }

     //reporte
    printf("\n--- /Reporte del lote/ ---\n");
    printf(
        "Registros repetidos: %d\n",
        repetidas
    );
    printf(
        "Registros invalidos: %d\n",
        invalidas
    );

    printf(
        "Total de producciones: %d\n",
        cantidad
    );
    // Guardar catalogo actualizado
    if (guardarCatalogoJSON(catalogo,cantidad) == 1)
    {
        printf("Catalogo actualizado correctamente\n");
    }
    else
    {
        printf("No se pudo guardar el catalogo\n");
    }
        // Guardar ejemplares
    if (guardarEjemplaresJSON(
        ejemplares,
        cantidadEjemplares
    ) == 1)
    {
        printf("Ejemplares guardados correctamente\n");
    }
    else
    {
        printf("No se pudieron guardar los ejemplares\n");
    }
     // Liberar cada produccion
    for (i = 0; i < cantidad; i++)
    {
        liberarProduccion(&catalogo[i]);
    }

    // Liberar catalogo
    free(catalogo);

    // Liberar ejemplares
    for (i = 0; i < cantidadEjemplares; i++)
    {
        liberarEjemplar(&ejemplares[i]);
    }

    free(ejemplares);

    // Liberar linea
    free(linea);
}

//E: Catalogo, cantidad de producciones y nombre a buscar.
//S: Retorna 1 si existe o 0 si no existe.
//R: El catalogo debe contener producciones validas.
//F: Revisar si una produccion ya existe por su nombre.
int existeProduccion(
    Produccion *catalogo,
    int cantidad,
    char *nombre
)
{
    int i;

    // Recorrer el catalogo
    for (i = 0; i < cantidad; i++)
    {
        // Comparar los nombres
        if (strcmp(catalogo[i].nombre, nombre) == 0)
        {
            return 1;
        }
    }

    return 0;
}

//E: Texto que se desea revisar.
//S: Retorna 1 si contiene solo numeros o 0 si no.
//R: El texto no debe estar vacio.
//F: Revisar si un texto representa un numero entero.
int esNumero(char *texto)
{
    int i = 0;

    // Revisar que tenga contenido
    if (texto == NULL || texto[0] == '\0')
    {
        return 0;
    }

    // Revisar cada caracter
    while (texto[i] != '\0')
    {
        // Permitir el salto de linea al final
        if (texto[i] == '\n' || texto[i] == '\r')
        {
            i++;
            continue;
        }

        // Revisar que sea un numero
        if (texto[i] < '0' || texto[i] > '9')
        {
            return 0;
        }

        i++;
    }

    return 1;
}
//E: Lista de ejemplares, cantidad y nuevo ejemplar.
//S: Retorna 1 si se agrega o 0 si ocurre un error.
//R: El ejemplar debe tener sus datos.
//F: Agregar un ejemplar a la lista.
int agregarEjemplar(
    Ejemplar **ejemplares,
    int *cantidad,
    Ejemplar *nuevo
)
{
    Ejemplar *temporal;

    // Aumentar espacio para otro ejemplar
    temporal = realloc(
        *ejemplares,
        ((*cantidad) + 1) * sizeof(Ejemplar)
    );

    if (temporal == NULL)
    {
        return 0;
    }

    // Actualizar la lista
    *ejemplares = temporal;

    // Guardar el ejemplar
    (*ejemplares)[*cantidad] = *nuevo;

    // Aumentar cantidad
    (*cantidad)++;

    return 1;
}
//E: Un Ejemplar.
//S: Libera la memoria utilizada.
//R: El ejemplar debe estar creado.
//F: Liberar la memoria de un ejemplar.
void liberarEjemplar(Ejemplar *ejemplar)
{
    // Liberar el nombre
    free((*ejemplar).nombreProduccion);
}

//E: Lista de ejemplares y cantidad.
//S: Retorna 1 si se guarda correctamente o 0 si ocurre un error.
//R: Los ejemplares deben estar creados correctamente.
//F: Guardar los ejemplares en un archivo JSON.
int guardarEjemplaresJSON(
    Ejemplar *ejemplares,
    int cantidad
)
{
    FILE *archivo;
    cJSON *arreglo;
    cJSON *objeto;
    char *textoJSON;
    int i;

    // Crear arreglo JSON
    arreglo = cJSON_CreateArray();

    if (arreglo == NULL)
    {
        return 0;
    }

    // Recorrer ejemplares
    for (i = 0; i < cantidad; i++)
    {
        objeto = cJSON_CreateObject();

        if (objeto == NULL)
        {
            cJSON_Delete(arreglo);
            return 0;
        }

        // Guardar identificador
        cJSON_AddNumberToObject(
            objeto,
            "id",
            ejemplares[i].id
        );

        // Guardar nombre de la produccion
        cJSON_AddStringToObject(
            objeto,
            "produccion",
            ejemplares[i].nombreProduccion
        );

        // Agregar al arreglo
        cJSON_AddItemToArray(arreglo, objeto);
    }

    // Convertir JSON a texto
    textoJSON = cJSON_Print(arreglo);

    if (textoJSON == NULL)
    {
        cJSON_Delete(arreglo);
        return 0;
    }

    // Abrir archivo
    archivo = fopen(ARCHIVO_EJEMPLARES, "w");

    if (archivo == NULL)
    {
        cJSON_free(textoJSON);
        cJSON_Delete(arreglo);
        return 0;
    }

    // Guardar JSON
    fprintf(archivo, "%s", textoJSON);

    fclose(archivo);

    // Liberar memoria
    cJSON_free(textoJSON);
    cJSON_Delete(arreglo);

    return 1;
}
//E: Lista de ejemplares y cantidad.
//S: Carga los ejemplares guardados anteriormente.
//R: El archivo debe tener el formato correcto.
//F: Leer los ejemplares guardados en JSON.
int cargarEjemplaresJSON(
    Ejemplar **ejemplares,
    int *cantidad
)
{
    FILE *archivo;
    char *texto;
    long tamano;

    cJSON *arreglo;
    cJSON *objeto;

    int i;
    int total;

    // Abrir archivo
    archivo = fopen(ARCHIVO_EJEMPLARES, "r");

    // Si no existe, empezar vacio
    if (archivo == NULL)
    {
        return 1;
    }

    // Obtener tamaño
    fseek(archivo, 0, SEEK_END);
    tamano = ftell(archivo);
    rewind(archivo);

    // Reservar memoria
    texto = malloc((tamano + 1) * sizeof(char));

    if (texto == NULL)
    {
        fclose(archivo);
        return 0;
    }

    // Leer archivo
    fread(texto, sizeof(char), tamano, archivo);

    texto[tamano] = '\0';

    fclose(archivo);

    // Convertir texto a JSON
    arreglo = cJSON_Parse(texto);

    free(texto);

    if (arreglo == NULL)
    {
        return 0;
    }

    total = cJSON_GetArraySize(arreglo);

    // Recorrer ejemplares guardados
    for (i = 0; i < total; i++)
    {
        Ejemplar nuevo;

        cJSON *id;
        cJSON *produccion;

        objeto = cJSON_GetArrayItem(arreglo, i);

        // Buscar datos
        id = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "id"
        );

        produccion = cJSON_GetObjectItemCaseSensitive(
            objeto,
            "produccion"
        );

        // Revisar datos
        if (
            !cJSON_IsNumber(id) ||
            !cJSON_IsString(produccion)
        )
        {
            continue;
        }

        // Guardar datos
        nuevo.id = id->valueint;

        nuevo.nombreProduccion =
            copiarTexto(produccion->valuestring);

        if (nuevo.nombreProduccion == NULL)
        {
            cJSON_Delete(arreglo);
            return 0;
        }

        // Agregar a memoria
        if (agregarEjemplar(ejemplares,cantidad,&nuevo
        ) == 0)
        {
            liberarEjemplar(&nuevo);
            cJSON_Delete(arreglo);

            return 0;
        }
    }

    // Liberar JSON
    cJSON_Delete(arreglo);

    return 1;
}
//E: Produccion y lista de ejemplares.
//S: Crea los ejemplares segun la cantidad.
//R: La produccion debe ser valida.
//F: Generar los ejemplares de una produccion.
void generarEjemplares(
    Produccion *produccion,
    Ejemplar **ejemplares,
    int *cantidadEjemplares
)
{
    int i;
    Ejemplar nuevo;

    // Crear la cantidad indicada
    for (i = 0; i < (*produccion).cantidadEjemplares; i++)
    {
        // Generar ID
        nuevo.id = siguienteId(
            *ejemplares,
            *cantidadEjemplares
        );

        // Guardar nombre de la produccion
        nuevo.nombreProduccion =
            copiarTexto((*produccion).nombre);

        if (nuevo.nombreProduccion == NULL)
        {
            printf("No se pudo crear el ejemplar\n");
            return;
        }

        // Agregar ejemplar
        if (agregarEjemplar(
            ejemplares,
            cantidadEjemplares,
            &nuevo
        ) == 0)
        {
            printf("No se pudo agregar el ejemplar\n");

            liberarEjemplar(&nuevo);

            return;
        }
    }
}
//E: Lista de ejemplares y cantidad.
//S: Retorna el siguiente identificador.
//R: Los ejemplares deben estar cargados.
//F: Buscar el ID mayor y generar el siguiente.
int siguienteId(
    Ejemplar *ejemplares,
    int cantidad
)
{
    int mayor = 0;
    int i;

    // Buscar ID mayor
    for (i = 0; i < cantidad; i++)
    {
        if (ejemplares[i].id > mayor)
        {
            mayor = ejemplares[i].id;
        }
    }

    return mayor + 1;
}

//OPCION 2

//E: No recibe datos.
//S: Muestra todas las producciones del catalogo.
//R: El archivo catalogo.json debe existir o estar vacio.
//F: Mostrar el catalogo completo.
void mostrarCatalogo(void)
{
    Produccion *catalogo = NULL;
    int cantidad = 0;
    int i;

    // Cargar catalogo
    if (cargarCatalogoJSON(
        &catalogo,
        &cantidad
    ) == 0)
    {
        printf("No se pudo cargar el catalogo\n");
        return;
    }

    // Revisar si esta vacio
    if (cantidad == 0)
    {
        printf("El catalogo esta vacio\n");
        return;
    }

    printf("\n----- CATALOGO COMPLETO -----\n");

    // Mostrar cada produccion
    for (i = 0; i < cantidad; i++)
    {
        printf("\nProduccion %d\n", i + 1);

        printf(
            "Nombre: %s\n",
            catalogo[i].nombre
        );

        printf(
            "Autor: %s\n",
            catalogo[i].autor
        );

        printf(
            "Anio: %d\n",
            catalogo[i].anPubli
        );

        printf(
            "Genero: %s\n",
            catalogo[i].genero
        );

        printf(
            "Resumen: %s\n",
            catalogo[i].resumen
        );

        printf(
            "Cantidad: %d\n",
            catalogo[i].cantidadEjemplares
        );
    }

    // Liberar memoria
    for (i = 0; i < cantidad; i++)
    {
        liberarProduccion(&catalogo[i]);
    }

    free(catalogo);
}