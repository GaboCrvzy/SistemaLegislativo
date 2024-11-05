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
    char *rutPolitico; 
    int tipoVoto;         // 1 = a favor, 2 = en contra, 3 = abstención
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
    struct NodoParlamentario *rec = headLista;

    while(rec != NULL)
    {
        if (strcmp(rec->parlamentario->rut, rutPolitico) == 0)
        {
            return rec->parlamentario;
        }
        rec = rec->sig;
    }

    return NULL;
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

void BuscarPoliticoPorRut(struct NodoParlamentario *listaParlamentarios,  char *rut)
{
    struct NodoParlamentario *actual = listaParlamentarios;
    while (actual != NULL) {
        if (strcmp(actual->parlamentario->rut, rut) == 0) 
        {
            printf("Información del político con RUT %s:\n", rut);
            printf("Nombre: %s\n", actual->parlamentario->nombrePolitico);
            printf("Partido: %s\n", actual->parlamentario->partido);
            printf("Cantidad de Proyectos: %d\n", actual->parlamentario->cantProyectos);

            for (int i = 0; i < actual->parlamentario->cantProyectos; i++) {
                printf("Proyecto %d: %s\n", i + 1, actual->parlamentario->proyectos[i]->tituloProyecto);
            }
            return;
        }
        actual = actual->sig;
    }
    printf("Político con RUT %s no encontrado.\n", rut);
}

void agregarPoliticoACamara(struct Camara *camara, struct Politico *politico, struct Comision *comision) 
{
    if (camara == NULL || politico == NULL) return;
    struct NodoParlamentario *nuevoNodo;
    struct NodoParlamentario *nuevoNodoComision;
    struct NodoParlamentario *actual;
    struct NodoParlamentario *actualComision;

    nuevoNodo = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    nuevoNodo->parlamentario = politico;
    nuevoNodo->ant = NULL;
    nuevoNodo->sig = NULL;

    if (camara->listaParlamentarios == NULL) {
        camara->listaParlamentarios = nuevoNodo;
    } 
    else 
    {
        actual = camara->listaParlamentarios;
        while (actual->sig != NULL){
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
        nuevoNodo->ant = actual;
    }

    if (comision != NULL) 
    {
        nuevoNodoComision = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
        nuevoNodoComision->parlamentario = politico;
        nuevoNodoComision->ant = NULL;
        nuevoNodoComision->sig = NULL;

        if (comision->miembros == NULL) {
            comision->miembros = nuevoNodoComision;
        } 
        else 
        {
            actualComision = comision->miembros;
            while (actualComision->sig != NULL) {
                actualComision = actualComision->sig;
            }
            actualComision->sig = nuevoNodoComision;
            nuevoNodoComision->ant = actualComision;
        }
    }
}

struct Voto *crearVoto(char *rutPolitico, int tipoVoto) 
{
    struct Voto *nuevoVoto;
    
    nuevoVoto = (struct Voto *)malloc(sizeof(struct Voto));
    if (nuevoVoto == NULL) return NULL;

    nuevoVoto->rutPolitico = (char *)malloc((strlen(rutPolitico) + 1) * sizeof(char));
    if (nuevoVoto->rutPolitico == NULL) {
        return NULL;
    }
    strcpy(nuevoVoto->rutPolitico, rutPolitico);
    nuevoVoto->tipoVoto = tipoVoto;
    return nuevoVoto;
}

int buscarVoto(struct Voto **votos, int tam, char *rut)
{
    for (int i = 0; i < tam; i++) {
        if (votos[i] != NULL && strcmp(votos[i]->rutPolitico, rut) == 0) {
            return 1;
        }
    }
    return 0;
}


int agregarVoto(struct Voto **votos, struct Voto *nuevoVoto, int capacidad) 
{
    int i;
    for (i = 0; i < capacidad; i++)
    {
        if (votos[i] == NULL && buscarVoto(votos, capacidad, nuevoVoto->rutPolitico) == 0) 
        {
            votos[i] = nuevoVoto;
            return 1;
        }
    }

    return 0;
}


int eliminarVoto(struct Votacion *votacion, char *rut)
{
    int i;
    for (i = 0; i < votacion->totalVotos; i++) 
    {
        if (votacion->votos[i] != NULL && strcmp(votacion->votos[i]->rutPolitico, rut) == 0) 
        {
            votacion->votos[i] = NULL;
            votacion->totalVotos--;
            return 1;
        }
    }
    return 0; 
}


int inicializarVotacion(struct Votacion *votacion, int cantidadParticipantes) 
{
    if (votacion == NULL || cantidadParticipantes <= 0) return 0;

    votacion->votos = (struct Voto **)malloc(cantidadParticipantes * sizeof(struct Voto *));
    if (votacion->votos == NULL) return 0;

    votacion->votosAFavor = 0;
    votacion->votosEnContra = 0;
    votacion->abstenciones = 0;
    votacion->totalVotos = 0;
    votacion->resultado = 0;

    return 1;
}

struct Votacion *crearVotacion(char *fechaInicio, int cantidadParticipantes) 
{
    if (fechaInicio == NULL) return NULL;

    struct Votacion *votacionNueva = (struct Votacion *)malloc(sizeof(struct Votacion));
    if (votacionNueva == NULL) return NULL;

    votacionNueva->fechaVotacion = (char *)malloc((strlen(fechaInicio) + 1) * sizeof(char));
    if (votacionNueva->fechaVotacion == NULL) {

        return NULL;
    }
    strcpy(votacionNueva->fechaVotacion, fechaInicio);
    votacionNueva->votos = NULL;

    if (!inicializarVotacion(votacionNueva, cantidadParticipantes)) {
        free(votacionNueva->fechaVotacion);
        free(votacionNueva);
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
    struct NodoParlamentario *nuevoNodo;

    if (buscarMiembroEnComision(comision->miembros, nuevoMiembro->rut) == 1)
        return 0;

    nuevoNodo = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
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

int idProyectoExiste(struct NodoABB *raiz, int id)
{
    if (raiz == NULL) return 0; 

    if (raiz->proyecto->idProyecto == id) {
        return 1;
    } else if (id < raiz->proyecto->idProyecto) {
        return idProyectoExiste(raiz->izquierda, id); 
    } else {
        return idProyectoExiste(raiz->derecha, id); 
    }
}

struct ProyectoLey *crearProyecto(struct NodoABB *abbProyectos, int id, char *titulo, char *iniciativa, int urgencia, 
char *estadoDelProyecto)
{

    struct ProyectoLey *nuevoProyecto = (struct ProyectoLey *)malloc(sizeof(struct ProyectoLey));
    if (nuevoProyecto == NULL) return NULL;

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

void agregarProyectoLey(struct SistemaLegislativo *sistema, struct ProyectoLey *proyecto) 
{
    if (sistema == NULL || proyecto == NULL) return;
    struct NodoABB *nuevoNodo;

    nuevoNodo = (struct NodoABB *)malloc(sizeof(struct NodoABB));
    nuevoNodo->proyecto = proyecto;
    nuevoNodo->izquierda = NULL;
    nuevoNodo->derecha = NULL;

    if (sistema->abbProyectos == NULL) {
        sistema->abbProyectos = nuevoNodo;
    } 
    else 
    {
        struct NodoABB *actual = sistema->abbProyectos;
        struct NodoABB *anterior = NULL;

        while (actual != NULL) 
        {
            anterior = actual;
            if (proyecto->idProyecto < actual->proyecto->idProyecto) 
            {
                actual = actual->izquierda;
            } 
            else 
            {
                actual = actual->derecha;
            }
        }

        if (proyecto->idProyecto < anterior->proyecto->idProyecto) {
            anterior->izquierda = nuevoNodo;
        }
        else
        {
            anterior->derecha = nuevoNodo;
        }
    }
}
void agregarProyectoAPolitico(struct Politico *politico, struct ProyectoLey *proyecto)
{
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

    if (nuevoProyecto->idProyecto == nodoActual->proyecto->idProyecto)
    {
        printf("Error: El ID %d ya existe. No se puede insertar.\n", nuevoProyecto->idProyecto);
        return nodoActual;
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

struct ProyectoLey *buscarProyecto(struct NodoABB *nodo, int idProyecto)
{
    if (nodo == NULL) return NULL; 

    if (nodo->proyecto->idProyecto == idProyecto) {
        return nodo->proyecto;  
    } 
    else if (idProyecto < nodo->proyecto->idProyecto) {
        return buscarProyecto(nodo->izquierda, idProyecto); 
    } 
    else {
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
    if (sistema == NULL) {
        printf("Error: No se pudo asignar memoria para SistemaLegislativo.\n");
        return NULL;
    }
    
    sistema->TC = crearTribunalConstitucional();
    sistema->abbProyectos = NULL;
    sistema->diputados = NULL;
    sistema->senadores = NULL;
    sistema->comisionMixta = NULL;

    sistema->camaraDiputados = (struct Camara *)malloc(sizeof(struct Camara));
    if (sistema->camaraDiputados == NULL)
    {
        printf("Error: No se pudo asignar memoria para la Camara de Diputados.\n");
        return NULL;
    }

    sistema->camaraDiputados->nombreDeCamara = NULL;
    sistema->camaraDiputados->listaParlamentarios = NULL;
    sistema->camaraDiputados->comisiones = NULL;
    sistema->camaraDiputados->votacionGeneral = NULL;
    sistema->camaraDiputados->votacionEspecifica = NULL;

    sistema->camaraSenadores = (struct Camara *)malloc(sizeof(struct Camara));
    if (sistema->camaraSenadores == NULL) {
        printf(stderr, "Error: No se pudo asignar memoria para camaraSenadores.\n");
        return NULL;
    }

    sistema->camaraSenadores->nombreDeCamara = NULL;
    sistema->camaraSenadores->listaParlamentarios = NULL;
    sistema->camaraSenadores->comisiones = NULL;
    sistema->camaraSenadores->votacionGeneral = NULL;
    sistema->camaraSenadores->votacionEspecifica = NULL;

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
            printf("(No hay miembros en esta comision)\n");
        }

        actual = actual->sig;
    }
}

void mostrarProyectos(struct NodoABB *raiz)
{
    if (raiz == NULL) {
        return;
    }
    mostrarProyectos(raiz->izquierda);
    printf("- Proyecto %d: %s (Estado: %s)\n", raiz->proyecto->idProyecto, raiz->proyecto->tituloProyecto,
        raiz->proyecto->estadoProyecto);
    mostrarProyectos(raiz->derecha);
}

void generarReporteActividades(struct NodoParlamentario *lista)
{
    struct NodoParlamentario *actual = lista;
    struct Politico *politico;
    
    printf("=== Reporte de Actividades de los Parlamentarios ===\n");

    while (actual != NULL)
    {
        politico = actual->parlamentario;
        printf("Politico: %s (RUT: %s, Partido: %s)\n", politico->nombrePolitico, politico->rut, politico->partido);
        printf("Proyectos en los que participa: %d\n", politico->cantProyectos);
        actual = actual->sig;
    }
}

void agregarPoliticoMenu(struct NodoParlamentario **listaParlamentarios)
{
    char nombre[100], rut[20], partido[50];
    struct Politico *nuevoPolitico;

    printf("Ingrese el nombre del politico: ");
    scanf(" %[^\n]s", nombre);

    while (1) {
        printf("Ingrese el RUT del politico (sin puntos ni guion, con digito verificador): ");
        scanf(" %s", rut);

        if (strlen(rut) == 9 && strspn(rut, "0123456789") == 9) {
            break;  
        } 
        else {
            printf("Rut no valido. Intente nuevamente.\n");
        }
    }

    printf("Ingrese el partido del politico: ");
    scanf(" %[^\n]s", partido);

    nuevoPolitico = crearPolitico(nombre, rut, partido);
    if (nuevoPolitico == NULL)
    {
        printf("Error al crear el politico.\n");
        return;
    }
    
    if (enlazarPolitico(listaParlamentarios, nuevoPolitico))
    {
        printf("\nPolitico agregado exitosamente.\n");
    } 
    else {
        printf("No se pudo agregar el politico (puede que ya este en la lista).\n");
    }
}

void eliminarPoliticoMenu(struct NodoParlamentario **listaParlamentarios) 
{
    char rut[20];

    while (1)
    {
        printf("Ingrese el RUT del politico a eliminar (sin puntos ni guion, con digito verificador): ");
        scanf("%s", rut);

        if (strlen(rut) == 9 && strspn(rut, "0123456789") == 9) {
            break; 
        } 
        else {
            printf("Rut no valido. Intente nuevamente.\n");
        }
    }

    if (eliminarPolitico(listaParlamentarios, rut))
    {
        printf("Politico eliminado exitosamente.\n");
    } else {
        printf("No se encontro un político con ese RUT en la lista.\n");
    }
}

void mostrarListaPoliticos(struct NodoParlamentario *lista, char *tipo)
{
    mostrarParlamentario(lista, tipo);
}

void mostrarInformacionPolitico(struct Politico *politico)
{
    int i;
    if (politico == NULL) {
        printf("Politico no encontrado.\n");
        return;
    }

    printf("Informacion del politico con RUT %s:\n", politico->rut);
    printf("Nombre: %s\n", politico->nombrePolitico);
    printf("Partido: %s\n", politico->partido);
    printf("Cantidad de Proyectos: %d\n", politico->cantProyectos);

    for (i = 0; i < politico->cantProyectos; i++) {
        if (politico->proyectos[i] != NULL) {
            printf("Proyecto %d: %s\n", i + 1, politico->proyectos[i]->tituloProyecto);
        } else {
            printf("Proyecto %d no esta inicializado.\n", i + 1);
        }
    }
}

void buscarPoliticoPorRUTMenu(struct NodoParlamentario **lista) 
{
    char rut[20];

    while (1)
    {
        printf("Ingrese RUT del politico (sin puntos ni guion, con digito verificador): ");
        scanf("%s", rut);

        if (strlen(rut) == 9 && strspn(rut, "0123456789") == 9)
        {
            struct Politico *politico = buscarPolitico(*lista, rut);
            mostrarInformacionPolitico(politico);
            break;
        }
        else {
            printf("RUT no valido. Intente nuevamente.\n");
        }
    }
}

void seleccionarPoliticoYBuscar(struct NodoParlamentario **diputados, struct NodoParlamentario **senadores)
{
    int tipoLista;

    printf("Seleccione la lista para buscar:\n");
    printf("1. Diputados\n");
    printf("2. Senadores\n");
    printf("Ingrese una opcion: ");
    scanf("%d", &tipoLista);

    if (tipoLista == 1) {
        buscarPoliticoPorRUTMenu(diputados);
    } else if (tipoLista == 2) {
        buscarPoliticoPorRUTMenu(senadores);
    } else {
        printf("Opcion no valida. Regresando al menu.\n");
    }
}

void mostrarMenuPoliticos(struct NodoParlamentario **diputados, struct NodoParlamentario **senadores)
{
    int opcion;
    do {
        printf("\n=== Menu de Politicos ===\n");
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
                agregarPoliticoMenu(diputados);
                break;
            case 2:
                eliminarPoliticoMenu(diputados);
                break;
            case 3:
                seleccionarPoliticoYBuscar(diputados,senadores);
                break;
            case 4:
                mostrarListaPoliticos(*diputados,"Diputados");
                break;
            case 5:
                agregarPoliticoMenu(senadores);
                break;
            case 6:
                eliminarPoliticoMenu(senadores);
                break;
            case 7:
                mostrarListaPoliticos(*senadores, "Senadores");
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

void mostrarDetallesProyecto(struct ProyectoLey *proyecto)
{
    if (proyecto != NULL) {
        printf("Proyecto encontrado:\n");
        printf("ID: %d\n", proyecto->idProyecto);
        printf("Titulo: %s\n", proyecto->tituloProyecto);
        printf("Iniciativa: %s\n", proyecto->iniciativaLegislativa);
        printf("Urgencia: %d\n", proyecto->urgenciaProyecto);
        printf("Estado: %s\n", proyecto->estadoProyecto);
    } 
    else {
        printf("El proyecto no existe.\n");
    }
}

struct ProyectoLey *leerCrearProyecto(struct NodoABB **proyectos)
{
    int id, urgencia;
    char titulo[100], iniciativa[100], estado[100];
    struct ProyectoLey *nuevoProyecto;

    while (1) {
        printf("Ingrese ID del proyecto (numerico): ");
        if (scanf("%d", &id) != 1) {
            printf("Error: Debe ingresar un ID numerico.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (idProyectoExiste(*proyectos, id)) {
            printf("Error: El ID %d ya esta en uso. Por favor, elija un ID unico.\n", id);
        } else {
            break;
        }
    }

    printf("Ingrese titulo del proyecto: ");
    scanf(" %[^\n]", titulo);

    printf("Ingrese iniciativa legislativa: ");
    scanf(" %[^\n]", iniciativa);

    printf("Ingrese urgencia del proyecto (1: Simple, 2: Suma Urgencia, 3: Discusion Inmediata): ");
    
    if (scanf("%d", &urgencia) != 1 || urgencia < 1 || urgencia > 3) 
    {
        printf("Entrada invalida para la urgencia del proyecto.\n");
        return NULL;
    }
    printf("Ingrese estado del proyecto: ");
    scanf(" %[^\n]", estado);

    nuevoProyecto = crearProyecto(*proyectos, id, titulo, iniciativa, urgencia, estado);
    if (nuevoProyecto == NULL) {
        printf("Error al crear el proyecto.\n");
        return NULL;
    }
    return nuevoProyecto;
}

void mostrarMenuProyectos(struct NodoABB **proyectos) 
{
    int opcion, idProyecto;

    while (1) {
        printf("\n------ Menu de Proyectos -----\n");
        printf("1. Crear y Agregar Proyecto\n");
        printf("2. Mostrar Proyectos\n");
        printf("3. Buscar Proyecto\n");
        printf("4. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) 
        {
            case 1: {
                struct ProyectoLey *proyectoNuevo = leerCrearProyecto(proyectos);
                if (proyectoNuevo != NULL) {
                    *proyectos = insertarProyecto(*proyectos, proyectoNuevo);
                    printf("Proyecto creado y agregado exitosamente.\n");
                } 
                else 
                {
                    printf("No se pudo crear el proyecto debido a un error en la entrada o ID duplicado.\n");
                }
                break;
            }
            case 2:
                printf("\n----- Lista de Proyectos -----\n");
                if (*proyectos == NULL) {
                    printf("No hay proyectos de ley en el sistema.\n");
                } else {
                    mostrarProyectos(*proyectos);
                }
                break;
            case 3:
                printf("Ingrese el ID del proyecto a buscar: ");
                scanf("%d", &idProyecto);
                struct ProyectoLey *proyectoEncontrado = buscarProyecto(*proyectos, idProyecto);
                mostrarDetallesProyecto(proyectoEncontrado); 
                break;
            case 4:
                printf("Volviendo al Menu Principal...\n");
                return;
            default:
                printf("Opcion no valida. Intente nuevamente.\n");
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
        printf("La comision '%s' ya existe.\n", nombreDeComision);
        return;
    }

    nuevaComision = crearComision(nombreDeComision);
    if (nuevaComision == NULL)
    {
        printf("Error al crear la comision.\n");
        return;
    }

    if (enlazarComision(listaComisiones, nuevaComision))
    {
        printf("Comision '%s' agregada con exito.\n", nombreDeComision);
    }
    else
    {
        printf("Error al agregar la comision.\n");
    }
}

void gestionarAgregarMiembroComision(struct NodoComision **listaComisiones,struct NodoParlamentario *listaDiputados,
                                     struct NodoParlamentario *listaSenadores)
{
    char nombreComisionBuscar[100];
    char rutPolitico[20];
    struct Comision *comisionSeleccionada;
    struct Politico *nuevoMiembro;
    int tipoMiembro;

    printf("¿Desea agregar un (1) Diputado o un (2) Senador a la comision? ");
    scanf("%d", &tipoMiembro);

    printf("Ingrese el nombre de la comision a la que desea agregar un miembro: ");
    scanf("%s", nombreComisionBuscar);

    comisionSeleccionada = buscarComision(*listaComisiones, nombreComisionBuscar);
    if (comisionSeleccionada != NULL)
    {
        printf("Ingrese el RUT del politico a agregar: ");
        scanf("%s", rutPolitico);

        if (tipoMiembro == 1)
        {
            nuevoMiembro = buscarPolitico(listaDiputados, rutPolitico);
        }
        else
        {
            nuevoMiembro = buscarPolitico(listaSenadores, rutPolitico);
        }

        if (nuevoMiembro != NULL)
        {
            if (agregarMiembroComision(comisionSeleccionada, nuevoMiembro))
            {
                printf("Miembro '%s' agregado a la comision '%s' con exito.\n", nuevoMiembro->nombrePolitico, nombreComisionBuscar);
            }
            else
            {
                printf("El miembro ya esta en la comision o no se pudo agregar.\n");
            }
        }
        else
        {
            printf("No se encontro el politico con RUT '%s'.\n", rutPolitico);
        }
    }
    else
    {
        printf("No se encontro la comision '%s'.\n", nombreComisionBuscar);
    }
}


void menuComisiones(struct NodoComision **listaComisiones, struct NodoParlamentario *listaDiputados,struct NodoParlamentario *listaSenadores)
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
                    printf("Comision '%s' eliminada con exito.\n", nombreComisionBorrar);
                }
                else
                {
                    printf("No se encontro la comision '%s'.\n", nombreComisionBorrar);
                }
                break;
            case 3:
                printf("Ingrese el nombre de la comision a buscar: ");
                scanf("%s", nombreComisionBuscar);
                if (buscarComision(*listaComisiones, nombreComisionBuscar))
                {
                    printf("La comision '%s' fue encontrada.\n", nombreComisionBuscar);
                }
                else
                {
                    printf("No se encontro la comision '%s'.\n", nombreComisionBuscar);
                }
                break;
            case 4:
                mostrarComisiones(*listaComisiones);
                break;
            case 5:
                gestionarAgregarMiembroComision(listaComisiones, listaDiputados,listaSenadores);
                break;
            case 6:
                printf("Volviendo al Menu Principal...\n");
                return;
            default:
                printf("Opcion no valida. Intente nuevamente.\n");
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
            printf("Resultados de la votacion en la comision '%s':\n", comisionActual->nombreDeComision);
            printf("Fecha de la votacion: %s\n", comisionActual->votacion->fechaVotacion);
            printf("Votos a favor: %d\n", comisionActual->votacion->votosAFavor);
            printf("Votos en contra: %d\n", comisionActual->votacion->votosEnContra);
            printf("Abstenciones: %d\n", comisionActual->votacion->abstenciones);
            printf("Total de votos: %d\n", comisionActual->votacion->totalVotos);
            printf("Resultado: %s\n", comisionActual->votacion->resultado == 1 ? "Aprobado" : "Rechazado");
        }
        else
        {
            printf("No hay votacion registrada en la comision '%s'.\n", comisionActual->nombreDeComision);
        }

        actual = actual->sig;
    }
}

void mostrarResultadosVotacionesCamara(struct Camara *camara, char *nombreCamara)
{
    float porcentajeAprobacion = 0.0;

    if (camara == NULL)
    {
        printf("Error: Camara no inicializada.\n");
        return;
    }

    printf("\n--- Resultados de Votacion para %s ---\n", nombreCamara);

    if (camara->votacionGeneral != NULL)
    {
        printf("Fecha de votacion: %s\n", camara->votacionGeneral->fechaVotacion);
        printf("Votos a favor: %d\n", camara->votacionGeneral->votosAFavor);
        printf("Votos en contra: %d\n", camara->votacionGeneral->votosEnContra);
        printf("Abstenciones: %d\n", camara->votacionGeneral->abstenciones);
        printf("Total de votos: %d\n", camara->votacionGeneral->totalVotos);

        porcentajeAprobacion = calcularPorcentajeAprobacion(camara->votacionGeneral);
        printf("Porcentaje de aprobacion: %.2f%%\n", porcentajeAprobacion);
        printf("Resultado: %s\n", (camara->votacionGeneral->resultado == 1) ? "Aprobado" : "Rechazado");
    }
    else
    {
        printf("No hay votacion registrada para esta camara.\n");
    }
}

void crearVotacionParaComision(struct NodoComision *comisiones)
{
    char nombreComision[100];
    char fechaVotacion[50];
    int cantidadParticipantes;
    struct Comision *comisionSeleccionada = NULL;
    struct Votacion *nuevaVotacion = NULL;

    printf("Ingrese el nombre de la comision: \n");
    fgets(nombreComision, sizeof(nombreComision), stdin);
    nombreComision[strcspn(nombreComision, "\n")] = '\0'; 

    comisionSeleccionada = buscarComision(comisiones, nombreComision);

    if (comisionSeleccionada)
    {
        printf("Ingrese la fecha de la votacion (DD/MM/AAAA): \n");
        fgets(fechaVotacion, sizeof(fechaVotacion), stdin);
        fechaVotacion[strcspn(fechaVotacion, "\n")] = '\0'; 

        printf("Ingrese la cantidad de participantes: \n");
        if (scanf("%d", &cantidadParticipantes) != 1 || cantidadParticipantes <= 0) {
            printf("Error: Cantidad de participantes inválida. Debe ser un número positivo.\n");
            return;
        }

        nuevaVotacion = crearVotacion(fechaVotacion, cantidadParticipantes);

        if (nuevaVotacion != NULL)
        {
            comisionSeleccionada->votacion = nuevaVotacion;
            inicializarVotacion(comisionSeleccionada->votacion, cantidadParticipantes);
            printf("Votacion creada con exito para la comision '%s' con %d participantes.\n",
                comisionSeleccionada->nombreDeComision, cantidadParticipantes);
        }
        else
        {
            printf("Error al crear la votacion.\n");
        }
    }
    else
    {
        printf("No se encontró la comision '%s'.\n", nombreComision);
    }
}

int validarFecha(char *fecha)
{
    int dia, mes, anio;
    
    if (sscanf(fecha, "%d-%d-%d", &dia, &mes, &anio) != 3) return 0; 

    if (anio < 1900 || anio > 2100) {
        return 0; 
    }

    if (mes < 1 || mes > 12) {
        return 0; 
    }

    int diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
        diasPorMes[2] = 29; 
    }

    if (dia < 1 || dia > diasPorMes[mes]) 
    {
        return 0; 
    }
    return 1;
}

void crearVotacionCamaraDiputados(struct Camara *camaraDiputados) 
{
    char fechaVotacion[50];
    int cantidadParticipantes;

    printf("Ingrese la fecha de la votacion para la Camara de Diputados (DD-MM-AAAA): ");
    scanf("%s", fechaVotacion);

    if (!validarFecha(fechaVotacion)) 
    {
        printf("Error: La fecha ingresada no es valida. Asegurese de usar el formato (DD-MM-AAAA).\n");
        return;
    }

    while (1)
        {
        printf("Ingrese la cantidad de participantes (numerico): ");
        if (scanf("%d", &cantidadParticipantes) != 1 || cantidadParticipantes <= 0) {
            printf("Error: La cantidad de participantes debe ser un numero positivo. Intente nuevamente.\n");
            while (getchar() != '\n');
        } else {
            getchar();
            break; 
        }
    }

    camaraDiputados->votacionGeneral = crearVotacion(fechaVotacion, cantidadParticipantes);
    if (camaraDiputados->votacionGeneral != NULL) {
        printf("Votacion creada con exito para la Camara de Diputados con %d participantes.\n", cantidadParticipantes);
    } 
    else {
        printf("Error al crear la votacion.\n");
    }
}

void crearVotacionCamaraSenadores(struct Camara *camaraSenadores) 
{
    char fechaVotacion[50];
    int cantidadParticipantes;

    printf("Ingrese la fecha de la votacion para la Camara de Senadores (DD-MM-AAAA): ");
    scanf("%s", fechaVotacion);

    if (!validarFecha(fechaVotacion)) 
    {
        printf("Error: La fecha ingresada no es valida. Asegurese de usar el formato (DD-MM-AAAA).\n");
        return; 
    }

    while (1)
    {
        printf("Ingrese la cantidad de participantes (numerico): ");
        if (scanf("%d", &cantidadParticipantes) != 1 || cantidadParticipantes <= 0) {
            printf("Error: La cantidad de participantes debe ser un numero positivo. Intente nuevamente.\n");
            while (getchar() != '\n'); 
        } 
        else {
            getchar();
            break;
        }
    }

    camaraSenadores->votacionGeneral = crearVotacion(fechaVotacion, cantidadParticipantes);
    if (camaraSenadores->votacionGeneral != NULL) {
        printf("Votacion creada con exito para la Camara de Senadores con %d participantes.\n", cantidadParticipantes);
    } else {
        printf("Error al crear la votacion.\n");
    }
}

void agregarVotoAComision(struct NodoComision *comisiones)
{
    char rutPolitico[20];
    int tipoVoto;
    struct Politico *parlamentarioSeleccionado = NULL;
    struct Voto *nuevoVoto = NULL;

    printf("Ingrese el RUT del parlamentario para agregar el voto: ");
    scanf("%s", rutPolitico);

    parlamentarioSeleccionado = buscarPolitico(comisiones->comision->miembros, rutPolitico);
    if (parlamentarioSeleccionado == NULL) {
        printf("Error: No se encontró el parlamentario con RUT '%s'.\n", rutPolitico);
        return;
    }

    printf("Ingrese el tipo de voto (1 = A Favor, 2 = En Contra, 3 = Abstención): ");
    if (scanf("%d", &tipoVoto) != 1 || (tipoVoto < 1 || tipoVoto > 3)) {
        printf("Error: Tipo de voto inválido. Debe ser 1, 2 o 3.\n");
        while (getchar() != '\n');
        return;
    }

    if (comisiones != NULL && comisiones->comision != NULL && comisiones->comision->votacion != NULL)
    {
        if (comisiones->comision->votacion->votos == NULL) 
        {
            comisiones->comision->votacion->votos = (struct Voto **)malloc(sizeof(struct Voto *));
            if (comisiones->comision->votacion->votos == NULL)
            {
                printf("Error: No se pudo asignar memoria para la lista de votos.\n");
                return;
            }
            comisiones->comision->votacion->totalVotos = 0; 
        }
        else 
        {
            comisiones->comision->votacion->votos = (struct Voto **)realloc(
                comisiones->comision->votacion->votos, (comisiones->comision->votacion->totalVotos + 1) * sizeof(struct Voto *));
            
            if (comisiones->comision->votacion->votos == NULL) 
            {
                printf("Error: No se pudo reasignar memoria para la lista de votos.\n");
                return;
            }
        }

        nuevoVoto = crearVoto(rutPolitico, tipoVoto);
        if (nuevoVoto == NULL) {
            printf("Error: No se pudo asignar memoria para el voto.\n");
            return;
        }

        comisiones->comision->votacion->votos[comisiones->comision->votacion->totalVotos] = nuevoVoto;
        comisiones->comision->votacion->totalVotos++;

        if (tipoVoto == 1) {
            comisiones->comision->votacion->votosAFavor++;
        } else if (tipoVoto == 2) {
            comisiones->comision->votacion->votosEnContra++;
        } else if (tipoVoto == 3) {
            comisiones->comision->votacion->abstenciones++;
        }
        
        printf("Voto agregado con éxito.\n");
    }
    else {
        printf("Error: La comisión o la votación no están inicializadas.\n");
    }
}

void eliminarVotoDeComision(struct Comision *comisionSeleccionada)
{
    char rutPolitico[20];
    if (comisionSeleccionada == NULL) {
        printf("No se ha seleccionado ninguna comisión.\n");
        return;
    }

    printf("Ingrese el RUT del parlamentario para eliminar el voto: ");
    scanf("%s", rutPolitico);

    if (eliminarVoto(comisionSeleccionada->votacion, rutPolitico)) {
        printf("Voto eliminado con éxito.\n");
    } else {
        printf("No se encontró el voto para el RUT proporcionado.\n");
    }
}

void menuVotaciones(struct NodoComision *comisiones, struct Camara *camaraDiputados, struct Camara *camaraSenadores) {
    int opcion;
    struct Comision *comisionSeleccionada = NULL; // Debe ser establecido según la elección del usuario

    while (1) {
        printf("\n--- Menu Votaciones ---\n");
        printf("1. Crear Votacion para una Comision\n");
        printf("2. Crear Votacion para la Camara de Diputados\n");
        printf("3. Crear Votacion para la Camara de Senadores\n");
        printf("4. Agregar Voto a una Votacion\n");
        printf("5. Eliminar Voto de una Votacion\n");
        printf("6. Mostrar Resultados de Votaciones de Comisiones\n");
        printf("7. Mostrar Resultados de Votaciones de las Camaras\n");
        printf("8. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
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
                eliminarVotoDeComision(comisionSeleccionada); // Asegúrate de que comisionSeleccionada esté establecida
                break;
            case 6:
                mostrarResultadosVotacionesComisiones(comisiones);
                break;
            case 7:
                mostrarResultadosVotacionesCamara(camaraDiputados, "Camara de Diputados");
                mostrarResultadosVotacionesCamara(camaraSenadores, "Camara de Senadores");
                break;
            case 8:
                printf("Volviendo al Menu Principal...\n");
                return;
            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }
    }
}

void menuTribunalConstitucional(struct TribunalConstitucional *tribunal) 
{
    int opcion;
    int esRevisado, esConstitucional;

    while (1) {
        printf("\n--- Menu Tribunal Constitucional ---\n");
        printf("1. Crear Tribunal Constitucional\n");
        printf("2. Controlar Constitucionalidad\n");
        printf("3. Mostrar Estado del Tribunal Constitucional\n");
        printf("4. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                if (tribunal == NULL) {
                    tribunal = crearTribunalConstitucional();
                    if (tribunal != NULL) {
                        printf("Tribunal Constitucional creado con exito.\n");
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
                    printf("Revisado: %s\n", tribunal->esRevisado ? "Si" : "No");
                    printf("Constitucional: %s\n", tribunal->esConstitucional ? "Si" : "No");
                } else {
                    printf("No hay Tribunal Constitucional creado.\n");
                }
                break;

            case 4:
                printf("Volviendo al Menu Principal...\n");
                return;

            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }
    }
}

void menuPresidencia(struct Presidencia *presidencia)
{
    int opcion;
    char motivo[256];
    int tipoVeto;

    while (1) {
        printf("\n--- Menu de Presidencia ---\n");
        printf("1. Asignar Veto\n");
        printf("2. Mostrar Informacion de la Presidencia\n");
        printf("3. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Ingrese el tipo de veto (1 = Veto parcial, 2 = Veto total): ");
                scanf("%d", &tipoVeto);
                printf("Ingrese el motivo del veto: ");
                scanf(" %[^\n]", motivo);

                presidencia->veto = crearVeto(tipoVeto, motivo);
                printf("\nVeto asignado con exito.\n");
                break;
            case 2:
                mostrarPresidencia(presidencia);
                break;
            case 3:
                printf("Volviendo al Menu Principal...\n");
                return;

            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }
    }
}

int menuPrincipal(struct SistemaLegislativo *sistema, struct Presidencia *presidencia) 
{
    int opcion;

    while (1) {
        printf("\n--- Sistema Legislativo ---\n");
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
                mostrarMenuPoliticos(&sistema->diputados, &sistema->senadores);
                break;
            case 2:
                mostrarMenuProyectos(&sistema->abbProyectos);
                break;
            case 3:
                menuComisiones(&sistema->comisionMixta, sistema->diputados, sistema->senadores);
                break;
            case 4:
                menuVotaciones(sistema->comisionMixta, sistema->camaraDiputados, sistema->camaraSenadores);
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
                printf("Opcion no valida. Intente nuevamente.\n");
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
