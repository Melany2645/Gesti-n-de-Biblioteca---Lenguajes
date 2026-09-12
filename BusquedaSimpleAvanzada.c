#include "BusquedaSimpleAvanzada.h"

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

// Función auxiliar para cargar catálogo desde JSON
static ProduccionBusqueda *cargarCatalogo(int *cantidad) {
    FILE *archivoCat = fopen("catalogo.json", "r");
    if (archivoCat == NULL) {
        printf("Error: No se pudo abrir catalogo.json\n");
        return NULL;
    }
    
    fseek(archivoCat, 0, SEEK_END);
    long tamanoCat = ftell(archivoCat);
    rewind(archivoCat);
    
    char *textoCat = malloc((tamanoCat + 1) * sizeof(char));
    if (textoCat == NULL) {
        fclose(archivoCat);
        printf("Error de memoria.\n");
        return NULL;
    }
    
    fread(textoCat, sizeof(char), tamanoCat, archivoCat);
    textoCat[tamanoCat] = '\0';
    fclose(archivoCat);
    
    cJSON *arregloCat = cJSON_Parse(textoCat);
    free(textoCat);
    
    if (arregloCat == NULL) {
        printf("Error al parsear catalogo.json\n");
        return NULL;
    }
    
    *cantidad = cJSON_GetArraySize(arregloCat);
    ProduccionBusqueda *catalogo = malloc(*cantidad * sizeof(ProduccionBusqueda));
    if (catalogo == NULL) {
        cJSON_Delete(arregloCat);
        printf("Error de memoria.\n");
        return NULL;
    }
    
    for (int i = 0; i < *cantidad; i++) {
        cJSON *obj = cJSON_GetArrayItem(arregloCat, i);
        cJSON *nombre = cJSON_GetObjectItemCaseSensitive(obj, "nombre");
        cJSON *autor = cJSON_GetObjectItemCaseSensitive(obj, "autor");
        cJSON *genero = cJSON_GetObjectItemCaseSensitive(obj, "genero");
        cJSON *resumen = cJSON_GetObjectItemCaseSensitive(obj, "resumen");
        cJSON *anio = cJSON_GetObjectItemCaseSensitive(obj, "anioPublicacion");
        cJSON *cantidadEj = cJSON_GetObjectItemCaseSensitive(obj, "cantidad");
        
        catalogo[i].nombre = strdup(nombre ? nombre->valuestring : "");
        catalogo[i].autor = strdup(autor ? autor->valuestring : "");
        catalogo[i].genero = strdup(genero ? genero->valuestring : "");
        catalogo[i].resumen = strdup(resumen ? resumen->valuestring : "");
        catalogo[i].anPubli = anio ? anio->valueint : 0;
        catalogo[i].cantidadEjemplares = cantidadEj ? cantidadEj->valueint : 0;
    }
    
    cJSON_Delete(arregloCat);
    return catalogo;
}

// Función auxiliar para liberar catálogo
static void liberarCatalogo(ProduccionBusqueda *catalogo, int cantidad) {
    if (catalogo == NULL) return;
    for (int i = 0; i < cantidad; i++) {
        free(catalogo[i].nombre);
        free(catalogo[i].autor);
        free(catalogo[i].genero);
        free(catalogo[i].resumen);
    }
    free(catalogo);
}



void busquedaSimple(void) {
    int cantidad = 0;
    ProduccionBusqueda *catalogo = cargarCatalogo(&cantidad);
    if (catalogo == NULL) return;
    
    char textoBusqueda[200];
    printf("\n========== BUSQUEDA SIMPLE ==========\n");
    printf("Ingrese el texto a buscar: ");
    limpiarBuffer();  // <-- LIMPIAR BUFFER ANTES DE LEER
    scanf("%[^\n]", textoBusqueda);
    limpiarBuffer();
    
    if (strlen(textoBusqueda) == 0) {
        printf("Error: Debe ingresar un texto de búsqueda.\n");
        liberarCatalogo(catalogo, cantidad);
        return;
    }
    
    printf("\n--- Resultados de busqueda para '%s' ---\n", textoBusqueda);
    printf("Buscando en: nombre, autor y resumen\n\n");
    
    int encontrados = 0;
    
    for (int i = 0; i < cantidad; i++) {
        int coincide = 0;
        
        if (contiene(catalogo[i].nombre, textoBusqueda) ||
            contiene(catalogo[i].autor, textoBusqueda) ||
            contiene(catalogo[i].resumen, textoBusqueda)) {
            coincide = 1;
        }
        
        if (coincide) {
            encontrados++;
            printf("================================\n");
            printf("ID: %d\n", i + 1);
            printf("Nombre: %s\n", catalogo[i].nombre);
            printf("Autor: %s\n", catalogo[i].autor);
            printf("Anno: %d\n", catalogo[i].anPubli);
            printf("Genero: %s\n", catalogo[i].genero);
            printf("Resumen: %s\n", catalogo[i].resumen);
            printf("================================\n\n");
        }
    }
    
    if (encontrados == 0) {
        printf("No se encontraron resultados para '%s'\n", textoBusqueda);
    } else {
        printf("Total de resultados: %d\n", encontrados);
    }
    
    liberarCatalogo(catalogo, cantidad);
}


void busquedaAvanzada(void) {
    int cantidad = 0;
    ProduccionBusqueda *catalogo = cargarCatalogo(&cantidad);
    if (catalogo == NULL) return;
    
    char busquedaNombre[200] = "";
    char busquedaAutor[200] = "";
    char busquedaGenero[200] = "";
    char busquedaResumen[200] = "";
    char tipoBusqueda[20];
    char operadorLogico[5];
    
    printf("\n========== BUSQUEDA AVANZADA ==========\n");
    printf("Tipo de busqueda:\n");
    printf("1. Contiene (busqueda parcial)\n");
    printf("2. Exacta (coincidencia total)\n");
    printf("Seleccione: ");
    int opcionTipo;
    scanf("%d", &opcionTipo);
    limpiarBuffer();
    
    strcpy(tipoBusqueda, opcionTipo == 1 ? "contiene" : "exacta");
    
    printf("\nCampos de búsqueda (deje vacío para omitir):\n");
    printf("Nombre: ");
    limpiarBuffer(); 
    scanf("%[^\n]", busquedaNombre);
    limpiarBuffer();
    
    printf("Autor: ");
    limpiarBuffer();  
    scanf("%[^\n]", busquedaAutor);
    limpiarBuffer();
    
    printf("Genero: ");
    limpiarBuffer(); 
    scanf("%[^\n]", busquedaGenero);
    limpiarBuffer();
    
    printf("Resumen: ");
    limpiarBuffer();  
    scanf("%[^\n]", busquedaResumen);
    limpiarBuffer();
    
    int usarNombre = strlen(busquedaNombre) > 0;
    int usarAutor = strlen(busquedaAutor) > 0;
    int usarGenero = strlen(busquedaGenero) > 0;
    int usarResumen = strlen(busquedaResumen) > 0;
    
    if (!usarNombre && !usarAutor && !usarGenero && !usarResumen) {
        printf("Error: Debe ingresar al menos un campo de búsqueda.\n");
        liberarCatalogo(catalogo, cantidad);
        return;
    }
    
    int camposActivos = usarNombre + usarAutor + usarGenero + usarResumen;
    if (camposActivos > 1) {
        printf("\nOperador logico:\n1. Y (todos deben coincidir)\n2. O (al menos uno debe coincidir)\nSeleccione: ");
        int op;
        scanf("%d", &op);
        limpiarBuffer();
        strcpy(operadorLogico, op == 1 ? "y" : "o");
    } else {
        strcpy(operadorLogico, "y");
    }
    
    printf("\n--- Resultados ---\n");
    printf("Filtros: ");
    if (usarNombre) printf("Nombre='%s' ", busquedaNombre);
    if (usarAutor) printf("Autor='%s' ", busquedaAutor);
    if (usarGenero) printf("Género='%s' ", busquedaGenero);
    if (usarResumen) printf("Resumen='%s' ", busquedaResumen);
    printf("| Tipo: %s | Operador: %s\n\n", tipoBusqueda, operadorLogico);
    
    int encontrados = 0;
    
    for (int i = 0; i < cantidad; i++) {
        int cNombre = !usarNombre || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(catalogo[i].nombre, busquedaNombre) : 
                    esExacta(catalogo[i].nombre, busquedaNombre));
        int cAutor = !usarAutor || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(catalogo[i].autor, busquedaAutor) : 
                    esExacta(catalogo[i].autor, busquedaAutor));
        int cGenero = !usarGenero || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(catalogo[i].genero, busquedaGenero) : 
                    esExacta(catalogo[i].genero, busquedaGenero));
        int cResumen = !usarResumen || (strcmp(tipoBusqueda, "contiene") == 0 ? 
                    contiene(catalogo[i].resumen, busquedaResumen) : 
                    esExacta(catalogo[i].resumen, busquedaResumen));
        
        int coincide = strcmp(operadorLogico, "y") == 0 ? 
                      (cNombre && cAutor && cGenero && cResumen) : 
                      (cNombre || cAutor || cGenero || cResumen);
        
        if (coincide) {
            encontrados++;
            printf("================================\n");
            printf("ID: %d\n", i + 1);
            printf("Nombre: %s\n", catalogo[i].nombre);
            printf("Autor: %s\n", catalogo[i].autor);
            printf("Genero: %s\n", catalogo[i].genero);
            printf("Anno: %d\n", catalogo[i].anPubli);
            printf("Resumen: %s\n", catalogo[i].resumen);
            printf("================================\n\n");
        }
    }
    
    printf("Total: %d resultados\n", encontrados);
    
    liberarCatalogo(catalogo, cantidad);
}