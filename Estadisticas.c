#include "Estadisticas.h"
#include "Usuario.h"

// Función auxiliar para limpiar buffer
static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Función para ordenar ProduccionConteo de mayor a menor
static void ordenarProducciones(ProduccionConteo *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].cantidadPrestamos < arr[j + 1].cantidadPrestamos) {
                ProduccionConteo temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Función para ordenar UsuarioConteo de mayor a menor
static void ordenarUsuarios(UsuarioConteo *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].cantidadPrestamos < arr[j + 1].cantidadPrestamos) {
                UsuarioConteo temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Función para ordenar MesAnioConteo de mayor a menor
static void ordenarMesesAnio(MesAnioConteo *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].totalDinero < arr[j + 1].totalDinero) {
                MesAnioConteo temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Función para ordenar GeneroConteo de mayor a menor
static void ordenarGeneros(GeneroConteo *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].cantidadPrestamos < arr[j + 1].cantidadPrestamos) {
                GeneroConteo temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void estadisticasProduccionesMasPrestadas(void) {
    printf("\n========== TOP 3 PRODUCCIONES MAS PRESTADAS ==========\n");
    
    // Abrir archivo de préstamos
    FILE *archivoPrestamos = fopen("prestamos.json", "r");
    if (archivoPrestamos == NULL) {
        printf("No hay prestamos registrados.\n");
        return;
    }
    
    fseek(archivoPrestamos, 0, SEEK_END);
    long tamano = ftell(archivoPrestamos);
    rewind(archivoPrestamos);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoPrestamos);
        printf("Error de memoria.\n");
        return;
    }
    
    fread(texto, sizeof(char), tamano, archivoPrestamos);
    texto[tamano] = '\0';
    fclose(archivoPrestamos);
    
    cJSON *arregloPrestamos = cJSON_Parse(texto);
    free(texto);
    
    if (arregloPrestamos == NULL || cJSON_GetArraySize(arregloPrestamos) == 0) {
        printf("No hay prestamos registrados.\n");
        cJSON_Delete(arregloPrestamos);
        return;
    }
    
    // Contar préstamos por producción
    int totalPrestamos = cJSON_GetArraySize(arregloPrestamos);
    ProduccionConteo *conteos = malloc(totalPrestamos * sizeof(ProduccionConteo));
    int uniqueCount = 0;
    
    for (int i = 0; i < totalPrestamos; i++) {
        cJSON *prestamo = cJSON_GetArrayItem(arregloPrestamos, i);
        cJSON *ejemplares = cJSON_GetObjectItemCaseSensitive(prestamo, "ejemplares");
        
        int numEjemplares = cJSON_GetArraySize(ejemplares);
        for (int j = 0; j < numEjemplares; j++) {
            cJSON *ejemplar = cJSON_GetArrayItem(ejemplares, j);
            cJSON *produccion = cJSON_GetObjectItemCaseSensitive(ejemplar, "produccion");
            
            // Verificar si ya existe esta producción
            int encontrado = 0;
            for (int k = 0; k < uniqueCount; k++) {
                if (strcmp(conteos[k].nombreProduccion, produccion->valuestring) == 0) {
                    conteos[k].cantidadPrestamos++;
                    encontrado = 1;
                    break;
                }
            }
            
            if (!encontrado) {
                conteos[uniqueCount].nombreProduccion = strdup(produccion->valuestring);
                conteos[uniqueCount].cantidadPrestamos = 1;
                uniqueCount++;
            }
        }
    }
    
    cJSON_Delete(arregloPrestamos);
    
    if (uniqueCount == 0) {
        printf("No hay registros de prestamos.\n");
        free(conteos);
        return;
    }
    
    // Ordenar y mostrar top 3
    ordenarProducciones(conteos, uniqueCount);
    
    printf("\n%-5s | %-40s | %s\n", "Pos", "Produccion", "Prestamos");
    printf("------|------------------------------------------|----------\n");
    
    int max = (uniqueCount < 3) ? uniqueCount : 3;
    for (int i = 0; i < max; i++) {
        printf("%-5d | %-40s | %d\n", i + 1, conteos[i].nombreProduccion, conteos[i].cantidadPrestamos);
    }
    
    printf("\n========================================================\n");
    
    // Liberar memoria
    for (int i = 0; i < uniqueCount; i++) {
        free(conteos[i].nombreProduccion);
    }
    free(conteos);
}


void estadisticasUsuarioMasPrestamos(void) {
    printf("\n========== TOP 3 USUARIOS CON MÁS PRESTAMOS ==========\n");
    
    // Abrir archivo de préstamos
    FILE *archivoPrestamos = fopen("prestamos.json", "r");
    if (archivoPrestamos == NULL) {
        printf("No hay prestamos registrados.\n");
        return;
    }
    
    fseek(archivoPrestamos, 0, SEEK_END);
    long tamano = ftell(archivoPrestamos);
    rewind(archivoPrestamos);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoPrestamos);
        printf("Error de memoria.\n");
        return;
    }
    
    fread(texto, sizeof(char), tamano, archivoPrestamos);
    texto[tamano] = '\0';
    fclose(archivoPrestamos);
    
    cJSON *arregloPrestamos = cJSON_Parse(texto);
    free(texto);
    
    if (arregloPrestamos == NULL || cJSON_GetArraySize(arregloPrestamos) == 0) {
        printf("No hay prestamos registrados.\n");
        cJSON_Delete(arregloPrestamos);
        return;
    }
    
    // Contar préstamos por usuario
    int totalPrestamos = cJSON_GetArraySize(arregloPrestamos);
    UsuarioConteo *conteos = malloc(totalPrestamos * sizeof(UsuarioConteo));
    int uniqueCount = 0;
    
    for (int i = 0; i < totalPrestamos; i++) {
        cJSON *prestamo = cJSON_GetArrayItem(arregloPrestamos, i);
        cJSON *idUsuario = cJSON_GetObjectItemCaseSensitive(prestamo, "idUsuario");
        
        // Verificar si ya existe este usuario
        int encontrado = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (conteos[j].idUsuario == idUsuario->valueint) {
                conteos[j].cantidadPrestamos++;
                encontrado = 1;
                break;
            }
        }
        
        if (!encontrado) {
            conteos[uniqueCount].idUsuario = idUsuario->valueint;
            conteos[uniqueCount].nombreUsuario = NULL; // Se cargará después
            conteos[uniqueCount].cantidadPrestamos = 1;
            uniqueCount++;
        }
    }
    
    cJSON_Delete(arregloPrestamos);
    
    if (uniqueCount == 0) {
        printf("No hay registros de prestamos.\n");
        free(conteos);
        return;
    }
    
    // Cargar usuarios para obtener nombres
    FILE *archivoUsuarios = fopen("usuarios.json", "r");
    if (archivoUsuarios != NULL) {
        fseek(archivoUsuarios, 0, SEEK_END);
        tamano = ftell(archivoUsuarios);
        rewind(archivoUsuarios);
        
        texto = malloc((tamano + 1) * sizeof(char));
        if (texto != NULL) {
            fread(texto, sizeof(char), tamano, archivoUsuarios);
            texto[tamano] = '\0';
            fclose(archivoUsuarios);
            
            cJSON *arregloUsuarios = cJSON_Parse(texto);
            free(texto);
            
            if (arregloUsuarios != NULL) {
                int totalUsuarios = cJSON_GetArraySize(arregloUsuarios);
                for (int i = 0; i < uniqueCount; i++) {
                    for (int j = 0; j < totalUsuarios; j++) {
                        cJSON *usuario = cJSON_GetArrayItem(arregloUsuarios, j);
                        cJSON *id = cJSON_GetObjectItemCaseSensitive(usuario, "id");
                        cJSON *nombre = cJSON_GetObjectItemCaseSensitive(usuario, "nombre");
                        
                        if (id->valueint == conteos[i].idUsuario) {
                            conteos[i].nombreUsuario = strdup(nombre->valuestring);
                            break;
                        }
                    }
                }
                cJSON_Delete(arregloUsuarios);
            }
        }
    }
    
    // Ordenar y mostrar Top 3
    ordenarUsuarios(conteos, uniqueCount);

    printf("\n%-5s | %-30s | %s\n", "Pos", "Usuario", "Prestamos");
    printf("------|--------------------------------|----------\n");

    int max = (uniqueCount < 3) ? uniqueCount : 3;

    for (int i = 0; i < max; i++) {
        printf("%-5d | %-30s | %d\n",
            i + 1,
            conteos[i].nombreUsuario ? conteos[i].nombreUsuario : "Desconocido",
            conteos[i].cantidadPrestamos);
    }
    
    // Liberar memoria
    for (int i = 0; i < uniqueCount; i++) {
        free(conteos[i].nombreUsuario);
    }
    free(conteos);
}


void estadisticasMesesAnioMasDinero(void) {
    printf("\n========== TOP 5 MESES/ANIO CON MAS DINERO ==========\n");
    
    FILE *archivoPrestamos = fopen("prestamos.json", "r");
    if (archivoPrestamos == NULL) {
        printf("No hay prestamos registrados.\n");
        return;
    }
    
    fseek(archivoPrestamos, 0, SEEK_END);
    long tamano = ftell(archivoPrestamos);
    rewind(archivoPrestamos);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoPrestamos);
        printf("Error de memoria.\n");
        return;
    }
    
    fread(texto, sizeof(char), tamano, archivoPrestamos);
    texto[tamano] = '\0';
    fclose(archivoPrestamos);
    
    cJSON *arregloPrestamos = cJSON_Parse(texto);
    free(texto);
    
    if (arregloPrestamos == NULL || cJSON_GetArraySize(arregloPrestamos) == 0) {
        printf("No hay prestamos registrados.\n");
        cJSON_Delete(arregloPrestamos);
        return;
    }
    
    int totalPrestamos = cJSON_GetArraySize(arregloPrestamos);
    MesAnioConteo *conteos = malloc(totalPrestamos * sizeof(MesAnioConteo));
    int uniqueCount = 0;
    
    for (int i = 0; i < totalPrestamos; i++) {
        cJSON *prestamo = cJSON_GetArrayItem(arregloPrestamos, i);
        cJSON *multa = cJSON_GetObjectItemCaseSensitive(prestamo, "multa");
        cJSON *fechaInicio = cJSON_GetObjectItemCaseSensitive(prestamo, "fechaInicio");
        
        int mes = cJSON_GetArrayItem(fechaInicio, 1)->valueint;
        int anio = cJSON_GetArrayItem(fechaInicio, 2)->valueint;
        double monto = multa->valuedouble;
        
        int encontrado = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (conteos[j].mes == mes && conteos[j].anio == anio) {
                conteos[j].totalDinero += monto;
                encontrado = 1;
                break;
            }
        }
        
        if (!encontrado) {
            conteos[uniqueCount].mes = mes;
            conteos[uniqueCount].anio = anio;
            conteos[uniqueCount].totalDinero = monto;
            uniqueCount++;
        }
    }
    
    cJSON_Delete(arregloPrestamos);
    
    if (uniqueCount == 0) {
        printf("No hay registros de prestamos.\n");
        free(conteos);
        return;
    }
    
    ordenarMesesAnio(conteos, uniqueCount);
    
    printf("\n%-5s | %-15s | %s\n", "Pos", "Mes/Anio", "Total Multas");
    printf("------|-----------------|--------------\n");
    
    int max = (uniqueCount < 5) ? uniqueCount : 5;
    for (int i = 0; i < max; i++) {
        printf("%-5d | %02d/%-10d | $%.2f\n", i + 1, conteos[i].mes, conteos[i].anio, conteos[i].totalDinero);
    }
    
    printf("\n======================================================\n");
    
    free(conteos);
}

void estadisticasGeneros(void) {
    printf("\n========== ESTADISTICAS POR GENERO ==========\n");
    
    // Abrir catálogo para obtener géneros
    FILE *archivoCatalogo = fopen("catalogo.json", "r");
    if (archivoCatalogo == NULL) {
        printf("No hay catalogo disponible.\n");
        return;
    }
    
    fseek(archivoCatalogo, 0, SEEK_END);
    long tamano = ftell(archivoCatalogo);
    rewind(archivoCatalogo);
    
    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivoCatalogo);
        printf("Error de memoria.\n");
        return;
    }
    
    fread(texto, sizeof(char), tamano, archivoCatalogo);
    texto[tamano] = '\0';
    fclose(archivoCatalogo);
    
    cJSON *arregloCatalogo = cJSON_Parse(texto);
    free(texto);
    
    if (arregloCatalogo == NULL) {
        printf("Error al leer el catalogo.\n");
        return;
    }
    
    int totalCatalogo = cJSON_GetArraySize(arregloCatalogo);
    char **producciones = malloc(totalCatalogo * sizeof(char *));
    char **generos = malloc(totalCatalogo * sizeof(char *));
    
    for (int i = 0; i < totalCatalogo; i++) {
        cJSON *obj = cJSON_GetArrayItem(arregloCatalogo, i);
        cJSON *nombre = cJSON_GetObjectItemCaseSensitive(obj, "nombre");
        cJSON *genero = cJSON_GetObjectItemCaseSensitive(obj, "genero");
        
        producciones[i] = strdup(nombre->valuestring);
        generos[i] = strdup(genero->valuestring);
    }
    
    cJSON_Delete(arregloCatalogo);
    
    // Abrir préstamos
    FILE *archivoPrestamos = fopen("prestamos.json", "r");
    if (archivoPrestamos == NULL) {
        printf("No hay prestamos registrados.\n");
        for (int i = 0; i < totalCatalogo; i++) {
            free(producciones[i]);
            free(generos[i]);
        }
        free(producciones);
        free(generos);
        return;
    }
    
    fseek(archivoPrestamos, 0, SEEK_END);
    tamano = ftell(archivoPrestamos);
    rewind(archivoPrestamos);
    
    texto = malloc((tamano + 1) * sizeof(char));
    fread(texto, sizeof(char), tamano, archivoPrestamos);
    texto[tamano] = '\0';
    fclose(archivoPrestamos);
    
    cJSON *arregloPrestamos = cJSON_Parse(texto);
    free(texto);
    
    if (arregloPrestamos == NULL || cJSON_GetArraySize(arregloPrestamos) == 0) {
        printf("No hay prestamos registrados.\n");
        cJSON_Delete(arregloPrestamos);
        for (int i = 0; i < totalCatalogo; i++) {
            free(producciones[i]);
            free(generos[i]);
        }
        free(producciones);
        free(generos);
        return;
    }
    
    GeneroConteo *conteos = malloc(totalCatalogo * sizeof(GeneroConteo));
    int uniqueCount = 0;
    
    int totalPrestamos = cJSON_GetArraySize(arregloPrestamos);
    for (int i = 0; i < totalPrestamos; i++) {
        cJSON *prestamo = cJSON_GetArrayItem(arregloPrestamos, i);
        cJSON *ejemplares = cJSON_GetObjectItemCaseSensitive(prestamo, "ejemplares");
        
        int numEjemplares = cJSON_GetArraySize(ejemplares);
        for (int j = 0; j < numEjemplares; j++) {
            cJSON *ejemplar = cJSON_GetArrayItem(ejemplares, j);
            cJSON *produccion = cJSON_GetObjectItemCaseSensitive(ejemplar, "produccion");
            
            // Buscar el género de esta producción
            for (int k = 0; k < totalCatalogo; k++) {
                if (strcmp(producciones[k], produccion->valuestring) == 0) {
                    // Verificar si ya existe este género
                    int encontrado = 0;
                    for (int m = 0; m < uniqueCount; m++) {
                        if (strcmp(conteos[m].genero, generos[k]) == 0) {
                            conteos[m].cantidadPrestamos++;
                            encontrado = 1;
                            break;
                        }
                    }
                    
                    if (!encontrado) {
                        conteos[uniqueCount].genero = strdup(generos[k]);
                        conteos[uniqueCount].cantidadPrestamos = 1;
                        uniqueCount++;
                    }
                    break;
                }
            }
        }
    }
    
    cJSON_Delete(arregloPrestamos);
    
    for (int i = 0; i < totalCatalogo; i++) {
        free(producciones[i]);
        free(generos[i]);
    }
    free(producciones);
    free(generos);
    
    if (uniqueCount == 0) {
        printf("No hay registros de prestamos.\n");
        free(conteos);
        return;
    }
    
    ordenarGeneros(conteos, uniqueCount);
    
    printf("\n%-5s | %-25s | %s\n", "Pos", "Genero", "Prestamos");
    printf("------|---------------------------|----------\n");
    
    for (int i = 0; i < uniqueCount; i++) {
        printf("%-5d | %-25s | %d\n", i + 1, conteos[i].genero, conteos[i].cantidadPrestamos);
    }
    
    printf("\n-------------------------------------------\n");
    printf("GENERO CON MAS PRESTAMOS: %s (%d)\n", conteos[0].genero, conteos[0].cantidadPrestamos);
    printf("GENERO CON MENOS PRESTAMOS: %s (%d)\n", conteos[uniqueCount - 1].genero, conteos[uniqueCount - 1].cantidadPrestamos);
    printf("===========================================\n");
    
    // Liberar memoria
    for (int i = 0; i < uniqueCount; i++) {
        free(conteos[i].genero);
    }
    free(conteos);
}


void menuEstadisticas(void) {
    int opcion;
    int continuar = 1;
    
    do {
        printf("\n========== MENU DE ESTADISTICAS ==========\n");
        printf("1. Top 3 producciones mas prestadas\n");
        printf("2. Usuario con mas prestamos\n");
        printf("3. Top 5 meses/anio con mas dinero\n");
        printf("4. Estadisticas por genero\n");
        printf("5. Volver al menu principal\n");
        printf("Seleccione una opcion: ");
        
        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = 0;
        } else {
            limpiarBuffer();
        }
        
        switch (opcion) {
            case 1:
                estadisticasProduccionesMasPrestadas();
                break;
            case 2:
                estadisticasUsuarioMasPrestamos();
                break;
            case 3:
                estadisticasMesesAnioMasDinero();
                break;
            case 4:
                estadisticasGeneros();
                break;
            case 5:
                printf("Volviendo al menu principal...\n");
                continuar = 0;
                break;
            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }
    } while (continuar);
}