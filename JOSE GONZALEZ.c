#include <ctype.h>
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
struct congreso* inicializarCongreso();
void mostrarCongresistasVotacion(struct nodoCongresista* lista, const char* categoria);
void agregarCongresistaAVotacion(struct votacion* votacion, struct congreso* congreso);
void agregarVotacion(struct congreso* congreso, int idProyecto);
void recorrerArbolEnOrden(struct nodoProyectoLey* nodo, struct proyectoLey*** proyectosArray, int* proyectosCount, int* proyectosCapacidad);
int compararPorUrgencia(const void* a, const void* b);
struct nodoComision* crearNodoComision(struct comision* datos);
struct comision* buscarComision(struct congreso* congreso, char* nombre);
struct comision* crearComision(struct congreso* congreso);
void agregarComision(struct congreso* congreso);
void eliminarComision(struct congreso* congreso, char* nombre);
void modificarComision(struct congreso* congreso, char* nombre);
void mostrarComisionPorNombre(struct congreso* congreso, char* nombre);
void listarComisiones(struct congreso* congreso);
void funcionSwitch(char opcion, struct congreso* congreso, void (*submenu)(struct congreso*));
void menuCongresistas(struct congreso* congreso);
void menuComisiones(struct congreso* congreso);
/*NOTA: LAS FUNCIONES DE VOTACIÓN SE ENCUENTRAN EN EL APARTADO DE LAS FUNCIONES DE PROYECTO DE LEY.*/


/*TODO: FUNCIONES AUXILIARES---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int validarRUTSinFormato(const char *rut) {
    int largo = strlen(rut);

    // El RUT debe tener al menos 2 caracteres (números + dígito verificador)
    if (largo < 2 || largo > 10) return 0;

    // Verifica que todos los caracteres menos el último sean dígitos
    for (int i = 0; i < largo - 1; i++) {
        if (!isdigit(rut[i])) return 0;
    }

    // Verifica que el último carácter sea un número o 'K/k'
    char verificador = rut[largo - 1];
    if (!isdigit(verificador) && tolower(verificador) != 'k') return 0;

    return 1; // RUT válido
}

void solicitarRUT(char *rut) {
    while (1) {
        printf("Ingrese el RUT (sin puntos ni guion, con digito verificador): ");
        scanf(" %s", rut);

        if (validarRUTSinFormato(rut)) {
            printf("RUT valido: %s\n", rut);
            break;
        } else {
            printf("RUT no valido. Intente nuevamente.\n");
        }
    }
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
    nuevoCongreso->comisionesMixtas = NULL;

    // Inicializa los arreglos para las comisiones
    nuevoCongreso->comisiones = calloc(MAX_COMISIONES, sizeof(struct comision*));

    // Inicializa la raíz de proyectos de ley
    nuevoCongreso->raiz = NULL;
    return nuevoCongreso;
}


//TODO: FUNCIÓNES DEL PROYECTO DE LEY----------------------------------------------------------------------------------------------------------------------------------------------------//
//TODO-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
struct proyectoLey* crearProyectoLey(int idProyecto, char* nombre, char* tipo, int urgencia, int fase)
{
  struct proyectoLey *nuevoProyecto = NULL;

  nuevoProyecto = (struct proyectoLey*)malloc(sizeof(struct proyectoLey));

  nuevoProyecto->idProyecto = idProyecto;
  nuevoProyecto->nombre = (char*)malloc(strlen(nombre)+1);
  strcpy(nuevoProyecto->nombre, nombre);

  nuevoProyecto->tipo = (char*)malloc(strlen(tipo)+1);
  strcpy(nuevoProyecto->tipo, tipo);

  nuevoProyecto->urgencia = urgencia;
  nuevoProyecto->fase = fase;
  nuevoProyecto->votacion = NULL;

  return nuevoProyecto;
}

struct nodoProyectoLey* insertarProyectoLey(struct nodoProyectoLey *raiz, struct proyectoLey *proyecto)
{
  struct nodoProyectoLey *nuevoNodo = NULL;

  if (raiz == NULL)
  {
    nuevoNodo = (struct nodoProyectoLey*)malloc(sizeof(struct nodoProyectoLey));
    nuevoNodo->datos = proyecto;
    nuevoNodo->izq = NULL;
    nuevoNodo->der = NULL;
    return nuevoNodo;
  }

  if (proyecto->idProyecto < raiz->datos->idProyecto)
  {
    raiz->izq = insertarProyectoLey(raiz->izq, proyecto);
  }
  else if (proyecto->idProyecto > raiz->datos->idProyecto)
  {
    raiz->der = insertarProyectoLey(raiz->der, proyecto);
  }
  else {
      return raiz;
  }
}

struct proyectoLey* buscarProyectoLeyPorID(struct nodoProyectoLey *raiz, int id)
{
    if (raiz == NULL) {
        return NULL;
    }

    if (raiz->datos->idProyecto == id) {
        return raiz->datos;
    }
    else if (id < raiz->datos->idProyecto) {
        return buscarProyectoLeyPorID(raiz->izq, id);
    }
    else {
}

        return buscarProyectoLeyPorID(raiz->der, id);
    }
int modificarProyectoLey(struct nodoProyectoLey *raiz, int idBuscado, int faseNueva)
{
  struct proyectoLey *proyecto = NULL;

  proyecto = buscarProyectoLeyPorID(raiz, idBuscado);

  if (proyecto == NULL) return 0;

  proyecto->fase = faseNueva;
  return 1;
}

void mostrarProyectoLeyPorID(struct nodoProyectoLey *raiz, int idBuscado)
{
  struct proyectoLey *proyecto = NULL;
  proyecto = buscarProyectoLeyPorID(raiz, idBuscado);

  if (proyecto == NULL)
  {
     printf("Proyecto con ID %d no encontrado.\n", idBuscado);
    return;
  }

  printf("Proyecto encontrado:\n");
  printf("ID: %d\n", proyecto->idProyecto);
  printf("Nombre: %s\n", proyecto->nombre);
  printf("Tipo: %s\n", proyecto->tipo);
  printf("Urgencia : %d\n", proyecto->urgencia);
  printf("Fase: %d\n", proyecto->fase);
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

/*TODO: FUNCIONES DE CONGRESISTAS------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

struct congresista *crearCongresista(char *nombre, char *rut, int ocupacion, char *especializacion)
{
    struct congresista* nuevoCongresista = NULL;
    nuevoCongresista = (struct congresista*)malloc(sizeof(struct congresista));

    nuevoCongresista->nombre = (char *)malloc(strlen(nombre) + 1);
    nuevoCongresista->rut = (char *)malloc(strlen(rut) + 1);
    nuevoCongresista->especializacion = (char *)malloc(strlen(especializacion) + 1);

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
                printf("- Ocupacion: %dn\n", rec->datos->ocupacion);
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
void mostrarProyectoLeyMenu(struct congreso *congreso) {
    int id;

    printf("\nIngrese el ID del Proyecto de Ley a mostrar: ");
    scanf("%d", &id);

    mostrarProyectoLeyPorID(congreso->raiz, id);
}
void modificarProyectoLeyMenu(struct congreso *congreso) {
    int id, nuevaFase;

    printf("\nIngrese el ID del Proyecto de Ley a modificar: ");
    scanf("%d", &id);
    do {
        printf("Ingrese la nueva fase (1-5): ");
        scanf("%d", &nuevaFase);
        if (nuevaFase < 1 || nuevaFase > 5) {
            printf("Error: la fase debe estar entre 1 y 5. Intente nuevamente.\n");
        }
    } while (nuevaFase < 1 || nuevaFase > 5);

    if (modificarProyectoLey(congreso->raiz, id, nuevaFase)) {
        printf("Proyecto de Ley con ID %d actualizado exitosamente.\n", id);
    } else {
        printf("No se pudo encontrar o modificar el Proyecto de Ley con ID %d.\n", id);
    }
}
void crearEInsertarProyectoLeyMenu(struct congreso *congreso) {
    int id, urgencia, fase;
    char nombre[50], tipo[50];

    printf("\n--- Crear e Insertar Proyecto de Ley ---\n");
    printf("Ingrese los datos del Proyecto de Ley:\n");
    printf("ID del Proyecto: ");
    scanf("%d", &id);

    // Verificar si ya existe un proyecto con este ID
    if (buscarProyectoLeyPorID(congreso->raiz, id) != NULL) {
        printf("Ya existe un Proyecto de Ley con el ID %d. Operacion cancelada.\n", id);
        return;
    }

    printf("Nombre: ");
    scanf(" %[^\n]", nombre);
    printf("Tipo: ");
    scanf(" %[^\n]", tipo);
    printf("Urgencia (0: Baja, 1: Media, 2: Alta): ");
    scanf("%d", &urgencia);
    do {
        printf("Fase (1-5): ");
        scanf("%d", &fase);
        if (fase < 1 || fase > 5) {
            printf("Error: la fase debe estar entre 1 y 5. Intente nuevamente.\n");
        }
    } while (fase < 1 || fase > 5);

    // Crear el proyecto de ley
    struct proyectoLey *nuevoProyecto = crearProyectoLey(id, nombre, tipo, urgencia, fase);
    congreso->raiz = insertarProyectoLey(congreso->raiz, nuevoProyecto);
    printf("Proyecto de Ley con ID %d insertado exitosamente.\n", id);
}
void buscarProyectoLeyMenu(struct congreso *congreso) {
    int id;

    printf("\nIngrese el ID del Proyecto de Ley a buscar: ");
    scanf("%d", &id);

    struct proyectoLey *proyecto = buscarProyectoLeyPorID(congreso->raiz, id);
    if (proyecto == NULL) {
        printf("Proyecto con ID %d no encontrado.\n", id);
    } else {
        printf("Proyecto encontrado:\n");
        printf("ID: %d, Nombre: %s, Tipo: %s, Urgencia: %d, Fase: %d\n",
               proyecto->idProyecto, proyecto->nombre, proyecto->tipo,
               proyecto->urgencia, proyecto->fase);
    }
}
void menuProyectosLey(struct congreso *congreso) {
    int opcion;

    while (1) {
        printf("\n--- Menu de Proyectos de Ley ---\n");
        printf("1: Crear e Insertar Proyecto de Ley\n");
        printf("2: Buscar Proyecto de Ley por ID\n");
        printf("3: Modificar Fase de Proyecto de Ley\n");
        printf("4: Mostrar Proyecto de Ley por ID\n");
        printf("5: Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                crearEInsertarProyectoLeyMenu(congreso);
                break;
            case 2:
                buscarProyectoLeyMenu(congreso);
                break;
            case 3:
                modificarProyectoLeyMenu(congreso);
                break;
            case 4:
                mostrarProyectoLeyMenu(congreso);
                break;
            case 5:
                return; // Volver al menú principal
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
        }
    }
}

// Función para listar todos los congresistas
void listarCongresistas(struct congreso *congreso) {
    printf("\n--- Lista de Diputados ---\n");
    mostrarCongresistas(congreso->diputados, congreso->maxDiputados);
    printf("\n--- Lista de Senadores ---\n");
    mostrarCongresistas(congreso->senadores, congreso->maxSenadores);
}

// Función para buscar un congresista
void buscarCongresista(struct congreso *congreso) {
    char rut[20];
    solicitarRUT(rut);
    struct congresista *resultado = buscarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut);
    if (resultado == NULL) {
        resultado = buscarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut);
    }

    if (resultado != NULL) {
        printf("Congresista encontrado:\n");
        printf("Nombre: %s\n", resultado->nombre);
        printf("RUT: %s\n", resultado->rut);
        printf("Ocupacion: %d\n", resultado->ocupacion);
        printf("Especializacion: %s\n", resultado->especializacion);
    } else {
        printf("No existe un congresista con el RUT ingresado.\n");
    }
}
// Función para agregar un congresista
void agregarCongresista(struct congreso *congreso) {
    struct congresista *nuevo=NULL;
    char nombre[100], rut[20], especializacion[100];
    int ocupacion;

    printf("\nIngrese los datos del congresista:\n");
    printf("Nombre: ");
    scanf(" %[^\n]", nombre);
    solicitarRUT(rut);
    printf("Especializacion: ");
    scanf(" %[^\n]", especializacion);
    do {
        printf("Ocupacion (1 = Diputado, 2 = Senador): ");
        scanf("%d", &ocupacion);
        if (ocupacion < 1 || ocupacion > 2) {
            printf("Error: la ocupacion debe estar entre 1 y 2. Intente nuevamente.\n");
        }
    } while (ocupacion < 1 || ocupacion > 2);

    nuevo = crearCongresista(nombre, rut, ocupacion, especializacion);
    if (ocupacion == 1) {
        if (agregarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, nuevo)) {
            printf("Diputado agregado exitosamente.\n");
        } else {
            printf("No se pudo agregar el diputado.\n");
        }
    } else if (ocupacion == 2) {
        if (agregarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, nuevo)) {
            printf("Senador agregado exitosamente.\n");
        } else {
            printf("No se pudo agregar el senador.\n");
        }
    }
}

// Función para borrar un congresista
void eliminarCongresista(struct congreso *congreso) {
    char rut[20];
    solicitarRUT(rut);

    if (eliminarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut)) {
        printf("Diputado eliminado exitosamente.\n");
    } else if (eliminarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut)) {
        printf("Senador eliminado exitosamente.\n");
    } else {
        printf("No existe un congresista con el RUT ingresado.\n");
    }
}

void menuCongresistas(struct congreso* congreso) {
    int opcion;
    while (1) {
        printf("Menu Congresistas.\n");
        printf("Opcion 1: Crear y Agregar Congresista\n");
        printf("Opcion 2: Eliminar Congresista\n");
        printf("Opcion 3: Buscar Congresista\n");
        printf("Opcion 4: Listar Congresistas\n");
        printf("Opcion 5: Volver al menu principal\n");
        scanf("%s",&opcion);

        switch (opcion) {
        case 1:
            agregarCongresista(congreso);
            break;
        case 2:
            eliminarCongresista(congreso);
            break;
        case 3:
            buscarCongresista(congreso);
            break;
        case 4:
            listarCongresistas(congreso);
            break;
        case 5:
            return; // Salir del menú
        default:
            printf("Opcion invalida, por favor intente otra vez.\n");
            break;
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
