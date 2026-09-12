#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"
#include "BusquedaSimpleAvanzada.h"

// Estructura para ejemplar con disponibilidad
typedef struct {
    int id;
    char *nombreProduccion;
    int disponible;
} EjemplarInfo;

// Función auxiliar para limpiar buffer
static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Función auxiliar para convertir a minúsculas
static void aMinusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Función auxiliar para buscar si una cadena contiene otra (case-insensitive)
static int contiene(const char *texto, const char *busqueda) {
    if (texto == NULL || busqueda == NULL || strlen(busqueda) == 0) {
        return 0;
    }

    char textoLower[500];
    char busquedaLower[500];
    
    strncpy(textoLower, texto, sizeof(textoLower) - 1);
    textoLower[sizeof(textoLower) - 1] = '\0';
    strncpy(busquedaLower, busqueda, sizeof(busquedaLower) - 1);
    busquedaLower[sizeof(busquedaLower) - 1] = '\0';
    
    aMinusculas(textoLower);
    aMinusculas(busquedaLower);
    
    return strstr(textoLower, busquedaLower) != NULL;
}

// Función auxiliar para búsqueda exacta (case-insensitive)
static int esExacta(const char *texto, const char *busqueda) {
    if (texto == NULL || busqueda == NULL) {
        return 0;
    }

    char textoLower[500];
    char busquedaLower[500];
    
    strncpy(textoLower, texto, sizeof(textoLower) - 1);
    textoLower[sizeof(textoLower) - 1] = '\0';
    strncpy(busquedaLower, busqueda, sizeof(busquedaLower) - 1);
    busquedaLower[sizeof(busquedaLower) - 1] = '\0';
    
    aMinusculas(textoLower);
    aMinusculas(busquedaLower);
    
    return strcmp(textoLower, busquedaLower) == 0;
}

// Función para cargar ejemplares y verificar disponibilidad
static EjemplarInfo *cargarEjemplaresConDisponibilidad(int *totalEjemplares) {
    FILE *archivoEjemplares = fopen("ejemplares.json", "r");
    if (archivoEjemplares == NULL) {
        *totalEjemplares = 0;
        return NULL;
    }
    
    fseek(archivoEjemplares, 0, SEEK_END);
    long tamano = ftell(archivoEjemplares);
    rewind(archivoEjemplares);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoEjemplares);
        *totalEjemplares = 0;
        return NULL;
    }
    
    fread(texto, sizeof(char), tamano, archivoEjemplares);
    texto[tamano] = '\0';
    fclose(archivoEjemplares);
    
    cJSON *arreglo = cJSON_Parse(texto);
    free(texto);
    
    if (arreglo == NULL) {
        *totalEjemplares = 0;
        return NULL;
    }
    
    *totalEjemplares = cJSON_GetArraySize(arreglo);
    EjemplarInfo *ejemplares = malloc(*totalEjemplares * sizeof(EjemplarInfo));
    
    // Inicializar todos como disponibles
    for (int i = 0; i < *totalEjemplares; i++) {
        cJSON *obj = cJSON_GetArrayItem(arreglo, i);
        cJSON *id = cJSON_GetObjectItemCaseSensitive(obj, "id");
        cJSON *produccion = cJSON_GetObjectItemCaseSensitive(obj, "produccion");
        
        ejemplares[i].id = id->valueint;
        ejemplares[i].nombreProduccion = strdup(produccion->valuestring);
        ejemplares[i].disponible = 1; // Por defecto disponible
    }
    
    // Verificar qué ejemplares están prestados (no disponibles)
    FILE *archivoPrestamos = fopen("prestamos.json", "r");
    if (archivoPrestamos != NULL) {
        fseek(archivoPrestamos, 0, SEEK_END);
        tamano = ftell(archivoPrestamos);
        rewind(archivoPrestamos);
        
        texto = malloc((tamano + 1) * sizeof(char));
        if (texto != NULL) {
            fread(texto, sizeof(char), tamano, archivoPrestamos);
            texto[tamano] = '\0';
            fclose(archivoPrestamos);
            
            cJSON *arregloPrestamos = cJSON_Parse(texto);
            free(texto);
            
            if (arregloPrestamos != NULL) {
                int totalPrestamos = cJSON_GetArraySize(arregloPrestamos);
                for (int i = 0; i < totalPrestamos; i++) {
                    cJSON *prestamo = cJSON_GetArrayItem(arregloPrestamos, i);
                    cJSON *estado = cJSON_GetObjectItemCaseSensitive(prestamo, "estado");
                    
                    // Si el préstamo está ACTIVO (estado = 1), los ejemplares no están disponibles
                    if (estado != NULL && estado->valueint == 1) {
                        cJSON *ejemplaresPrestamo = cJSON_GetObjectItemCaseSensitive(prestamo, "ejemplares");
                        int numEjemplares = cJSON_GetArraySize(ejemplaresPrestamo);
                        
                        for (int j = 0; j < numEjemplares; j++) {
                            cJSON *ejPrestado = cJSON_GetArrayItem(ejemplaresPrestamo, j);
                            cJSON *idEjemplar = cJSON_GetObjectItemCaseSensitive(ejPrestado, "id");
                            
                            // Marcar como no disponible
                            for (int k = 0; k < *totalEjemplares; k++) {
                                if (ejemplares[k].id == idEjemplar->valueint) {
                                    ejemplares[k].disponible = 0;
                                    break;
                                }
                            }
                        }
                    }
                }
                cJSON_Delete(arregloPrestamos);
            }
        }
    }
    
    cJSON_Delete(arreglo);
    return ejemplares;
}


void busquedaSimple(void) {
    char textoBusqueda[200];

    printf("\n========== BUSQUEDA SIMPLE ==========\n");
    printf("Ingrese el texto a buscar: ");
    scanf(" %199[^\n]", textoBusqueda);
    limpiarBuffer();
    
    if (strlen(textoBusqueda) == 0) {
        printf("Error: Debe ingresar un texto de búsqueda.\n");
        return;
    }
    
    // Cargar catálogo
    FILE *archivoCat = fopen("catalogo.json", "r");
    if (archivoCat == NULL) {
        printf("Error: No se pudo abrir catalogo.json\n");
        return;
    }
    
    fseek(archivoCat, 0, SEEK_END);
    long tamano = ftell(archivoCat);
    rewind(archivoCat);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoCat);
        printf("Error de memoria.\n");
        return;
    }
    
    fread(texto, sizeof(char), tamano, archivoCat);
    texto[tamano] = '\0';
    fclose(archivoCat);
    
    cJSON *arregloCat = cJSON_Parse(texto);
    free(texto);
    
    if (arregloCat == NULL) {
        printf("Error al parsear catalogo.json\n");
        return;
    }
    
    // Cargar ejemplares con disponibilidad
    int totalEjemplares = 0;
    EjemplarInfo *ejemplares = cargarEjemplaresConDisponibilidad(&totalEjemplares);
    
    printf("\n--- Resultados de busqueda para '%s' ---\n", textoBusqueda);
    printf("Buscando en: nombre, autor y resumen\n\n");
    
    int encontrados = 0;
    int totalCatalogo = cJSON_GetArraySize(arregloCat);
    
    for (int i = 0; i < totalCatalogo; i++) {
        cJSON *obj = cJSON_GetArrayItem(arregloCat, i);
        cJSON *nombre = cJSON_GetObjectItemCaseSensitive(obj, "nombre");
        cJSON *autor = cJSON_GetObjectItemCaseSensitive(obj, "autor");
        cJSON *resumen = cJSON_GetObjectItemCaseSensitive(obj, "resumen");
        
        int coincide = 0;
        
        // Buscar en nombre, autor o resumen (operador OR)
        if (contiene(nombre->valuestring, textoBusqueda) ||
            contiene(autor->valuestring, textoBusqueda) ||
            contiene(resumen->valuestring, textoBusqueda)) {
            coincide = 1;
        }
        
        if (coincide) {
            encontrados++;
            printf("--------------------------------\n");
            printf("Produccion: %s\n", nombre->valuestring);
            printf("Autor: %s\n", autor->valuestring);
            printf("Resumen: %s\n", resumen->valuestring);
            printf("Ejemplares disponibles:\n");
            
            // Mostrar ejemplares de esta producción con su ID y disponibilidad
            int ejemplaresEncontrados = 0;
            for (int j = 0; j < totalEjemplares; j++) {
                if (strcmp(ejemplares[j].nombreProduccion, nombre->valuestring) == 0) {
                    ejemplaresEncontrados++;
                    printf("  - ID: %-5d | Estado: %s\n", 
                           ejemplares[j].id,
                           ejemplares[j].disponible ? "Disponible" : "No disponible");
                }
            }
            
            if (ejemplaresEncontrados == 0) {
                printf("  Sin ejemplares registrados\n");
            }
            printf("\n");
        }
    }
    
    if (encontrados == 0) {
        printf("No se encontraron resultados para '%s'\n", textoBusqueda);
    } else {
        printf("Total de resultados: %d producciones encontradas\n", encontrados);
    }
    
    // Liberar memoria
    cJSON_Delete(arregloCat);
    if (ejemplares != NULL) {
        for (int i = 0; i < totalEjemplares; i++) {
            free(ejemplares[i].nombreProduccion);
        }
        free(ejemplares);
    }
}


void busquedaAvanzada(void) {
    char busquedaNombre[200] = "";
    char busquedaAutor[200] = "";
    char busquedaGenero[200] = "";
    char busquedaResumen[200] = "";
    char tipoBusqueda[20];
    char operadorLogico[5];
    
    printf("\n========== BUSQUEDA AVANZADA ==========\n");
    printf("Tipo de busqueda:\n");
    printf("1. Contiene (búsqueda parcial)\n");
    printf("2. Exacta (coincidencia total)\n");
    printf("Seleccione: ");
    int opcionTipo;
    scanf("%d", &opcionTipo);
    limpiarBuffer();
    
    strcpy(tipoBusqueda, opcionTipo == 1 ? "contiene" : "exacta");
    
    printf("\nCampos de búsqueda (deje vacio para omitir):\n");
    printf("Nombre: ");
    scanf("%[^\n]", busquedaNombre);
    limpiarBuffer();
    
    printf("Autor: ");
    scanf("%[^\n]", busquedaAutor);
    limpiarBuffer();
    
    printf("Género: ");
    scanf("%[^\n]", busquedaGenero);
    limpiarBuffer();
    
    printf("Resumen: ");
    scanf("%[^\n]", busquedaResumen);
    limpiarBuffer();
    
    int usarNombre = strlen(busquedaNombre) > 0;
    int usarAutor = strlen(busquedaAutor) > 0;
    int usarGenero = strlen(busquedaGenero) > 0;
    int usarResumen = strlen(busquedaResumen) > 0;
    
    if (!usarNombre && !usarAutor && !usarGenero && !usarResumen) {
        printf("Error: Debe ingresar al menos un campo de búsqueda.\n");
        return;
    }
    
    int camposActivos = usarNombre + usarAutor + usarGenero + usarResumen;
    if (camposActivos > 1) {
        printf("\nOperador lógico:\n1. Y (todos deben coincidir)\n2. O (al menos uno debe coincidir)\nSeleccione: ");
        int op;
        scanf("%d", &op);
        limpiarBuffer();
        strcpy(operadorLogico, op == 1 ? "y" : "o");
    } else {
        strcpy(operadorLogico, "y");
    }
    
    // Cargar catálogo
    FILE *archivoCat = fopen("catalogo.json", "r");
    if (archivoCat == NULL) {
        printf("Error: No se pudo abrir catalogo.json\n");
        return;
    }
    
    fseek(archivoCat, 0, SEEK_END);
    long tamano = ftell(archivoCat);
    rewind(archivoCat);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoCat);
        printf("Error de memoria.\n");
        return;
    }
    
    fread(texto, sizeof(char), tamano, archivoCat);
    texto[tamano] = '\0';
    fclose(archivoCat);
    
    cJSON *arregloCat = cJSON_Parse(texto);
    free(texto);
    
    if (arregloCat == NULL) {
        printf("Error al parsear catalogo.json\n");
        return;
    }
    
    // Cargar ejemplares con disponibilidad
    int totalEjemplares = 0;
    EjemplarInfo *ejemplares = cargarEjemplaresConDisponibilidad(&totalEjemplares);
    
    printf("\n--- Resultados ---\n");
    printf("Filtros: ");
    if (usarNombre) printf("Nombre='%s' ", busquedaNombre);
    if (usarAutor) printf("Autor='%s' ", busquedaAutor);
    if (usarGenero) printf("Género='%s' ", busquedaGenero);
    if (usarResumen) printf("Resumen='%s' ", busquedaResumen);
    printf("| Tipo: %s | Operador: %s\n\n", tipoBusqueda, operadorLogico);
    
    int encontrados = 0;
    int totalCatalogo = cJSON_GetArraySize(arregloCat);
    
    for (int i = 0; i < totalCatalogo; i++) {
        cJSON *obj = cJSON_GetArrayItem(arregloCat, i);
        cJSON *nombre = cJSON_GetObjectItemCaseSensitive(obj, "nombre");
        cJSON *autor = cJSON_GetObjectItemCaseSensitive(obj, "autor");
        cJSON *genero = cJSON_GetObjectItemCaseSensitive(obj, "genero");
        cJSON *resumen = cJSON_GetObjectItemCaseSensitive(obj, "resumen");
        
        int cNombre = !usarNombre || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(nombre->valuestring, busquedaNombre) : 
                    esExacta(nombre->valuestring, busquedaNombre));
        int cAutor = !usarAutor || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(autor->valuestring, busquedaAutor) : 
                    esExacta(autor->valuestring, busquedaAutor));
        int cGenero = !usarGenero || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(genero->valuestring, busquedaGenero) : 
                    esExacta(genero->valuestring, busquedaGenero));
        int cResumen = !usarResumen || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(resumen->valuestring, busquedaResumen) : 
                    esExacta(resumen->valuestring, busquedaResumen));
        
        int coincide = strcmp(operadorLogico, "y") == 0 ? 
                      (cNombre && cAutor && cGenero && cResumen) : 
                      (cNombre || cAutor || cGenero || cResumen);
        
        if (coincide) {
            encontrados++;
            printf("--------------------------------\n");
            printf("Produccion: %s\n", nombre->valuestring);
            printf("Autor: %s\n", autor->valuestring);
            printf("Genero: %s\n", genero->valuestring);
            printf("Resumen: %s\n", resumen->valuestring);
            printf("Ejemplares disponibles:\n");
            
            // Mostrar ejemplares de esta producción con su ID y disponibilidad
            int ejemplaresEncontrados = 0;
            for (int j = 0; j < totalEjemplares; j++) {
                if (strcmp(ejemplares[j].nombreProduccion, nombre->valuestring) == 0) {
                    ejemplaresEncontrados++;
                    printf("  - ID: %-5d | Estado: %s\n", 
                           ejemplares[j].id,
                           ejemplares[j].disponible ? "Disponible" : "No disponible");
                }
            }
            
            if (ejemplaresEncontrados == 0) {
                printf("  Sin ejemplares registrados\n");
            }
            printf("\n");
        }
    }
    
    printf("Total: %d resultados\n", encontrados);
    
    // Liberar memoria
    cJSON_Delete(arregloCat);
    if (ejemplares != NULL) {
        for (int i = 0; i < totalEjemplares; i++) {
            free(ejemplares[i].nombreProduccion);
        }
        free(ejemplares);
    }
}