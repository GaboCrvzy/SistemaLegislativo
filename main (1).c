#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Politico {
    char *nombrePolitico;
    char *rut;
    char *partido;
    struct ProyectoLey **proyectos;        // Proyectos en los que participa el político
    int cantProyectos;
};

struct NodoParlamentario {
    struct Politico *parlamentario;
    struct NodoParlamentario *ant, *sig;
};

struct TribunalConstitucional {
    int esRevisado;          // 0 NO REVISADO, 1 REVISADO
    int esConstitucional;    // 0 INCONSTITUCIONAL, 1 CONSTITUCIONAL
};

struct Voto {
    struct Politico *parlamentario;
    int tipoVoto;                   // 1 = a favor, 2 = en contra, 3 = abstención
};

struct Votacion {
    char *fechaVotacion;
    int votosAFavor;
    int votosEnContra;
    int abstenciones;
    struct Voto **votos;
    int totalVotos;
    int resultado;            // 0 = Rechazado, 1 = Aprobado
};

struct Comision {
    char *nombreDeComision;
    struct NodoParlamentario *miembros;
    struct Votacion *votacion;
};

struct NodoComision {
    struct Comision *comision;
    struct NodoComision *sig;
};

struct Camara {
    char *nombreDeCamara;
    struct NodoParlamentario *listaParlamentarios;
    struct NodoComision *comisiones;
    struct Votacion *votacionGeneral;
    struct Votacion *votacionEspecifica;
};

struct ProyectoLey {
    int idProyecto;                         // Identificador UNICO del proyecto
    char *tituloProyecto;
    char *iniciativaLegislativa;
    int urgenciaProyecto;                  //  1:Simple - 2:Suma Urgencia - 3:Discusion Inmediata
    char *estadoProyecto;
};

struct NodoABB {
    struct ProyectoLey *proyecto;
    struct NodoABB *izquierda;           // Proyectos con ID menor
    struct NodoABB *derecha;           // Proyectos con ID mayor
};

struct SistemaLegislativo {
    struct TribunalConstitucional *TC;
    struct NodoABB *abbProyectos;          // ABB de proyectos de ley por ID
    struct NodoParlamentario *diputados;
    struct NodoParlamentario *senadores;
    struct Camara *camaraDiputados;
    struct Camara *camaraSenadores;
    struct NodoComision *comisionMixta;
};

struct Veto {
    int tipoVeto;                // 1 si veto parcial o 2 si veto total
    char *motivo;
};

struct Presidencia {
    char *nombrePresidente;
    struct Veto *veto;           // NULL si no ha vetado
    struct SistemaLegislativo *sistema;
};

struct Politico *crearPolitico(char *nombre, char *rut, char *partidoPolitico)
{
    struct Politico *nuevoPolitico;

    nuevoPolitico = (struct Politico *)malloc(sizeof(struct Politico));

    if(nuevoPolitico == NULL) return NULL;

    nuevoPolitico->nombrePolitico = (char *)malloc((strlen(nombre) + 1) * sizeof(char));
    strcpy(nuevoPolitico->nombrePolitico, nombre);

    nuevoPolitico->rut = (char *)malloc((strlen(rut) + 1) * sizeof(char));
    strcpy(nuevoPolitico->rut, rut);

    nuevoPolitico->partido = (char *)malloc((strlen(partidoPolitico) + 1) * sizeof(char));
    strcpy(nuevoPolitico->partido, partidoPolitico);

    nuevoPolitico->proyectos = NULL;
    nuevoPolitico->cantProyectos = 0;

    return nuevoPolitico;
}

struct Politico *buscarPolitico(struct NodoParlamentario *headLista, char *rutPolitico)
{
    struct NodoParlamentario *rec = NULL;

    rec = headLista;

    while(rec!= NULL)
    {
         if (strcmp(rec->parlamentario->rut, rutPolitico) == 0)
         {
            return rec->parlamentario;
        }
        rec = rec->sig;
    }

    return 0;
}

int enlazarPolitico(struct NodoParlamentario **headLista, struct Politico *nuevoPolitico)
{

    struct NodoParlamentario *nuevoNodo , *rec;

    if(*headLista == NULL)
    {
        nuevoNodo = (struct NodoParlamentario*)malloc(sizeof(struct NodoParlamentario));
        nuevoNodo->parlamentario = nuevoPolitico;
        nuevoNodo->ant = NULL;
        nuevoNodo->sig = NULL;

        *headLista = nuevoNodo;
        return 1;
    }
    else
    {
        if (buscarPolitico(*headLista, nuevoPolitico->rut) == NULL)
        {
            rec = *headLista;

            while (rec->sig != NULL)
            {
                rec = rec->sig;
            }

            nuevoNodo = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));

            nuevoNodo->parlamentario = nuevoPolitico;
            nuevoNodo->ant = rec;
            nuevoNodo->sig = NULL;
            rec->sig = nuevoNodo;

            return 1;
        }
    }
    return 0;
}

int eliminarPolitico(struct NodoParlamentario **headLista, char *rut)
{
    struct NodoParlamentario *rec = NULL;

    if(*headLista != NULL && strcmp((*headLista)->parlamentario->rut, rut) == 0)
    {
        *headLista = (*headLista)->sig;

        if (*headLista != NULL){
            (*headLista)->ant = NULL;
        }
        return 1;
    }

    rec = (*headLista)->sig;
    while (rec != NULL)
    {
        if(strcmp(rec->parlamentario->rut, rut) == 0)
        {
            if (rec->ant != NULL) {
                rec->ant->sig = rec->sig;
            }

            if (rec->sig != NULL) {
                rec->sig->ant = rec->ant;
            }
            return 1;
        }
        rec = rec->sig;
    }
    return 0;
}
void agregarPoliticoACamara(struct Camara *camara, struct Politico *politico) {
    if (camara == NULL || politico == NULL) return;

    // Crear nuevo nodo de parlamentario
    struct NodoParlamentario *nuevoNodo = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    nuevoNodo->parlamentario = politico;
    nuevoNodo->ant = NULL;
    nuevoNodo->sig = NULL;

    // Insertar en la lista de parlamentarios
    if (camara->listaParlamentarios == NULL) {
        camara->listaParlamentarios = nuevoNodo;
    } else {
        struct NodoParlamentario *actual = camara->listaParlamentarios;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
        nuevoNodo->ant = actual;
    }
}

struct Voto *crearVoto(struct Politico *parlamentario, int tipoVoto)
{
    struct Voto *nuevoVoto = (struct Voto *)malloc(sizeof(struct Voto));
    if (nuevoVoto == NULL) return NULL;

    nuevoVoto->parlamentario = parlamentario;
    nuevoVoto->tipoVoto = tipoVoto;
    return nuevoVoto;
}

int buscarVoto(struct Voto **votos, int tam, char *rut)
{
    int i;
    for (i = 0; i < tam; i++)
    {
        if (votos[i] != NULL && strcmp(votos[i]->parlamentario->rut, rut) == 0){
            return 1;
        }
    }
    return 0;
}

int agregarVoto(struct Voto **votos, int tam, struct Voto *nuevoVoto)
{
    int i;

    if(buscarVoto(votos, tam, nuevoVoto->parlamentario->rut) == 0)
    {
        for(i = 0; i < tam; i++)
        {
            if(votos[i] == NULL)
            {
                votos[i] = nuevoVoto;
                return 1;
            }
        }
    }
    return 0;
}

int eliminarVoto(struct Voto **votos, int tam, char *rut)
{
    int i;

    for (i = 0; i < tam; i++)
    {
        if (votos[i] != NULL && strcmp(votos[i]->parlamentario->rut, rut) == 0)
        {
            votos[i] = NULL;
            return 1;
        }
    }
    return 0;
}

int inicializarVotacion(struct Votacion *votacion, int cantidadParticipantes)
{
    if(votacion == NULL) return 0;

    votacion->votos = (struct Voto **)malloc(cantidadParticipantes * sizeof(struct Voto *));
    if (votacion->votos == NULL) return 0;

    votacion->votosAFavor = 0;
    votacion->votosEnContra = 0;
    votacion->abstenciones = 0;
    votacion->totalVotos = 0;

    return 1;
}

struct Votacion *crearVotacion(char *fechaInicio, int cantidadParticipantes) {
    if (fechaInicio == NULL) return NULL;

    struct Votacion *votacionNueva = (struct Votacion *)malloc(sizeof(struct Votacion));
    if (votacionNueva == NULL) return NULL;

    votacionNueva->fechaVotacion = (char *)malloc((strlen(fechaInicio) + 1) * sizeof(char));
    strcpy(votacionNueva->fechaVotacion, fechaInicio);

    // Inicializamos `votos` con NULL en caso de que `inicializarVotacion` falle
    votacionNueva->votos = NULL;

    // Llamamos a `inicializarVotacion` con el nuevo objeto `votacionNueva` y la cantidad de participantes
    if (!inicializarVotacion(votacionNueva, cantidadParticipantes)) {
        // Si la inicialización falla, devolvemos NULL
        return NULL;
    }

    return votacionNueva;
}

float calcularPorcentajeAprobacion(struct Votacion *votacion)
{
    if (votacion == NULL || votacion->totalVotos == 0)
    {
        return 0.0;
    }
    return ((float)votacion->votosAFavor / (float)votacion->totalVotos) * 100;
}

struct Comision *crearComision(char *nombreDeComision)
{
    if (nombreDeComision == NULL) return NULL;

    struct Comision *nuevaComision = (struct Comision *)malloc(sizeof(struct Comision));
    if (nuevaComision == NULL) return NULL;

    nuevaComision->nombreDeComision = (char *)malloc((strlen(nombreDeComision) + 1) * sizeof(char));
    strcpy(nuevaComision->nombreDeComision, nombreDeComision);

    nuevaComision->miembros = NULL;
    nuevaComision->votacion = NULL;

    return nuevaComision;
}

struct Comision *buscarComision(struct NodoComision *head, char *nombreComision)
{
    struct NodoComision *rec = head;

    while(rec != NULL)
    {
        if (strcmp(rec->comision->nombreDeComision, nombreComision) == 0)
            return rec->comision;
        rec = rec->sig;
    }
    return NULL;
}

int enlazarComision(struct NodoComision **headLista, struct Comision *nuevaComision)
{
    struct NodoComision *nuevoNodo, *rec;

    if (*headLista == NULL)
    {
        nuevoNodo = (struct NodoComision *)malloc(sizeof(struct NodoComision));
        if (nuevoNodo == NULL) return 0;

        nuevoNodo->comision = nuevaComision;
        nuevoNodo->sig = NULL;

        *headLista = nuevoNodo;
        return 1;
    }
    else
    {
        if(buscarComision(*headLista, nuevaComision->nombreDeComision) == NULL)
        {
            rec = *headLista;

            while (rec->sig != NULL)
            {
                rec = rec->sig;
            }

            nuevoNodo = (struct NodoComision *)malloc(sizeof(struct NodoComision));
            if (nuevoNodo == NULL) return 0;

            nuevoNodo->comision = nuevaComision;
            nuevoNodo->sig = NULL;
            rec->sig = nuevoNodo;
            return 1;
        }
    }
    return 0;
}

int eliminarComision(struct NodoComision **headComision, char *nombreComisionBorrar)
{
    struct NodoComision *rec = NULL;

    if (*headComision != NULL && strcmp((*headComision)->comision->nombreDeComision, nombreComisionBorrar) == 0)
    {
        *headComision = (*headComision)->sig;
        return 1;
    }

    rec = *headComision;
    while (rec->sig != NULL)
    {
        if (strcmp(rec->sig->comision->nombreDeComision, nombreComisionBorrar) == 0)
        {
            rec->sig = rec->sig->sig;
            return 1;
        }
        rec = rec->sig;
    }
    return 0;
}

int buscarMiembroEnComision(struct NodoParlamentario *miembros, char *rutPolitico)
{
    struct NodoParlamentario *rec = miembros;

    while (rec != NULL)
    {
        if (strcmp(rec->parlamentario->rut, rutPolitico) == 0) {
            return 1;
        }
        rec = rec->sig;
    }
    return 0;
}

int agregarMiembroComision(struct Comision *comision, struct Politico *nuevoMiembro)
{
    if (comision == NULL || nuevoMiembro == NULL) return 0;

    if (buscarMiembroEnComision(comision->miembros, nuevoMiembro->rut) == 1)
        return 0;

    struct NodoParlamentario *nuevoNodo = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    if (nuevoNodo == NULL) return 0;

    nuevoNodo->parlamentario = nuevoMiembro;
    nuevoNodo->sig = comision->miembros;
    nuevoNodo->ant = NULL;

    if (comision->miembros != NULL)
        comision->miembros->ant = nuevoNodo;

    comision->miembros = nuevoNodo;
    return 1;
}

int eliminarMiembroComision(struct Comision *comision, char *rut)
{
    if (comision == NULL || rut == NULL) return 0;

    return eliminarPolitico(&comision->miembros, rut);
}

struct Camara *crearCamara(char *nombreCamara)
{
    struct Camara *nuevaCamara;
    nuevaCamara = (struct Camara *)malloc(sizeof(struct Camara));
    if(nuevaCamara == NULL) return NULL;

    nuevaCamara->nombreDeCamara = (char *)malloc((strlen(nombreCamara) + 1) * sizeof(char));
    strcpy(nuevaCamara->nombreDeCamara, nombreCamara);

    nuevaCamara->comisiones = NULL;
    nuevaCamara->votacionGeneral = NULL;
    nuevaCamara->votacionEspecifica = NULL;

    return nuevaCamara;
}

int buscarCamara(struct SistemaLegislativo *sistema, char *nombreCamara)
{
    if (sistema == NULL) return 0;

    if (sistema->camaraDiputados != NULL && strcmp(sistema->camaraDiputados->nombreDeCamara, nombreCamara) == 0) return 1;

    if (sistema->camaraSenadores != NULL && strcmp(sistema->camaraSenadores->nombreDeCamara, nombreCamara) == 0) return 1;

    return 0;
}

struct ProyectoLey *crearProyecto(int id, char *titulo, char *iniciativa, int urgencia, char *estadoDelProyecto)
{
    struct ProyectoLey *nuevoProyecto;
    nuevoProyecto = (struct ProyectoLey *)malloc(sizeof(struct ProyectoLey));
    if(nuevoProyecto == NULL) return NULL;

    nuevoProyecto->idProyecto = id;

    nuevoProyecto->tituloProyecto = (char *)malloc((strlen(titulo) + 1) * sizeof(char));
    strcpy(nuevoProyecto->tituloProyecto, titulo);

    nuevoProyecto->iniciativaLegislativa = (char *)malloc((strlen(iniciativa) + 1) * sizeof(char));
    strcpy(nuevoProyecto->iniciativaLegislativa, iniciativa);

    nuevoProyecto->urgenciaProyecto = urgencia;

    nuevoProyecto->estadoProyecto = (char *)malloc((strlen(estadoDelProyecto) + 1) * sizeof(char));
    strcpy(nuevoProyecto->estadoProyecto, estadoDelProyecto);

    return nuevoProyecto;
}

void agregarProyectoLey(struct SistemaLegislativo *sistema, struct ProyectoLey *proyecto) {
    if (sistema == NULL || proyecto == NULL) return;

    struct NodoABB *nuevoNodo = (struct NodoABB *)malloc(sizeof(struct NodoABB));
    nuevoNodo->proyecto = proyecto;
    nuevoNodo->izquierda = NULL;
    nuevoNodo->derecha = NULL;

    if (sistema->abbProyectos == NULL) {
        sistema->abbProyectos = nuevoNodo;
    } else {
        struct NodoABB *actual = sistema->abbProyectos;
        struct NodoABB *anterior = NULL;

        while (actual != NULL) {
            anterior = actual;
            if (proyecto->idProyecto < actual->proyecto->idProyecto) {
                actual = actual->izquierda;
            } else {
                actual = actual->derecha;
            }
        }

        if (proyecto->idProyecto < anterior->proyecto->idProyecto) {
            anterior->izquierda = nuevoNodo;
        } else {
            anterior->derecha = nuevoNodo;
        }
    }
}
void agregarProyectoAPolitico(struct Politico *politico, struct ProyectoLey *proyecto) {
    if (politico == NULL || proyecto == NULL) return;

    politico->proyectos = (struct ProyectoLey **)realloc(
        politico->proyectos, (politico->cantProyectos + 1) * sizeof(struct ProyectoLey *));

    if (politico->proyectos != NULL) {
        politico->proyectos[politico->cantProyectos] = proyecto;
        politico->cantProyectos++;
    }
}

struct NodoABB *insertarProyecto(struct NodoABB *nodoActual, struct ProyectoLey *nuevoProyecto)
{
    if (nodoActual == NULL)
    {
        struct NodoABB *nuevoNodo = (struct NodoABB *)malloc(sizeof(struct NodoABB));
        nuevoNodo->proyecto = nuevoProyecto;
        nuevoNodo->izquierda = NULL;
        nuevoNodo->derecha = NULL;
        return nuevoNodo;
    }

    if (nuevoProyecto->idProyecto < nodoActual->proyecto->idProyecto)
    {
        nodoActual->izquierda = insertarProyecto(nodoActual->izquierda, nuevoProyecto);
    }
    else
    {
        nodoActual->derecha = insertarProyecto(nodoActual->derecha, nuevoProyecto);
    }

    return nodoActual;
}

int buscarProyecto(struct NodoABB *nodo, int idProyecto)
{
    if (nodo == NULL) return 0;

    if (nodo->proyecto->idProyecto == idProyecto)
    {
        return 1;
    }
    else if (idProyecto < nodo->proyecto->idProyecto)
    {
        return buscarProyecto(nodo->izquierda, idProyecto);
    }
    else
    {
        return buscarProyecto(nodo->derecha, idProyecto);
    }
}


struct TribunalConstitucional* crearTribunalConstitucional()
{
    struct TribunalConstitucional* tribunal = (struct TribunalConstitucional*)malloc(sizeof(struct TribunalConstitucional));
    if (tribunal != NULL)
    {
        tribunal->esRevisado = 0;
        tribunal->esConstitucional = 0;
    }
    return tribunal;
}

int controlConstitucional(struct TribunalConstitucional* tribunal, int esRevisado, int esConstitucional)
{
    if (tribunal == NULL) return 0;

    tribunal->esRevisado = esRevisado;
    tribunal->esConstitucional = esConstitucional;
    return 1;

}

struct SistemaLegislativo *crearSistemaLegislativo()
{
    struct SistemaLegislativo* sistema = (struct SistemaLegislativo*)malloc(sizeof(struct SistemaLegislativo));
    sistema->TC = crearTribunalConstitucional();
    sistema->abbProyectos = NULL;
    sistema->diputados = NULL;
    sistema->senadores = NULL;
    sistema->camaraDiputados = NULL;
    sistema->camaraSenadores = NULL;
    sistema->comisionMixta = NULL;
    return sistema;
}

struct Veto *crearVeto(int tipo, char *motivo)
{
    struct Veto *nuevoVeto;
    nuevoVeto = (struct Veto *)malloc(sizeof(struct Veto));
    nuevoVeto->tipoVeto = tipo;

    nuevoVeto->motivo = (char *)malloc((strlen(motivo) + 1) * sizeof(char));
    strcpy(nuevoVeto->motivo, motivo);

    return nuevoVeto;
}

struct Presidencia *crearPresidencia(char *nombre, struct SistemaLegislativo *sistema)
{
    struct Presidencia *nuevaPresidencia = (struct Presidencia *)malloc(sizeof(struct Presidencia));
    if (nuevaPresidencia == NULL) return NULL;

    nuevaPresidencia->nombrePresidente = (char *)malloc((strlen(nombre) + 1) * sizeof(char));
    strcpy(nuevaPresidencia->nombrePresidente, nombre);

    nuevaPresidencia->veto = NULL;
    nuevaPresidencia->sistema = sistema;
    return nuevaPresidencia;
}

void mostrarPresidencia(struct Presidencia *presidencia)
{
    if (presidencia != NULL)
    {
        printf("Presidente: %s\n", presidencia->nombrePresidente);
        if (presidencia->veto != NULL)
        {
            printf("Veto: Tipo %d, Motivo: %s\n", presidencia->veto->tipoVeto, presidencia->veto->motivo);
        } else
        {
            printf("No hay veto asignado.\n");
        }
    }
}

void mostrarParlamentario(struct NodoParlamentario *lista, char *tipoParlamentario)
{
    struct NodoParlamentario *actual = lista;
    if (lista == NULL)
    {
        printf("No hay %s en el sistema.\n", tipoParlamentario);
        return;
    }

    printf("Lista de %s:\n", tipoParlamentario);

    while (actual != NULL) {
        printf("- %s (%s, %s)\n", actual->parlamentario->nombrePolitico, actual->parlamentario->rut, actual->parlamentario->partido);
        actual = actual->sig;
    }
}

void mostrarComisiones(struct NodoComision *listaComisiones)
{
    struct NodoComision *actual = listaComisiones;

    if (actual == NULL)
    {
        printf("No hay comisiones en el sistema.\n");
        return;
    }

    printf("Lista de Comisiones:\n");
    while (actual != NULL)
    {
        printf("- %s ", actual->comision->nombreDeComision);

        if (actual->comision->miembros != NULL && actual->comision->miembros->parlamentario != NULL)
        {
            printf("(Nombre del Parlamentario: %s, Rut del Parlamentario: %s)\n",
                   actual->comision->miembros->parlamentario->nombrePolitico,
                   actual->comision->miembros->parlamentario->rut);
        }
        else
        {
            printf("(No hay miembros en esta comisión)\n");
        }

        actual = actual->sig;
    }
}


void mostrarProyectos(struct NodoABB *raiz)
{
    if (raiz == NULL)
    {
        printf("No hay proyectos de ley en el sistema.\n");
        return;
    }
    mostrarProyectos(raiz->izquierda);
    printf("- Proyecto %d: %s (Estado: %s)\n", raiz->proyecto->idProyecto, raiz->proyecto->tituloProyecto, raiz->proyecto->estadoProyecto);
    mostrarProyectos(raiz->derecha);
}

void generarReporteActividades(struct NodoParlamentario *lista)
{
    struct NodoParlamentario *actual = lista;
    printf("=== Reporte de Actividades de los Parlamentarios ===\n");

    while (actual != NULL)
    {
        struct Politico *politico = actual->parlamentario;
        printf("Político: %s (RUT: %s, Partido: %s)\n", politico->nombrePolitico, politico->rut, politico->partido);
        printf("Proyectos en los que participa: %d\n", politico->cantProyectos);
        actual = actual->sig;
    }
}

void agregarPoliticoMenu(struct NodoParlamentario **listaParlamentarios)
{
    char nombre[100], rut[20], partido[50];
    struct Politico *nuevoPolitico;

    printf("Ingrese el nombre del político: ");
    scanf(" %[^\n]s", nombre);
    printf("Ingrese el RUT del político: ");
    scanf(" %[^\n]s", rut);
    printf("Ingrese el partido del político: ");
    scanf(" %[^\n]s", partido);

    nuevoPolitico = crearPolitico(nombre, rut, partido);

    if (nuevoPolitico == NULL)
    {
        printf("Error al crear el político. No hay suficiente memoria.\n");
        return;
    }

    if (enlazarPolitico(listaParlamentarios, nuevoPolitico))
    {
        printf("Político agregado exitosamente.\n");
    }
    else
    {
        printf("No se pudo agregar el político (puede que ya esté en la lista).\n");
    }
}

void eliminarPoliticoMenu(struct NodoParlamentario **listaParlamentarios)
{
    char rut[20];

    printf("Ingrese el RUT del político a eliminar: ");
    scanf(" %[^\n]s", rut);

    if (eliminarPolitico(listaParlamentarios, rut)) {
        printf("Político eliminado exitosamente.\n");
    } else {
        printf("No se encontró un político con ese RUT en la lista.\n");
    }
}

void mostrarListaPoliticos(struct NodoParlamentario *lista, char *tipo) {
    mostrarParlamentario(lista, tipo);
}

void mostrarMenuPoliticos(struct NodoParlamentario **diputados, struct NodoParlamentario **senadores)
{
    int opcion;
    do {
        printf("\n=== Menú de Políticos ===\n");
        printf("1. Agregar Diputado\n");
        printf("2. Eliminar Diputado\n");
        printf("3. Listar Diputados\n");
        printf("4. Agregar Senador\n");
        printf("5. Eliminar Senador\n");
        printf("6. Listar Senadores\n");
        printf("7. Volver al Menu Principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                agregarPoliticoMenu(diputados);
                break;
            case 2:
                eliminarPoliticoMenu(diputados);
                break;
            case 3:
                mostrarListaPoliticos(*diputados,"Diputados");
                break;
            case 4:
                agregarPoliticoMenu(senadores);
                break;
            case 5:
                eliminarPoliticoMenu(senadores);
                break;
            case 6:
                mostrarListaPoliticos(*senadores, "Senadores");
                break;
            case 7:
                printf("Volviendo al Menu Principal...\n");
                break;
            default:
                printf("Opción no válida. Por favor, intente nuevamente.\n");
        }
    } while(opcion != 7);
}

struct ProyectoLey *leerCrearProyecto(struct NodoABB **proyectos)
{
    int id, urgencia;
    char titulo[100], iniciativa[100], estado[100];
    struct ProyectoLey *nuevoProyecto;

    printf("Ingrese ID del proyecto: ");
    if (scanf("%d", &id) != 1)
    {
        printf("Entrada inválida para el ID del proyecto.\n");
        return NULL;
    }

    printf("Ingrese título del proyecto: ");
    scanf(" %[^\n]", titulo);

    printf("Ingrese iniciativa legislativa: ");
    scanf(" %[^\n]", iniciativa);

    printf("Ingrese urgencia del proyecto (1: Simple, 2: Suma Urgencia, 3: Discusión Inmediata): ");
    if (scanf("%d", &urgencia) != 1 || urgencia < 1 || urgencia > 3)
    {
        printf("Entrada inválida para la urgencia del proyecto.\n");
        return NULL;
    }

    printf("Ingrese estado del proyecto: ");
    scanf(" %[^\n]", estado);

    nuevoProyecto = crearProyecto(id, titulo, iniciativa, urgencia, estado);
    if (nuevoProyecto == NULL)
    {
        printf("Error al crear el proyecto.\n");
        return NULL;
    }
    return nuevoProyecto;
}

void mostrarMenuProyectos(struct NodoABB **proyectos)
{
    int opcion, idProyecto;
    struct ProyectoLey *proyectoNuevo = NULL;

    while (1) {
        printf("\n------ Menu de Proyectos -----\n");
        printf("1. Crear Proyecto\n");
        printf("2. Agregar Proyecto\n");
        printf("3. Mostrar Proyectos\n");
        printf("4. Buscar Proyecto\n");
        printf("5. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                proyectoNuevo = leerCrearProyecto(proyectos);
                if (proyectoNuevo != NULL) {
                    printf("Proyecto creado exitosamente.\n");
                } else {
                    printf("No se pudo crear el proyecto debido a un error en la entrada.\n");
                }
                break;
            case 2:
                if (proyectoNuevo != NULL) {
                    *proyectos = insertarProyecto(*proyectos, proyectoNuevo);
                    printf("Proyecto agregado exitosamente.\n");
                } else {
                    printf("Primero debe crear un proyecto antes de agregarlo.\n");
                }
                break;
            case 3:
                mostrarProyectos(*proyectos);
                break;
            case 4:
                printf("Ingrese el ID del proyecto a buscar: ");
                scanf("%d", &idProyecto);
                if (buscarProyecto(*proyectos, idProyecto)) {
                    printf("Proyecto encontrado.\n");
                } else {
                    printf("Proyecto con ID %d no encontrado.\n", idProyecto);
                }
                break;
            case 5:
                printf("Volviendo al Menu Principal...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
    }
}


void agregarComision(struct NodoComision **listaComisiones)
{
    char nombreDeComision[100];
    struct Comision *nuevaComision;

    printf("Ingrese el nombre de la nueva comision: ");
    scanf("%s", nombreDeComision);

    if (buscarComision(*listaComisiones, nombreDeComision))
    {
        printf("La comisión '%s' ya existe.\n", nombreDeComision);
        return;
    }

    nuevaComision = crearComision(nombreDeComision);
    if (nuevaComision == NULL)
    {
        printf("Error al crear la comisión.\n");
        return;
    }

    if (enlazarComision(listaComisiones, nuevaComision))
    {
        printf("Comisión '%s' agregada con éxito.\n", nombreDeComision);
    }
    else
    {
        printf("Error al agregar la comisión.\n");
    }
}

void gestionarAgregarMiembroComision(struct NodoComision **listaComisiones, struct NodoParlamentario *listaParlamentarios)
{
    char nombreComisionBuscar[100];
    char rutPolitico[20];
    struct Comision *comisionSeleccionada;
    struct Politico *nuevoMiembro;
    int tipoMiembro;

    printf("¿Desea agregar un (1) Diputado o un (2) Senador a la comisión? ");
    scanf("%d", &tipoMiembro);

    printf("Ingrese el nombre de la comisión a la que desea agregar un miembro: ");
    scanf("%s", nombreComisionBuscar);

    comisionSeleccionada = buscarComision(*listaComisiones, nombreComisionBuscar);
    if (comisionSeleccionada != NULL)
    {
        printf("Ingrese el RUT del político a agregar: ");
        scanf("%s", rutPolitico);

        nuevoMiembro = buscarPolitico(listaParlamentarios, rutPolitico);

        if (nuevoMiembro != NULL)
        {
            if (agregarMiembroComision(comisionSeleccionada, nuevoMiembro))
            {
                printf("Miembro '%s' agregado a la comisión '%s' con éxito.\n", nuevoMiembro->nombrePolitico, nombreComisionBuscar);
            }
            else
            {
                printf("El miembro ya está en la comisión o no se pudo agregar.\n");
            }
        }
        else
        {
            printf("No se encontró el político con RUT '%s'.\n", rutPolitico);
        }
    }
    else
    {
        printf("No se encontró la comisión '%s'.\n", nombreComisionBuscar);
    }
}


void menuComisiones(struct NodoComision **listaComisiones, struct NodoParlamentario *listaParlamentarios)
{
    int opcion;
    char nombreComisionBorrar[100];
    char nombreComisionBuscar[100];

    while (1)
    {
        printf("\n--- Menu Comisiones ---\n");
        printf("1. Agregar Comision\n");
        printf("2. Eliminar Comision\n");
        printf("3. Buscar Comision\n");
        printf("4. Mostrar Comisiones\n");
        printf("5. Agregar Miembro a Comision\n");
        printf("6. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion)
        {
            case 1:
                agregarComision(listaComisiones);
                break;
            case 2:
                printf("Ingrese el nombre de la comision a eliminar: ");
                scanf("%s", nombreComisionBorrar);
                if (eliminarComision(listaComisiones, nombreComisionBorrar))
                {
                    printf("Comisión '%s' eliminada con éxito.\n", nombreComisionBorrar);
                }
                else
                {
                    printf("No se encontró la comisión '%s'.\n", nombreComisionBorrar);
                }
                break;
            case 3:
                printf("Ingrese el nombre de la comision a buscar: ");
                scanf("%s", nombreComisionBuscar);
                if (buscarComision(*listaComisiones, nombreComisionBuscar))
                {
                    printf("La comisión '%s' fue encontrada.\n", nombreComisionBuscar);
                }
                else
                {
                    printf("No se encontró la comisión '%s'.\n", nombreComisionBuscar);
                }
                break;
            case 4:
                mostrarComisiones(*listaComisiones);
                break;
            case 5:
                gestionarAgregarMiembroComision(listaComisiones, listaParlamentarios);
                break;
            case 6:
                printf("Volviendo al Menu Principal...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
    }
}


void mostrarResultadosVotacionesComisiones(struct NodoComision *listaComisiones)
{
    struct NodoComision *actual = listaComisiones;
    struct Comision *comisionActual;

    while (actual != NULL)
    {
        comisionActual = actual->comision;

        if (comisionActual->votacion != NULL)
        {
            printf("Resultados de la votación en la comisión '%s':\n", comisionActual->nombreDeComision);
            printf("Fecha de la votación: %s\n", comisionActual->votacion->fechaVotacion);
            printf("Votos a favor: %d\n", comisionActual->votacion->votosAFavor);
            printf("Votos en contra: %d\n", comisionActual->votacion->votosEnContra);
            printf("Abstenciones: %d\n", comisionActual->votacion->abstenciones);
            printf("Total de votos: %d\n", comisionActual->votacion->totalVotos);
            printf("Resultado: %s\n", comisionActual->votacion->resultado == 1 ? "Aprobado" : "Rechazado");
        }
        else
        {
            printf("No hay votación registrada en la comisión '%s'.\n", comisionActual->nombreDeComision);
        }

        actual = actual->sig;
    }
}

void mostrarResultadosVotacionesCamara(struct Camara *camara, char *nombreCamara)
{
    float porcentajeAprobacion = 0.0;

    if (camara == NULL)
    {
        printf("Error: Cámara no inicializada.\n");
        return;
    }

    printf("\n--- Resultados de Votación para %s ---\n", nombreCamara);

    if (camara->votacionGeneral != NULL)
    {
        printf("Fecha de votación: %s\n", camara->votacionGeneral->fechaVotacion);
        printf("Votos a favor: %d\n", camara->votacionGeneral->votosAFavor);
        printf("Votos en contra: %d\n", camara->votacionGeneral->votosEnContra);
        printf("Abstenciones: %d\n", camara->votacionGeneral->abstenciones);
        printf("Total de votos: %d\n", camara->votacionGeneral->totalVotos);

        porcentajeAprobacion = calcularPorcentajeAprobacion(camara->votacionGeneral);
        printf("Porcentaje de aprobación: %.2f%%\n", porcentajeAprobacion);
        printf("Resultado: %s\n", (camara->votacionGeneral->resultado == 1) ? "Aprobado" : "Rechazado");
    }
    else
    {
        printf("No hay votación registrada para esta cámara.\n");
    }
}

void crearVotacionParaComision(struct NodoComision *comisiones)
{
    char nombreComision[100];
    char fechaVotacion[50];
    int cantidadParticipantes;
    struct Comision *comisionSeleccionada = NULL;
    struct Votacion *nuevaVotacion = NULL;


    printf("Ingrese el nombre de la comisión: ");
    scanf("%s", nombreComision);
    comisionSeleccionada = buscarComision(comisiones, nombreComision);

    if (comisionSeleccionada)
    {
        printf("Ingrese la fecha de la votación (DD/MM/AAAA): ");
        scanf("%s", fechaVotacion);
        printf("Ingrese la cantidad de participantes: ");
        scanf("%d", &cantidadParticipantes);
        nuevaVotacion = crearVotacion(fechaVotacion,cantidadParticipantes);

        if (nuevaVotacion != NULL)
        {
            printf("Ingrese la cantidad de participantes: ");
            scanf("%d", &cantidadParticipantes);

            comisionSeleccionada->votacion = nuevaVotacion;
            inicializarVotacion(comisionSeleccionada->votacion, cantidadParticipantes);
            printf("Votación creada con éxito para la comisión '%s' con %d participantes.\n", nombreComision, cantidadParticipantes);
        }
        else
        {
            printf("Error al crear la votación.\n");
        }
    }
    else
    {
        printf("No se encontró la comisión '%s'.\n", nombreComision);
    }
}

void crearVotacionCamaraDiputados(struct Camara *camaraDiputados)
{
    char fechaVotacion[50];
    int cantidadParticipantes;

    printf("Ingrese la fecha de la votación para la Cámara de Diputados (DD/MM/AAAA): ");
    scanf("%s", fechaVotacion);

    printf("Ingrese la cantidad de participantes: ");
    scanf("%d", &cantidadParticipantes);

    camaraDiputados->votacionGeneral = crearVotacion(fechaVotacion,cantidadParticipantes);
    if (camaraDiputados->votacionGeneral != NULL)
    {
        inicializarVotacion(camaraDiputados->votacionGeneral, cantidadParticipantes);
        printf("Votación creada con éxito para la Cámara de Diputados con %d participantes.\n", cantidadParticipantes);
    }
    else
    {
        printf("Error al crear la votación.\n");
    }
}

void crearVotacionCamaraSenadores(struct Camara *camaraSenadores)
{
    char fechaVotacion[50];
    int cantidadParticipantes;

    printf("Ingrese la fecha de la votación para la Cámara de Senadores (DD/MM/AAAA): ");
    scanf("%s", fechaVotacion);

    printf("Ingrese la cantidad de participantes: ");
    scanf("%d", &cantidadParticipantes);

    camaraSenadores->votacionGeneral = crearVotacion(fechaVotacion,cantidadParticipantes);
    if (camaraSenadores->votacionGeneral != NULL)
    {
        inicializarVotacion(camaraSenadores->votacionGeneral, cantidadParticipantes);
        printf("Votación creada con éxito para la Cámara de Senadores con %d participantes.\n", cantidadParticipantes);
    }
    else
    {
        printf("Error al crear la votación.\n");
    }
}

void agregarVotoAComision(struct NodoComision *comisiones)
{
    char rutPolitico[20];
    struct Politico *parlamentarioSeleccionado = NULL;
    struct Voto *nuevoVoto = NULL;
    int tipoVoto;
    int cantidadVotantes;

    printf("Ingrese el RUT del parlamentario para agregar el voto: ");
    scanf("%s", rutPolitico);

    parlamentarioSeleccionado = buscarPolitico(comisiones->comision->miembros, rutPolitico);

    if (parlamentarioSeleccionado != NULL)
    {
        printf("Ingrese el tipo de voto (1 = a favor, 2 = en contra, 3 = abstención): ");
        scanf("%d", &tipoVoto);

        printf("Ingrese la cantidad de votantes: ");
        scanf("%d", &cantidadVotantes);

        nuevoVoto = crearVoto(parlamentarioSeleccionado, tipoVoto);

        if (agregarVoto(comisiones->comision->votacion->votos, cantidadVotantes, nuevoVoto))
        {
            printf("Voto agregado con éxito.\n");
        }
        else
        {
            printf("No se pudo agregar el voto o ya existe.\n");
        }
    }
    else
    {
        printf("No se encontró al parlamentario con RUT '%s'.\n", rutPolitico);
    }
}

void eliminarVotoDeComision(struct Comision *comisionSeleccionada)
{
    char rutPolitico[20];
    int cantidadParticipantes;

    if (comisionSeleccionada == NULL)
    {
        printf("No se ha seleccionado ninguna comisión.\n");
        return;
    }

    printf("Ingrese el RUT del parlamentario para eliminar el voto: ");
    scanf("%s", rutPolitico);

    cantidadParticipantes = comisionSeleccionada->votacion->totalVotos;

    if (eliminarVoto(comisionSeleccionada->votacion->votos, cantidadParticipantes, rutPolitico))
    {
        printf("Voto eliminado con éxito.\n");
    }
    else
    {
        printf("No se encontró el voto para el RUT proporcionado.\n");
    }
}

void menuVotaciones(struct NodoComision *comisiones, struct Camara *camaraDiputados, struct Camara *camaraSenadores)
{
    int opcion;
    char nombreComision[100];
    char fechaVotacion[50];
    struct Comision *comisionSeleccionada = NULL;
    struct Votacion *nuevaVotacion = NULL;
    char rutPolitico[20];
    int tipoVoto;
    struct Politico *parlamentarioSeleccionado = NULL;
    struct Voto *nuevoVoto = NULL;
    int cantidadParticipantes;

    while (1)
    {
        printf("\n--- Menu Votaciones ---\n");
        printf("1. Crear Votación para una Comisión\n");
        printf("2. Crear Votación para la Cámara de Diputados\n");
        printf("3. Crear Votación para la Cámara de Senadores\n");
        printf("4. Agregar Voto a una Votación\n");
        printf("5. Eliminar Voto de una Votación\n");
        printf("6. Mostrar Resultados de Votaciones de Comisiones\n");
        printf("7. Mostrar Resultados de Votaciones de las Cámaras\n");
        printf("8. Volver al Menu Principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion)
        {
            case 1:
                crearVotacionParaComision(comisiones);
                break;


            case 2:
                crearVotacionCamaraDiputados(camaraDiputados);
                 break;

            case 3:
                crearVotacionCamaraSenadores(camaraSenadores);
                break;

            case 4:
                agregarVotoAComision(comisiones);
                break;

            case 5:
                eliminarVotoDeComision(comisionSeleccionada);
                break;

            case 6:
                mostrarResultadosVotacionesComisiones(comisiones);
                break;

            case 7:
                mostrarResultadosVotacionesCamara(camaraDiputados, "Cámara de Diputados");
                mostrarResultadosVotacionesCamara(camaraSenadores, "Cámara de Senadores");
                break;

            case 8:
                printf("Volviendo al Menu Principal...\n");
                return;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
    }
}

void menuTribunalConstitucional(struct TribunalConstitucional *tribunal) {
    int opcion;
    int esRevisado, esConstitucional;

    while (1) {
        printf("\n--- Menú Tribunal Constitucional ---\n");
        printf("1. Crear Tribunal Constitucional\n");
        printf("2. Controlar Constitucionalidad\n");
        printf("3. Mostrar Estado del Tribunal Constitucional\n");
        printf("4. Volver al Menú Principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                if (tribunal == NULL) {
                    tribunal = crearTribunalConstitucional();
                    if (tribunal != NULL) {
                        printf("Tribunal Constitucional creado con éxito.\n");
                    } else {
                        printf("Error al crear el Tribunal Constitucional.\n");
                    }
                } else {
                    printf("El Tribunal Constitucional ya ha sido creado.\n");
                }
                break;

            case 2:
                if (tribunal != NULL) {
                    printf("Ingrese 1 si es revisado y 0 si no: ");
                    scanf("%d", &esRevisado);
                    printf("Ingrese 1 si es constitucional y 0 si no: ");
                    scanf("%d", &esConstitucional);
                    if (controlConstitucional(tribunal, esRevisado, esConstitucional)) {
                        printf("Estado del Tribunal Constitucional actualizado.\n");
                    } else {
                        printf("Error al actualizar el estado del Tribunal Constitucional.\n");
                    }
                } else {
                    printf("Primero debe crear el Tribunal Constitucional.\n");
                }
                break;

            case 3:
                if (tribunal != NULL) {
                    printf("Estado del Tribunal Constitucional:\n");
                    printf("Revisado: %s\n", tribunal->esRevisado ? "Sí" : "No");
                    printf("Constitucional: %s\n", tribunal->esConstitucional ? "Sí" : "No");
                } else {
                    printf("No hay Tribunal Constitucional creado.\n");
                }
                break;

            case 4:
                printf("Volviendo al Menú Principal...\n");
                return;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
    }
}

void menuPresidencia(struct Presidencia *presidencia) {
    int opcion;
    char motivo[256];
    int tipoVeto;

    while (1) {
        printf("\n--- Menu de Presidencia ---\n");
        printf("1. Asignar Veto\n");
        printf("2. Mostrar Información de la Presidencia\n");
        printf("3. Volver al Menu Principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Ingrese el tipo de veto (1 = Veto parcial, 2 = Veto total): ");
            scanf("%d", &tipoVeto);
            printf("Ingrese el motivo del veto: ");
            scanf(" %[^\n]", motivo); // Leer cadena con espacios

            // Crear y asignar el veto
            if (presidencia->veto != NULL) {
                // Si ya existe un veto, se podría liberar antes de crear uno nuevo
                free(presidencia->veto->motivo);
                free(presidencia->veto);
            }
            presidencia->veto = crearVeto(tipoVeto, motivo);
            printf("Veto asignado con éxito.\n");
            break;

            case 2:
                mostrarPresidencia(presidencia);
            break;

            case 3:
                printf("Volviendo al Menu Principal...\n");
            return;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
            break;
        }
    }
}


void generarDatosDePrueba(struct SistemaLegislativo *sistema) {
    // Creación de proyectos de ley
    struct ProyectoLey *proyecto1 = (struct ProyectoLey *)malloc(sizeof(struct ProyectoLey));
    proyecto1->idProyecto = 1;
    proyecto1->tituloProyecto = strdup("Reforma Educativa");
    proyecto1->iniciativaLegislativa = strdup("Ministerio de Educación");
    proyecto1->urgenciaProyecto = 2;
    proyecto1->estadoProyecto = strdup("En discusión");

    struct ProyectoLey *proyecto2 = (struct ProyectoLey *)malloc(sizeof(struct ProyectoLey));
    proyecto2->idProyecto = 2;
    proyecto2->tituloProyecto = strdup("Ley de Salud Pública");
    proyecto2->iniciativaLegislativa = strdup("Ministerio de Salud");
    proyecto2->urgenciaProyecto = 1;
    proyecto2->estadoProyecto = strdup("Aprobado");

    struct ProyectoLey *proyecto3 = (struct ProyectoLey *)malloc(sizeof(struct ProyectoLey));
    proyecto3->idProyecto = 3;
    proyecto3->tituloProyecto = strdup("Protección de Datos Personales");
    proyecto3->iniciativaLegislativa = strdup("Ministerio de Ciencia");
    proyecto3->urgenciaProyecto = 3;
    proyecto3->estadoProyecto = strdup("En espera");

    // Inicialización de diputados
    struct Politico *diputado1 = (struct Politico *)malloc(sizeof(struct Politico));
    diputado1->nombrePolitico = strdup("Juan Perez");
    diputado1->rut = strdup("12345678-9");
    diputado1->partido = strdup("Partido A");
    diputado1->proyectos = (struct ProyectoLey **)malloc(2 * sizeof(struct ProyectoLey *));
    diputado1->proyectos[0] = proyecto1;
    diputado1->proyectos[1] = proyecto2;
    diputado1->cantProyectos = 2;

    struct Politico *diputado2 = (struct Politico *)malloc(sizeof(struct Politico));
    diputado2->nombrePolitico = strdup("Maria Lopez");
    diputado2->rut = strdup("98765432-1");
    diputado2->partido = strdup("Partido B");
    diputado2->proyectos = (struct ProyectoLey **)malloc(1 * sizeof(struct ProyectoLey *));
    diputado2->proyectos[0] = proyecto3;
    diputado2->cantProyectos = 1;

    // Creación de lista de diputados
    struct NodoParlamentario *nodoDiputado1 = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    nodoDiputado1->parlamentario = diputado1;
    nodoDiputado1->ant = NULL;
    nodoDiputado1->sig = NULL;

    struct NodoParlamentario *nodoDiputado2 = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    nodoDiputado2->parlamentario = diputado2;
    nodoDiputado2->ant = nodoDiputado1;
    nodoDiputado2->sig = NULL;
    nodoDiputado1->sig = nodoDiputado2;

    sistema->diputados = nodoDiputado1;

    // Inicialización de senadores
    struct Politico *senador1 = (struct Politico *)malloc(sizeof(struct Politico));
    senador1->nombrePolitico = strdup("Carlos Diaz");
    senador1->rut = strdup("54321678-9");
    senador1->partido = strdup("Partido C");
    senador1->proyectos = (struct ProyectoLey **)malloc(1 * sizeof(struct ProyectoLey *));
    senador1->proyectos[0] = proyecto1;
    senador1->cantProyectos = 1;

    struct NodoParlamentario *nodoSenador1 = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    nodoSenador1->parlamentario = senador1;
    nodoSenador1->ant = NULL;
    nodoSenador1->sig = NULL;

    sistema->senadores = nodoSenador1;

    // Creación de comisiones
    struct Comision *comisionSalud = (struct Comision *)malloc(sizeof(struct Comision));
    comisionSalud->nombreDeComision = strdup("Comisión de Salud");
    comisionSalud->miembros = nodoDiputado1; // Primera comisión con un diputado
    comisionSalud->votacion = NULL;

    struct Comision *comisionEducacion = (struct Comision *)malloc(sizeof(struct Comision));
    comisionEducacion->nombreDeComision = strdup("Comisión de Educación");
    comisionEducacion->miembros = nodoSenador1; // Segunda comisión con un senador
    comisionEducacion->votacion = NULL;

    struct NodoComision *nodoComisionSalud = (struct NodoComision *)malloc(sizeof(struct NodoComision));
    nodoComisionSalud->comision = comisionSalud;
    nodoComisionSalud->sig = NULL;

    struct NodoComision *nodoComisionEducacion = (struct NodoComision *)malloc(sizeof(struct NodoComision));
    nodoComisionEducacion->comision = comisionEducacion;
    nodoComisionEducacion->sig = nodoComisionSalud;

    sistema->comisionMixta = nodoComisionEducacion;

    // Creación de votaciones
    struct Votacion *votacion1 = (struct Votacion *)malloc(sizeof(struct Votacion));
    votacion1->fechaVotacion = strdup("2024-11-01");
    votacion1->votosAFavor = 8;
    votacion1->votosEnContra = 5;
    votacion1->abstenciones = 1;
    votacion1->totalVotos = 14;
    votacion1->resultado = 1;

    sistema->camaraDiputados->votacionGeneral = votacion1;

    struct Votacion *votacion2 = (struct Votacion *)malloc(sizeof(struct Votacion));
    votacion2->fechaVotacion = strdup("2024-11-02");
    votacion2->votosAFavor = 4;
    votacion2->votosEnContra = 7;
    votacion2->abstenciones = 3;
    votacion2->totalVotos = 14;
    votacion2->resultado = 0;

    sistema->camaraSenadores->votacionGeneral = votacion2;

    printf("Datos de prueba generados correctamente.\n");
}


int menuPrincipal(struct SistemaLegislativo *sistema, struct Presidencia *presidencia) {
    int opcion;

    while (1) {
        printf("\n ------ Sistema Legislativo -----\n");
        printf("1. Menu Politicos\n");
        printf("2. Menu Proyectos\n");
        printf("3. Menu Comisiones\n");
        printf("4. Menu Votaciones\n");
        printf("5. Tribunal Constitucional\n");
        printf("6. Presidencia\n");
        printf("7. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                mostrarMenuPoliticos(&sistema->diputados,&sistema->senadores);
                break;
            case 2:
                mostrarMenuProyectos(&sistema->abbProyectos);
                break;
            case 3:
                menuComisiones(&sistema->comisionMixta,sistema->diputados);
                break;
            case 4:
                menuVotaciones(sistema->comisionMixta,sistema->camaraDiputados,sistema->camaraSenadores);
                break;
            case 5:
                menuTribunalConstitucional(sistema->TC);
                break;
            case 6:
                menuPresidencia(presidencia);
                break;
            case 7:
                printf("Saliendo...\n");
                return 0;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
    }
}


int main() {
    struct SistemaLegislativo *sistema;
    struct Presidencia *presidencia;

    sistema =crearSistemaLegislativo();
    presidencia = crearPresidencia("Gabriel Boric", sistema);

    menuPrincipal(sistema,presidencia);
    return 0;
}

