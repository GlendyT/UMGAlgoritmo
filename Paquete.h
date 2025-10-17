#ifndef PAQUETE_H
#define PAQUETE_H

#include <string>
#include <ctime>
using namespace std;

enum EstadoPaquete {
    SOLICITADO,
    EN_TRANSITO,
    ENTREGADO
};

class Paquete {
private:
    string id;
    string clienteId;
    string direccionOrigen;
    string direccionDestino;
    double peso;
    string descripcion;
    EstadoPaquete estado;
    time_t fechaSolicitud;
    time_t fechaEntrega;
    double costo;
    string mensajeroId;

public:
    Paquete(string i, string cId, string origen, string destino, double p, string desc);
    
    // Getters
    string getId() const { return id; }
    string getClienteId() const { return clienteId; }
    string getDireccionOrigen() const { return direccionOrigen; }
    string getDireccionDestino() const { return direccionDestino; }
    double getPeso() const { return peso; }
    string getDescripcion() const { return descripcion; }
    EstadoPaquete getEstado() const { return estado; }
    time_t getFechaSolicitud() const { return fechaSolicitud; }
    time_t getFechaEntrega() const { return fechaEntrega; }
    double getCosto() const { return costo; }
    string getMensajeroId() const { return mensajeroId; }
    
    // Setters
    void setEstado(EstadoPaquete e) { estado = e; }
    void setFechaEntrega(time_t f) { fechaEntrega = f; }
    void setCosto(double c) { costo = c; }
    void setMensajeroId(string mId) { mensajeroId = mId; }
    
    string getEstadoString() const;
};

#endif

/* Aqui se define que se puede hacer con el paquete, por ejemplo un paquete puede tener asignado un ID, el Cliente, una direccion,etc*/