#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "cJSON.h"
#include "prestamosDevoluciones.h"

#define ARCHIVO_PRESTAMOS "prestamos.json"
#define MULTA_POR_DIA 1000.0
#define DIAS_PROXIMO_VENCIMIENTO 5



typedef struct {
    char *nombreProduccion;
    int cantidadDisponible;
} ProduccionDisponible;



static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static char *trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static int compararSinMayusculas(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 1;
        }
        a++;
        b++;
    }
    return *a != *b;
}

static int compararFechas(Fecha f1, Fecha f2) {
    if (f1.anio != f2.anio) return f1.anio - f2.anio;
    if (f1.mes != f2.mes) return f1.mes - f2.mes;
    return f1.dia - f2.dia;
}

static int diferenciaEntreDias(Fecha f1, Fecha f2) {
    int dias1 = f1.anio * 365 + f1.mes * 30 + f1.dia;
    int dias2 = f2.anio * 365 + f2.mes * 30 + f2.dia;
    return dias2 - dias1;
}

static Fecha obtenerFechaActual(void) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    Fecha hoy;
    hoy.dia = tm_info->tm_mday;
    hoy.mes = tm_info->tm_mon + 1;
    hoy.anio = tm_info->tm_year + 1900;
    return hoy;
}


Usuario *buscarUsuarioPorID(ListaUsuarios *lista, int id) {
    if (lista == NULL) return NULL;
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->usuarios[i].id == id) {
            return &lista->usuarios[i];
        }
    }
    return NULL;
}

static Usuario *buscarUsuarioPorNombre(ListaUsuarios *lista, const char *nombre) {
    if (lista == NULL || nombre == NULL) return NULL;
    for (int i = 0; i < lista->cantidad; i++) {
        if (strstr(lista->usuarios[i].nombre, nombre) != NULL) {
            return &lista->usuarios[i];
        }
    }
    return NULL;
}

static Ejemplar *buscarEjemplarPorID(SistemaPrestamos *sistema, int id) {
    if (sistema == NULL || sistema->ejemplares == NULL) return NULL;
    for (int i = 0; i < sistema->cantidadEjemplares; i++) {
        if (sistema->ejemplares[i].id == id) {
            return &sistema->ejemplares[i];
        }
    }
    return NULL;
}

static ProduccionDisponible *buscarProduccionPorNombre(
    ProduccionDisponible *producciones,
    int cantidad,
    const char *nombreBuscado
) {
    if (producciones == NULL || nombreBuscado == NULL) return NULL;

    char nombreLimpio[200];
    strncpy(nombreLimpio, nombreBuscado, sizeof(nombreLimpio) - 1);
    nombreLimpio[sizeof(nombreLimpio) - 1] = '\0';
    char *nombreTrim = trim(nombreLimpio);

    for (int i = 0; i < cantidad; i++) {
        if (strstr(producciones[i].nombreProduccion, nombreTrim) != NULL) {
            return &producciones[i];
        }
        if (strstr(nombreTrim, producciones[i].nombreProduccion) != NULL) {
            return &producciones[i];
        }
    }

    return NULL;
}


void inicializarSistemaPrestamos(SistemaPrestamos *sistema) {
    if (sistema == NULL) return;
    sistema->prestamos = NULL;
    sistema->cantidadPrestamos = 0;
    sistema->capacidadPrestamos = 0;
    sistema->totalPrestamos = 0;
    sistema->siguienteIDPrestamo = 1;
    sistema->usuarios = NULL;
    sistema->ejemplares = NULL;
    sistema->cantidadEjemplares = 0;
}

void liberarPrestamo(Prestamo *p) {
    if (p == NULL) return;
    for (int i = 0; i < p->cantidadEjemplares; i++) {
        free(p->nombreProducciones[i]);
    }
    free(p->nombreProducciones);
    free(p->idEjemplares);
    free(p);
}

void liberarSistemaPrestamos(SistemaPrestamos *sistema) {
    if (sistema == NULL) return;
    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        liberarPrestamo(sistema->prestamos[i]);
    }
    free(sistema->prestamos);
}


int crearArchivoPrestamos(void) {
    FILE *archivo = fopen(ARCHIVO_PRESTAMOS, "ab");
    if (archivo == NULL) return 0;
    fclose(archivo);
    return 1;
}

int guardarPrestamosJSON(SistemaPrestamos *sistema) {
    if (sistema == NULL) return 0;

    cJSON *arreglo = cJSON_CreateArray();
    if (arreglo == NULL) return 0;

    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        Prestamo *p = sistema->prestamos[i];
        cJSON *obj = cJSON_CreateObject();

        cJSON_AddNumberToObject(obj, "idPrestamo", p->idPrestamo);
        cJSON_AddNumberToObject(obj, "idUsuario", p->idUsuario);
        cJSON_AddNumberToObject(obj, "estado", p->estado);
        cJSON_AddNumberToObject(obj, "entregaTardia", p->entregaTardia);
        cJSON_AddNumberToObject(obj, "multa", p->multa);

        cJSON *fInicio = cJSON_CreateObject();
        cJSON_AddNumberToObject(fInicio, "dia", p->fechaInicio.dia);
        cJSON_AddNumberToObject(fInicio, "mes", p->fechaInicio.mes);
        cJSON_AddNumberToObject(fInicio, "anio", p->fechaInicio.anio);
        cJSON_AddItemToObject(obj, "fechaInicio", fInicio);

        cJSON *fFin = cJSON_CreateObject();
        cJSON_AddNumberToObject(fFin, "dia", p->fechaFin.dia);
        cJSON_AddNumberToObject(fFin, "mes", p->fechaFin.mes);
        cJSON_AddNumberToObject(fFin, "anio", p->fechaFin.anio);
        cJSON_AddItemToObject(obj, "fechaFin", fFin);

        cJSON *fDev = cJSON_CreateObject();
        cJSON_AddNumberToObject(fDev, "dia", p->devolucion.dia);
        cJSON_AddNumberToObject(fDev, "mes", p->devolucion.mes);
        cJSON_AddNumberToObject(fDev, "anio", p->devolucion.anio);
        cJSON_AddItemToObject(obj, "devolucion", fDev);

        cJSON *arrEjemplares = cJSON_CreateArray();
        for (int j = 0; j < p->cantidadEjemplares; j++) {
            cJSON *ej = cJSON_CreateObject();
            cJSON_AddNumberToObject(ej, "id", p->idEjemplares[j]);
            cJSON_AddStringToObject(ej, "produccion", p->nombreProducciones[j]);
            cJSON_AddItemToArray(arrEjemplares, ej);
        }
        cJSON_AddItemToObject(obj, "ejemplares", arrEjemplares);
        cJSON_AddItemToArray(arreglo, obj);
    }

    char *textoJSON = cJSON_Print(arreglo);
    cJSON_Delete(arreglo);

    if (textoJSON == NULL) return 0;

    FILE *archivo = fopen(ARCHIVO_PRESTAMOS, "w");
    if (archivo == NULL) {
        cJSON_free(textoJSON);
        return 0;
    }

    fputs(textoJSON, archivo);
    fclose(archivo);
    cJSON_free(textoJSON);
    return 1;
}

static int cargarPrestamosJSON(SistemaPrestamos *sistema) {
    FILE *archivo = fopen(ARCHIVO_PRESTAMOS, "r");
    if (archivo == NULL) return 1;

    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    rewind(archivo);

    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) { fclose(archivo); return 0; }

    fread(texto, sizeof(char), tamano, archivo);
    texto[tamano] = '\0';
    fclose(archivo);

    cJSON *arreglo = cJSON_Parse(texto);
    free(texto);
    if (arreglo == NULL) return 0;

    int total = cJSON_GetArraySize(arreglo);
    sistema->prestamos = malloc(total * sizeof(Prestamo *));
    sistema->capacidadPrestamos = total;
    sistema->cantidadPrestamos = 0;
    sistema->totalPrestamos = total;
    sistema->siguienteIDPrestamo = 1;

    for (int i = 0; i < total; i++) {
        cJSON *obj = cJSON_GetArrayItem(arreglo, i);
        Prestamo *p = malloc(sizeof(Prestamo));

        p->idPrestamo = cJSON_GetObjectItemCaseSensitive(obj, "idPrestamo")->valueint;
        p->idUsuario = cJSON_GetObjectItemCaseSensitive(obj, "idUsuario")->valueint;
        p->estado = cJSON_GetObjectItemCaseSensitive(obj, "estado")->valueint;
        p->entregaTardia = cJSON_GetObjectItemCaseSensitive(obj, "entregaTardia")->valueint;
        p->multa = cJSON_GetObjectItemCaseSensitive(obj, "multa")->valuedouble;

        cJSON *fInicio = cJSON_GetObjectItemCaseSensitive(obj, "fechaInicio");
        p->fechaInicio.dia = cJSON_GetArrayItem(fInicio, 0)->valueint;
        p->fechaInicio.mes = cJSON_GetArrayItem(fInicio, 1)->valueint;
        p->fechaInicio.anio = cJSON_GetArrayItem(fInicio, 2)->valueint;

        cJSON *fFin = cJSON_GetObjectItemCaseSensitive(obj, "fechaFin");
        p->fechaFin.dia = cJSON_GetArrayItem(fFin, 0)->valueint;
        p->fechaFin.mes = cJSON_GetArrayItem(fFin, 1)->valueint;
        p->fechaFin.anio = cJSON_GetArrayItem(fFin, 2)->valueint;

        cJSON *fDev = cJSON_GetObjectItemCaseSensitive(obj, "devolucion");
        p->devolucion.dia = cJSON_GetArrayItem(fDev, 0)->valueint;
        p->devolucion.mes = cJSON_GetArrayItem(fDev, 1)->valueint;
        p->devolucion.anio = cJSON_GetArrayItem(fDev, 2)->valueint;

        cJSON *arrEj = cJSON_GetObjectItemCaseSensitive(obj, "ejemplares");
        p->cantidadEjemplares = cJSON_GetArraySize(arrEj);
        p->idEjemplares = malloc(p->cantidadEjemplares * sizeof(int));
        p->nombreProducciones = malloc(p->cantidadEjemplares * sizeof(char *));

        for (int j = 0; j < p->cantidadEjemplares; j++) {
            cJSON *ej = cJSON_GetArrayItem(arrEj, j);
            p->idEjemplares[j] = cJSON_GetObjectItemCaseSensitive(ej, "id")->valueint;
            const char *prodStr = cJSON_GetObjectItemCaseSensitive(ej, "produccion")->valuestring;
            p->nombreProducciones[j] = malloc(strlen(prodStr) + 1);
            strcpy(p->nombreProducciones[j], prodStr);
        }

        if (p->idPrestamo >= sistema->siguienteIDPrestamo) {
            sistema->siguienteIDPrestamo = p->idPrestamo + 1;
        }

        sistema->prestamos[sistema->cantidadPrestamos++] = p;
    }

    cJSON_Delete(arreglo);
    return 1;
}


void mostrarTodosLosPrestamos(void) {
    FILE *archivo = fopen(ARCHIVO_PRESTAMOS, "r");
    if (archivo == NULL) {
        printf("No hay prestamos registrados.\n");
        return;
    }

    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    rewind(archivo);

    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivo);
        printf("Error de memoria.\n");
        return;
    }

    fread(texto, sizeof(char), tamano, archivo);
    texto[tamano] = '\0';
    fclose(archivo);

    cJSON *arreglo = cJSON_Parse(texto);
    free(texto);

    if (arreglo == NULL) {
        printf("No hay prestamos registrados.\n");
        return;
    }

    int total = cJSON_GetArraySize(arreglo);
    if (total == 0) {
        printf("No hay prestamos registrados.\n");
        cJSON_Delete(arreglo);
        return;
    }

    printf("\n========== HISTORIAL DE PRESTAMOS ==========\n");
    printf("Total de prestamos: %d\n", total);

    for (int i = 0; i < total; i++) {
        cJSON *obj = cJSON_GetArrayItem(arreglo, i);

        cJSON *idPrestamo = cJSON_GetObjectItemCaseSensitive(obj, "idPrestamo");
        cJSON *idUsuario = cJSON_GetObjectItemCaseSensitive(obj, "idUsuario");
        cJSON *estado = cJSON_GetObjectItemCaseSensitive(obj, "estado");
        cJSON *fechaInicio = cJSON_GetObjectItemCaseSensitive(obj, "fechaInicio");
        cJSON *fechaFin = cJSON_GetObjectItemCaseSensitive(obj, "fechaFin");
        cJSON *devolucion = cJSON_GetObjectItemCaseSensitive(obj, "devolucion");
        cJSON *ejemplares = cJSON_GetObjectItemCaseSensitive(obj, "ejemplares");

        const char *estadoStr = "Desconocido";
        if (estado->valueint == ACTIVO) estadoStr = "ACTIVO";
        else if (estado->valueint == VENCIDO) estadoStr = "VENCIDO";
        else if (estado->valueint == FINALIZADO) estadoStr = "FINALIZADO";

        printf("\n----------------------------------------\n");
        printf("Prestamo #%d\n", idPrestamo->valueint);
        printf("  Usuario ID: %d\n", idUsuario->valueint);
        printf("  Estado: %s\n", estadoStr);
        printf("  Fecha Inicio: %02d/%02d/%d\n", 
               cJSON_GetArrayItem(fechaInicio, 0)->valueint,
               cJSON_GetArrayItem(fechaInicio, 1)->valueint,
               cJSON_GetArrayItem(fechaInicio, 2)->valueint);
        printf("  Fecha Fin: %02d/%02d/%d\n",
               cJSON_GetArrayItem(fechaFin, 0)->valueint,
               cJSON_GetArrayItem(fechaFin, 1)->valueint,
               cJSON_GetArrayItem(fechaFin, 2)->valueint);

        if (devolucion != NULL && cJSON_GetArrayItem(devolucion, 2)->valueint != 0) {
            printf("  Fecha Devolucion: %02d/%02d/%d\n",
                   cJSON_GetArrayItem(devolucion, 0)->valueint,
                   cJSON_GetArrayItem(devolucion, 1)->valueint,
                   cJSON_GetArrayItem(devolucion, 2)->valueint);
        }

        printf("  Ejemplares:\n");
        int numEjemplares = cJSON_GetArraySize(ejemplares);
        for (int j = 0; j < numEjemplares; j++) {
            cJSON *ejemplar = cJSON_GetArrayItem(ejemplares, j);
            cJSON *nombreProd = cJSON_GetObjectItemCaseSensitive(ejemplar, "produccion");
            printf("    - %s\n", nombreProd->valuestring);
        }
    }

    printf("\n============================================\n");

    cJSON_Delete(arreglo);
}


int realizarPrestamoConProducciones(
    SistemaPrestamos *sistema,
    const char *nombreUsuario,
    Fecha fechaInicio,
    Fecha fechaFin,
    char **nombresProducciones,
    int cantidad,
    ProduccionDisponible *producciones,
    int cantidadProducciones
) {
    if (sistema == NULL || nombreUsuario == NULL || nombresProducciones == NULL || cantidad <= 0) {
        printf("Error: Datos invalidos para realizar el prestamo.\n");
        return 0;
    }

    if (compararFechas(fechaInicio, fechaFin) > 0) {
        printf("Error: La fecha de inicio no puede ser posterior a la fecha final.\n");
        return 0;
    }

    Usuario *usuario = buscarUsuarioPorNombre(sistema->usuarios, nombreUsuario);
    if (usuario == NULL) {
        printf("ERROR: Usuario con nombre '%s' NO ENCONTRADO.\n", nombreUsuario);
        return 0;
    }

    printf("Usuario encontrado: %s (ID: %d)\n", usuario->nombre, usuario->id);

    Prestamo *nuevo = malloc(sizeof(Prestamo));
    if (nuevo == NULL) {
        printf("Error de memoria al crear el prestamo.\n");
        return 0;
    }

    nuevo->idPrestamo = sistema->siguienteIDPrestamo++;
    nuevo->idUsuario = usuario->id;
    nuevo->fechaInicio = fechaInicio;
    nuevo->fechaFin = fechaFin;
    nuevo->devolucion = (Fecha){0, 0, 0};
    nuevo->estado = ACTIVO;
    nuevo->entregaTardia = 0;
    nuevo->multa = 0.0;
    nuevo->cantidadEjemplares = cantidad;

    nuevo->idEjemplares = malloc(cantidad * sizeof(int));
    nuevo->nombreProducciones = malloc(cantidad * sizeof(char *));

    for (int i = 0; i < cantidad; i++) {
        ProduccionDisponible *prod = buscarProduccionPorNombre(
            producciones, cantidadProducciones, nombresProducciones[i]
        );

        if (prod == NULL) {
            printf("ERROR: Produccion '%s' NO ENCONTRADA en el catalogo.\n", nombresProducciones[i]);
            for (int j = 0; j < i; j++) {
                free(nuevo->nombreProducciones[j]);
            }
            free(nuevo->nombreProducciones);
            free(nuevo->idEjemplares);
            free(nuevo);
            return 0;
        }

        if (prod->cantidadDisponible <= 0) {
            printf("ERROR: No hay ejemplares disponibles de '%s'.\n", prod->nombreProduccion);
            for (int j = 0; j < i; j++) {
                free(nuevo->nombreProducciones[j]);
            }
            free(nuevo->nombreProducciones);
            free(nuevo->idEjemplares);
            free(nuevo);
            return 0;
        }

        nuevo->idEjemplares[i] = i + 1;
        nuevo->nombreProducciones[i] = malloc(strlen(prod->nombreProduccion) + 1);
        strcpy(nuevo->nombreProducciones[i], prod->nombreProduccion);

        printf("Ejemplar: %s (Disponibles: %d)\n", prod->nombreProduccion, prod->cantidadDisponible);
    }

    if (sistema->cantidadPrestamos >= sistema->capacidadPrestamos) {
        int nuevaCap = sistema->capacidadPrestamos == 0 ? 5 : sistema->capacidadPrestamos * 2;
        sistema->prestamos = realloc(sistema->prestamos, nuevaCap * sizeof(Prestamo *));
        sistema->capacidadPrestamos = nuevaCap;
    }

    sistema->prestamos[sistema->cantidadPrestamos++] = nuevo;
    sistema->totalPrestamos++;
    guardarPrestamosJSON(sistema);

    printf("\n===== COMPROBANTE DE PRESTAMO =====\n");
    printf("ID Prestamo: %d\n", nuevo->idPrestamo);
    printf("Usuario: %s (ID: %d)\n", usuario->nombre, usuario->id);
    printf("Fecha Inicio: %02d/%02d/%d | Fecha Fin: %02d/%02d/%d\n",
           fechaInicio.dia, fechaInicio.mes, fechaInicio.anio,
           fechaFin.dia, fechaFin.mes, fechaFin.anio);
    printf("Ejemplares:\n");
    for (int i = 0; i < cantidad; i++) {
        printf("  - Produccion: %s\n", nuevo->nombreProducciones[i]);
    }
    printf("=====================================\n");

    return nuevo->idPrestamo;
}


void devolverPrestamo(SistemaPrestamos *sistema, int idPrestamo, Fecha fechaDevolucion) {
    Prestamo *p = NULL;
    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        if (sistema->prestamos[i]->idPrestamo == idPrestamo) {
            p = sistema->prestamos[i];
            break;
        }
    }

    if (p == NULL) {
        printf("Error: Prestamo con ID %d no encontrado.\n", idPrestamo);
        return;
    }

    if (p->estado == FINALIZADO) {
        printf("Este prestamo ya fue devuelto anteriormente.\n");
        return;
    }

    int diasRetraso = diferenciaEntreDias(p->fechaFin, fechaDevolucion);
    p->devolucion = fechaDevolucion;
    p->estado = FINALIZADO;

    if (diasRetraso > 0) {
        p->entregaTardia = 1;
        p->multa = diasRetraso * MULTA_POR_DIA;
        printf("\n[ALERTA] Entrega tardia de %d dias.\n", diasRetraso);
        printf("Multa aplicada: $%.2f\n", p->multa);
    } else {
        p->entregaTardia = 0;
        p->multa = 0.0;
        printf("\nDevolucion a tiempo. Sin multa.\n");
    }

    guardarPrestamosJSON(sistema);

    Usuario *u = buscarUsuarioPorID(sistema->usuarios, p->idUsuario);
    printf("\n===== COMPROBANTE DE DEVOLUCION =====\n");
    printf("ID Prestamo: %d\n", p->idPrestamo);
    printf("Usuario: %s\n", u ? u->nombre : "Desconocido");
    printf("Fecha Devolucion: %02d/%02d/%d\n", fechaDevolucion.dia, fechaDevolucion.mes, fechaDevolucion.anio);
    printf("Estado: FINALIZADO\n");
    printf("=======================================\n");
}



void consultarHistorialPrestamos(SistemaPrestamos *sistema, Fecha desde, Fecha hasta) {
    printf("\n===== HISTORIAL DE PRESTAMOS (%02d/%02d/%d - %02d/%02d/%d) =====\n",
           desde.dia, desde.mes, desde.anio, hasta.dia, hasta.mes, hasta.anio);

    int encontrados = 0;
    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        Prestamo *p = sistema->prestamos[i];
        if (compararFechas(p->fechaInicio, desde) >= 0 && compararFechas(p->fechaInicio, hasta) <= 0) {
            encontrados++;
            Usuario *u = buscarUsuarioPorID(sistema->usuarios, p->idUsuario);
            const char *estadoStr = (p->estado == ACTIVO) ? "Activo" : (p->estado == VENCIDO ? "Vencido" : "Finalizado");

            printf("\n[#%d] Usuario: %s | Estado: %s\n", p->idPrestamo, u ? u->nombre : "N/A", estadoStr);
            for (int j = 0; j < p->cantidadEjemplares; j++) {
                printf("  -> Produccion: %s\n", p->nombreProducciones[j]);
            }
            if (p->estado == FINALIZADO && p->entregaTardia) {
                printf("  [!] ENTREGA TARDIA - Multa: $%.2f\n", p->multa);
            }
        }
    }
    if (encontrados == 0) printf("No se encontraron prestamos en ese rango.\n");
    printf("================================================================\n");
}

void consultarVencimientos(SistemaPrestamos *sistema, Fecha fechaActual) {
    printf("\n===== CONSULTA DE VENCIMIENTOS (Fecha actual: %02d/%02d/%d) =====\n",
           fechaActual.dia, fechaActual.mes, fechaActual.anio);

    int vencidos = 0, proximos = 0;
    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        Prestamo *p = sistema->prestamos[i];
        if (p->estado != ACTIVO) continue;

        int diasRestantes = diferenciaEntreDias(fechaActual, p->fechaFin);
        Usuario *u = buscarUsuarioPorID(sistema->usuarios, p->idUsuario);

        if (diasRestantes < 0) {
            vencidos++;
            printf("\n[VENCIDO] Prestamo #%d - Usuario: %s\n", p->idPrestamo, u ? u->nombre : "N/A");
            printf("  Vencio el: %02d/%02d/%d (Atraso: %d dias)\n", p->fechaFin.dia, p->fechaFin.mes, p->fechaFin.anio, -diasRestantes);
        } else if (diasRestantes <= DIAS_PROXIMO_VENCIMIENTO) {
            proximos++;
            printf("\n[PROXIMO A VENCER] Prestamo #%d - Usuario: %s\n", p->idPrestamo, u ? u->nombre : "N/A");
            printf("  Vence el: %02d/%02d/%d (Restan: %d dias)\n", p->fechaFin.dia, p->fechaFin.mes, p->fechaFin.anio, diasRestantes);
        }
    }
    printf("\nResumen: %d vencidos, %d proximos a vencer.\n", vencidos, proximos);
    printf("======================================================================\n");
}

void consultarPrestamosPorUsuario(SistemaPrestamos *sistema, int idUsuario) {
    Usuario *u = buscarUsuarioPorID(sistema->usuarios, idUsuario);
    if (u == NULL) {
        printf("Usuario con ID %d no encontrado.\n", idUsuario);
        return;
    }

    printf("\n===== PRESTAMOS DE: %s (ID: %d) =====\n", u->nombre, u->id);
    int count = 0;
    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        Prestamo *p = sistema->prestamos[i];
        if (p->idUsuario == idUsuario) {
            count++;
            const char *estadoStr = (p->estado == ACTIVO) ? "Activo" : (p->estado == VENCIDO ? "Vencido" : "Finalizado");
            printf("\n- Prestamo #%d | Estado: %s | Periodo: %02d/%02d/%d al %02d/%02d/%d\n",
                   p->idPrestamo, estadoStr,
                   p->fechaInicio.dia, p->fechaInicio.mes, p->fechaInicio.anio,
                   p->fechaFin.dia, p->fechaFin.mes, p->fechaFin.anio);
        }
    }
    if (count == 0) printf("No tiene prestamos registrados.\n");
    printf("========================================\n");
}

void generarReporteEstadisticas(SistemaPrestamos *sistema, Fecha desde, Fecha hasta) {
    int totalPrestamosRango = 0, totalTardios = 0;
    double multaTotal = 0.0;

    for (int i = 0; i < sistema->cantidadPrestamos; i++) {
        Prestamo *p = sistema->prestamos[i];
        if (compararFechas(p->fechaInicio, desde) >= 0 && compararFechas(p->fechaInicio, hasta) <= 0) {
            totalPrestamosRango++;
            if (p->entregaTardia) {
                totalTardios++;
                multaTotal += p->multa;
            }
        }
    }

    printf("\n===== REPORTE ESTADISTICO (%02d/%02d/%d - %02d/%02d/%d) =====\n",
           desde.dia, desde.mes, desde.anio, hasta.dia, hasta.mes, hasta.anio);
    printf("Total de prestamos en el periodo: %d\n", totalPrestamosRango);
    printf("Total de entregas tardias: %d\n", totalTardios);
    printf("Multa total recaudada: $%.2f\n", multaTotal);
    printf("==============================================================\n");
}



void menuPrestamos(void) {
    SistemaPrestamos sistema;
    inicializarSistemaPrestamos(&sistema);
    cargarPrestamosJSON(&sistema);

    ListaUsuarios listaUsuarios;
    listaUsuarios.usuarios = NULL;
    listaUsuarios.cantidad = 0;
    listaUsuarios.capacidad = 0;
    CargarUsuariosDesdeArchivo(&listaUsuarios, "usuarios.json");
    sistema.usuarios = &listaUsuarios;
    printf("Usuarios cargados: %d\n", listaUsuarios.cantidad);

    ProduccionDisponible *producciones = NULL;
    int cantidadProducciones = 0;

    FILE *archivoCat = fopen("catalogo.json", "r");
    if (archivoCat != NULL) {
        fseek(archivoCat, 0, SEEK_END);
        long tamano = ftell(archivoCat);
        rewind(archivoCat);

        char *texto = malloc((tamano + 1) * sizeof(char));
        if (texto != NULL) {
            fread(texto, sizeof(char), tamano, archivoCat);
            texto[tamano] = '\0';
            fclose(archivoCat);

            cJSON *arreglo = cJSON_Parse(texto);
            free(texto);

            if (arreglo != NULL) {
                cantidadProducciones = cJSON_GetArraySize(arreglo);
                producciones = malloc(cantidadProducciones * sizeof(ProduccionDisponible));

                for (int i = 0; i < cantidadProducciones; i++) {
                    cJSON *obj = cJSON_GetArrayItem(arreglo, i);
                    cJSON *nombre = cJSON_GetObjectItemCaseSensitive(obj, "nombre");
                    cJSON *cantidad_obj = cJSON_GetObjectItemCaseSensitive(obj, "cantidad");

                    if (nombre != NULL && cantidad_obj != NULL) {
                        const char *nombreProd = nombre->valuestring;
                        int cantProd = cantidad_obj->valueint;

                        producciones[i].nombreProduccion = malloc(strlen(nombreProd) + 1);
                        strcpy(producciones[i].nombreProduccion, nombreProd);
                        producciones[i].cantidadDisponible = cantProd;
                    }
                }

                printf("Producciones cargadas: %d\n", cantidadProducciones);
                cJSON_Delete(arreglo);
            }
        }
    } else {
        printf("ERROR: No se pudo abrir catalogo.json\n");
    }

    int opcion, continuar = 1;
    do {
        printf("\n----- MENU DE PRESTAMOS -----\n");
        printf("1. Realizar prestamo\n");
        printf("2. Consultar historial de prestamos\n");
        printf("3. Consultar vencimientos\n");
        printf("4. Consultar prestamos por usuario\n");
        printf("5. Volver al menu principal\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = 0;
        } else {
            limpiarBuffer();
        }

        switch (opcion) {
            case 1: {
                char nombreUsuario[100];
                int cantidad;
                Fecha fInicio, fFin;

                printf("Nombre del usuario: ");
                scanf("%[^\n]", nombreUsuario);
                limpiarBuffer();

                printf("Fecha inicio (dia mes anio): ");
                scanf("%d %d %d", &fInicio.dia, &fInicio.mes, &fInicio.anio);
                limpiarBuffer();

                printf("Fecha fin (dia mes anio): ");
                scanf("%d %d %d", &fFin.dia, &fFin.mes, &fFin.anio);
                limpiarBuffer();

                printf("Cantidad de ejemplares: ");
                if (scanf("%d", &cantidad) != 1) {
                    printf("ERROR: La cantidad debe ser un numero entero.\n");
                    limpiarBuffer();
                    break;
                }
                limpiarBuffer();

                char **nombresProducciones = malloc(cantidad * sizeof(char *));
                for (int i = 0; i < cantidad; i++) {
                    nombresProducciones[i] = malloc(100 * sizeof(char));
                    printf("Nombre de la produccion del ejemplar %d: ", i + 1);
                    scanf("%[^\n]", nombresProducciones[i]);
                    limpiarBuffer();
                }

                int disponible = 1;
                for (int i = 0; i < cantidad; i++) {
                    ProduccionDisponible *prod = buscarProduccionPorNombre(
                        producciones, cantidadProducciones, nombresProducciones[i]
                    );

                    if (prod == NULL) {
                        printf("ERROR: Produccion '%s' NO ENCONTRADA en el catalogo.\n",
                               nombresProducciones[i]);
                        disponible = 0;
                        break;
                    }

                    if (prod->cantidadDisponible <= 0) {
                        printf("ERROR: No hay ejemplares disponibles de '%s'.\n",
                               prod->nombreProduccion);
                        disponible = 0;
                        break;
                    }
                }

                if (disponible) {
                    int idNuevo = realizarPrestamoConProducciones(
                        &sistema, nombreUsuario, fInicio, fFin,
                        nombresProducciones, cantidad,
                        producciones, cantidadProducciones
                    );

                    if (idNuevo > 0) {
                        for (int i = 0; i < cantidad; i++) {
                            ProduccionDisponible *prod = buscarProduccionPorNombre(
                                producciones, cantidadProducciones, nombresProducciones[i]
                            );
                            if (prod != NULL) {
                                prod->cantidadDisponible--;
                            }
                        }
                        printf("Prestamo realizado con exito. ID: %d\n", idNuevo);
                    } else {
                        printf("No se pudo realizar el prestamo.\n");
                    }
                } else {
                    printf("No se pudo realizar el prestamo.\n");
                }

                for (int i = 0; i < cantidad; i++) {
                    free(nombresProducciones[i]);
                }
                free(nombresProducciones);
                break;
            }
            case 2: {
                Fecha desde, hasta;
                printf("Fecha desde (dia mes anio): ");
                scanf("%d %d %d", &desde.dia, &desde.mes, &desde.anio);
                limpiarBuffer();

                printf("Fecha hasta (dia mes anio): ");
                scanf("%d %d %d", &hasta.dia, &hasta.mes, &hasta.anio);
                limpiarBuffer();

                consultarHistorialPrestamos(&sistema, desde, hasta);
                break;
            }
            case 3: {
                consultarVencimientos(&sistema, obtenerFechaActual());
                break;
            }
            case 4: {
                char nombreUsuario[100];
                printf("Nombre del usuario: ");
                scanf("%[^\n]", nombreUsuario);
                limpiarBuffer();

                Usuario *u = buscarUsuarioPorNombre(sistema.usuarios, nombreUsuario);
                if (u != NULL) {
                    consultarPrestamosPorUsuario(&sistema, u->id);
                } else {
                    printf("ERROR: Usuario '%s' NO ENCONTRADO.\n", nombreUsuario);
                }
                break;
            }
            case 5:
                mainPrincipal();
                printf("Volviendo al menu principal...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
    } while (continuar);

    for (int i = 0; i < cantidadProducciones; i++) {
        free(producciones[i].nombreProduccion);
    }
    free(producciones);
    LiberarListaUsuarios(&listaUsuarios);
    liberarSistemaPrestamos(&sistema);
}


void menuDevoluciones(void) {
    SistemaPrestamos sistema;
    inicializarSistemaPrestamos(&sistema);
    cargarPrestamosJSON(&sistema);

    ListaUsuarios listaUsuarios;
    listaUsuarios.usuarios = NULL;
    listaUsuarios.cantidad = 0;
    listaUsuarios.capacidad = 0;
    CargarUsuariosDesdeArchivo(&listaUsuarios, "usuarios.json");
    sistema.usuarios = &listaUsuarios;

    int opcion, continuar = 1;
    do {
        printf("\n----- MENU DE DEVOLUCIONES -----\n");
        printf("1. Devolver prestamo\n");
        printf("2. Volver al menu principal\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = 0;
        } else {
            limpiarBuffer();
        }

        switch (opcion) {
            case 1: {
                int idPrestamo;
                Fecha fDevolucion;
                printf("ID del prestamo a devolver: ");
                scanf("%d", &idPrestamo);
                limpiarBuffer();

                printf("Fecha de devolucion (dia mes anio): ");
                scanf("%d %d %d", &fDevolucion.dia, &fDevolucion.mes, &fDevolucion.anio);
                limpiarBuffer();

                devolverPrestamo(&sistema, idPrestamo, fDevolucion);
                break;
            }
            case 2:
                mainPrincipal();
                printf("Volviendo al menu principal...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
    } while (continuar);

    LiberarListaUsuarios(&listaUsuarios);
    liberarSistemaPrestamos(&sistema);
}