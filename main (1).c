#include <stdio.h>
#include <stdlib.h>

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
    struct NodoABB *abbProyectos;     
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

struct ComisionMixta {
    char *estadoActual;    
    struct NodoParlamentario *miembros; 
    struct Votacion votacion; 
};

struct Camara {
    char *nombreDeCamara;                    
    struct Comision *comisiones;     
    struct Votacion votacionGeneral;    
    struct Votacion votacionEspecifica; 
};

struct ProyectoLey {
    int id;                         // Identificador UNICO del proyecto
    char *tituloProyecto;  
    char *iniciativaLegislativa; 
    struct Camara camaraOrigen; 
    struct Camara camaraRevisora;  
    struct ComisionMixta *comisionMixta; 
    struct TribunalConstitucional *controlConstitucional; 
};

struct NodoABB {
    struct ProyectoLey *proyecto; 
    struct NodoABB *izq;           // Proyectos con ID menor
    struct NodoABB *der;           // Proyectos con ID mayor
};

struct SistemaLegislativo {
    struct NodoABB *abbProyectos;          // ABB de proyectos de ley por ID
    struct NodoParlamentario *diputados; 
    struct NodoParlamentario *senadores; 
    struct NodoCiudadano *ciudadanos;
};

struct Veto {
    int tipoVeto;                // 1 si veto parcial o 2 si veto total
    char *motivo;                     
};

struct Presidencia {
    char *nombrePresidente;              
    struct Veto *veto;           // NULL si no ha vetado
    struct SistemaLegislativo *sistema;
    struct TribunalConstitucional *TC;
};
