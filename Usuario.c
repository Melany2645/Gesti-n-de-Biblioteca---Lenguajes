#include "Usuario.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void LimpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void CapturarDatosUsuario(char *nombre, char *direccion) {
    printf("\n--- CAPTURAR DATOS DEL USUARIO ---\n");
    
    printf("Nombre: ");
    scanf("%[^\n]", nombre);
    LimpiarBuffer();
    
    printf("Direccion: ");
    scanf("%[^\n]", direccion);
    LimpiarBuffer();
}

int ObtenerMaximoID(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) return 0;
    
    int maxID = 0;
    int idActual;
    char linea[300];
    
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        char *ptr = strstr(linea, "\"id\":");
        if (ptr != NULL) {
            if (sscanf(ptr, "\"id\": %d", &idActual) == 1) {
                if (idActual > maxID) maxID = idActual;
            }
        }
    }
    
    fclose(archivo);
    return maxID;
}

void CargarUsuariosDesdeArchivo(ListaUsuarios *lista, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) return;

    char linea[300];
    
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        int id;
        char nombreTemp[MAX_NOMBRE];
        char direccionTemp[MAX_DIRECCION];

        if (sscanf(linea, "{\"id\": %d, \"nombre\": \"%[^\"]\", \"direccion\": \"%[^\"]\"}", 
                   &id, nombreTemp, direccionTemp) == 3) {
            
            if (lista->cantidad >= lista->capacidad) {
                int nuevaCapacidad = (lista->capacidad == 0) ? 5 : lista->capacidad * 2;
                lista->usuarios = realloc(lista->usuarios, nuevaCapacidad * sizeof(Usuario));
                lista->capacidad = nuevaCapacidad;
            }

            int indice = lista->cantidad;
            lista->usuarios[indice].id = id;
            lista->usuarios[indice].nombre = malloc(strlen(nombreTemp) + 1);
            lista->usuarios[indice].direccion = malloc(strlen(direccionTemp) + 1);
            
            strcpy(lista->usuarios[indice].nombre, nombreTemp);
            strcpy(lista->usuarios[indice].direccion, direccionTemp);
            lista->cantidad++;
        }
    }
    fclose(archivo);
}

void GuardarUsuariosEnArchivo(ListaUsuarios *lista, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        printf("Error: No se pudo guardar el archivo.\n");
        return;
    }
    
    for (int i = 0; i < lista->cantidad; i++) {
        fprintf(archivo, "{\"id\": %d, \"nombre\": \"%s\", \"direccion\": \"%s\"}\n", 
                lista->usuarios[i].id, 
                lista->usuarios[i].nombre, 
                lista->usuarios[i].direccion);
    }
    
    fclose(archivo);
}

void AgregarUsuario(ListaUsuarios *lista, const char *nombreArchivo, 
                    const char *nombre, const char *direccion) {
    
    if (lista == NULL) return;

    int nuevoID = ObtenerMaximoID(nombreArchivo) + 1;

    if (lista->cantidad >= lista->capacidad) {
        int nuevaCapacidad = (lista->capacidad == 0) ? 5 : lista->capacidad * 2;
        Usuario *nuevoArreglo = realloc(lista->usuarios, nuevaCapacidad * sizeof(Usuario));
        
        if (nuevoArreglo == NULL) {
            printf("Error: No hay memoria suficiente.\n");
            return;
        }
        lista->usuarios = nuevoArreglo;
        lista->capacidad = nuevaCapacidad;
    }

    int indice = lista->cantidad;
    lista->usuarios[indice].id = nuevoID;
    
    lista->usuarios[indice].nombre = malloc(strlen(nombre) + 1);
    lista->usuarios[indice].direccion = malloc(strlen(direccion) + 1);

    if (lista->usuarios[indice].nombre == NULL || lista->usuarios[indice].direccion == NULL) {
        printf("Error de memoria.\n");
        return;
    }

    strcpy(lista->usuarios[indice].nombre, nombre);
    strcpy(lista->usuarios[indice].direccion, direccion);
    lista->cantidad++;

    GuardarUsuariosEnArchivo(lista, nombreArchivo);
    printf("\nUsuario agregado con ID: %d\n", nuevoID);
}

void VisualizarUsuarioPorID(ListaUsuarios *lista, int id) {
    if (lista == NULL) return;
    
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->usuarios[i].id == id) {
            printf("\n--- USUARIO ENCONTRADO ---\n");
            printf("ID: %d\n", lista->usuarios[i].id);
            printf("Nombre: %s\n", lista->usuarios[i].nombre);
            printf("Direccion: %s\n", lista->usuarios[i].direccion);
            printf("--------------------------\n");
            return;
        }
    }
    printf("Usuario con ID %d no encontrado.\n", id);
}

void ModificarUsuario(ListaUsuarios *lista, const char *nombreArchivo, int id) {
    if (lista == NULL) return;
    
    int indice = -1;
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->usuarios[i].id == id) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) {
        printf("Usuario con ID %d no encontrado.\n", id);
        return;
    }
    
    char nombre[MAX_NOMBRE];
    char direccion[MAX_DIRECCION];
    
    /* Inicializar a vacío para evitar basura */
    nombre[0] = '\0';
    direccion[0] = '\0';
    
    printf("\n--- MODIFICAR USUARIO ID: %d ---\n", id);
    printf("Nombre actual: %s\n", lista->usuarios[indice].nombre);
    printf("Nuevo nombre (Enter para mantener): ");
    
    /* Verificar si scanf leyó algo */
    if (scanf("%[^\n]", nombre) != 1) {
        nombre[0] = '\0';
    }
    LimpiarBuffer();
    
    printf("Direccion actual: %s\n", lista->usuarios[indice].direccion);
    printf("Nueva direccion (Enter para mantener): ");
    
    if (scanf("%[^\n]", direccion) != 1) {
        direccion[0] = '\0';
    }
    LimpiarBuffer();
    
    /* Solo actualizar si se ingresó algo nuevo */
    if (strlen(nombre) > 0) {
        free(lista->usuarios[indice].nombre);
        lista->usuarios[indice].nombre = malloc(strlen(nombre) + 1);
        if (lista->usuarios[indice].nombre != NULL) {
            strcpy(lista->usuarios[indice].nombre, nombre);
        }
    }
    
    if (strlen(direccion) > 0) {
        free(lista->usuarios[indice].direccion);
        lista->usuarios[indice].direccion = malloc(strlen(direccion) + 1);
        if (lista->usuarios[indice].direccion != NULL) {
            strcpy(lista->usuarios[indice].direccion, direccion);
        }
    }
    
    GuardarUsuariosEnArchivo(lista, nombreArchivo);
    printf("Usuario modificado exitosamente.\n");
}

void EliminarUsuario(ListaUsuarios *lista, const char *nombreArchivo, int id) {
    if (lista == NULL) return;
    
    int indice = -1;
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->usuarios[i].id == id) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) {
        printf("Usuario con ID %d no encontrado.\n", id);
        return;
    }
    
    char confirmacion;
    printf("Eliminar usuario ID %d? (s/n): ", id);
    scanf(" %c", &confirmacion);
    LimpiarBuffer();
    
    if (confirmacion == 's' || confirmacion == 'S') {
        free(lista->usuarios[indice].nombre);
        free(lista->usuarios[indice].direccion);
        
        lista->cantidad--;
        if (indice < lista->cantidad) {
            lista->usuarios[indice] = lista->usuarios[lista->cantidad];
        }
        
        GuardarUsuariosEnArchivo(lista, nombreArchivo);
        printf("Usuario eliminado exitosamente.\n");
    } else {
        printf("Operacion cancelada.\n");
    }
}

void LiberarListaUsuarios(ListaUsuarios *lista) {
    if (lista == NULL) return;
    for (int i = 0; i < lista->cantidad; i++) {
        free(lista->usuarios[i].nombre);
        free(lista->usuarios[i].direccion);
    }
    free(lista->usuarios);
    lista->cantidad = 0;
    lista->capacidad = 0;
}

void menuUsuario(void) {
    int opcion;
    int continuar = 1;
    
    ListaUsuarios lista;
    lista.usuarios = NULL;
    lista.cantidad = 0;
    lista.capacidad = 0;

    CargarUsuariosDesdeArchivo(&lista, ARCHIVO_USUARIOS);
    printf("Se cargaron %d usuarios desde el archivo.\n", lista.cantidad);

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
            LimpiarBuffer();
            
            switch (opcion) {
                case 1: {
                    char nombre[MAX_NOMBRE];
                    char direccion[MAX_DIRECCION];
                    CapturarDatosUsuario(nombre, direccion);
                    AgregarUsuario(&lista, ARCHIVO_USUARIOS, nombre, direccion);
                    break;
                }
                case 2: {
                    int id;
                    printf("Ingrese el ID del usuario: ");
                    scanf("%d", &id);
                    LimpiarBuffer();
                    VisualizarUsuarioPorID(&lista, id);
                    break;
                }
                case 3: {
                    printf("\n--- LISTA DE USUARIOS ---\n");
                    if (lista.cantidad == 0) {
                        printf("No hay usuarios registrados.\n");
                    } else {
                        for (int i = 0; i < lista.cantidad; i++) {
                            printf("ID: %-5d | Nombre: %-20s | Direccion: %s\n", 
                                   lista.usuarios[i].id, 
                                   lista.usuarios[i].nombre, 
                                   lista.usuarios[i].direccion);
                        }
                    }
                    break;
                }
                case 4: {
                    int id;
                    printf("Ingrese el ID del usuario a modificar: ");
                    scanf("%d", &id);
                    LimpiarBuffer();
                    ModificarUsuario(&lista, ARCHIVO_USUARIOS, id);
                    break;
                }
                case 5: {
                    int id;
                    printf("Ingrese el ID del usuario a eliminar: ");
                    scanf("%d", &id);
                    LimpiarBuffer();
                    EliminarUsuario(&lista, ARCHIVO_USUARIOS, id);
                    break;
                }
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
            LimpiarBuffer();
        }
    } while (continuar == 1);

    LiberarListaUsuarios(&lista);
}

/*
int main(void) {    
    menuUsuario();
    return 0;
}
    */