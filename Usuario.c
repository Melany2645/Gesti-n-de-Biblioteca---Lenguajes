#include "Usuario.h"
#include "menu.h"

// Manipulación del archivo de los usuarios

void LimpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//E: Arreglos donde se almacenaran el nombre y la direccion del usuario
//S: Los datos bien arreglados y en orden que fueron dados por el usuario
//R: Valores válidos
//F: Solicita al usuario su nombre y direccion
void CapturarDatosUsuario(char *nombre, char *direccion) {
    printf("\n--- CAPTURAR DATOS DEL USUARIO ---\n");
    
    printf("Nombre: ");
    scanf("%[^\n]", nombre);
    LimpiarBuffer();
    
    printf("Direccion: ");
    scanf("%[^\n]", direccion);
    LimpiarBuffer();
}

//E: Lista de usuarios
//S: El identificador mas grande encontrado
//R: La lista debe ser valida y contener usuarios correctamente almacenados
//F: Obtiene el ID maximo de los usuarios registrados
int ObtenerMaximoID(ListaUsuarios *lista) {
    if (lista == NULL) return 0;
    
    int maxID = 0;
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->usuarios[i].id > maxID) {
            maxID = lista->usuarios[i].id;
        }
    }
    
    return maxID;
}

//E: Lista de usuarios y nombre del archivo JSON
//S: La lista se llena con los usuarios validos del archivo
//R: El archivo debe existir y contener un arreglo JSON con id, nombre y direccion
//F: Carga los usuarios desde un archivo JSON
void CargarUsuariosDesdeArchivo(ListaUsuarios *lista, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) return;

    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    rewind(archivo);

    char *texto = malloc((tamano + 1) * sizeof(char));
    if (texto == NULL) {
        fclose(archivo);
        return;
    }

    fread(texto, sizeof(char), tamano, archivo);
    texto[tamano] = '\0';
    fclose(archivo);

    cJSON *arreglo = cJSON_Parse(texto);
    free(texto);

    if (arreglo == NULL) return;

    int total = cJSON_GetArraySize(arreglo);

    for (int i = 0; i < total; i++) {
        cJSON *objeto = cJSON_GetArrayItem(arreglo, i);
        
        cJSON *id_json = cJSON_GetObjectItemCaseSensitive(objeto, "id");
        cJSON *nombre_json = cJSON_GetObjectItemCaseSensitive(objeto, "nombre");
        cJSON *direccion_json = cJSON_GetObjectItemCaseSensitive(objeto, "direccion");

        if (!cJSON_IsNumber(id_json) || !cJSON_IsString(nombre_json) || !cJSON_IsString(direccion_json)) {
            continue;
        }

        if (lista->cantidad >= lista->capacidad) {
            int nuevaCapacidad = (lista->capacidad == 0) ? 5 : lista->capacidad * 2;
            Usuario *nuevoArreglo = realloc(lista->usuarios, nuevaCapacidad * sizeof(Usuario));
            
            if (nuevoArreglo == NULL) {
                cJSON_Delete(arreglo);
                return;
            }
            lista->usuarios = nuevoArreglo;
            lista->capacidad = nuevaCapacidad;
        }

        int indice = lista->cantidad;
        lista->usuarios[indice].id = id_json->valueint;
        
        lista->usuarios[indice].nombre = malloc(strlen(nombre_json->valuestring) + 1);
        lista->usuarios[indice].direccion = malloc(strlen(direccion_json->valuestring) + 1);

        if (lista->usuarios[indice].nombre == NULL || lista->usuarios[indice].direccion == NULL) {
            cJSON_Delete(arreglo);
            return;
        }

        strcpy(lista->usuarios[indice].nombre, nombre_json->valuestring);
        strcpy(lista->usuarios[indice].direccion, direccion_json->valuestring);
        lista->cantidad++;
    }

    cJSON_Delete(arreglo);
}

//E: Lista de usuarios y nombre del archivo JSON
//S: El archivo JSON queda actualizado con los usuarios de la lista
//R: La lista y el nombre del archivo deben ser validos
//F: Guarda todos los usuarios en formato JSON
void GuardarUsuariosEnArchivo(ListaUsuarios *lista, const char *nombreArchivo) {
    cJSON *arreglo = cJSON_CreateArray();
    if (arreglo == NULL) {
        printf("Error: No se pudo crear el arreglo JSON.\n");
        return;
    }

    for (int i = 0; i < lista->cantidad; i++) {
        cJSON *objeto = cJSON_CreateObject();
        if (objeto == NULL) {
            cJSON_Delete(arreglo);
            printf("Error: No se pudo crear el objeto JSON.\n");
            return;
        }

        cJSON_AddNumberToObject(objeto, "id", lista->usuarios[i].id);
        cJSON_AddStringToObject(objeto, "nombre", lista->usuarios[i].nombre);
        cJSON_AddStringToObject(objeto, "direccion", lista->usuarios[i].direccion);

        cJSON_AddItemToArray(arreglo, objeto);
    }

    char *textoJSON = cJSON_Print(arreglo);
    if (textoJSON == NULL) {
        cJSON_Delete(arreglo);
        printf("Error: No se pudo convertir el JSON a texto.\n");
        return;
    }

    FILE *archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        cJSON_free(textoJSON);
        cJSON_Delete(arreglo);
        printf("Error: No se pudo abrir el archivo para guardar.\n");
        return;
    }

    fprintf(archivo, "%s", textoJSON);
    fclose(archivo);

    cJSON_free(textoJSON);
    cJSON_Delete(arreglo);
}

//E: Lista de usuarios, archivo, nombre y direccion del nuevo usuario
//S: La lista y el archivo quedan actualizados con un nuevo usuario
//R: La lista, el nombre y la direccion deben ser validos
//F: Agrega un usuario y le asigna un ID consecutivo
void AgregarUsuario(ListaUsuarios *lista, const char *nombreArchivo, 
                    const char *nombre, const char *direccion) {
    
    if (lista == NULL) return;

    int nuevoID = ObtenerMaximoID(lista) + 1;

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

//E: Lista de usuarios e ID del usuario que se desea consultar
//S: Muestra los datos del usuario encontrado
//R: La lista debe ser valida y el ID debe ser de un usuario registrado
//F: Busca y visualiza un usuario por su ID
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

//E: Lista de usuarios, archivo, ID y nuevos datos opcionales del usuario
//S: La lista y el archivo quedan actualizados con los cambios 
//R: El usuario debe existir y los nuevos datos deben ser validos
//F: Modifica el nombre o la direccion de un usuario
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
    
    nombre[0] = '\0';
    direccion[0] = '\0';
    
    printf("\n--- MODIFICAR USUARIO ID: %d ---\n", id);
    printf("Nombre actual: %s\n", lista->usuarios[indice].nombre);
    printf("Nuevo nombre (Enter para mantener): ");
    
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

//E: Lista de usuarios, nombre del archivo y ID del usuario a eliminar
//S: El usuario es eliminado si no tiene préstamos asociados
//R: El usuario debe existir y no tener préstamos asociados
//F: Elimina un usuario verificando que no tenga registros relacionados en préstamos
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
    
    // VERIFICAR SI EL USUARIO TIENE PRÉSTAMOS ASOCIADOS
    FILE *archivoPrestamos = fopen("prestamos.json", "r");
    if (archivoPrestamos != NULL) {
        fseek(archivoPrestamos, 0, SEEK_END);
        long tamano = ftell(archivoPrestamos);
        rewind(archivoPrestamos);
        
        char *texto = malloc((tamano + 1) * sizeof(char));
        if (texto != NULL) {
            fread(texto, sizeof(char), tamano, archivoPrestamos);
            texto[tamano] = '\0';
            fclose(archivoPrestamos);
            
            cJSON *arreglo = cJSON_Parse(texto);
            free(texto);
            
            if (arreglo != NULL) {
                int totalPrestamos = cJSON_GetArraySize(arreglo);
                int tienePrestamos = 0;
                
                for (int i = 0; i < totalPrestamos; i++) {
                    cJSON *prestamo = cJSON_GetArrayItem(arreglo, i);
                    cJSON *idUsuario = cJSON_GetObjectItemCaseSensitive(prestamo, "idUsuario");
                    
                    if (idUsuario != NULL && idUsuario->valueint == id) {
                        tienePrestamos = 1;
                        break;
                    }
                }
                
                cJSON_Delete(arreglo);
                
                if (tienePrestamos) {
                    printf("\nERROR: No se puede eliminar el usuario ID %d.\n", id);
                    printf("El usuario tiene préstamos asociados en el sistema.\n");
                    printf("Primero debe eliminar o finalizar todos sus préstamos.\n");
                    return;
                }
            }
        }
    }
    
    // Si no tiene préstamos, se puede eliminar el usuario
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

//E: Lista de usuarios
//S: Se libera la memoria de los usuarios y la lista queda vacia
//R: La lista debe haber sido inicializada correctamente
//F: Libera la memoria utilizada por la lista de usuarios
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

//E: Ninguna
//S: Permite administrar usuarios mediante un menu 
//R: Requiere una entrada valida por teclado y el archivo de usuarios disponible
//F: Ejecuta el menu principal de gestion de usuarios
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
                    mainPrincipal();
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