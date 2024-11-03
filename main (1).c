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
        if (buscarPolitico(*headLista, nuevoPolitico->rut) == 0) 
        {
            rec = *headLista;

            while (rec->sig != NULL) {
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

struct Votacion *crearVotacion(char *fechaInicio)
{
    if(fechaInicio == NULL) return NULL; 
    
    struct Votacion *votacionNueva;
    
    votacionNueva = (struct Votacion *)malloc(sizeof(struct Votacion));
    if(votacionNueva == NULL) return NULL;

    votacionNueva->fechaVotacion = (char *)malloc((strlen(fechaInicio) + 1) * sizeof(char));
    strcpy(votacionNueva->fechaVotacion, fechaInicio);

    votacionNueva->votos = NULL;

    return votacionNueva;
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

int buscarComision(struct NodoComision *head, char *nombreComision)
{
    struct NodoComision *rec = head;

    while(rec != NULL)
    {
        if(strcmp(rec->comision->nombreDeComision, nombreComision) == 0)
            return 1;
        
        rec = rec->sig;
    }
    return 0;
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
        if(buscarComision(*headLista, nuevaComision->nombreDeComision) == 0)
        {
            rec = *headLista;
            
            while (rec->sig != NULL) {
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



int agregarMiembroComision(struct Comision *comision, struct Politico *nuevoMiembro) 
{
    if (comision == NULL || nuevoMiembro == NULL) return 0;
}
    struct NodoParlamentario *nuevoNodo = (struct NodoParlamentario *)malloc(sizeof(struct NodoParlamentario));
    if (nuevoNodo == NULL) return 0;

    nuevoNodo->parlamentario = nuevoMiembro; // Referencia al político ya existente
    nuevoNodo->sig = comision->miembros; 
    nuevoNodo->ant = NULL; 

    if (comision->miembros != NULL) {
        comision->miembros->ant = nuevoNodo; 
    }

    comision->miembros = nuevoNodo; 
    return 1;
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


