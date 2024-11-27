#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CONGRESISTAS 200
#define MAX_SENADORES 50
#define MAX_DIPUTADOS 100
#define MAX_COMISIONES 10

struct congresista {
    char* nombre;
    char* rut;
    char* ocupacion;
    char* especializacion;
};

struct nodoCongresista {
    struct congresista* datos;
    struct nodoCongresista* sig;
};

struct congreso {
    struct congresista** senadores;
    struct congresista** diputados;
    struct nodoCongresista* congresistasMixtos;
    struct nodoComision* comisionesMixtas;
    struct comision** comisiones;
    struct nodoProyectoLey* raiz;
};

struct nodoProyectoLey {
    struct proyectoLey* datos;
    struct nodoProyectoLey* izq, * der;
};

struct proyectoLey {
    char* nombre;
    char* tipo;
    int idProyecto;
    int urgencia;
    struct nodoVotacion* votacion;
    int fase;
};

struct nodoComision {
    struct comision* datos;
    struct nodoComision* sig;
};

struct comision {
    struct nodoCongresista* headIntegrantes;
    char* nombre;
    char* tipo;
    char* descripcion;
};

struct nodoVotacion {
    struct votacion* datos;
    struct nodoVotacion* sig;
};

struct votacion {
    struct nodoCongresista* favor;
    struct nodoCongresista* contra;
    int fase;
};

// Function prototype
void convertirMinusculas(char* cadena);
int leerEnteroConLimite(char* mensaje, int min, int max);
struct congresista* buscarCongresistaPorRUT(struct congreso* congreso, char* rut);
char leerOpcion();
struct congreso* inicializarCongreso();
struct nodoProyectoLey* crearNodoProyectoLey(struct proyectoLey* datos);
void agregarNodoProyectoLey(struct congreso* congreso);
struct proyectoLey* buscarProyectoLeyPorID(struct nodoProyectoLey* raiz, int id);
struct nodoProyectoLey* minValorNodo(struct nodoProyectoLey* nodo);
struct nodoProyectoLey* borrarNodoProyectoLey(struct congreso* congreso, struct nodoProyectoLey* raiz, int id);
void borrarProyectoLey(struct congreso* congreso, int id);
void mostrarCongresistasVotacion(struct nodoCongresista* lista, const char* categoria);
void buscarYMostrarProyectoLey(struct congreso* congreso, int id);
void agregarCongresistaAVotacion(struct votacion* votacion, struct congreso* congreso);
void agregarVotacion(struct congreso* congreso, int idProyecto);
void imprimirProyectoLey(struct proyectoLey* proyecto);
void recorrerYImprimirProyectos(struct nodoProyectoLey* nodo);
void imprimirProyectosLey(struct congreso* congreso);
void agregarProyecto(struct proyectoLey* proyecto, struct proyectoLey*** proyectosArray, int* proyectosCount, int* proyectosCapacidad);
void recorrerArbolEnOrden(struct nodoProyectoLey* nodo, struct proyectoLey*** proyectosArray, int* proyectosCount, int* proyectosCapacidad);
int compararPorUrgencia(const void* a, const void* b);
void mostrarProyectosOrdenDeUrgencia(struct congreso* congreso);
void modificarProyectoLey(struct congreso* congreso, int idProyecto);
struct nodoCongresista* crearNodoCongresista(struct nodoCongresista* head, struct congresista* datos);
int comprobarCongresistaEnComision(struct nodoCongresista* head, char* rutBuscado);
struct congresista* comprobarCongresistaEnCongreso(struct congreso* congreso, char* rutBuscado);
struct congresista* crearCongresista(struct congreso* congreso);
void agregarCongresistaEnCongreso(struct congreso* congreso);
void agregarCongresistaEnComision(struct comision* comision, char* rut, struct congresista* congresista);
void eliminarCongresistaDeComision(struct comision* comision, char* rutQuitado);
void eliminarCongresistaDeCongreso(struct congreso* congreso, char* rutQuitado);
void modificarCongresista(struct congreso* congreso, char* rutBuscado);
void mostrarCongresista(struct congreso* congreso, char* rutBuscado);
void listarCongresistas(struct congreso* congreso);
struct nodoComision* crearNodoComision(struct comision* datos);
struct comision* buscarComision(struct congreso* congreso, char* nombre);
struct comision* crearComision(struct congreso* congreso);
void agregarComision(struct congreso* congreso);
void eliminarComision(struct congreso* congreso, char* nombre);
void modificarComision(struct congreso* congreso, char* nombre);
void mostrarComisionPorNombre(struct congreso* congreso, char* nombre);
void listarComisiones(struct congreso* congreso);
void funcionSwitch(char opcion, struct congreso* congreso, void (*submenu)(struct congreso*));
void menuProyectosLey(struct congreso* congreso);
void menuCongresistas(struct congreso* congreso);
void menuComisiones(struct congreso* congreso);
void mostrarLeyesPorFase(struct nodoProyectoLey* leyes, int faseRequerida);
/*NOTA: LAS FUNCIONES DE VOTACIÓN SE ENCUENTRAN EN EL APARTADO DE LAS FUNCIONES DE PROYECTO DE LEY.*/


/*TODO: FUNCIONES AUXILIARES---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void convertirMinusculas(char* cadena) {
    int i;
    for (i = 0; cadena[i]; i++) {
        if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
            cadena[i] += 'a' - 'A';
        }
    }
}


int leerEnteroConLimite(char* mensaje, int min, int max) {
    int valor;
    char input[10];
    while (1) {
        printf("%s (%d-%d): ", mensaje, min, max);
        scanf("%9s", input);
        valor = atoi(input);
        if (valor >= min && valor <= max) {
            break;
        }
        printf("Error: Valor inválido. Debe estar entre %d y %d.\n", min, max);
    }
    return valor;
}

// Función auxiliar para buscar un congresista por RUT en el congreso
struct congresista* buscarCongresistaPorRUT(struct congreso* congreso, char* rut) {
    int i;

    // Buscar en el arreglo de senadores
    for (i = 0; congreso->senadores[i] != 0; i++) {
        if (strcmp(congreso->senadores[i]->rut, rut) == 0) {
            return congreso->senadores[i];
        }
    }

    // Buscar en el arreglo de diputados
    for (i = 0; congreso->diputados[i] != 0; i++) {
        if (strcmp(congreso->diputados[i]->rut, rut) == 0) {
            return congreso->diputados[i];
        }
    }

    // Si no se encuentra el congresista
    return 0;
}

char leerOpcion() {
    char opcion;
    scanf("%c", &opcion);

    /* Convierte a mayúscula si es una letra minúscula */
    if (opcion >= 'a' && opcion <= 'z') {
        opcion -= 'a' - 'A';
    }

    return opcion;
}

//TODO: FUNCIÓN DE INICIALIZACIÓN DEL CONGRESO----------------------------------------------------------------------------------------------------------------------------------------------------//
//TODO: ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
/*Esta función almacena memoria para todos los datos que debe almacenar "congreso"*/
struct congreso* inicializarCongreso() {
    struct congreso* nuevoCongreso;
    nuevoCongreso = malloc(sizeof(struct congreso));

    if (nuevoCongreso == NULL) {
        return NULL;
    }

    // Inicializa los arreglos para los senadores y diputados
    nuevoCongreso->senadores = calloc(MAX_SENADORES, sizeof(struct congresista*));
    nuevoCongreso->diputados = calloc(MAX_DIPUTADOS, sizeof(struct congresista*));
    nuevoCongreso->congresistasMixtos = NULL;
    nuevoCongreso->comisionesMixtas = NULL;

    // Inicializa los arreglos para las comisiones
    nuevoCongreso->comisiones = calloc(MAX_COMISIONES, sizeof(struct comision*));

    // Inicializa la raíz de proyectos de ley
    nuevoCongreso->raiz = NULL;
    return nuevoCongreso;
}


//TODO: FUNCIÓNES DEL PROYECTO DE LEY----------------------------------------------------------------------------------------------------------------------------------------------------//
//TODO-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
struct nodoProyectoLey* crearNodoProyectoLey(struct proyectoLey* datos) {
    struct nodoProyectoLey* nodo = NULL;

    // Validar que los datos no sean NULL
    if (datos != NULL) {
        nodo = malloc(sizeof(struct nodoProyectoLey));

        if (nodo == NULL) {
            printf("Error: No se pudo asignar memoria para el nodo de proyecto de ley.\n");
            return NULL;
        }

        nodo->datos = datos; // Copiar los datos recibidos
        nodo->izq = NULL;
        nodo->der = NULL;
    }

    return nodo;
}

struct proyectoLey* crearProyectoLey(struct congreso* congreso) {

    struct proyectoLey* nuevoProyecto;
    char nombre[100], tipo[50];
    int idProyecto, urgencia, fase;

    nuevoProyecto = malloc(sizeof(struct proyectoLey));

    printf("Ingrese el nombre del proyecto de ley: ");
    if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
        printf("Error al leer el nombre del proyecto de ley.\n");
        return NULL;
    }
    else {
        nombre[strcspn(nombre, "\n")] = '\0';
    }

    printf("Ingrese el tipo del proyecto de ley: ");
    if (fgets(tipo, sizeof(tipo), stdin) == NULL) {
        printf("Error al leer el tipo del proyecto de ley.\n");
        return NULL;
    }
    else {
        tipo[strcspn(tipo, "\n")] = '\0';
    }

    // Validación y captura del ID con verificación de duplicados
    do {
        idProyecto = leerEnteroConLimite("Ingrese el ID del proyecto", 10000000, 99999999);
        if (buscarProyectoLeyPorID(congreso->raiz, idProyecto)) {
            printf("Error: El ID ya está en uso. Ingrese otro ID.\n");
        }
    } while (buscarProyectoLeyPorID(congreso->raiz, idProyecto));

    urgencia = leerEnteroConLimite("Ingrese la urgencia del proyecto", 1, 5);
    fase = leerEnteroConLimite("Ingrese la fase del proyecto", 1, 8);

    nuevoProyecto->nombre = malloc(strlen(nombre) + 1);
    nuevoProyecto->tipo = malloc(strlen(tipo) + 1);

    strcpy(nuevoProyecto->nombre, nombre);
    strcpy(nuevoProyecto->tipo, tipo);
    nuevoProyecto->idProyecto = idProyecto;
    nuevoProyecto->urgencia = urgencia;
    nuevoProyecto->fase = fase;
    nuevoProyecto->votacion = NULL;

    return nuevoProyecto;
}


void agregarNodoProyectoLey(struct congreso* congreso) {
    struct proyectoLey* datos;
    struct nodoProyectoLey* nuevoNodo, * actual, * padre;

    datos = crearProyectoLey(congreso); // Crear proyecto de ley
    nuevoNodo = crearNodoProyectoLey(datos); // Crear nodo para el proyecto de ley

    if (congreso->raiz == NULL) {
        congreso->raiz = nuevoNodo;
    }
    else
    {
        actual = congreso->raiz;
        padre = NULL;

        while (actual != NULL)
        {
            padre = actual;
            if (datos->idProyecto < actual->datos->idProyecto) {
                actual = actual->izq;
            }
            else {
                actual = actual->der;
            }
        }

        if (datos->idProyecto < padre->datos->idProyecto) {
            padre->izq = nuevoNodo;
        }
        else {
            padre->der = nuevoNodo;
        }
    }
}

// Función para buscar un proyecto de ley por ID
struct proyectoLey* buscarProyectoLeyPorID(struct nodoProyectoLey* raiz, int id) {
    if (raiz == NULL) {
        return NULL;
    }

    if (raiz->datos->idProyecto == id) {
        return raiz->datos;
    }
    else if (id < raiz->datos->idProyecto) {
        // Buscamos en el subárbol izquierdo si el ID es menor
        return buscarProyectoLeyPorID(raiz->izq, id);
    }
    else {
        // Buscamos en el subárbol derecho si el ID es mayor
        return buscarProyectoLeyPorID(raiz->der, id);
    }
}

// Encuentra el nodo mínimo en el árbol (usado para encontrar el sucesor en caso de eliminación)
struct nodoProyectoLey* minValorNodo(struct nodoProyectoLey* nodo) {
    struct nodoProyectoLey* actual;

    actual = nodo;

    /* Recorre el subárbol hacia la izquierda para encontrar el nodo mínimo */
    while (actual != NULL && actual->izq != NULL) {
        actual = actual->izq;
    }
    return actual;
}

// Función para borrar un nodo en el árbol binario de búsqueda
struct nodoProyectoLey* borrarNodoProyectoLey(struct congreso* congreso, struct nodoProyectoLey* raiz, int id) {
    struct nodoProyectoLey* temp;

    /* Caso base */
    if (raiz == NULL) return raiz;

    /* Si el id a eliminar es más pequeño que el id de la raíz, está en el subárbol izquierdo */
    if (id < raiz->datos->idProyecto) {
        raiz->izq = borrarNodoProyectoLey(congreso, raiz->izq, id);
    }
    /* Si el id a eliminar es mayor que el id de la raíz, está en el subárbol derecho */
    else if (id > raiz->datos->idProyecto) {
        raiz->der = borrarNodoProyectoLey(congreso, raiz->der, id);
    }
    /* Si el id es el mismo que el id de la raíz, este es el nodo a eliminar */
    else {
        /* Nodo con solo un hijo o sin hijos */
        if (raiz->izq == NULL) {
            temp = raiz->der;
            return temp;
        }
        else if (raiz->der == NULL) {
            temp = raiz->izq;
            return temp;
        }

        /* Nodo con dos hijos: obtener el sucesor en el orden (el más pequeño en el subárbol derecho) */
        temp = minValorNodo(raiz->der);

        /* Copiar el contenido del sucesor al nodo actual */
        raiz->datos->idProyecto = temp->datos->idProyecto;

        /* Asegurarse de que haya suficiente memoria para copiar los campos */
        raiz->datos->nombre = realloc(raiz->datos->nombre, strlen(temp->datos->nombre) + 1);
        raiz->datos->tipo = realloc(raiz->datos->tipo, strlen(temp->datos->tipo) + 1);
        if (raiz->datos->nombre != NULL && raiz->datos->tipo != NULL) {
            strcpy(raiz->datos->nombre, temp->datos->nombre);
            strcpy(raiz->datos->tipo, temp->datos->tipo);
        }
        raiz->datos->urgencia = temp->datos->urgencia;
        raiz->datos->fase = temp->datos->fase;

        /* Borrar el sucesor */
        raiz->der = borrarNodoProyectoLey(congreso, raiz->der, temp->datos->idProyecto);
    }

    return raiz;
}

// Función para borrar un proyecto de ley
void borrarProyectoLey(struct congreso* congreso, int id) {
    congreso->raiz = borrarNodoProyectoLey(congreso, congreso->raiz, id);
}

// Función auxiliar para contar y mostrar los congresistas en una lista de votación
void mostrarCongresistasVotacion(struct nodoCongresista* lista, const char* categoria) {
    int contador = 0;
    struct nodoCongresista* actual;

    actual = lista;

    printf("Votos %s:\n", categoria);
    while (actual != NULL) {
        printf("- %s (RUT: %s)\n", actual->datos->nombre, actual->datos->rut);
        contador++;
        actual = actual->sig;
    }
    printf("Total de votos %s: %d\n", categoria, contador);
}

// Función para buscar y mostrar un proyecto de ley por ID en el árbol binario de búsqueda
void buscarYMostrarProyectoLey(struct congreso* congreso, int id) {
    struct proyectoLey* proyecto;
    struct nodoVotacion* nodoVot;

    proyecto = buscarProyectoLeyPorID(congreso->raiz, id);

    if (proyecto != NULL) {
        printf("Nombre: %s\n", proyecto->nombre);
        printf("Tipo: %s\n", proyecto->tipo);
        printf("ID Proyecto: %d\n", proyecto->idProyecto);
        printf("Urgencia: %d\n", proyecto->urgencia);
        printf("Fase: %d\n", proyecto->fase);

        // Descripción de la fase dependiendo del caso
        switch (proyecto->fase) {
        case 1:
            printf("Descripción de la fase: Iniciativa Legislativa\n");
            break;
        case 2:
            printf("Descripción de la fase: Cámara de Origen\n");
            break;
        case 3:
            printf("Descripción de la fase: Cámara Revisora\n");
            break;
        case 4:
            printf("Descripción de la fase: Comisión Mixta\n");
            break;
        case 5:
            printf("Descripción de la fase: Promulgación\n");
            break;
        case 6:
            printf("Descripción de la fase: Veto presidencial\n");
            break;
        case 7:
            printf("Descripción de la fase: Publicación y vigencia\n");
            break;
        case 8:
            printf("Descripción de la fase: Control constitucional\n");
            break;
        default:
            printf("Fase desconocida.\n");
            break;
        }
        // Verificar si hay votaciones y mostrarlas
        if (proyecto->votacion != NULL) {
            nodoVot = proyecto->votacion;

            // Recorre la lista de votaciones
            while (nodoVot != NULL) {
                printf("\nFase de votación: %d\n", nodoVot->datos->fase);

                // Mostrar congresistas a favor y contar votos
                mostrarCongresistasVotacion(nodoVot->datos->favor, "a favor");

                // Mostrar congresistas en contra y contar votos
                mostrarCongresistasVotacion(nodoVot->datos->contra, "en contra");

                nodoVot = nodoVot->sig; // Avanzar a la siguiente votación
            }
        }
        else {
            printf("No hay votaciones registradas para este proyecto de ley.\n");
        }
    }
    else {
        printf("Proyecto de ley con ID %d no encontrado.\n", id);
    }
}


// Función para añadir congresistas a las listas de votos (a favor o en contra) en un nodo de votación
void agregarCongresistaAVotacion(struct votacion* votacion, struct congreso* congreso)
{
    int opcionLista, opcionAgregar;
    char rut[20];
    struct congresista* congresista;
    struct nodoCongresista* nuevoNodo;
    struct nodoCongresista* actual;

    printf("Seleccione la lista de votación:\n");
    printf("1. A favor\n");
    printf("2. En contra\n");
    printf("Ingrese su opción: ");

    while (scanf("%d", &opcionLista) != 1 || (opcionLista != 1 && opcionLista != 2)) {
        printf("Opción inválida. Seleccione 1 (A favor) o 2 (En contra): ");
    }

    while (1) {
        printf("\nSeleccione una opción:\n");
        printf("1. Agregar congresista\n");
        printf("2. Terminar\n");
        printf("Ingrese su opción: ");

        while (scanf("%d", &opcionAgregar) != 1 || (opcionAgregar != 1 && opcionAgregar != 2)) {
            printf("Opción inválida. Intente de nuevo.\n");
        }
        switch (opcionAgregar) {
        case 1: // Agregar congresista
            while (1) {
                printf("¿Qué congresista añadiremos a la lista? Ingrese el RUT(sin puntos ni guion, con digito verificador): ");
                scanf(" %s", rut);
                if (strlen(rut) == 9 && strspn(rut, "0123456789") == 9) {
                    break;  // Si es válido, salir del bucle
                }
                else {
                    printf("RUT no válido. Debe tener 9 dígitos numéricos. Intente nuevamente.\n");
                }
            }
            // Buscar el congresista en el congreso
            congresista = comprobarCongresistaEnCongreso(congreso, rut);
            if (congresista == NULL) {
                printf("Error: Congresista con RUT %s no encontrado.\n", rut);
                continue;
            }

            // Crear un nuevo nodoCongresista
            nuevoNodo = (struct nodoCongresista*)malloc(sizeof(struct nodoCongresista));
            if (nuevoNodo == NULL) {
                printf("Error: No se pudo asignar memoria para el nuevo nodo de congresista.\n");
                return;
            }
            nuevoNodo->datos = congresista;
            nuevoNodo->sig = NULL;

            // Añadir el nuevo nodo a la lista seleccionada
            if (opcionLista == 1) { // Lista de votos a favor
                if (votacion->favor == NULL) {
                    votacion->favor = nuevoNodo;
                }
                else {
                    actual = votacion->favor;
                    while (actual->sig != NULL) {
                        actual = actual->sig;
                    }
                    actual->sig = nuevoNodo;
                }
                printf("Congresista con RUT %s añadido correctamente a la lista de votos a favor.\n", rut);
            }
            else if (opcionLista == 2) { // Lista de votos en contra
                if (votacion->contra == NULL) {
                    votacion->contra = nuevoNodo;
                }
                else {
                    actual = votacion->contra;
                    while (actual->sig != NULL) {
                        actual = actual->sig;
                    }
                    actual->sig = nuevoNodo;
                }
                printf("Congresista con RUT %s añadido correctamente a la lista de votos en contra.\n", rut);
            }
            break;

        case 2: // Terminar
            printf("Finalizando el agregado de congresistas.\n");
            return;

        default:
            printf("Opción inválida. Intente de nuevo.\n");
            break;
        }
    }
}

void agregarVotacion(struct congreso* congreso, int idProyecto) {
    struct proyectoLey* proyecto;
    struct nodoVotacion* nuevoNodoVotacion, * actual;
    int fase;

    // Buscar el proyecto de ley por ID
    proyecto = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
    if (proyecto == NULL) {
        printf("Error: Proyecto con ID %d no encontrado.\n", idProyecto);
        return;
    }

    // Solicitar y verificar la fase de la votación
    fase = leerEnteroConLimite("Ingrese la fase de la votación", 1, 4);

    // Crear nuevo nodoVotacion
    nuevoNodoVotacion = (struct nodoVotacion*)malloc(sizeof(struct nodoVotacion));
    if (nuevoNodoVotacion == NULL) {
        printf("Error: No se pudo asignar memoria para la nueva votación.\n");
        return;
    }

    // Crear el struct votacion y asignar datos
    nuevoNodoVotacion->datos = (struct votacion*)malloc(sizeof(struct votacion));
    nuevoNodoVotacion->datos->fase = fase;
    nuevoNodoVotacion->datos->favor = NULL;   // Inicializar lista de votantes a favor
    nuevoNodoVotacion->datos->contra = NULL;  // Inicializar lista de votantes en contra
    nuevoNodoVotacion->sig = NULL;

    // Llamar a agregarCongresistaAVotacion para rellenar las listas de favor y en contra
    agregarCongresistaAVotacion(nuevoNodoVotacion->datos, congreso);

    // Agregar el nuevo nodo al final de la lista de votaciones del proyecto
    if (proyecto->votacion == NULL) {
        // Caso 1: Primera votación para este proyecto
        proyecto->votacion = nuevoNodoVotacion;
    }
    else {
        // Caso 2: Ya existen votaciones, recorrer hasta el final de la lista
        actual = proyecto->votacion;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodoVotacion;
    }

    printf("Votación añadida correctamente al proyecto con ID %d.\n", idProyecto);
}

// Función auxiliar para imprimir un proyecto de ley
void imprimirProyectoLey(struct proyectoLey* proyecto) {
    if (proyecto == NULL) {
        return;
    }

    printf("Nombre: %s\n", proyecto->nombre ? proyecto->nombre : "N/A");
    printf("Tipo: %s\n", proyecto->tipo ? proyecto->tipo : "N/A");
    printf("ID Proyecto: %d\n", proyecto->idProyecto);
    printf("Urgencia: %d\n", proyecto->urgencia);
    printf("Fase: %d\n", proyecto->fase);

    if (proyecto->votacion != NULL) {
        printf("Votación: (existe votación)\n"); // Detallar según la estructura
    }
    else {
        printf("Votación: N/A\n");
    }

    printf("----\n");
}

// Función recursiva para recorrer e imprimir los proyectos de ley en el árbol
void recorrerYImprimirProyectos(struct nodoProyectoLey* nodo) {
    if (nodo == NULL) {
        return;
    }

    // Recorrer el subárbol izquierdo
    recorrerYImprimirProyectos(nodo->izq);

    // Imprimir el proyecto de ley en el nodo actual
    imprimirProyectoLey(nodo->datos);

    // Recorrer el subárbol derecho
    recorrerYImprimirProyectos(nodo->der);
}

// Función principal para imprimir todos los proyectos de ley del árbol
void imprimirProyectosLey(struct congreso* congreso) {
    if (congreso == NULL || congreso->raiz == NULL) {
        printf("No hay proyectos de ley para mostrar.\n");
        return;
    }

    // Llamar a la función que recorre e imprime los proyectos de ley
    recorrerYImprimirProyectos(congreso->raiz);
}

void agregarProyecto(struct proyectoLey* proyecto, struct proyectoLey*** proyectosArray, int* proyectosCount, int* proyectosCapacidad) {
    struct proyectoLey** tempArray;

    // Verificar si se necesita redimensionar el array
    if (*proyectosCount >= *proyectosCapacidad) {
        *proyectosCapacidad *= 2; // Duplicar la capacidad
        tempArray = (struct proyectoLey**)realloc(*proyectosArray, (*proyectosCapacidad) * sizeof(struct proyectoLey*));
        if (tempArray == NULL) {
            printf("Error al redimensionar la memoria.\n");
            exit(1); // Terminar el programa en caso de fallo en realloc
        }
        *proyectosArray = tempArray; // Actualizar el puntero al nuevo array
    }

    // Agregar el nuevo proyecto al array y aumentar el contador
    (*proyectosArray)[(*proyectosCount)++] = proyecto;
}

// Recorrido en orden del árbol binario para agregar los proyectos a la lista
void recorrerArbolEnOrden(struct nodoProyectoLey* nodo, struct proyectoLey*** proyectosArray, int* proyectosCount, int* proyectosCapacidad) {
    if (nodo == NULL) {
        return; // Caso base: si el nodo es NULL, salir de la función
    }

    // Recorrer el subárbol izquierdo
    recorrerArbolEnOrden(nodo->izq, proyectosArray, proyectosCount, proyectosCapacidad);

    // Agregar el proyecto del nodo actual al array
    agregarProyecto(nodo->datos, proyectosArray, proyectosCount, proyectosCapacidad);

    // Recorrer el subárbol derecho
    recorrerArbolEnOrden(nodo->der, proyectosArray, proyectosCount, proyectosCapacidad);
}

// Función de comparación para ordenar los proyectos por urgencia (mayor a menor)
int compararPorUrgencia(const void* a, const void* b) {
    struct proyectoLey* proyectoA, * proyectoB;

    proyectoA = *(struct proyectoLey**)a; // Convertir el primer argumento
    proyectoB = *(struct proyectoLey**)b; // Convertir el segundo argumento
    return proyectoB->urgencia - proyectoA->urgencia; // Comparar por urgencia
}

// Función principal para mostrar los proyectos en orden de urgencia
void mostrarProyectosOrdenDeUrgencia(struct congreso* congreso) {
    struct proyectoLey** proyectosArray;
    int proyectosCount = 0;
    int proyectosCapacidad = 10;
    int i;
    struct proyectoLey* proyecto;

    // Inicializar el array dinámico de proyectos
    proyectosArray = (struct proyectoLey**)malloc(proyectosCapacidad * sizeof(struct proyectoLey*));
    if (proyectosArray == NULL) {
        printf("Error al asignar memoria.\n");
        exit(1); // Terminar el programa en caso de fallo en malloc
    }

    // Recorrer el árbol y recolectar los proyectos
    recorrerArbolEnOrden(congreso->raiz, &proyectosArray, &proyectosCount, &proyectosCapacidad);

    // Ordenar los proyectos por urgencia de mayor a menor
    qsort(proyectosArray, proyectosCount, sizeof(struct proyectoLey*), compararPorUrgencia);

    // Imprimir los proyectos en el orden de urgencia
    printf("Proyectos de ley en orden de urgencia (de mayor a menor):\n");
    for (i = 0; i < proyectosCount; i++) {
        proyecto = proyectosArray[i];
        printf("Nombre: %s\n", proyecto->nombre ? proyecto->nombre : "N/A");
        printf("Tipo: %s\n", proyecto->tipo ? proyecto->tipo : "N/A");
        printf("ID Proyecto: %d\n", proyecto->idProyecto);
        printf("Urgencia: %d\n", proyecto->urgencia);
        printf("\n");
    }
}

void modificarProyectoLey(struct congreso* congreso, int idProyecto) {
    struct proyectoLey* proyecto;
    char opcion;

    // Buscar el proyecto de ley por ID
    proyecto = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
    if (!proyecto) {
        printf("Error: Proyecto de ley no encontrado.\n");
        return;
    }

    do {
        printf("Seleccione el campo a modificar:\n");
        printf("a. Nombre\n");
        printf("b. Tipo\n");
        printf("c. ID Proyecto\n");
        printf("d. Urgencia\n");
        printf("e. Fase\n");
        printf("f. Agregar votación\n");
        printf("g. Salir\n");
        printf("Opción: ");

        // Leer opción y limpiar buffer de entrada
        scanf(" %c", &opcion);
        // Convertir a minúscula si es una letra mayúscula
        if (opcion >= 'A' && opcion <= 'Z') {
            opcion += 'a' - 'A';
        }

        switch (opcion) {
        case 'a': {
            char nuevoNombre[100];
            printf("Ingrese nuevo nombre: ");
            if (fgets(nuevoNombre, sizeof(nuevoNombre), stdin) == NULL) {
                printf("Error al leer el nombre.\n");
                break;
            }
            nuevoNombre[strcspn(nuevoNombre, "\n")] = '\0'; // Remover salto de línea
            proyecto->nombre = (char*)malloc(strlen(nuevoNombre) + 1);
            if (proyecto->nombre) {
                strcpy(proyecto->nombre, nuevoNombre);
            }
            else {
                printf("Error: No se pudo asignar memoria para el nuevo nombre.\n");
            }
            break;
        }
        case 'b': {
            char nuevoTipo[50];
            printf("Ingrese nuevo tipo: ");
            if (fgets(nuevoTipo, sizeof(nuevoTipo), stdin) == NULL) {
                printf("Error al leer el tipo.\n");
                break;
            }
            nuevoTipo[strcspn(nuevoTipo, "\n")] = '\0'; // Remover salto de línea
            proyecto->tipo = (char*)malloc(strlen(nuevoTipo) + 1);
            if (proyecto->tipo) {
                strcpy(proyecto->tipo, nuevoTipo);
            }
            else {
                printf("Error: No se pudo asignar memoria para el nuevo tipo.\n");
            }
            break;
        }
        case 'c': {
            int nuevoID = leerEnteroConLimite("Ingrese nuevo ID de proyecto", 10000000, 99999999);
            proyecto->idProyecto = nuevoID;
            break;
        }
        case 'd': {
            int nuevaUrgencia = leerEnteroConLimite("Ingrese nueva urgencia", 1, 5);
            proyecto->urgencia = nuevaUrgencia;
            break;
        }
        case 'e': {
            int nuevaFase = leerEnteroConLimite("Ingrese nueva fase", 1, 8);
            proyecto->fase = nuevaFase;
            break;
        }
        case 'f':
            // Llamar a la función para agregar votación al proyecto de ley
            agregarVotacion(congreso, idProyecto);
            break;
        case 'g':
            printf("Saliendo de la modificación del proyecto de ley.\n");
            break;
        default:
            printf("Opción inválida. Intente nuevamente.\n");
        }
    } while (opcion != 'i');
}

/*TODO: FUNCIONES DE CONGRESISTAS------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
struct nodoCongresista* crearNodoCongresista(struct nodoCongresista* head, struct congresista* datos) {
    struct nodoCongresista* nodo = NULL;

    // Pregunto primero que los datos recibidos no sean NULL
    if (datos != NULL) {
        nodo = (struct nodoCongresista*)malloc(sizeof(struct nodoCongresista));

        if (nodo == NULL) {
            // Si esto ocurre, hay un error al asignar la memoria
            return NULL;
        }

        nodo->datos = datos; // Copio los datos que recibí
        nodo->sig = NULL; // Asigno NULL al siguiente nodo
    }

    return nodo; // Retorno el nuevo nodo (o NULL si no se creó)
}

/*

comprobar que exista congresista, lo haré de manera que retorne 0 si NO existe el RUT, o que retorne 1 si existe
la idea es que el RUT sea el buscado, por lo tanto las otras funciones que la llamen deben ingresar el RUT
aunque esto puede estar sujeto a cambios si se desea, quizas recibir el nodo entero para comodidad
recordar que es circular con fantasma, por lo tanto tengo que iniciar el head->sig para el rec y usar do while

*/

int comprobarCongresistaEnComision(struct nodoCongresista* head, char* rutBuscado) {
    struct nodoCongresista* rec = head; // Iniciamos en el nodo fantasma

    rec = head->sig; // Saltar el nodo fantasma
    do {
        // Verificamos que rec->datos no sea NULL y comparamos el RUT
        if (rec->datos != NULL && strcmp(rec->datos->rut, rutBuscado) == 0) {
            return 1; // Se encontró el congresista en la lista
        }
        rec = rec->sig; // Avanzar al siguiente nodo
    } while (rec != head); // Volver al nodo fantasma

    return 0; // No se encontró el congresista en la lista
}

//funcion para recorrer los arreglos, el de diputados o el de senadores correspondientemente
struct congresista* comprobarCongresistaEnCongreso(struct congreso* congreso, char* rutBuscado) {
    struct nodoCongresista* head = congreso->congresistasMixtos;
    struct nodoCongresista* rec = NULL;
    int i;

    if (rutBuscado != NULL) {
        // Buscar en diputados
        for (i = 0; i < MAX_DIPUTADOS; i++) {
            if (congreso->diputados[i] != NULL) {
                if (strcmp(congreso->diputados[i]->rut, rutBuscado) == 0) {
                    return congreso->diputados[i]; // RUT encontrado
                }
            }
        }

        // Buscar en senadores
        for (i = 0; i < MAX_SENADORES; i++) {
            if (congreso->senadores[i] != NULL) {
                if (strcmp(congreso->senadores[i]->rut, rutBuscado) == 0) {
                    return congreso->senadores[i]; // RUT encontrado
                }
            }
        }

        // Buscar en la lista de congresistas mixtos
        if (head != NULL) {
            rec = head; // Iniciar en la cabeza de la lista
            while (rec != NULL) {
                if (rec->datos != NULL && strcmp(rec->datos->rut, rutBuscado) == 0) {
                    return rec->datos; // RUT encontrado
                }
                rec = rec->sig; // Avanzar al siguiente nodo
            }
        }
    }
    return NULL; // RUT no encontrado
}
/*
El crearCongresista va a ser un poco distinto, esta funcion tiene que recibir(scanf) el rut y la ocupacion
al saber la ocupacion eligirá que arreglo recorrer si el de senadores o el de diputados
ahí en ese momento buscará comparando el rut en cada pos del arreglo, si lo encuentra retornará 1
si no es así retornará 0 y dará paso a la copia de datos para ingresarlos en el arreglo

aunque no estoy seguro si la ocupacion fuese el diputado/senador o si es la especializacion

*/

struct congresista* crearCongresista(struct congreso* congreso) {
    struct congresista* nuevoCongresista; // Para guardar los datos del congresista
    char nombre[100], rut[20], ocupacion[20], especializacion[100]; // Datos que se reciben

    nuevoCongresista = (struct congresista*)malloc(sizeof(struct congresista));
    if (nuevoCongresista == NULL) {
        return NULL; // Error al asignar memoria
    }

    // Tomar el RUT del congresista
    printf("Ingresa el RUT del congresista (12.345.678-9): ");
    scanf("%19[^\n]", rut);
    if (strlen(rut) > 20) {
        printf("El RUT ingresado es muy largo, inténtelo de nuevo\n");
        return NULL;
    }

    // Ingresar la ocupación
    printf("Ingrese la ocupación (senador/diputado): ");
    scanf("%19s", ocupacion);
    if (comprobarCongresistaEnCongreso(congreso, rut) != NULL) {
        printf("Este congresista ya se encuentra en el sistema\n");
        return NULL; // El congresista ya existe
    }

    // Escanear los últimos datos
    printf("Ingrese su especialización: ");
    scanf("%[^\n]", especializacion);
    printf("Ingrese su Nombre: ");
    scanf("%[^\n]", nombre);
    if (strlen(rut) > 20 || strlen(especializacion) > 100 || strlen(nombre) > 100) {
        printf("Uno de los valores ingresados es muy largo, inténtelo otra vez\n");
        return NULL;
    }

    // Asignación de memoria para los campos de texto
    nuevoCongresista->nombre = (char*)malloc(sizeof(char) * (strlen(nombre) + 1));
    nuevoCongresista->rut = (char*)malloc(sizeof(char) * (strlen(rut) + 1));
    nuevoCongresista->ocupacion = (char*)malloc(sizeof(char) * (strlen(ocupacion) + 1));
    nuevoCongresista->especializacion = (char*)malloc(sizeof(char) * (strlen(especializacion) + 1));

    // Copiar los datos a los campos
    strcpy(nuevoCongresista->nombre, nombre);
    strcpy(nuevoCongresista->rut, rut);
    strcpy(nuevoCongresista->ocupacion, ocupacion);
    strcpy(nuevoCongresista->especializacion, especializacion);

    printf("Se ha agregado con éxito\n");
    return nuevoCongresista;
}

/*
primero hago la funcion para insertar el nuevo congresista en el arreglo correspondiente

agregue un MAX_CONGRESISTAS = 200 por que son arreglos separados
al ver la historia de chile te das cuenta que nunca han habido mas de 150 diputados
y tampoco han habido mas de 50 senadores en periodos de mas de 200 años
por lo tanto dudo que en otros 100 años se superen los 200
era esta solución o configurar el struct congreso para agregar un plibre de cada arreglo

*/

//este agrega un NUEVO congresista al arreglo correspondiente
void agregarCongresistaEnCongreso(struct congreso* congreso) {
    struct congresista* nuevoCongresista = crearCongresista(congreso); // Se crea el congresista para insertarlo
    struct nodoCongresista* nuevoNodoCongresista = NULL; // Nodo para congresistas mixtos
    struct congresista** arreglo = NULL; // Decidirá a qué arreglo pertenece
    int i = 0;

    // Preguntar si el congresista es válido
    if (nuevoCongresista != NULL) {
        if (strcmp(nuevoCongresista->ocupacion, "senador") == 0) {
            arreglo = congreso->senadores; // El congresista es un senador
        }
        else if (strcmp(nuevoCongresista->ocupacion, "diputado") == 0) {
            arreglo = congreso->diputados; // El congresista es un diputado
        }
        else {
            nuevoNodoCongresista = crearNodoCongresista(congreso->congresistasMixtos, nuevoCongresista);
            if (nuevoNodoCongresista == NULL) {
                printf("Error al asignar memoria\n");
                return; // Error al asignar memoria
            }
            nuevoNodoCongresista->datos = nuevoCongresista;
            nuevoNodoCongresista->sig = congreso->congresistasMixtos;
            congreso->congresistasMixtos = nuevoNodoCongresista;

            return; // Se agrega correctamente
        }

        // Se recorre el arreglo elegido validando que se haya elegido uno
        if (arreglo != NULL) {
            if (arreglo == congreso->senadores) {
                while (i < MAX_SENADORES && arreglo[i] != NULL) {
                    i++; // Se busca la primera posición vacía
                }
                if (i < MAX_SENADORES) {
                    arreglo[i] = nuevoCongresista; // Añade el nuevo congresista
                    return; // Se logra asignar sin problema
                }
                else {
                    printf("El arreglo está lleno, no se ha agregado\n");
                    return;
                }
            }

            if (arreglo == congreso->diputados) {
                while (i < MAX_DIPUTADOS && arreglo[i] != NULL) {
                    i++; // Se busca la primera posición vacía
                }
                if (i < MAX_DIPUTADOS) {
                    arreglo[i] = nuevoCongresista; // Añade el nuevo congresista
                    return; // Se logra asignar sin problema
                }
                else {
                    printf("El arreglo está lleno, no se ha agregado\n");
                    return;
                }
            }
        }
    }
    // El nuevo congresista era NULL
    printf("Error: No se pudo crear el nuevo congresista.\n");
}

/*
esta funcion agrega un ya EXISTENTE congresista a la lista de comisiones
voy a darle la comision, pues para acceder a la lista simplemente hago comision->headintegrantes
esto puede cambiar por int para los print
*/

void agregarCongresistaEnComision(struct comision* comision, char* rut, struct congresista* congresista) {
    struct nodoCongresista* nuevoNodo = NULL;
    struct nodoCongresista* rec = NULL;

    // Preguntar que ni uno de los dos sea NULL
    if (comision != NULL && congresista != NULL) {
        // Verifico que el congresista no esté en la comisión
        if (comprobarCongresistaEnComision(comision->headIntegrantes, rut) == 0) {
            // Creo y enlazo el nuevo nodo
            nuevoNodo = crearNodoCongresista(comision->headIntegrantes, congresista);
            if (nuevoNodo == NULL) {
                return; // Error al crear el nuevo nodo
            }

            rec = comision->headIntegrantes;
            if (rec != NULL) {
                while (rec->sig != comision->headIntegrantes) {
                    rec = rec->sig; // Encuentra el último nodo
                }
                rec->sig = nuevoNodo;         // Enlaza el nuevo nodo al final de la lista
                nuevoNodo->sig = comision->headIntegrantes; // Enlaza el nuevo nodo al nodo fantasma
            }
            else {
                // Si headIntegrantes es NULL, significa que es el primer nodo
                nuevoNodo->sig = nuevoNodo; // Enlaza a sí mismo
                comision->headIntegrantes = nuevoNodo; // Establece el nuevo nodo como head
            }
        }
    }
}

/*
Funciones para eliminar:
una solo eliminará a un congresista de la comision
la otra eliminará a un congresista del arreglo, por lo tanto se debe eliminar de todas las otras comisiones

igualmente que en funciones anteriores, se puede cambiar el char que recibe por el congresista entero
lo haré int para retornar 0 si hubo un fallo en la eliminacion o 1 si se eliminó bien
*/

void eliminarCongresistaDeComision(struct comision* comision, char* rutQuitado) {
    struct nodoCongresista* rec;          // Nodo actual para recorrer la lista
    struct nodoCongresista* anterior = NULL; // Nodo anterior para el manejo de la lista circular

    // Verifico si la lista está vacía o solo contiene el nodo fantasma
    if (comision == NULL || comision->headIntegrantes == NULL || comision->headIntegrantes->sig == comision->headIntegrantes) {
        return; // No existe ningún congresista en la comisión, se termina el proceso
    }

    anterior = comision->headIntegrantes; // Inicializo anterior al nodo fantasma
    rec = comision->headIntegrantes->sig;  // Inicializo rec al primer nodo real

    do {
        if (rec->datos != NULL && strcmp(rec->datos->rut, rutQuitado) == 0) {
            // Se encontró el congresista
            anterior->sig = rec->sig; // Desvincula el nodo encontrado de la lista
            return; // Salgo de la función tras eliminar
        }
        anterior = rec; // Avanzo en la lista
        rec = rec->sig;

    } while (rec != comision->headIntegrantes); // Continúo hasta volver al nodo fantasma

    // No se encontró el congresista en la lista
}

void eliminarCongresistaDeCongreso(struct congreso* congreso, char* rutQuitado) {
    struct comision** arreglo = NULL;
    int i, flag = 0;
    struct congresista* congresista;
    struct nodoCongresista* rec;
    struct nodoCongresista* recExterno, * ant;

    // Verifico que el RUT no sea NULL
    if (rutQuitado != NULL) {
        congresista = comprobarCongresistaEnCongreso(congreso, rutQuitado);

        if (congresista != NULL) {
            // Busco en el arreglo de senadores
            for (i = 0; i < MAX_SENADORES; i++) {
                if (congreso->senadores[i] != NULL && strcmp(congreso->senadores[i]->rut, rutQuitado) == 0) {
                    congreso->senadores[i] = NULL;
                    flag = 1;
                }
            }
            // Si no se encontró en senadores, busco en diputados
            if (flag == 0) {
                for (i = 0; i < MAX_DIPUTADOS; i++) {
                    if (congreso->diputados[i] != NULL && strcmp(congreso->diputados[i]->rut, rutQuitado) == 0) {
                        congreso->diputados[i] = NULL;
                        flag = 1;
                    }
                }
            }
            // Si aún no se ha encontrado, busco en congresistas mixtos
            if (flag == 0 && congreso->congresistasMixtos != NULL) {
                recExterno = congreso->congresistasMixtos;
                ant = NULL;
                while (recExterno != NULL && strcmp(recExterno->datos->rut, rutQuitado) != 0) {
                    ant = recExterno;
                    recExterno = recExterno->sig;
                }

                if (recExterno != NULL) {
                    if (ant == NULL) {
                        congreso->congresistasMixtos = recExterno->sig; // Eliminar el primer nodo
                    }
                    else {
                        ant->sig = recExterno->sig; // Eliminar nodo intermedio o final
                    }
                    flag = 1;
                }
            }
        }

        // Ahora comienzo a borrar de las comisiones
        if (congreso->comisiones != NULL) {
            arreglo = congreso->comisiones;

            for (i = 0; i < MAX_COMISIONES; i++) {
                if (arreglo[i] != NULL) {
                    rec = arreglo[i]->headIntegrantes; // Recorro el arreglo

                    do {
                        if (comprobarCongresistaEnComision(rec, rutQuitado) == 1) {
                            // Llamo a la función para eliminar de la comisión
                            eliminarCongresistaDeComision(arreglo[i], rutQuitado);
                        }
                        rec = rec->sig;
                    } while (rec != arreglo[i]->headIntegrantes);
                }
            }
        }
    }

    if (flag == 1) {
        printf("El congresista fue eliminado\n");
    }
    else {
        printf("El congresista no fue encontrado\n");
    }
}

/*
Funcion para modificar a un congresista
primero tengo que elegir el arreglo en el que está o simplemente recorrer ambos arreglos
en este caso no incluí la modificacion de la ocupación
ya que al cambiar la ocupacion es mejor eliminarlo del arreglo y agregarlo al otro simplemente
*/

void modificarCongresista(struct congreso* congreso, char* rutBuscado) {
    char nombre[100], especializacion[100], rut[20];
    struct congresista* congresista = comprobarCongresistaEnCongreso(congreso, rutBuscado);

    if (congresista != NULL) {
        // Escaneo los datos nuevos
        printf("Ingrese nuevo Nombre: ");
        scanf(" %99[^\n]", nombre); // Ingrese nuevo nombre

        printf("Ingrese nueva especializacion: ");
        scanf(" %99[^\n]", especializacion); // Ingrese nueva especializacion

        printf("Ingrese nuevo rut: ");
        scanf(" %19[^\n]", rut); // Ingrese nuevo rut

        // Asignación de memoria para los nuevos datos
        congresista->rut = (char*)malloc(sizeof(char) * (strlen(rut) + 1));
        congresista->nombre = (char*)malloc(sizeof(char) * (strlen(nombre) + 1));
        congresista->especializacion = (char*)malloc(sizeof(char) * (strlen(especializacion) + 1));

        // Verificación de asignación de memoria
        if (congresista->rut == NULL || congresista->nombre == NULL || congresista->especializacion == NULL) {
            printf("Error al asignar memoria para los datos del congresista.\n");
            return; // Salgo si hubo error en la asignación
        }

        // Copiar los nuevos datos
        strcpy(congresista->rut, rut);
        strcpy(congresista->nombre, nombre);
        strcpy(congresista->especializacion, especializacion);

        printf("Fue modificado correctamente\n");
        return;
    }
    printf("Hubo un problema al modificar al congresista\n");
}

void mostrarCongresista(struct congreso* congreso, char* rutBuscado) {
    struct congresista* congresistaBuscado = NULL;

    // Busco el congresista
    congresistaBuscado = comprobarCongresistaEnCongreso(congreso, rutBuscado);
    if (congresistaBuscado == NULL) {
        printf("No existe el congresista en el sistema\n");
        return;
    }

    // Muestra la información del congresista
    printf("Nombre: %s\n", congresistaBuscado->nombre);
    printf("RUT: %s\n", congresistaBuscado->rut);
    printf("Ocupacion: %s\n", congresistaBuscado->ocupacion);
    printf("Especializacion: %s\n\n", congresistaBuscado->especializacion);
}

/*
funcion listar congresista:
esta funcion es un poco redundante, pues llama al mostrarCongresista y esta lo busca de nuevo pero cumple con el requisito de listar
*/
void listarCongresistas(struct congreso* congreso) {
    struct congresista** arregloDiputados = congreso->diputados;
    struct congresista** arregloSenadores = congreso->senadores;
    struct nodoCongresista* rec;
    int i, flagDiputados = 0, flagSenadores = 0, flagExternos = 0;

    // Verifica que el congreso no sea NULL
    if (congreso == NULL) {
        return;
    }

    printf("Diputados en sistema:\n");
    if (arregloDiputados != NULL) {
        for (i = 0; i < MAX_DIPUTADOS; i++) {
            if (arregloDiputados[i] != NULL) {
                printf("Nombre: %s\n", arregloDiputados[i]->nombre);
                printf("RUT: %s\n\n", arregloDiputados[i]->rut);
                flagDiputados = 1;
            }
        }
    }
    printf("\n\n");

    if (arregloSenadores != NULL) {
        printf("Senadores en sistema:\n");
        for (i = 0; i < MAX_SENADORES; i++) {
            if (arregloSenadores[i] != NULL) {
                printf("Nombre: %s\n", arregloSenadores[i]->nombre);
                printf("RUT: %s\n\n", arregloSenadores[i]->rut);
                flagSenadores = 1;
            }
        }
    }
    printf("\n\n");

    if (congreso->congresistasMixtos != NULL) {
        printf("Externos en sistema:\n");
        rec = congreso->congresistasMixtos;
        while (rec != NULL) {
            if (rec->datos != NULL) {
                printf("Nombre: %s\n", rec->datos->nombre);
                printf("RUT: %s\n\n", rec->datos->rut);
                flagExternos = 1;
            }
            rec = rec->sig; // Se asume que esto es correcto, aunque normalmente la lista enlazada debería tener un nodo final
        }
    }

    if (flagDiputados == 0 && flagSenadores == 0 && flagExternos == 0) {
        printf("El sistema no tiene congresistas\n");
    }
}

/*TODO: FUNCIONES DE COMISIONES------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

struct nodoComision* crearNodoComision(struct comision* datos) {
    // Asignar memoria para el nuevo nodo
    struct nodoComision* nuevoNodo = (struct nodoComision*)malloc(sizeof(struct nodoComision));
    if (nuevoNodo == NULL) {
        // Manejo de error en caso de que malloc falle
        return NULL;
    }

    // Inicializar el nodo
    nuevoNodo->datos = datos;
    nuevoNodo->sig = NULL;

    return nuevoNodo; // Retornar el nuevo nodo
}

struct comision* buscarComision(struct congreso* congreso, char* nombre) {
    int i;
    struct nodoComision* rec = NULL;
    struct comision* buscado = NULL;

    for (i = 0; i < MAX_COMISIONES; i++) {
        if (congreso->comisiones[i] != NULL && strcmp(congreso->comisiones[i]->nombre, nombre) == 0) {
            buscado = congreso->comisiones[i];
            return buscado; // Retorna la comisión si se encuentra
        }
    }

    rec = congreso->comisionesMixtas;
    while (rec != NULL) {
        if (strcmp(rec->datos->nombre, nombre) == 0) {
            buscado = rec->datos;
            return buscado; // Retorna la comisión si se encuentra
        }
        rec = rec->sig; // Avanza al siguiente nodo
    }

    return buscado; // Retorna NULL si no se encuentra
}

struct comision* crearComision(struct congreso* congreso) {
    struct comision* nuevaComision;
    struct nodoCongresista* fantasma;
    char nombre[100], tipo[50], descripcion[256];

    nuevaComision = (struct comision*)malloc(sizeof(struct comision));

    printf("Ingresa el nombre de la comision:\n");
    scanf("%99[^\n]", nombre);
    if (buscarComision(congreso, nombre) != NULL) {
        printf("La comision ya existe en el sistema\n");
        return NULL;
    }

    printf("Ingresa descripcion de la comision:\n");
    scanf("%255[^\n]", descripcion);
    printf("Ingresa el tipo de la comision (senadores/diputados/otros):\n");
    scanf("%49[^\n]", tipo);
    convertirMinusculas(tipo);
    nuevaComision->nombre = (char*)malloc(sizeof(char) * (strlen(nombre) + 1));
    nuevaComision->descripcion = (char*)malloc(sizeof(char) * (strlen(descripcion) + 1));
    nuevaComision->tipo = (char*)malloc(sizeof(char) * (strlen(tipo) + 1));
    strcpy(nuevaComision->nombre, nombre);
    strcpy(nuevaComision->descripcion, descripcion);
    strcpy(nuevaComision->tipo, tipo);

    // Crear el nodo fantasma
    fantasma = (struct nodoCongresista*)malloc(sizeof(struct nodoCongresista));
    fantasma->datos = NULL;
    fantasma->sig = fantasma; // Apunta a sí mismo, indicando que es un nodo fantasma
    nuevaComision->headIntegrantes = fantasma;

    return nuevaComision;
}

void agregarComision(struct congreso* congreso) {
    struct comision* nuevaComision = crearComision(congreso);
    struct nodoComision* nuevoNodo = NULL;
    int i;

    if (strcmp(nuevaComision->tipo, "senadores") == 0 || strcmp(nuevaComision->tipo, "diputados") == 0) {
        // Intentar agregar al arreglo de comisiones
        for (i = 0; i < MAX_COMISIONES; i++) {
            if (congreso->comisiones[i] == NULL) {
                congreso->comisiones[i] = nuevaComision;
                printf("Comision agregada al arreglo de comisiones\n");
                return;
            }
        }
        // Si se alcanza el límite del arreglo
        printf("Error al asignar en el arreglo: se ha alcanzado el máximo de comisiones\n");
    }
    else
    {
        // Agregar como comisión mixta
        nuevoNodo = crearNodoComision(nuevaComision);
        nuevoNodo->sig = congreso->comisionesMixtas;
        congreso->comisionesMixtas = nuevoNodo;
        printf("Comision agregada como mixta\n");
    }
}

void eliminarComision(struct congreso* congreso, char* nombre) {
    struct comision* buscada = buscarComision(congreso, nombre);
    struct nodoComision* rec, * ant;
    int i;

    if (buscada == NULL) {
        printf("Error al eliminar la comision: no existe en el sistema\n");
        return;
    }

    for (i = 0; i < MAX_COMISIONES; i++) {
        if (congreso->comisiones[i] == buscada)
        {
            congreso->comisiones[i] = NULL; // Marcar como vacío
            printf("Comision eliminada correctamente del arreglo\n");
            return;
        }
    }

    rec = congreso->comisionesMixtas;
    ant = NULL;

    while (rec != NULL) {
        if (rec->datos == buscada)
        {
            // Caso del primer nodo
            if (ant == NULL) {
                congreso->comisionesMixtas = rec->sig;
            }
            else {
                ant->sig = rec->sig; // Desvincular el nodo
            }

            printf("Comisión eliminada correctamente de la lista\n");
            return;
        }
        ant = rec;
        rec = rec->sig;
    }

    printf("Error al eliminar la comision: no se encontró en la lista de comisiones mixtas\n");
}

void modificarComision(struct congreso* congreso, char* nombre) {
    struct congresista* nuevoCongresista = NULL;
    struct comision* comisionAModificar = buscarComision(congreso, nombre);
    int opcion, subOpcion;
    char nuevoNombre[100], nuevoTipo[50], nuevaDescripcion[256], rut[20];

    if (comisionAModificar == NULL) {
        printf("No existe una comision con el nombre especificado.\n");
        return;
    }

    printf("Seleccione el campo que desea modificar:\n");
    printf("1. Nombre\n");
    printf("2. Tipo\n");
    printf("3. Descripcion\n");
    printf("4. Modificar Miembros\n");
    printf("Ingrese su opción: ");
    scanf("%d", &opcion);

    switch (opcion) {
    case 1: // Modificar nombre
        printf("Ingrese el nuevo nombre de la comision:\n");
        scanf("%99[^\n]", nuevoNombre);
        if (buscarComision(congreso, nuevoNombre) != NULL) {
            printf("El nombre ya está en uso. Modificación cancelada.\n");
            return;
        }
        comisionAModificar->nombre = (char*)malloc(strlen(nuevoNombre) + 1);
        if (comisionAModificar->nombre == NULL) {
            printf("Error al asignar memoria para el nuevo nombre.\n");
            return;
        }
        strcpy(comisionAModificar->nombre, nuevoNombre);
        printf("Nombre de la comisión actualizado correctamente.\n");
        break;

    case 2: // Modificar tipo
        printf("Ingrese el nuevo tipo de la comisión (senadores/diputados/otros):\n");
        scanf("%49[^\n]", nuevoTipo);

        comisionAModificar->tipo = (char*)malloc(strlen(nuevoTipo) + 1);
        if (comisionAModificar->tipo == NULL) {
            printf("Error al asignar memoria para el nuevo tipo.\n");
            return;
        }
        strcpy(comisionAModificar->tipo, nuevoTipo);
        printf("Tipo de la comisión actualizado correctamente.\n");
        break;

    case 3: // Modificar descripción
        printf("Ingrese la nueva descripción de la comisión:\n");
        scanf("%255[^\n]", nuevaDescripcion);

        comisionAModificar->descripcion = (char*)malloc(strlen(nuevaDescripcion) + 1);
        if (comisionAModificar->descripcion == NULL) {
            printf("Error al asignar memoria para la nueva descripción.\n");
            return;
        }
        strcpy(comisionAModificar->descripcion, nuevaDescripcion);
        printf("Descripción de la comisión actualizada correctamente.\n");
        break;

    case 4: // Modificar miembros
        printf("Menú de modificar congresistas en la comisión\n");
        printf("Elija una opción:\n");
        printf("1. Agregar Congresista a la comisión\n");
        printf("2. Eliminar Congresista de la comisión\n");
        scanf("%d", &subOpcion);

        switch (subOpcion) {
        case 1: // Agregar congresista
            printf("Ingrese el RUT del congresista a agregar\n");
            scanf("%19s", rut);
            nuevoCongresista = comprobarCongresistaEnCongreso(congreso, rut);
            if (nuevoCongresista != NULL) {
                if (comprobarCongresistaEnComision(comisionAModificar->headIntegrantes, rut) == 1) {
                    printf("Este congresista ya se encuentra en la lista\n");
                }
                else {
                    agregarCongresistaEnComision(comisionAModificar, rut, nuevoCongresista);
                    printf("Congresista agregado correctamente\n");
                }
            }
            else {
                printf("Congresista no encontrado en el sistema\n");
            }
            break;

        case 2: // Eliminar congresista
            printf("Ingrese el RUT del congresista a eliminar\n");
            scanf("%19s", rut);
            if (comprobarCongresistaEnComision(comisionAModificar->headIntegrantes, rut) == 1) {
                eliminarCongresistaDeComision(comisionAModificar, rut);
                printf("Congresista eliminado correctamente\n");
            }
            else {
                printf("No se encontró al congresista en la comisión\n");
            }
            break;

        default:
            printf("Opción no válida\n");
            break;
        }
        break;

    default:
        printf("Opción no válida.\n");
        break;
    }
}

void mostrarComisionPorNombre(struct congreso* congreso, char* nombre) {
    struct comision* buscada = buscarComision(congreso, nombre);
    struct nodoCongresista* rec;

    if (buscada == NULL) {
        printf("No existe una comision con este nombre\n");
        return;
    }

    printf("Comision encontrada:\n\n");
    printf("Nombre: %s\n", buscada->nombre);
    printf("Descripcion: %s\n", buscada->descripcion);
    printf("Tipo: %s\n", buscada->tipo);

    rec = buscada->headIntegrantes;
    if (rec == NULL || rec->sig == rec) { // Chequeo si no hay integrantes
        printf("No hay integrantes en la comision\n");
    }
    else {
        printf("Integrantes de la comision:\n\n");
        rec = rec->sig; // Saltar el nodo fantasma
        do {
            if (rec->datos != NULL) {
                printf("- Nombre: %s\n", rec->datos->nombre);
                printf("- Ocupacion: %s\n\n", rec->datos->ocupacion);
            }
            rec = rec->sig;
        } while (rec != buscada->headIntegrantes); // Termina cuando regresa al nodo fantasma
    }
}

void listarComisiones(struct congreso* congreso) {
    int i;
    struct nodoComision* rec;

    // Verificar si el congreso es NULL
    if (congreso == NULL) {
        printf("El congreso no está inicializado.\n");
        return;
    }

    printf("Comisiones de senadores y diputados:\n");
    for (i = 0; i < MAX_COMISIONES; i++) {
        if (congreso->comisiones[i] != NULL) {
            printf("Nombre: %s\n", congreso->comisiones[i]->nombre);
            printf("Descripcion: %s\n", congreso->comisiones[i]->descripcion);
            printf("Tipo: %s\n", congreso->comisiones[i]->tipo);
            printf("\n");
        }
    }

    printf("Comisiones mixtas:\n");
    rec = congreso->comisionesMixtas;
    while (rec != NULL) {
        if (rec->datos != NULL) { // Verificar que los datos no sean NULL
            printf("Nombre: %s\n", rec->datos->nombre);
            printf("Descripcion: %s\n", rec->datos->descripcion);
            printf("Tipo: %s\n", rec->datos->tipo);
            printf("\n");
        }
        rec = rec->sig;
    }
}

/*TODO: FUNCIONES CON LOS SWITCH (MENUS)----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void funcionSwitch(char opcion, struct congreso* congreso, void (*submenu)(struct congreso*)) {
    switch (opcion) {
    case 'A':
        printf("Seleccionaste la opcion A. Accediendo al menu...\n");
        submenu(congreso);
        break;
    case 'B':
        printf("Seleccionaste la opcion B. Accediendo al menu...\n");
        submenu(congreso);
        break;
    case 'C':
        printf("Seleccionaste la opcion C. Accediendo al menu...\n");
        submenu(congreso);
        break;
    case 'D':
        printf("Seleccionaste la opcion D. Accediendo al menu...\n");
        submenu(congreso);
        break;
    case 'E':
        printf("Seleccionaste la opcion E. Cerrando el menu.\n");
        break;
    default:
        printf("Opcion invalida, por favor intente otra vez.\n");
        break;
    }
}

void menuProyectosLey(struct congreso* congreso) {
    char opcion[2];
    int id, fase;
    while (1) {
        printf("Menu Proyectos de Ley.\n"
            "Opcion A: Agregar nuevo Proyecto de Ley\n"
            "Opcion B: Borrar Proyecto de Ley\n"
            "Opcion C: Mostrar Proyecto de Ley\n"
            "Opcion D: Modificar Proyecto de Ley\n"
            "Opcion E: Listar Proyectos de Ley\n"
            "Opcion F: Mostrar Proyectos en Orden de Urgencia\n"
            "Opcion G: Mostrar Proyectos por fase\n"
            "Opcion H: Volver al menu principal\n");

        scanf("%1s", opcion);

        // Convierte la opción a mayúscula si está en minúscula
        opcion[0] = (opcion[0] >= 'a' && opcion[0] <= 'z') ? opcion[0] - ('a' - 'A') : opcion[0];

        switch (opcion[0]) {
        case 'A':
            printf("Funcion: Agregar nuevo Proyecto de Ley\n");
            agregarNodoProyectoLey(congreso);
            break;
        case 'B':
            printf("Funcion: Borrar Proyecto de Ley\n");
            id = leerEnteroConLimite("Ingrese la ID del Proyecto que quiere borrar", 10000000, 99999999);
            borrarProyectoLey(congreso, id);
            break;
        case 'C':
            printf("Funcion: Mostrar Proyecto de Ley\n");
            id = leerEnteroConLimite("Ingrese la ID del Proyecto que quiere buscar", 10000000, 99999999);
            buscarYMostrarProyectoLey(congreso, id);
            break;
        case 'D':
            printf("Funcion: Modificar Proyecto de Ley\n");
            id = leerEnteroConLimite("Ingrese la ID del Proyecto que quiere modificar", 10000000, 99999999);
            modificarProyectoLey(congreso, id);
            break;
        case 'E':
            printf("Funcion: Listar Proyectos de Ley\n");
            imprimirProyectosLey(congreso);
            break;
        case 'F':
            printf("Funcion: Mostrar Proyectos en Orden de Urgencia\n");
            mostrarProyectosOrdenDeUrgencia(congreso);
            break;
        case 'G':
            printf("Funcion: Mostrar Proyectos por fase\n");
            fase = leerEnteroConLimite("Ingrese la fase de los proyectos que quiere mostrar", 1, 8);
            mostrarLeyesPorFase(congreso->raiz, fase);
            break; // Se añadió break aquí para evitar caer en el caso 'H'
        case 'H':
            return; // Salir del menú
        default:
            printf("Opcion invalida, por favor intente otra vez.\n");
            break;
        }
    }
}

void menuCongresistas(struct congreso* congreso) {
    char opcion[2];
    char rut[20];
    struct nodoCongresista*actual = NULL;

    while (1) {
        printf("Menu Congresistas.\n"
            "Opcion A: Agregar Congresista\n"
            "Opcion B: Borrar Congresista\n"
            "Opcion C: Buscar Congresista\n"
            "Opcion D: Modificar Congresista\n"
            "Opcion E: Listar Congresistas\n"
            "Opcion F: Volver al menu principal\n");

        scanf("%1s", opcion);

        // Convierte la opción a mayúscula si está en minúscula
        opcion[0] = (opcion[0] >= 'a' && opcion[0] <= 'z') ? opcion[0] - ('a' - 'A') : opcion[0];

        switch (opcion[0]) {
        case 'A':
            printf("Funcion: Agregar Congresista\n");
            agregarCongresistaEnCongreso(congreso);
            break; // Se añadió break aquí
        case 'B':
            printf("Funcion: Borrar Congresista\n Ingrese el rut del congresista a eliminar: ");
            scanf("%19[^\n]", rut);
            eliminarCongresistaDeCongreso(congreso, rut);
            break; // Se añadió break aquí
        case 'C':
            printf("Funcion: Buscar Congresista\n");
            printf("Ingrese el rut del congresista a buscar: ");
            scanf("%19[^\n]", rut);
            mostrarCongresista(congreso, rut);
            break; // Se añadió break aquí
        case 'D':
            printf("Funcion: Modificar Congresista\n");
            printf("Ingrese el rut del congresista a modificar:\n");
            scanf(" %19[^\n]", rut);
            modificarCongresista(congreso, rut);
            break; // Se añadió break aquí
        case 'E':
            printf("Funcion: Listar Congresistas\n");
            listarCongresistas(congreso);
            break; // Se añadió break aquí
        case 'F':
            return; // Salir del menú
        default:
            printf("Opcion invalida, por favor intente otra vez.\n");
            break; // Se añadió break aquí
        }
    }
}

void menuComisiones(struct congreso* congreso) {
    char opcion[2];
    char nombre[100];

    while (1) {
        printf("Menu Comisiones.\n"
            "Opcion A: Agregar Comision\n"
            "Opcion B: Borrar Comision\n"
            "Opcion C: Buscar Comision\n"
            "Opcion D: Modificar Comision\n"
            "Opcion E: Listar Comisiones\n"
            "Opcion F: Volver al menu principal\n");

        scanf("%1s", opcion);
        opcion[0] = (opcion[0] >= 'a' && opcion[0] <= 'z') ? opcion[0] - ('a' - 'A') : opcion[0];

        switch (opcion[0]) {
        case 'A':
            printf("Funcion: Agregar Comision\n");
            agregarComision(congreso);
            break;
        case 'B':
            printf("Funcion: Borrar Comision\n");
            printf("Ingrese el nombre de la comision a borrar:\n");
            scanf("%99[^\n]", nombre);
            eliminarComision(congreso, nombre);
            break;
        case 'C':
            printf("Funcion: Buscar Comision\n");
            printf("Ingrese el nombre de la comision buscada:\n");
            scanf("%99[^\n]", nombre);
            mostrarComisionPorNombre(congreso, nombre);
            break;
        case 'D':
            printf("Funcion: Modificar Comision\n");
            printf("Ingrese el nombre de la comision a modificar:\n");
            scanf("%99[^\n]", nombre);
            modificarComision(congreso, nombre);
            break;
        case 'E':
            printf("Funcion: Listar Comisiones\n");
            listarComisiones(congreso);
            break;
        case 'F':
            return; // Salir del menú
        default:
            printf("Opcion invalida, por favor intente otra vez.\n");
            break;
        }
    }
}


void mostrarLeyesPorFase(struct nodoProyectoLey* leyes, int faseRequerida) {
    // Verifica si el árbol no está vacío
    if (leyes != NULL) {
        // Verifica si el nodo actual está en la fase requerida
        if (leyes->datos->fase == faseRequerida) {
            printf("Nombre de Ley: %s\n", leyes->datos->nombre);
            printf("Tipo de Ley: %s\n", leyes->datos->tipo);
            printf("Urgencia de Ley: %d\n", leyes->datos->urgencia);
            printf("ID de Ley: %d\n", leyes->datos->idProyecto);
            printf("---------------------------------\n"); // Separador para claridad
        }

        // Llama recursivamente para los hijos izquierdo y derecho
        mostrarLeyesPorFase(leyes->izq, faseRequerida);
        mostrarLeyesPorFase(leyes->der, faseRequerida);
    }
}


int main(void) {
    int flag = 1; // Variable de control del bucle principal
    char opcion;
    struct congreso* congreso;

    // Inicialización del congreso
    congreso = inicializarCongreso();

    // Bucle principal del menú
    while (flag == 1) {
        printf("Opciones:\n"
            "A: Proyectos de Ley.\n"
            "B: Congresistas.\n"
            "C: Comisiones.\n"
            "D: Salir.\n\n");

        opcion = leerOpcion(); // Lee solo un carácter y limpia el buffer

        // Manejo de las opciones seleccionadas
        switch (opcion) {
        case 'A':
            funcionSwitch(opcion, congreso, menuProyectosLey);
            break;
        case 'B':
            funcionSwitch(opcion, congreso, menuCongresistas);
            break;
        case 'C':
            funcionSwitch(opcion, congreso, menuComisiones);
            break;
        case 'D':
            flag = 0; // Cambiar la bandera para salir del bucle
            break;
        default:
            printf("Opción inválida, por favor intente otra vez.\n");
            break;
        }
    }

    return 0;
}
