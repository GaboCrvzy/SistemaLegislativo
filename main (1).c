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
    char *estadoActual;    
    struct NodoParlamentario *miembros; 
    struct Votacion *votacion;
};

struct Camara {
    char *nombreDeCamara;                    
    struct Comision *comisiones;     
    struct Votacion *votacionGeneral;
    struct Votacion *votacionEspecifica;
};

struct ProyectoLey {
    int id;                         // Identificador UNICO del proyecto
    char *tituloProyecto;  
    char *iniciativaLegislativa; 
    int urgenciaProyecto;
    char *estadoProyecto;
};

struct NodoABB {
    struct ProyectoLey *proyecto; 
    struct NodoABB *izq;           // Proyectos con ID menor
    struct NodoABB *der;           // Proyectos con ID mayor
};

struct SistemaLegislativo {
    struct TribunalConstitucional *TC;
    struct NodoABB *abbProyectos;          // ABB de proyectos de ley por ID
    struct NodoParlamentario *diputados; 
    struct NodoParlamentario *senadores;
    struct Camara *camaraDiputados;
    struct Camara *camaraSenadores;
    struct Comision *comisionMixta;
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


int buscarPolitico(struct NodoParlamentario *headLista, char * rutPolitico)
{
    struct NodoParlamentario *rec = NULL;
    
    rec = headLista;
    
    while(rec!= NULL)
    {
        if(strcmp(rec->parlamentario->rut,rutPolitico) == 0 ) return 1;
        rec = rec->sig;
    }
    
    return 0;
}

int enlazarPolitico(struct NodoParlamentario **headLista, struct Politico *nuevoPolitico)
{

    struct NodoParlamentario *nuevoNodo , *rec;

    if(*headLista == NULL && buscarPolitico(*headLista, nuevoPolitico->rut) == 0) {
        nuevoNodo = (struct NodoParlamentario*)malloc(sizeof(struct NodoParlamentario));
        nuevoNodo->parlamentario = nuevoPolitico;
        nuevoNodo->ant = NULL;
        nuevoNodo->sig = NULL;
        
        *headLista = nuevoNodo;
        return 1;
    }else{

        rec = *headLista;
        while(rec->sig != NULL)
            rec = rec->sig;
        
        nuevoNodo = (struct NodoParlamentario*)malloc(sizeof(struct NodoParlamentario));

        nuevoNodo->parlamentario = nuevoPolitico;
        nuevoNodo->ant = rec;
        nuevoNodo->sig = NULL;
        
        rec->sig = nuevoNodo;
        return 1;
    }
    return 0;
}


int eliminarPolitico(struct NodoParlamentario **headLista, char *rut) {
    struct NodoParlamentario *rec = *headLista;

    // Caso 1: La lista está vacía
    if (rec == NULL) {
        return 0; // No hay nada que eliminar
    }

    // Caso 2: El primer nodo es el que queremos eliminar

    if (strcmp(rec->parlamentario->rut, rut) == 0) {
        *headLista = rec->sig; // Actualizar la cabeza de la lista
        if (*headLista != NULL) {
            (*headLista)->ant = NULL; // El nuevo primer nodo no tiene anterior
        }

        return 1;
    }

    // Caso 3: Buscar en el resto de la lista
    while (rec != NULL && strcmp(rec->parlamentario->rut, rut) != 0) {
        rec = rec->sig;
    }

    // Si no se encontró el político
    if (rec == NULL) {
        return 0;
    }

    // Caso 4: El nodo a eliminar está en el medio o al final de la lista
    if (rec->ant != NULL) {
        rec->ant->sig = rec->sig; // Ajustar el enlace del nodo anterior
    }
    if (rec->sig != NULL) {
        rec->sig->ant = rec->ant; // Ajustar el enlace del nodo siguiente
    }


    return 1; // Eliminación exitosa
}

struct Voto *crearVoto(struct Politico *parlamentario, int tipoVoto) {
    struct Voto *nuevoVoto = (struct Voto *)malloc(sizeof(struct Voto));
    if (nuevoVoto == NULL) return NULL;

    nuevoVoto->parlamentario = parlamentario;
    nuevoVoto->tipoVoto = tipoVoto;

    return nuevoVoto;
}

int inicializarVotacion(struct Votacion *votacion, int capacidadInicial) {
    votacion->votos = (struct Voto **)malloc(capacidadInicial * sizeof(struct Voto *));
    if (votacion->votos == NULL) return 0;

    votacion->totalVotos = 0;
    votacion->votosAFavor = 0;
    votacion->votosEnContra = 0;
    votacion->abstenciones = 0;

    return 1;
}





int eliminarVoto(struct Votacion *votacion, char *rut) {
    for (int i = 0; i < votacion->totalVotos; i++) {
        if (strcmp(votacion->votos[i]->parlamentario->rut, rut) == 0) {
            struct Voto *votoAEliminar = votacion->votos[i];

            if (votoAEliminar->tipoVoto == 1) votacion->votosAFavor--;
            else if (votoAEliminar->tipoVoto == 2) votacion->votosEnContra--;
            else if (votoAEliminar->tipoVoto == 3) votacion->abstenciones--;

            free(votoAEliminar);

            votacion->votos[i] = votacion->votos[votacion->totalVotos - 1];
            votacion->totalVotos--;

            return 1; // Eliminación exitosa
        }
    }

    return 0; // No se encontró el voto
}

struct Voto *buscarVoto(struct Votacion *votacion, char *rut) {
    for (int i = 0; i < votacion->totalVotos; i++) {
        if (strcmp(votacion->votos[i]->parlamentario->rut, rut) == 0) {
            return votacion->votos[i];
        }
    }
    return NULL; // No se encontró el voto
}

struct Comision *crearComision(char *estadoActual) {
    struct Comision *nuevaComision = (struct Comision *)malloc(sizeof(struct Comision));
    if (nuevaComision == NULL) return NULL;

    nuevaComision->estadoActual = (char *)malloc((strlen(estadoActual) + 1) * sizeof(char));
    if (nuevaComision->estadoActual == NULL) {
        free(nuevaComision);
        return NULL;
    }
    strcpy(nuevaComision->estadoActual, estadoActual);

    nuevaComision->miembros = NULL;  // Lista vacía de miembros
    inicializarVotacion(&nuevaComision->votacion, 10);  // Inicializamos la votación con una capacidad inicial

    return nuevaComision;
}

int agregarMiembroComision(struct Comision *comision, struct Politico *nuevoMiembro) {
    if (comision == NULL || nuevoMiembro == NULL) return 0;

    return enlazarPolitico(&comision->miembros, nuevoMiembro);  // Utiliza la función enlazarPolitico
}

struct Politico *buscarMiembroComision(struct Comision *comision, char *rut) {
    if (comision == NULL || rut == NULL) return NULL;

    struct NodoParlamentario *nodoEncontrado = comision->miembros;
    while (nodoEncontrado != NULL) {
        if (strcmp(nodoEncontrado->parlamentario->rut, rut) == 0) {
            return nodoEncontrado->parlamentario;
        }
        nodoEncontrado = nodoEncontrado->sig;
    }

    return NULL;  // Miembro no encontrado
}

int eliminarMiembroComision(struct Comision *comision, char *rut) {
    if (comision == NULL || rut == NULL) return 0;

    return eliminarPolitico(&comision->miembros, rut);  // Utiliza la función eliminarPolitico
}

int modificarEstadoComision(struct Comision *comision, char *nuevoEstado) {
    if (comision == NULL || nuevoEstado == NULL) return 0;

    comision->estadoActual = (char *)malloc((strlen(nuevoEstado) + 1) * sizeof(char));
    if (comision->estadoActual == NULL) return 0;

    strcpy(comision->estadoActual, nuevoEstado);
    return 1;
}


void listarMiembrosComision(struct Comision *comision) {
    if (comision == NULL) {
        printf("La comisión no existe.\n");
        return;
    }

    struct NodoParlamentario *actual = comision->miembros;
    printf("Miembros de la Comisión (Estado: %s):\n", comision->estadoActual);

    while (actual != NULL) {
        printf("Nombre: %s, RUT: %s, Partido: %s\n",
               actual->parlamentario->nombrePolitico,
               actual->parlamentario->rut,
               actual->parlamentario->partido);
        actual = actual->sig;
    }
}


