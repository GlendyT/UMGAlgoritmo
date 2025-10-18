#include "Paquete.h"

Paquete::Paquete(string i, string cId, string origen, string destino, double p, string desc)
    : id(i), clienteId(cId), direccionOrigen(origen), direccionDestino(destino), 
      peso(p), descripcion(desc), estado(SOLICITADO), costo(0.0), mensajeroId("") {
    fechaSolicitud = time(0);
    fechaEntrega = 0;
}

string Paquete::getEstadoString() const {
    switch(estado) {
        case SOLICITADO: return "Solicitado";
        case EN_TRANSITO: return "En Transito";
        case ENTREGADO: return "Entregado";
        default: return "Desconocido";
    }
}

/*Aqui es donde funciona el codigo, aqui se le asigna a cada orden de paquetes sus acciones o funciones, por ejemplo cuando se crea un paquete aqui es donde se genera todo el resumen de la orden */