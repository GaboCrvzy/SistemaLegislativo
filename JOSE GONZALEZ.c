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
    int maxComisiones;
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
    struct nodoVotacion* votacion;  //Votaciones sobre el proyec en comision o camara
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
    int idComision;
    char* descripcion;
};

struct nodoVotacion {
    struct votacion* datos;
    struct nodoVotacion* sig;
};

struct votacion {
    int idVotacion;
    int idProyecto;
    int tipoVotacion;  // (1) Comision  (2) Camara
    int totalFavor;
    int totalContra;
    int totalAbstenciones;
    int resultado;    // (1) Aprobado (0) rechazado
    struct nodoCongresista* favor;
    struct nodoCongresista* contra;
    struct nodoCongresista* abstenciones;
};

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
    nuevoCongreso->diputados = calloc(MAX_DIPUTADOS, sizeof(struct congresista*));
    nuevoCongreso->maxDiputados = 100;
    nuevoCongreso->senadores = calloc(MAX_SENADORES, sizeof(struct congresista*));
    nuevoCongreso->maxSenadores = 50;
    nuevoCongreso->comisionesMixtas = NULL;

    // Inicializa los arreglos para las comisiones
    nuevoCongreso->comisiones = calloc(MAX_COMISIONES, sizeof(struct comision*));
    nuevoCongreso->maxComisiones = 20;

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
  return raiz;
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
    else return buscarProyectoLeyPorID(raiz->der, id);
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

/*TODO: FUNCIONES DE VOTACION------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

struct votacion *crearVotacion(int idVotacion, int idProyecto, int tipoVotacion)
{
    struct votacion *nuevaVotacion = NULL;

    nuevaVotacion = (struct votacion*)malloc(sizeof(struct votacion));

    nuevaVotacion->idVotacion = idVotacion;
    nuevaVotacion->idProyecto = idProyecto;
    nuevaVotacion->tipoVotacion = tipoVotacion;
    nuevaVotacion->totalFavor = 0;
    nuevaVotacion->totalContra = 0;
    nuevaVotacion->totalAbstenciones = 0;
    nuevaVotacion->resultado = -1;   //Sin calculo aun
    nuevaVotacion->favor = NULL;
    nuevaVotacion->contra = NULL;
    nuevaVotacion->abstenciones = NULL;

    return nuevaVotacion;
}

struct votacion *buscarVotacionEnLista(struct nodoVotacion *head, int idVotacion)
{
    while (head != NULL)
    {
        if (head->datos->idVotacion == idVotacion)
            return head->datos;

        head = head->sig;
    }
    return NULL;
}

int enlazarVotacion(struct nodoVotacion **head, struct votacion *votacionNueva)
{
    struct nodoVotacion *rec, *nuevoNodo = NULL;

    nuevoNodo = (struct nodoVotacion*)malloc(sizeof(struct nodoVotacion));
    nuevoNodo->datos = votacionNueva;
    nuevoNodo->sig = NULL;

    if(*head == NULL)
    {
        *head = nuevoNodo;
        return 1;
    }
    else
    {
        if(buscarVotacionEnLista(*head, votacionNueva->idVotacion) == NULL)
        {
            rec = *head;
            while(rec->sig != NULL)
                rec = rec->sig;

            rec->sig = nuevoNodo;
            return 1;
        }
    }
    return 0;
}

int registrarVotoDeCongresitaEnVotacion(struct votacion *votacion, struct congresista *congresista, int tipoVoto) 
{
    struct nodoCongresista *nuevoNodo = NULL;

    if (buscarCongresistaEnLista(votacion->favor, congresista->rut) ||buscarCongresistaEnLista(votacion->contra, congresista->rut) ||
        buscarCongresistaEnLista(votacion->abstenciones, congresista->rut)) 
    {
        return 0; // Ya votó
    }

    nuevoNodo = enlazarCongresista(congresista);

    // Determinar la lista de votos a usar
    if (tipoVoto == 1) 
    { // A favor
        nuevoNodo->sig = votacion->favor;
        votacion->favor = nuevoNodo;
        votacion->totalFavor++;
    } 
    else if (tipoVoto == 2)
    { // En contra
        nuevoNodo->sig = votacion->contra;
        votacion->contra = nuevoNodo;
        votacion->totalContra++;
    } 
    else
    { // Abstención
        nuevoNodo->sig = votacion->abstenciones;
        votacion->abstenciones = nuevoNodo;
        votacion->totalAbstenciones++;
    } 

    return 1; // Voto registrado
}

int calcularResultadoVotacion(struct votacion *votacion) 
{
    if (votacion->totalFavor > votacion->totalContra)
    {
        votacion->resultado = 1; // Aprobado
    } 
    else 
    {
        votacion->resultado = 0; // Rechazado
    }
    return votacion->resultado;
}

void mostrarVotantes(struct votacion *votacion) 
{
    printf("Congresistas que votaron a favor:\n");
    mostrarCongresistaEnComisionLista(votacion->favor);

    printf("Congresistas que votaron en contra:\n");
    mostrarCongresistaEnComisionLista(votacion->contra);

    printf("Congresistas que se abstuvieron:\n");
    mostrarCongresistaEnComisionLista(votacion->abstenciones);
}

void mostrarVotaciones(struct nodoVotacion *head)
{
    struct nodoVotacion *rec = head;
    while (rec != NULL)
    {
        printf("ID Votación: %d\n", rec->datos->idVotacion);
        printf("ID Proyecto: %d\n", rec->datos->idProyecto);
        printf("Tipo Votación: %d\n", rec->datos->tipoVotacion);
        printf("Total Favor: %d\n", rec->datos->totalFavor);
        printf("Total Contra: %d\n", rec->datos->totalContra);
        printf("Total Abstenciones: %d\n", rec->datos->totalAbstenciones);
        printf("Resultado: %s\n", rec->datos->resultado == 1 ? "Aprobado" : "Rechazado");
        printf("--------------------\n");
        rec = rec->sig;
    }
}

/*TODO: FUNCIONES DE COMISIONES------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

struct comision *crearComision(char *nombre, int tipoComision, int idComision, char *descripcion)
{
    struct comision *nuevaComision = NULL;

    nuevaComision = (struct comision*)malloc(sizeof(struct comision));

    nuevaComision->nombre = (char *)malloc(strlen(nombre) + 1);
    strcpy(nuevaComision->nombre, nombre);

    nuevaComision->tipo = tipoComision;
    nuevaComision->idComision = idComision;

    nuevaComision->descripcion = (char *)malloc(strlen(descripcion) + 1);
    strcpy(nuevaComision->descripcion, descripcion);
    nuevaComision->headIntegrantes = NULL;

    return nuevaComision;
}

struct comision *buscarComisionEnArreglo(struct comision **arreglo, int maxComisiones, int idComisionBuscada)
{
    struct comision *comisionBuscada = NULL;
    int i;

    for(i = 0; i < maxComisiones; i++)
    {
        if(arreglo[i] != NULL && arreglo[i]->idComision == idComisionBuscada)
        {
            comisionBuscada = arreglo[i];
            return comisionBuscada;
        }
    }
    return NULL;
}

int agregarComisionEnArreglo(struct comision **arreglo, int maxComisiones, struct comision *nuevaComision)
{
    int i;

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] != NULL && arreglo[i]->tipo == nuevaComision->tipo && strcmp(arreglo[i]->nombre, nuevaComision->nombre) == 0)
            return 0;
    }

    for(i = 0; i < maxComisiones; i++)
    {
        if(arreglo[i] == NULL)
        {
            arreglo[i] = nuevaComision;
            return 1;
        }   
    }
    return 0;
}

int eliminarComisionEnArreglo(struct comision **arreglo, int maxComisiones, int idComisionEliminar)
{
    int i;

    for(i = 0; i < maxComisiones; i++)
    {
        if(arreglo[i] != NULL && arreglo[i]->idComision == idComisionEliminar)
        {
            arreglo[i] = NULL;
            return 1;
        }
    }
    return 0;
}

void mostrarComisionPorID(struct comision **arreglo, int maxComisiones, int idComision)
{
    int i;

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] != NULL && arreglo[i]->idComision == idComision)
        {
            printf("Comisión ID: %d\n", arreglo[i]->idComision);
            printf("Nombre: %s\n", arreglo[i]->nombre);
            printf("Tipo: %d\n", arreglo[i]->tipo);
            printf("Descripción: %s\n", arreglo[i]->descripcion);
            return;
        }
    }
    printf("Comisión con ID %d no encontrada.\n", idComision);
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
void mostrarProyectoLeyMenu(struct congreso *congreso) 
{
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

void crearEInsertarProyectoLeyMenu(struct congreso *congreso)
{
    int id, urgencia, fase;
    char nombre[50], tipo[50];
    struct proyectoLey *nuevoProyecto = NULL;

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

    nuevoProyecto = crearProyectoLey(id, nombre, tipo, urgencia, fase);
    congreso->raiz = insertarProyectoLey(congreso->raiz, nuevoProyecto);
    printf("Proyecto de Ley con ID %d insertado exitosamente.\n", id);
}

void buscarProyectoLeyMenu(struct congreso *congreso) 
{
    int id;
    struct proyectoLey *proyecto = NULL;

    printf("\nIngrese el ID del Proyecto de Ley a buscar: ");
    scanf("%d", &id);

    proyecto = buscarProyectoLeyPorID(congreso->raiz, id);
    if (proyecto == NULL) {
        printf("Proyecto con ID %d no encontrado.\n", id);
    } else {
        printf("Proyecto encontrado:\n");
        printf("ID: %d, Nombre: %s, Tipo: %s, Urgencia: %d, Fase: %d\n",
               proyecto->idProyecto, proyecto->nombre, proyecto->tipo,
               proyecto->urgencia, proyecto->fase);
    }
}
void menuProyectosLey(struct congreso *congreso) 
{
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

void listarCongresistas(struct congreso *congreso)
{
    printf("\n--- Lista de Diputados ---\n");
    mostrarCongresistas(congreso->diputados, congreso->maxDiputados);
    printf("\n--- Lista de Senadores ---\n");
    mostrarCongresistas(congreso->senadores, congreso->maxSenadores);
}

void buscarCongresista(struct congreso *congreso)
{
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

void agregarCongresista(struct congreso *congreso)
{
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

void eliminarCongresista(struct congreso *congreso) 
{
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

void menuCongresistas(struct congreso* congreso) 
{
    int opcion;
    while (1) {
        printf("Menu Congresistas.\n");
        printf("Opcion 1: Crear y Agregar Congresista\n");
        printf("Opcion 2: Eliminar Congresista\n");
        printf("Opcion 3: Buscar Congresista\n");
        printf("Opcion 4: Listar Congresistas\n");
        printf("Opcion 5: Volver al menu principal\n");
        scanf("%d",&opcion);

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

void menuComisiones(struct congreso* congreso)
{
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
    char opcion;
    struct congreso* congreso;
    int flag = 1; // Variable de control del bucle principal

    // Inicialización del congreso
    congreso = inicializarCongreso();

    // Bucle principal del menú
    while (flag == 1) {
        printf("Opciones:\n"
               "A: Proyectos de Ley.\n"
               "B: Congresistas.\n"
               "C: Comisiones.\n"
               "D: Salir.\n\n");

        // Leer la opción seleccionada por el usuario
        printf("Seleccione una opción: ");
        scanf(" %c", &opcion);  // Espacio antes de %c para capturar correctamente la entrada

        // Manejo de las opciones seleccionadas
        switch (opcion) {
        case 'A':
            funcionSwitch(opcion, congreso, menuCongresistas);
            break;
        case 'B':
            flag = 0; // Cambiar la bandera para salir del bucle
            break;
        case 'C':
            // Aquí iría la función para manejar la opción C, si la tienes.
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
