#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct congresista {
    char* nombre;
    char* rut;
    int ocupacion;        // (1) DIPUTADOS  (2) SENADORES
    char* partidoPolitico;
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
    struct grafoCongreso* relacionesCongreso;
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

struct grafoCongreso {
    int** matrizAdyacencia;  // Matriz de adyacencia
    int numCongresistas;
    struct nodoCongresista* listaCongresistas;
};
/*TODO: FUNCIONES AUXILIARES---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int validarRUTSinFormato(const char* rut)
{
    char verificador;
    int i;
    int largo = strlen(rut);

    // El RUT debe tener al menos 2 caracteres (números + dígito verificador)
    if (largo < 2 || largo > 10) 
        return 0;

    // Verifica que todos los caracteres menos el último sean dígitos
    for (i = 0; i < largo - 1; i++) {
        if (!isdigit(rut[i])) 
            return 0;
    }

    // Verifica que el último carácter sea un número o 'K/k'
    verificador = rut[largo - 1];
    if (!isdigit(verificador) && tolower(verificador) != 'k') 
        return 0;

    return 1;
}

void solicitarRUT(char* rut) {
    while (1) {
        printf("Ingrese el RUT (sin puntos ni guion, con digito verificador): ");
        scanf(" %s", rut);

        if (validarRUTSinFormato(rut)) {
            printf("RUT valido: %s\n", rut);
            break;
        }
        else {
            printf("RUT no valido. Intente nuevamente.\n");
            break;
        }
    }
}
void solicitarPartidoPolitico(char* partido) {
    do {
        printf("Ingrese el nombre del partido politico (solo caracteres alfabeticos): ");
        
        // Leemos la entrada como una cadena de caracteres
        scanf(" %s", partido);  // El espacio antes de %s asegura que se ignore cualquier salto de línea previo
        
        // Verificamos que todos los caracteres sean alfabéticos
        int esValido = 1;
        for (int i = 0; i < strlen(partido); i++) {
            if (!isalpha(partido[i])) {
                esValido = 0;  // Si encontramos un carácter no alfabético, la entrada es inválida
                break;
            }
        }

        // Si la entrada es válida (solo contiene letras), salimos del bucle
        if (esValido) {
            break;  // Salimos del bucle si la palabra es válida
        } else {
            printf("Error: Ingrese solo caracteres alfabeticos, no se permiten numeros ni caracteres especiales.\n");
            while (getchar() != '\n'); // Limpiar el buffer de entrada
        }

    } while (1);  // El bucle continúa hasta que se ingrese una palabra válida
}
//TODO: FUNCIÓN DE INICIALIZACIÓN DEL CONGRESO----------------------------------------------------------------------------------------------------------------------------------------------------//
//TODO: ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
/*Esta función almacena memoria para todos los datos que debe almacenar "congreso"*/
struct congreso* inicializarCongreso()
{
    struct congreso* nuevoCongreso;
    nuevoCongreso = malloc(sizeof(struct congreso));

    nuevoCongreso->maxDiputados = 100;
    nuevoCongreso->diputados = calloc(nuevoCongreso->maxDiputados, sizeof(struct congresista*));
    nuevoCongreso->maxSenadores = 50;
    nuevoCongreso->senadores = calloc(nuevoCongreso->maxSenadores, sizeof(struct congresista*));
    nuevoCongreso->comisionesMixtas = NULL;
    nuevoCongreso->maxComisiones = 20;
    nuevoCongreso->comisiones = calloc(nuevoCongreso->maxComisiones = 20, sizeof(struct comision*));
    nuevoCongreso->raiz = NULL;
    nuevoCongreso->relacionesCongreso = NULL;
    return nuevoCongreso;
}

//TODO: FUNCIÓNES DEL PROYECTO DE LEY----------------------------------------------------------------------------------------------------------------------------------------------------//
//TODO-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
struct proyectoLey* crearProyectoLey(int idProyecto, char* nombre, char* tipo, int urgencia, int fase)
{
    struct proyectoLey* nuevoProyecto;

    nuevoProyecto = (struct proyectoLey*)malloc(sizeof(struct proyectoLey));

    nuevoProyecto->idProyecto = idProyecto;
    nuevoProyecto->nombre = (char*)malloc(strlen(nombre) + 1);
    strcpy(nuevoProyecto->nombre, nombre);

    nuevoProyecto->tipo = (char*)malloc(strlen(tipo) + 1);
    strcpy(nuevoProyecto->tipo, tipo);

    nuevoProyecto->urgencia = urgencia;
    nuevoProyecto->fase = fase;
    nuevoProyecto->votacion = NULL;

    return nuevoProyecto;
}

struct nodoProyectoLey* insertarProyectoLey(struct nodoProyectoLey* raiz, struct proyectoLey* proyecto)
{
    struct nodoProyectoLey* nuevoNodo = NULL;

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

struct proyectoLey* buscarProyectoLeyPorID(struct nodoProyectoLey* raiz, int id)
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


int modificarProyectoLey(struct nodoProyectoLey* raiz, int idBuscado, int faseNueva)
{
    struct proyectoLey* proyecto = NULL;

    proyecto = buscarProyectoLeyPorID(raiz, idBuscado);

    if (proyecto == NULL) return 0;

    proyecto->fase = faseNueva;
    return 1;
}



void mostrarProyectoLeyPorID(struct nodoProyectoLey* raiz, int idBuscado)
{
    struct proyectoLey* proyecto = NULL;
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

struct congresista* crearCongresista(char* nombre, char* rut, int ocupacion, char* partidoPolitico)
{
    struct congresista* nuevoCongresista = NULL;
    nuevoCongresista = (struct congresista*)malloc(sizeof(struct congresista));

    nuevoCongresista->nombre = (char*)malloc(strlen(nombre) + 1);
    nuevoCongresista->rut = (char*)malloc(strlen(rut) + 1);
    nuevoCongresista->partidoPolitico = (char*)malloc(strlen(partidoPolitico) + 1);

    strcpy(nuevoCongresista->nombre, nombre);
    strcpy(nuevoCongresista->rut, rut);
    nuevoCongresista->ocupacion = ocupacion;
    strcpy(nuevoCongresista->partidoPolitico, partidoPolitico);

    return nuevoCongresista;
}

struct congresista* buscarCongresistaEnArreglo(struct congresista** arreglo, int tamMax, char* rutBuscado)
{
    struct congresista* datosCongresista = NULL;
    int i;

    for (i = 0; i < tamMax; i++)
    {
        if (arreglo[i] != NULL && strcmp(arreglo[i]->rut, rutBuscado) == 0)
        {
            datosCongresista = arreglo[i];
        }
    }
    return datosCongresista;
}

int agregarCongresistaEnArreglo(struct congresista** arreglo, int tamMax, struct congresista* nuevoCongresista)
{
    int i;
    for (i = 0; i < tamMax; i++)
    {
        if (arreglo[i] == NULL && buscarCongresistaEnArreglo(arreglo, tamMax, nuevoCongresista->rut) == NULL)
        {
            arreglo[i] = nuevoCongresista;
            return 1;
        }
    }
    return 0;
}

int eliminarCongresistaEnArreglo(struct congresista** arreglo, int tamMax, char* rutEliminar)
{
    int i;

    for (i = 0; i < tamMax; i++)
    {
        if (arreglo[i] != NULL && strcmp(arreglo[i]->rut, rutEliminar) == 0)
        {
            arreglo[i] = NULL;
            return 1;
        }
    }
    return 0;
}

void mostrarCongresistas(struct congresista** arreglo, int tamMax)
{
    int i;

    for (i = 0; i < tamMax; i++)
    {
        if (arreglo[i] != NULL)
        {
            printf("Nombre: %s\n", arreglo[i]->nombre);
            printf("RUT: %s\n", arreglo[i]->rut);
            printf("Ocupacion: %d\n", arreglo[i]->ocupacion);
            printf("Partido Politico: %s\n", arreglo[i]->partidoPolitico);
            printf("\n");
        }
    }
}

struct nodoCongresista* enlazarCongresista(struct congresista* nuevoCongresista)
{
    struct nodoCongresista* nodoNuevo = NULL;

    if (nuevoCongresista != NULL)
    {
        nodoNuevo = (struct nodoCongresista*)malloc(sizeof(struct nodoCongresista));
        nodoNuevo->datos = nuevoCongresista;
        nodoNuevo->sig = NULL;
    }
    return nodoNuevo;
}

struct congresista* buscarCongresistaEnLista(struct nodoCongresista* head, char* rutBuscado)
{
    struct nodoCongresista* rec;
    struct congresista* congresista = NULL;

    rec = head;
    if (head != NULL)
    {
        while (rec != NULL)
        {
            if (strcmp(rec->datos->rut, rutBuscado) == 0)
            {
                congresista = rec->datos;
                return congresista;
            }
            rec = rec->sig;
        }
    }
    return NULL;
}

int agregarCongresistaEnComisionLista(struct nodoCongresista** head, struct congresista* nuevoCongresista)
{
    struct nodoCongresista* rec, * nodoNuevo = NULL;

    if (*head == NULL)
    {
        (*head)->datos = nuevoCongresista;
        (*head)->sig = NULL;
        return 1;
    }

    else
    {
        rec = *head;

        while (rec->sig != NULL)
            rec = rec->sig;

        if (buscarCongresistaEnLista(*head, nuevoCongresista->rut) == NULL)
        {
            nodoNuevo = enlazarCongresista(nuevoCongresista);
            rec->sig = nodoNuevo;
            return 1;
        }
    }
    return 0;
}

int eliminarCongresistaEnComisionLista(struct nodoCongresista** head, char* rutEliminar)
{
    struct nodoCongresista* rec;

    if (*head != NULL && strcmp((*head)->datos->rut, rutEliminar) == 0)
    {
        *head = (*head)->sig;
        return 1;
    }

    else
    {
        rec = *head;

        while (rec->sig != NULL)
        {
            if (strcmp(rec->sig->datos->rut, rutEliminar) == 0)
            {
                rec->sig = rec->sig->sig;
                return 1;
            }
            rec = rec->sig;
        }
    }
    return 0;
}

void mostrarCongresistaEnComisionLista(struct nodoCongresista* head)
{
    struct nodoCongresista* rec;
    rec = head;

    while (rec != NULL)
    {
        printf("Nombre: %s\n", rec->datos->nombre);
        printf("RUT: %s\n", rec->datos->rut);
        printf("Ocupacion: %d\n", rec->datos->ocupacion);
        printf("Partido Politico: %s\n", rec->datos->partidoPolitico);
        printf("\n");

        rec = rec->sig;
    }
}

/*TODO: FUNCIONES DE VOTACION------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

struct votacion* crearVotacion(int idVotacion, int idProyecto, int tipoVotacion)
{
    struct votacion* nuevaVotacion = NULL;

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

struct votacion* buscarVotacionEnLista(struct nodoVotacion* head, int idVotacion)
{
    while (head != NULL)
    {
        if (head->datos->idVotacion == idVotacion)
            return head->datos;

        head = head->sig;
    }
    return NULL;
}

int enlazarVotacion(struct nodoVotacion** head, struct votacion* votacionNueva)
{
    struct nodoVotacion* rec, * nuevoNodo = NULL;

    nuevoNodo = (struct nodoVotacion*)malloc(sizeof(struct nodoVotacion));
    nuevoNodo->datos = votacionNueva;
    nuevoNodo->sig = NULL;

    if (*head == NULL)
    {
        *head = nuevoNodo;
        return 1;
    }
    else
    {
        if (buscarVotacionEnLista(*head, votacionNueva->idVotacion) == NULL)
        {
            rec = *head;
            while (rec->sig != NULL)
                rec = rec->sig;

            rec->sig = nuevoNodo;
            return 1;
        }
    }
    return 0;
}

int registrarVotoDeCongresitaEnVotacion(struct votacion* votacion, struct congresista* congresista, int tipoVoto)
{
    struct nodoCongresista* nuevoNodo = NULL;

    if (buscarCongresistaEnLista(votacion->favor, congresista->rut) || buscarCongresistaEnLista(votacion->contra, congresista->rut) ||
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

int calcularResultadoVotacion(struct votacion* votacion)
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

void mostrarVotantes(struct votacion* votacion)
{
    printf("Congresistas que votaron a favor:\n");
    mostrarCongresistaEnComisionLista(votacion->favor);

    printf("Congresistas que votaron en contra:\n");
    mostrarCongresistaEnComisionLista(votacion->contra);

    printf("Congresistas que se abstuvieron:\n");
    mostrarCongresistaEnComisionLista(votacion->abstenciones);
}

void mostrarVotaciones(struct nodoVotacion* head)
{
    struct nodoVotacion* rec = head;
    while (rec != NULL)
    {
        printf("ID Votacion: %d\n", rec->datos->idVotacion);
        printf("ID Proyecto: %d\n", rec->datos->idProyecto);
        printf("Tipo Votacion (1: Comision, 2: Camara) : %d\n", rec->datos->tipoVotacion);
        printf("Total A Favor: %d\n", rec->datos->totalFavor);
        printf("Total Contra: %d\n", rec->datos->totalContra);
        printf("Total Abstenciones: %d\n", rec->datos->totalAbstenciones);
        printf("Resultado: %s\n", rec->datos->resultado == 1 ? "Aprobado" : "Rechazado");
        printf("--------------------\n");
        rec = rec->sig;
    }
}

/*TODO: FUNCIONES DE COMISIONES------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

struct comision* crearComision(char* nombre, int tipoComision, int idComision, char* descripcion)
{
    struct comision* nuevaComision = NULL;

    nuevaComision = (struct comision*)malloc(sizeof(struct comision));

    nuevaComision->nombre = (char*)malloc(strlen(nombre) + 1);
    strcpy(nuevaComision->nombre, nombre);

    nuevaComision->tipo = tipoComision;
    nuevaComision->idComision = idComision;

    nuevaComision->descripcion = (char*)malloc(strlen(descripcion) + 1);
    strcpy(nuevaComision->descripcion, descripcion);
    nuevaComision->headIntegrantes = NULL;

    return nuevaComision;
}

struct comision* buscarComisionEnArreglo(struct comision** arreglo, int maxComisiones, int idComisionBuscada)
{
    struct comision* comisionBuscada = NULL;
    int i;

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] != NULL && arreglo[i]->idComision == idComisionBuscada)
        {
            comisionBuscada = arreglo[i];
            return comisionBuscada;
        }
    }
    return NULL;
}

int agregarComisionEnArreglo(struct comision** arreglo, int maxComisiones, struct comision* nuevaComision)
{
    int i;

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] != NULL && arreglo[i]->tipo == nuevaComision->tipo && strcmp(arreglo[i]->nombre, nuevaComision->nombre) == 0)
        {
            return 0;
        }
    }

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] == NULL)
        {
            arreglo[i] = nuevaComision;
            return 1;
        }
    }
    return 0;
}

int eliminarComisionEnArreglo(struct comision** arreglo, int maxComisiones, int idComisionEliminar)
{
    int i;

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] != NULL && arreglo[i]->idComision == idComisionEliminar)
        {
            arreglo[i] = NULL;
            return 1;
        }
    }
    return 0;
}

void mostrarComisionPorID(struct comision** arreglo, int maxComisiones, int idComision)
{
    int i;

    for (i = 0; i < maxComisiones; i++)
    {
        if (arreglo[i] != NULL && arreglo[i]->idComision == idComision)
        {
            printf("Comision ID: %d\n", arreglo[i]->idComision);
            printf("Nombre: %s\n", arreglo[i]->nombre);
            printf("Tipo: %d\n", arreglo[i]->tipo);
            printf("Descripcion: %s\n", arreglo[i]->descripcion);
            return;
        }
    }
    printf("Comision con ID %d no encontrada.\n", idComision);
}


/*TODO: FUNCIONES CON LOS SWITCH (MENUS)----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void mostrarProyectoLeyMenu(struct congreso* congreso)
{
    int id;

    printf("\nIngrese el ID del Proyecto de Ley a mostrar: ");
    scanf("%d", &id);

    mostrarProyectoLeyPorID(congreso->raiz, id);
}

void modificarProyectoLeyMenu(struct congreso* congreso) {
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
    }
    else {
        printf("No se pudo encontrar o modificar el Proyecto de Ley con ID %d.\n", id);
    }
}

void crearEInsertarProyectoLeyMenu(struct congreso* congreso)
{
    int id, urgencia, fase;
    char nombre[50], tipo[50];
    struct proyectoLey* nuevoProyecto = NULL;

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
    do {
        printf("Urgencia (0: Baja, 1: Media, 2: Alta): ");
        scanf("%d", &urgencia);
        if (urgencia < 0 || urgencia > 2) {
            printf("Error: La urgencia debe estar entre 0 y 2. Intente nuevamente.\n");
        }
    } while (urgencia < 0 || urgencia > 2);

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

void buscarProyectoLeyMenu(struct congreso* congreso)
{
    int id;
    struct proyectoLey* proyecto = NULL;

    printf("\nIngrese el ID del Proyecto de Ley a buscar: ");
    scanf("%d", &id);

    proyecto = buscarProyectoLeyPorID(congreso->raiz, id);
    if (proyecto == NULL) {
        printf("Proyecto con ID %d no encontrado.\n", id);
    }
    else {
        printf("Proyecto encontrado, para ver detalles seleccione la opcion 4.\n");
    }
}
void crearYAgregarVotacionMenu(struct nodoProyectoLey* raizProyectos)
{
    int idVotacion, idProyecto, tipoVotacion;
    struct votacion* nuevaVotacion;
    struct proyectoLey* proyectoExistente;

    printf("\n--- Crear y Agregar Votacion ---\n");

    printf("ID del Proyecto: ");
    scanf("%d", &idProyecto);
    proyectoExistente = buscarProyectoLeyPorID(raizProyectos, idProyecto);
    if (proyectoExistente == NULL) {
        printf("No existe un proyecto de ley con el ID %d. Operacion cancelada.\n", idProyecto);
        return;
    }

    printf("Ingrese el ID de la Votacion: ");
    scanf("%d", &idVotacion);

    // Verificar si ya existe una votación con este ID
    if (buscarVotacionEnLista(proyectoExistente->votacion, idVotacion) != NULL) {
        printf("Ya existe una votacion con el ID %d. Operacion cancelada.\n", idVotacion);
        return;
    }

    printf("Tipo de Votacion (1: Comision, 2: Camara): ");
    do {
        scanf("%d", &tipoVotacion);
        if (tipoVotacion != 1 && tipoVotacion != 2) {
            printf("Tipo invalido. Ingrese 1 para Comision o 2 para Camara: ");
        }
    } while (tipoVotacion != 1 && tipoVotacion != 2);

    nuevaVotacion = crearVotacion(idVotacion, idProyecto, tipoVotacion);
    if (enlazarVotacion(&proyectoExistente->votacion, nuevaVotacion)) {
        printf("Votacion creada y agregada exitosamente.\n");
    }
    else {
        printf("No se pudo agregar la votacion. Puede que ya exista con el mismo ID.\n");
    }
}

void registrarVotoMenu(struct nodoVotacion* head, struct congresista* congresista) {
    int idVotacion, tipoVoto;
    struct votacion* votacion;

    printf("\n--- Registrar Voto ---\n");
    printf("Ingrese el ID de la votacion: ");
    scanf("%d", &idVotacion);

    votacion = buscarVotacionEnLista(head, idVotacion);
    if (votacion == NULL) {
        printf("No existe una votacion con el ID %d.\n", idVotacion);
        return;
    }

    printf("Ingrese el tipo de voto (1: A Favor, 2: En Contra, 3: Abstencion): ");
    do {
        scanf("%d", &tipoVoto);
        if (tipoVoto < 1 || tipoVoto > 3) {
            printf("Tipo de voto invalido. Ingrese 1, 2 o 3: ");
        }
    } while (tipoVoto < 1 || tipoVoto > 3);

    if (registrarVotoDeCongresitaEnVotacion(votacion, congresista, tipoVoto)) {
        printf("Voto registrado exitosamente.\n");
    }
    else {
        printf("El congresista ya fue registrado en esta votacion.\n");
    }
}

void calcularResultadoMenu(struct nodoVotacion* head) {
    int idVotacion;
    struct votacion* votacion;

    printf("\n--- Calcular Resultado de Votacion ---\n");
    printf("Ingrese el ID de la votacion: ");
    scanf("%d", &idVotacion);

    votacion = buscarVotacionEnLista(head, idVotacion);
    if (votacion == NULL) {
        printf("No se encontro una votacion con el ID %d.\n", idVotacion);
        return;
    }

    calcularResultadoVotacion(votacion);
    printf("Resultado calculado (1: Aprobado, 0: Rechazado) : %d\n", votacion->resultado);
}

void mostrarVotantesMenu(struct nodoVotacion* head) {
    int idVotacion;
    struct votacion* votacion;

    printf("\n--- Mostrar Votantes ---\n");
    printf("Ingrese el ID de la votacion: ");
    scanf("%d", &idVotacion);

    votacion = buscarVotacionEnLista(head, idVotacion);
    if (votacion == NULL) {
        printf("No existe una votacion con el ID %d.\n", idVotacion);
        return;
    }
    mostrarVotantes(votacion);
}

void listarVotacionesMenu(struct nodoVotacion* head) {
    printf("\n--- Listar Todas las Votaciones ---\n");
    mostrarVotaciones(head);
}

void registrarVotoEnProyectoDeLey(struct congreso* congreso) {
    int idProyecto, ocupacion;
    char rut[20];
    struct proyectoLey* proyectoSeleccionado = NULL;
    struct congresista* congresista = NULL;

    // Pedir el ID del Proyecto de Ley
    printf("Ingrese el ID del Proyecto de Ley para registrar voto: ");
    scanf("%d", &idProyecto);

    // Buscar el proyecto de ley por ID
    proyectoSeleccionado = buscarProyectoLeyPorID(congreso->raiz, idProyecto);

    if (proyectoSeleccionado == NULL) {
        printf("No existe el proyecto de ley con ID %d.\n", idProyecto);
        return;
    }

    // Pedir el RUT del congresista
    printf("Ingrese el RUT del Congresista para registrar su voto: ");
    solicitarRUT(rut);

    printf("De que tipo es el congresista (1) Diputado (2) Senador: ");
    do {
        scanf("%d", &ocupacion);
        if (ocupacion != 1 && ocupacion != 2) {
            printf("Tipo invalido. Ingrese 1 para Diputados o 2 para Senadores: ");
        }
    } while (ocupacion != 1 && ocupacion != 2);

    if (ocupacion == 1) {
        congresista = buscarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut);
    }
    else if (ocupacion == 2) {
        congresista = buscarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut);
    }

    if (congresista == NULL) {
        printf("No existe el congresista con RUT %s.\n", rut);
        return;
    }

    // Registrar el voto si el congresista y el proyecto son válidos
    registrarVotoMenu(proyectoSeleccionado->votacion, congresista);
}

void menuProyectosLey(struct congreso* congreso)
{
    int opcion, idProyecto;
    struct proyectoLey* proyectoSeleccionado;

    while (1)
    {
        printf("\n--- Menu de Proyectos de Ley ---\n");
        printf("1: Crear e Insertar Proyecto de Ley\n");
        printf("2: Buscar Proyecto de Ley por ID\n");
        printf("3: Modificar Fase de Proyecto de Ley\n");
        printf("4: Mostrar Proyecto de Ley por ID\n");
        printf("5: Crear y Agregar Votacion a Proyecto\n");
        printf("6: Registrar Voto en Proyecto de Ley\n");
        printf("7: Calcular Resultado de Votacion en Proyecto de Ley\n");
        printf("8: Mostrar Votantes de Votacion en Proyecto de Ley\n");
        printf("9: Listar Todas las Votaciones de Proyecto\n");
        printf("10: Volver al Menu Principal\n");
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
            printf("Ingrese el ID del Proyecto de Ley para agregar votacion: ");
            scanf("%d", &idProyecto);
            proyectoSeleccionado = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
            if (proyectoSeleccionado != NULL) {
                crearYAgregarVotacionMenu(congreso->raiz);
            }
            else {
                printf("No existe un Proyecto asociado a ese ID. Intente Nuevamente\n");
            }
            break;
        case 6:
            registrarVotoEnProyectoDeLey(congreso);
            break;
        case 7:
            printf("Ingrese el ID del Proyecto de Ley para calcular resultado: ");
            scanf("%d", &idProyecto);
            proyectoSeleccionado = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
            if (proyectoSeleccionado != NULL) {
                calcularResultadoMenu(proyectoSeleccionado->votacion);
            }
            else{
                printf("No existe un Proyecto asociado a ese ID. Intente Nuevamente\n");
            }
            break;
        case 8:
            printf("Ingrese el ID del Proyecto de Ley para mostrar votantes: ");
            scanf("%d", &idProyecto);
            proyectoSeleccionado = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
            if (proyectoSeleccionado != NULL) {
                mostrarVotantesMenu(proyectoSeleccionado->votacion);
            }
            else{
                printf("No existe un Proyecto asociado a ese ID. Intente Nuevamente\n");
            }
            break;
        case 9:
            printf("Ingrese el ID del Proyecto de Ley para listar votaciones: ");
            scanf("%d", &idProyecto);
            proyectoSeleccionado = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
            if (proyectoSeleccionado != NULL) {
                listarVotacionesMenu(proyectoSeleccionado->votacion);
            }
            else{
                printf("No existe un Proyecto asociado a ese ID. Intente Nuevamente\n");
            }
            break;
        case 10:
            return;
        default:
            printf("Opcion invalida. Intente nuevamente.\n");
        }
    }
}

void ordenarCongresistasPorRUT(struct congresista **arreglo, int tam)
{
    int gap, i, j;
    struct congresista *temp;

    for (gap = tam / 2; gap > 0; gap /= 2)
    {
        for (i = gap; i < tam; i++)
        {
            if (arreglo[i] == NULL) continue;  // Salta elementos NULL

            temp = arreglo[i];
            for (j = i; j >= gap && arreglo[j - gap] != NULL &&
                 strcmp(arreglo[j - gap]->rut, temp->rut) > 0; j -= gap)
            {
                arreglo[j] = arreglo[j - gap];
            }
            arreglo[j] = temp;
        }
    }
}

void ordenarCongresistasPorNombre(struct congresista **arreglo, int tam) {
    int gap, i, j;
    struct congresista *temp;

    for (gap = tam / 2; gap > 0; gap /= 2)
    {
        for (i = gap; i < tam; i++)
        {
            if (arreglo[i] == NULL) continue;  // Salta elementos NULL

            temp = arreglo[i];
            for (j = i; j >= gap && arreglo[j - gap] != NULL &&
                    strcmp(arreglo[j - gap]->nombre, temp->nombre) > 0; j -= gap)
            {
                arreglo[j] = arreglo[j - gap];
            }
            arreglo[j] = temp;
        }
    }
}

void listarCongresistas(struct congreso* congreso)
{
    printf("\n--- Lista de Diputados ---\n");
    mostrarCongresistas(congreso->diputados, congreso->maxDiputados);
    printf("\n--- Lista de Senadores ---\n");
    mostrarCongresistas(congreso->senadores, congreso->maxSenadores);
}

void ordenarCongresistas(struct congreso *congreso)
{
    int opcion, tipo;

    printf("\n--- Ordenar Congresistas ---\n");
    printf("1 : Ordenar por RUT\n");
    printf("2 : Ordenar por Nombre\n");

    do {
        printf("Seleccione una opcion (1 o 2): ");
        scanf("%d", &opcion);
        if (opcion < 1 || opcion > 2) {
            printf("Opcion invalida. Intente nuevamente.\n");
        }
    } while (opcion < 1 || opcion > 2);

    printf("Seleccione el tipo de Congresista (1) Diputado (2) Senador: \n");

    do {
        scanf("%d", &tipo);
        if (tipo < 1 || tipo > 2) {
            printf("Tipo invalido. Intente nuevamente.\n");
        }
    } while (tipo < 1 || tipo > 2);

    if (tipo == 1)
    {
        if (opcion == 1) {
            ordenarCongresistasPorRUT(congreso->diputados, congreso->maxDiputados);
            printf("Diputados Ordenados Por Rut Correctamente\n");
        } else {
            ordenarCongresistasPorNombre(congreso->diputados, congreso->maxDiputados);
            printf("Diputados Ordenados Por Nombre Correctamente\n");
        }
    }
    else if (tipo == 2)
    {
        if (opcion == 1) {
            ordenarCongresistasPorRUT(congreso->senadores, congreso->maxSenadores);
            printf("Senadores Ordenados Por Rut Correctamente\n");
        } else {
            ordenarCongresistasPorNombre(congreso->senadores, congreso->maxSenadores);
            printf("Senadores Ordenados Por Nombre Correctamente\n");
        }
    }
}


void buscarCongresista(struct congreso* congreso)
{
    struct congresista* resultado;
    char rut[20];
    solicitarRUT(rut);
    resultado = buscarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut);
    if (resultado == NULL) {
        resultado = buscarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut);
    }

    if (resultado != NULL) {
        printf("Congresista encontrado:\n");
        printf("Nombre: %s\n", resultado->nombre);
        printf("RUT: %s\n", resultado->rut);
        printf("Ocupacion: %d\n", resultado->ocupacion);
        printf("Partido Politico: %s\n", resultado->partidoPolitico);
    }
    else {
        printf("No existe un congresista con el RUT ingresado.\n");
    }
}

void agregarCongresista(struct congreso* congreso)
{
    struct congresista* nuevo = NULL;
    char nombre[100], rut[20], partidoPolitico[100];
    int ocupacion;
    printf("\nIngrese los datos del congresista:\n");

    printf("Nombre: ");
    scanf(" %[^\n]", nombre);
    solicitarRUT(rut);
    printf("Partido Politico: ");
    solicitarPartidoPolitico(partidoPolitico);
    if (buscarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut) != NULL ||
        buscarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut) != NULL) {
        printf("Error: El RUT %s ya esta registrado como congresista (diputado o senador).\n", rut);
        return;
    }

    do {
        printf("Ocupacion (1 = Diputado, 2 = Senador): ");
        scanf("%d", &ocupacion);
        if (ocupacion < 1 || ocupacion > 2) {
            printf("Error: la ocupacion debe estar entre 1 y 2. Intente nuevamente.\n");
        }
    } while (ocupacion < 1 || ocupacion > 2);

    nuevo = crearCongresista(nombre, rut, ocupacion, partidoPolitico);
    if (ocupacion == 1) {
        if (agregarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, nuevo)) {
            printf("Diputado agregado exitosamente.\n");
        }
        else {
            printf("No se pudo agregar el diputado.\n");
        }
    }
    else if (ocupacion == 2) {
        if (agregarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, nuevo)) {
            printf("Senador agregado exitosamente.\n");
        }
        else {
            printf("No se pudo agregar el senador.\n");
        }
    }
}

void eliminarCongresista(struct congreso* congreso)
{
    char rut[20];
    solicitarRUT(rut);

    if (eliminarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut)) {
        printf("Diputado eliminado exitosamente.\n");
    }
    else if (eliminarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut)) {
        printf("Senador eliminado exitosamente.\n");
    }
    else {
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
        printf("Opcion 4: Ordenar Congresistas\n");
        printf("Opcion 5: Listar Congresistas\n");
        printf("Opcion 6: Volver al menu principal\n");
        scanf("%d", &opcion);

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
            ordenarCongresistas(congreso);
            break;
        case 5:
            listarCongresistas(congreso);
            break;
        case 6:
            return; // Salir del menú
        default:
            printf("Opcion invalida, por favor intente otra vez.\n");
            break;
        }
    }
}
void crearYAgregarComisionMenu(struct congreso* congreso) {
    struct comision* nuevaComision = NULL;
    char nombre[100], descripcion[200];
    int tipo, id;
    printf("\n--- Crear y Agregar Comision ---\n");
    printf("Ingrese el id de la Comision: ");
    scanf("%d", &id);

    if (buscarComisionEnArreglo(congreso->comisiones, congreso->maxComisiones, id) != NULL) {
        printf("Ya existe una comision con el ID %d. Operacion cancelada.\n", id);
        return;
    }

    printf("Nombre: ");
    scanf(" %[^\n]", nombre);

    printf("Tipo de Comision (1: Diputados, 2: Senadores): ");
    do {
        scanf("%d", &tipo);
        if (tipo != 1 && tipo != 2) {
            printf("Tipo invalido. Ingrese 1 para Diputados o 2 para Senadores: ");
        }
    } while (tipo != 1 && tipo != 2);

    printf("Descripcion: ");
    scanf(" %[^\n]", descripcion);

    nuevaComision = crearComision(nombre, tipo, id, descripcion);
    if (agregarComisionEnArreglo(congreso->comisiones, congreso->maxComisiones, nuevaComision)) {
        printf("Comision creada y agregada exitosamente.\n");
    }
    else {
        printf("No se pudo agregar la comision.\n");
    }
}
void eliminarComisionMenu(struct congreso* congreso) {
    int id;

    printf("\n--- Eliminar Comision ---\n");
    printf("Ingrese el ID de la comision a eliminar: ");
    scanf("%d", &id);

    if (eliminarComisionEnArreglo(congreso->comisiones, congreso->maxComisiones, id)) {
        printf("Comision eliminada exitosamente.\n");
    }
    else {
        printf("No existe una comision con el ID %d.\n", id);
    }
}
void buscarComisionMenu(struct congreso* congreso) {
    int id;

    printf("\n--- Buscar Comision ---\n");
    printf("Ingrese el ID de la comision a buscar: ");
    scanf("%d", &id);
    mostrarComisionPorID(congreso->comisiones, congreso->maxComisiones, id);
}
void listarComisionesMenu(struct congreso* congreso) {
    int i;
    struct nodoCongresista* integrante;
    printf("\n--- Listar Comisiones ---\n");

    for (i = 0; i < congreso->maxComisiones; i++) {
        if (congreso->comisiones[i] != NULL) {
            // Mostrar detalles de la comisión
            printf("ID: %d\n", congreso->comisiones[i]->idComision);
            printf("Nombre: %s\n", congreso->comisiones[i]->nombre);
            printf("Tipo: %d\n", congreso->comisiones[i]->tipo);
            printf("Descripcion: %s\n", congreso->comisiones[i]->descripcion);

            // Mostrar los congresistas involucrados en esta comisión
            printf("Congresistas involucrados:\n");
            integrante = congreso->comisiones[i]->headIntegrantes;
            if (integrante == NULL) {
                printf("No hay congresistas en esta comision.\n");
            }
            else {
                while (integrante != NULL) {
                    printf("Nombre: %s, RUT: %s\n", integrante->datos->nombre, integrante->datos->rut);
                    integrante = integrante->sig;
                }
            }
            printf("\n");
        }
    }
}
int congresistaYaEnComision(struct comision* comision, char* rutBuscado) {
    struct nodoCongresista* temp = comision->headIntegrantes;

    while (temp != NULL) {
        struct congresista* congresista = buscarCongresistaEnLista(temp, rutBuscado);
        if (congresista != NULL) {
            return 1;
        }
        temp = temp->sig;
    }

    return 0; // El congresista no está en la comisión
}
void agregarCongresistaAComision(struct comision* comision, struct congresista* congresista) {
    struct nodoCongresista* nuevoNodo;
    if ((comision->tipo == 1 && congresista->ocupacion != 1) || (comision->tipo == 2 && congresista->ocupacion != 2)) {
        printf("Error: El congresista no pertenece al tipo de la comision %s.\n", comision->nombre);
        return;
    }
    if (congresistaYaEnComision(comision, congresista->rut)) {
        printf("El congresista con RUT %s ya esta en la comision %s.\n", congresista->rut, comision->nombre);
        return;
    }

    nuevoNodo = (struct nodoCongresista*)malloc(sizeof(struct nodoCongresista));
    nuevoNodo->datos = congresista;
    nuevoNodo->sig = NULL;

    if (comision->headIntegrantes == NULL) {
        comision->headIntegrantes = nuevoNodo;
    }
    else {
        struct nodoCongresista* temp = comision->headIntegrantes;
        while (temp->sig != NULL) {
            temp = temp->sig;
        }
        temp->sig = nuevoNodo;
    }

    printf("Congresista con RUT %s agregado a la comision %s.\n", congresista->rut, comision->nombre);
}
struct congresista* buscarCongresistaEnCongreso(struct congreso* congreso, char* rutBuscado) {
    struct congresista* congresistaBuscado = NULL;
    int i;

    // Buscar en los diputados
    for (i = 0; i < congreso->maxDiputados; i++) {
        if (congreso->diputados[i] != NULL && strcmp(congreso->diputados[i]->rut, rutBuscado) == 0) {
            congresistaBuscado = congreso->diputados[i];
            return congresistaBuscado;
        }
    }

    // Buscar en los senadores
    for (i = 0; i < congreso->maxSenadores; i++) {
        if (congreso->senadores[i] != NULL && strcmp(congreso->senadores[i]->rut, rutBuscado) == 0) {
            congresistaBuscado = congreso->senadores[i];
            return congresistaBuscado;
        }
    }

    return NULL;  // Si no se encuentra al congresista
}
void menuAgregarCongresistaAComision(struct congreso* congreso) {
    char rutBuscado[20];
    int idComision;
    struct congresista* congresistaBuscado = NULL;
    struct comision* comisionSeleccionada;

    // Solicitar el RUT del congresista a agregar
    solicitarRUT(rutBuscado);
    // Buscar al congresista por su RUT en el congreso
    congresistaBuscado = buscarCongresistaEnCongreso(congreso, rutBuscado);
    if (congresistaBuscado == NULL) {
        printf("Congresista con RUT %s no encontrado en el congreso.\n", rutBuscado);
        return;
    }

    // Solicitar el ID de la comisión a la que se desea agregar
    printf("Ingrese el ID de la comision: ");
    scanf("%d", &idComision);

    // Buscar la comisión por su ID
    comisionSeleccionada = buscarComisionEnArreglo(congreso->comisiones, congreso->maxComisiones, idComision);
    if (comisionSeleccionada == NULL) {
        printf("Comision con ID %d no encontrada.\n", idComision);
        return;
    }

    // Intentar agregar el congresista a la comisión
    agregarCongresistaAComision(comisionSeleccionada, congresistaBuscado);
}
void menuComisiones(struct congreso* congreso) {
    int opcion;

    while (1) {
        printf("\n--- Menu de Comisiones ---\n");
        printf("1. Crear y agregar comision\n");
        printf("2. Agregar congresista a comision\n");
        printf("3. Eliminar comision\n");
        printf("4. Buscar comision por ID\n");
        printf("5. Listar todas las comisiones\n");
        printf("6. Volver al menu principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
        case 1:
            crearYAgregarComisionMenu(congreso);
            break;
        case 2:
            menuAgregarCongresistaAComision(congreso);
            break;
        case 3:
            eliminarComisionMenu(congreso);
            break;
        case 4:
            buscarComisionMenu(congreso);
            break;
        case 5:
            listarComisionesMenu(congreso);
            break;
        case 6:
            return;
        default:
            printf("Opcion invalida. Intente nuevamente.\n");
        }
    }
}

/*TODO: FUNCIONES DE GRAFOS------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*TODO------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void inicializarGrafo(struct congreso *congreso)
{
    struct grafoCongreso* nuevoGrafo;
    nuevoGrafo = (struct grafoCongreso*)malloc(sizeof(struct grafoCongreso));
    nuevoGrafo->numCongresistas = 0;
    nuevoGrafo->listaCongresistas = NULL;
    nuevoGrafo->matrizAdyacencia = NULL;

    congreso->relacionesCongreso = nuevoGrafo;
}

void inicializarNuevasConexiones(int** matrizAdyacencia, int numCongresistas)
{
    int i;
    for (i = 0; i < numCongresistas; i++)
    {
        matrizAdyacencia[numCongresistas - 1][i] = 0;  // Última fila
        matrizAdyacencia[i][numCongresistas - 1] = 0;  // Última columna
    }
}

int agregarCongresistaAlGrafo(struct grafoCongreso* grafo, struct congresista* nuevoCongresista)
{
    struct nodoCongresista* nuevoNodo;
    int i;

    if (buscarCongresistaEnLista(grafo->listaCongresistas, nuevoCongresista->rut) != NULL)
    {
        return 0;  // El congresista ya está registrado
    }

    nuevoNodo = enlazarCongresista(nuevoCongresista);
    nuevoNodo->sig = grafo->listaCongresistas;
    grafo->listaCongresistas = nuevoNodo;
    grafo->numCongresistas++;

    grafo->matrizAdyacencia = realloc(grafo->matrizAdyacencia, grafo->numCongresistas * sizeof(int*));

    for (i = 0; i < grafo->numCongresistas; i++)
    {
        grafo->matrizAdyacencia[i] = realloc(grafo->matrizAdyacencia[i], grafo->numCongresistas * sizeof(int));
        if (grafo->matrizAdyacencia[i] == NULL) {
            return 0;
        }
    }

    inicializarNuevasConexiones(grafo->matrizAdyacencia, grafo->numCongresistas);

    return 1;
}

void conectarCongresistas(struct grafoCongreso* grafo, int indice1, int indice2)
{
    if (grafo == NULL || grafo->matrizAdyacencia == NULL) {
        printf("Error: Grafo o matriz de adyacencia no inicializados.\n");
        return;
    }

    if (indice1 < grafo->numCongresistas && indice2 < grafo->numCongresistas && indice1 != indice2)
    {
        grafo->matrizAdyacencia[indice1][indice2]++;
        grafo->matrizAdyacencia[indice2][indice1]++;  // Grafo no dirigido, conexión mutua
    } else {
        printf("Error: Indices fuera de rango o invalidos.\n");
    }
}


int obtenerIndiceCongresista(struct grafoCongreso* grafo, char* rut)
{
    struct nodoCongresista* actual = grafo->listaCongresistas;
    int indice = 0;

    while (actual != NULL)
    {
        if (strcmp(actual->datos->rut, rut) == 0)
        {
            return indice;
        }
        actual = actual->sig;
        indice++;
    }
    return -1;
}

int establecerConexionEntreCongresistas(struct grafoCongreso* grafo, char* rut1, char* rut2)
{
    int indice1;
    int indice2;
    
    indice1 = obtenerIndiceCongresista(grafo, rut1);
    indice2 = obtenerIndiceCongresista(grafo, rut2);

    if (indice1 != -1 && indice2 != -1)
    {
        conectarCongresistas(grafo, indice1, indice2);
        return 1;
    } else {
        return 0;
    }
}

void mostrarMatrizAdyacencia(struct grafoCongreso* grafo)
{
    int i,j;

    if (grafo->matrizAdyacencia == NULL) 
    {
        printf("La matriz de adyacencia no esta inicializada.\n");
        return;
    }
    
    printf("\n--- Matriz de Adyacencia ---\n");
    for (i = 0; i < grafo->numCongresistas; i++) {
        for (j = 0; j < grafo->numCongresistas; j++) {
            printf("%d ", grafo->matrizAdyacencia[i][j]);
        }
        printf("\n");
    }
}
void menuEstablecerConexion(struct grafoCongreso *grafo) {
    char rut1[20]; 
    char rut2[20];

    printf("Ingrese el RUT del primer congresista: ");
    solicitarRUT(rut1); // Valida el RUT antes de continuar

    printf("Ingrese el RUT del segundo congresista: ");
    solicitarRUT(rut2); // Valida el RUT antes de continuar

    if (establecerConexionEntreCongresistas(grafo, rut1, rut2)) {
        printf("Conexion establecida exitosamente entre %s y %s.\n", rut1, rut2);
    } else {
        printf("No se pudo establecer la conexion. Verifique los Ruts ingresados.\n");
    }
}

void mostrarCongresistasGrafo(struct nodoCongresista* listaCongresistas)
{
    printf("\n--- Lista de Congresistas en el Grafo ---\n");
    mostrarCongresistaEnComisionLista(listaCongresistas);
}
void mostrarCongresistasEnGrafoMenu(struct grafoCongreso *grafo) {
    mostrarCongresistasGrafo(grafo->listaCongresistas);
}

void mostrarMatrizAdyacenciaMenu(struct grafoCongreso *grafo) {
    mostrarMatrizAdyacencia(grafo);
}
void agregarCongresistaAlGrafoMenu(struct congreso *congreso, struct grafoCongreso *grafo)
{
    int tipo;
    char rut[20];
    struct congresista *nuevoCongresista;

    printf("Ingrese el tipo de Congresista (1) Diputado (2) Senador: ");
    do {
        scanf("%d", &tipo);
        if (tipo != 1 && tipo != 2) {
            printf("Tipo Incorrecto. Ingrese 1 para Diputados o 2 para Senadores: ");
        }
    } while (tipo != 1 && tipo != 2);

    solicitarRUT(rut);

    if (tipo == 1) {
        nuevoCongresista = buscarCongresistaEnArreglo(congreso->diputados, congreso->maxDiputados, rut);
    } else {
        nuevoCongresista = buscarCongresistaEnArreglo(congreso->senadores, congreso->maxSenadores, rut);
    }

    if (nuevoCongresista == NULL) {
        printf("El congresista con RUT %s no existe.\n", rut);
        return;
    }

    if (agregarCongresistaAlGrafo(grafo, nuevoCongresista)) {
        printf("Congresista agregado exitosamente al grafo.\n");
    } else {
        printf("El congresista ya esta registrado o hubo un error.\n");
    }
}
void menuGrafos(struct congreso *congreso) 
{
    int opcion;

    while (1) {
        printf("\n--- Menu de Grafos ---\n");
        printf("1. Agregar congresista al grafo\n");
        printf("2. Establecer conexion entre congresistas\n");
        printf("3. Mostrar congresistas en el grafo\n");
        printf("4. Mostrar matriz de adyacencia\n");
        printf("5. Volver al menu principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d",&opcion);
        switch (opcion) {
            case 1:
                agregarCongresistaAlGrafoMenu(congreso, congreso->relacionesCongreso);
                break;
            case 2:
                menuEstablecerConexion(congreso->relacionesCongreso);
                break;
            case 3:
                mostrarCongresistasEnGrafoMenu(congreso->relacionesCongreso);
                break;
            case 4:
                mostrarMatrizAdyacenciaMenu(congreso->relacionesCongreso);
                break;
            case 5:
                return; 
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
        }
    }
}


int main() {
    int opcion;
    struct congreso* congreso;

    // Inicialización del congreso y grafo
    congreso = inicializarCongreso();
    inicializarGrafo(congreso);

    do {
        // Menú principal
        printf("\n--- Menu Principal ---\n");
        printf("1. Menu Congresistas\n");
        printf("2. Menu Proyectos de Ley\n");
        printf("3. Menu Comisiones\n");
        printf("4. Menu Grafos\n");
        printf("0. Salir\n");
        printf("Ingrese opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 0: {
                printf("Saliendo del programa...\n");
                break;
            }

            case 1: {
                menuCongresistas(congreso);
                break;
            }

            case 2: {
                menuProyectosLey(congreso);
                break;
            }

            case 3: {
                menuComisiones(congreso);
                break;
            }

            case 4: {
                menuGrafos(congreso);
                break;
            }

            default: {
                printf("Opcion invalida, por favor intente nuevamente.\n");
                break;
            }
        }

    } while (opcion != 0);

    return 0;
}
