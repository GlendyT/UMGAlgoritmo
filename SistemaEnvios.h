#ifndef SISTEMA_ENVIOS_H
#define SISTEMA_ENVIOS_H

#include "Usuario.h"
#include "Paquete.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

class SistemaEnvios {
private:
    vector<Usuario*> usuarios;
    vector<Paquete> paquetes;
    map<string, string> mensajes;
    double tarifaBase;
    int contadorId;

public:
    SistemaEnvios();
    ~SistemaEnvios();
    
    // Gestión de usuarios
    bool registrarUsuario(Usuario* usuario);
    bool eliminarUsuario(string id);
    Usuario* buscarUsuario(string id);
    vector<Usuario*> obtenerUsuarios();
    
    // Gestión de paquetes
    string solicitarEnvio(string clienteId, string origen, string destino, double peso, string desc);
    bool asignarMensajero(string paqueteId, string mensajeroId, string controladorId);
    bool actualizarEstadoPaquete(string paqueteId, EstadoPaquete estado);
    
    // Sistema de mensajería
    void enviarMensaje(string destinatario, string mensaje);
    vector<string> obtenerMensajes(string usuarioId);
    
    // Tarifas
    double calcularTarifa(double peso, time_t fechaSolicitud);
    void actualizarTarifaBase(double nuevaTarifa);
    
    // Reportes
    vector<Paquete> obtenerPaquetesPorEstado(EstadoPaquete estado);
    void generarReporte();
    
private:
    string generarId();
    bool esHorarioNormal(time_t fecha);
};

#endif