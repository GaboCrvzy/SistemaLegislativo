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
    int ocupacion;        // (1) DIPUTADOS  (2) SENADORES
    char* especializacion;
};

struct nodoCongresista {
    struct congresista* datos;
    struct nodoCongresista* sig;
};

struct congreso {
    struct congresista** diputados;
    int maxDiputados;
    struct congresista** senadores;
    int maxSenadores;
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
    int tipo;           // (1) DIPUTADOS  (2) SENADORES
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
char leerOpcion();
struct congreso* inicializarCongreso();
struct nodoProyectoLey* crearNodoProyectoLey(struct proyectoLey* datos);
void agregarNodoProyectoLey(struct congreso* congreso);
struct proyectoLey* buscarProyectoLeyPorID(struct nodoProyectoLey* raiz, int id);
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

struct proyectoLey* crearProyectoLey(int idProyecto,char *nombre, char *tipo, int urgencia, int fase ) {

    struct proyectoLey *nuevoProyecto;
    nuevoProyecto = (struct proyectoLey*)malloc(sizeof(struct proyectoLey));

    nuevoProyecto->idProyecto = idProyecto;

    nuevoProyecto->nombre = (char *)malloc((strlen(nombre) + 1) * sizeof(char));
    strcpy(nuevoProyecto->nombre, nombre);

    nuevoProyecto->tipo = (char *)malloc((strlen(tipo) + 1) * sizeof(char));
    strcpy(nuevoProyecto->tipo, tipo);

    nuevoProyecto->urgencia = urgencia;
    nuevoProyecto->fase = fase;

    return nuevoProyecto;
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

struct congresista *crearCongresista(char *nombre, char *rut, int ocupacion, char *especializacion)
{
    struct congresista* nuevoCongresista = NULL;
    nuevoCongresista = (struct congresista*)malloc(sizeof(struct congresista));

    nuevoCongresista->nombre = (char*)malloc(sizeof(char) * strlen(nombre));
    nuevoCongresista->rut = (char*)malloc(sizeof(char) * strlen(rut));
    nuevoCongresista->especializacion = (char*)malloc(sizeof(char) * strlen(especializacion));

    strcpy(nuevoCongresista->nombre, nombre);
    strcpy(nuevoCongresista->rut, rut);
    nuevoCongresista->ocupacion = ocupacion;
    strcpy(nuevoCongresista->especializacion, especializacion);

    return nuevoCongresista;
}

struct congresista *buscarCongresistaEnArreglo(struct congresista **arreglo, int tamMax, char *rutBuscado)
{
    struct congresista *datosCongresista = NULL;
    int i;

    for(i = 0; i < tamMax; i++)
    {
        if(arreglo[i] != NULL && strcmp(arreglo[i]->rut, rutBuscado) == 0)
        {
            datosCongresista = arreglo[i];
        }
    }
    return datosCongresista;
}

int agregarCongresistaEnArreglo(struct congresista **arreglo, int tamMax, struct congresista *nuevoCongresista)
{
    int i;
    for(i = 0; i < tamMax; i++)
    {
        if(arreglo[i] == NULL && buscarCongresistaEnArreglo(arreglo, tamMax, nuevoCongresista->rut) == NULL)
        {
            arreglo[i] = nuevoCongresista;
            return 1;
        }
    }
    return 0;
}

int eliminarCongresistaEnArreglo(struct congresista **arreglo, int tamMax, char *rutEliminar)
{
  int i;

  for(i = 0; i < tamMax; i++)
  {
    if(arreglo[i] != NULL && strcmp(arreglo[i]->rut, rutEliminar) == 0)
    {
      arreglo[i] = NULL;
      return 1;
    } 
  }
  return 0;
}

void mostrarCongresistas(struct congresista **arreglo, int tamMax)
{
  int i;
  
  for(i = 0; i < tamMax; i++)
  {
    if(arreglo[i] != NULL)
    {
      printf("Nombre: %s\n", arreglo[i]->nombre);
      printf("RUT: %s\n", arreglo[i]->rut);
      printf("Ocupacion: %d\n", arreglo[i]->ocupacion);
      printf("Especializacion: %s\n", arreglo[i]->especializacion);
      printf("\n");
    }
  }
}

struct nodoCongresista *enlazarCongresista(struct congresista *nuevoCongresista)
{
    struct nodoCongresista *nodoNuevo = NULL;

    if (nuevoCongresista != NULL) 
    {
        nodoNuevo = (struct nodoCongresista*)malloc(sizeof(struct nodoCongresista));
        nodoNuevo->datos = nuevoCongresista; 
        nodoNuevo->sig = NULL;
    }
    return nodoNuevo;
}

struct congresista *buscarCongresistaEnLista(struct nodoCongresista *head, char *rutBuscado)
{
  struct nodoCongresista *rec;
  struct congresista *congresista = NULL;

  rec = head;
  if(head != NULL)
  {
    while(rec != NULL)
    {
      if(strcmp(rec->datos->rut, rutBuscado) == 0)
      {
        congresista = rec->datos;
        return congresista;
      }
      rec = rec->sig;
    }
  }
  return NULL;
}

int agregarCongresistaEnComisionLista(struct nodoCongresista **head, struct congresista *nuevoCongresista)
{
  struct nodoCongresista *rec, *nodoNuevo = NULL;
  
  if (*head == NULL)
  {
    (*head)->datos = nuevoCongresista;
    (*head)->sig = NULL;
    return 1;
  }

  else
  {
    rec = *head;
    
    while(rec->sig != NULL)
      rec = rec->sig;

    if(buscarCongresistaEnLista(*head, nuevoCongresista->rut) == NULL)
    {
      nodoNuevo = enlazarCongresista(nuevoCongresista);
      rec->sig = nodoNuevo;
      return 1;
    }
  }
  return 0;
}

int eliminarCongresistaEnComisionLista(struct nodoCongresista **head, char *rutEliminar)
{
  struct nodoCongresista *rec;

  if(*head != NULL && strcmp( (*head)->datos->rut, rutEliminar) == 0)
  {
    *head = (*head)->sig;
    return 1;
  }

  else
  {
    rec = *head;

    while(rec->sig != NULL)
    {
      if(strcmp(rec->sig->datos->rut, rutEliminar) == 0)
      {
        rec->sig = rec->sig->sig;
        return 1;
      }
      rec = rec->sig;
    }
  }
  return 0;
}

void mostrarCongresistaEnComisionLista(struct nodoCongresista *head)
{
  struct nodoCongresista *rec;
  rec = head;

  while(rec != NULL)
  {
    printf("Nombre: %s\n", rec->datos->nombre);
    printf("RUT: %s\n", rec->datos->rut);
    printf("Ocupacion: %d\n", rec->datos->ocupacion);
    printf("Especializacion: %s\n", rec->datos->especializacion);
    printf("\n");

    rec = rec->sig;
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
    char nombre[100], descripcion[256];
    int tipo;

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
    scanf("%d", &tipo);
    nuevaComision->nombre = (char*)malloc(sizeof(char) * (strlen(nombre) + 1));
    nuevaComision->descripcion = (char*)malloc(sizeof(char) * (strlen(descripcion) + 1));
    nuevaComision->tipo = tipo;
    strcpy(nuevaComision->nombre, nombre);
    strcpy(nuevaComision->descripcion, descripcion);

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

    // Agregar como comisión mixta
    nuevoNodo = crearNodoComision(nuevaComision);
    nuevoNodo->sig = congreso->comisionesMixtas;
    congreso->comisionesMixtas = nuevoNodo;
    printf("Comision agregada como mixta\n");
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
    char nuevoNombre[100], nuevaDescripcion[256], rut[20];
    int tipo;

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
        scanf("%d", &tipo);

        comisionAModificar->tipo = tipo;
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
    printf("Tipo: %d\n", buscada->tipo);

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
            printf("Tipo: %d\n", congreso->comisiones[i]->tipo);
            printf("\n");
        }
    }

    printf("Comisiones mixtas:\n");
    rec = congreso->comisionesMixtas;
    while (rec != NULL) {
        if (rec->datos != NULL) { // Verificar que los datos no sean NULL
            printf("Nombre: %s\n", rec->datos->nombre);
            printf("Descripcion: %s\n", rec->datos->descripcion);
            printf("Tipo: %d\n", rec->datos->tipo);
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

void menuCongresistas(struct congreso* congreso,struct congresista **arreglo) {
    struct congresista* nuevoCongresista = NULL;
    int tipo;
    char rut[20];
    int opcion;
    do {
        printf("\n=== Menu de Congresistas ===\n");
        printf("1. Agregar Diputado\n");
        printf("2. Eliminar Diputado\n");
        printf("3. Buscar Politico\n");
        printf("4. Mostrar Diputados\n");
        printf("5. Agregar Senador\n");
        printf("6. Eliminar Senador\n");
        printf("7. Mostrar Senadores\n");
        printf("8. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                agregarCongresistaEnArreglo(arreglo,congreso->maxDiputados,nuevoCongresista);
                break;
            case 2:
                eliminarCongresistaEnArreglo(arreglo,congreso->maxDiputados,rut);
                break;
            case 3:
                printf("Ingrese 1 para diputado o 2 para senador\n");
                scanf("%s",&tipo);
                if(tipo ==1) {
                    buscarCongresistaEnArreglo(arreglo,congreso->maxDiputados,rut);
                }else {
                    buscarCongresistaEnArreglo(arreglo,congreso->maxSenadores,rut);
                }
                break;
            case 4:
                mostrarCongresistas(arreglo,congreso->maxDiputados);
                break;
            case 5:
                agregarCongresistaEnArreglo(arreglo,congreso->maxSenadores,nuevoCongresista);
                break;
            case 6:
                eliminarCongresistaEnArreglo(arreglo,congreso->maxSenadores,rut);
                break;
            case 7:
                mostrarCongresistas(arreglo,congreso->maxSenadores);
                break;
            case 8:
                printf("Volviendo al Menu Principal...\n");
                return;
            default:
                printf("Opcion no valida. Por favor, intente nuevamente.\n");
                break;
        }

    } while(opcion != 7);
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
