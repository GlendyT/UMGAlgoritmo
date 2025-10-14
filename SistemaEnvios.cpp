#include "SistemaEnvios.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

SistemaEnvios::SistemaEnvios() : tarifaBase(10.0), contadorId(1000), controladorUsuarios(1) {}

SistemaEnvios::~SistemaEnvios()
{
    for (Usuario *usuario : usuarios)
    {
        delete usuario;
    }
}

bool SistemaEnvios::registrarUsuario(Usuario *usuario)
{
    if (buscarUsuario(usuario->getId()) != nullptr)
    {
        return false;
    }
    usuarios.push_back(usuario);
    return true;
}

string SistemaEnvios::generarIdUsuario(string prefijo)
{
    // Asegurar que el prefijo sea válido
    if (prefijo.empty()) {
        prefijo = "USR";
    }
    
    string id;
    do {
        // Convertir el contador a string y rellenar con ceros a la izquierda si es necesario
        string numStr = to_string(controladorUsuarios);
        string zeros(3 - numStr.length(), '0');
        
        id = prefijo + zeros + numStr;
        controladorUsuarios++;
        
        // Verificar que no exista un usuario con ese ID
    } while (buscarUsuario(id) != nullptr);
    
    return id;
}

bool SistemaEnvios::eliminarUsuario(string id)
{
    auto it = find_if(usuarios.begin(), usuarios.end(),
                      [id](Usuario *u)
                      { return u->getId() == id; });

    if (it != usuarios.end())
    {
        (*it)->setActivo(false);
        return true;
    }
    return false;
}

Usuario *SistemaEnvios::buscarUsuario(string id)
{
    auto it = find_if(usuarios.begin(), usuarios.end(),
                      [id](Usuario *u)
                      { return u->getId() == id && u->isActivo(); });

    return (it != usuarios.end()) ? *it : nullptr;
}

vector<Usuario *> SistemaEnvios::obtenerUsuarios()
{
    vector<Usuario *> activos;
    for (Usuario *u : usuarios)
    {
        if (u->isActivo())
        {
            activos.push_back(u);
        }
    }
    return activos;
}

string SistemaEnvios::solicitarEnvio(string clienteId, string origen, string destino, double peso, string desc)
{
    if (peso > 15.0)
    {
        return "";
    }

    Usuario *cliente = buscarUsuario(clienteId);
    if (!cliente || cliente->getTipo() != "Cliente")
    {
        return "";
    }

    string paqueteId = generarId();
    Paquete paquete(paqueteId, clienteId, origen, destino, peso, desc);

    double costo = calcularTarifa(peso, time(0));
    paquete.setCosto(costo);

    paquetes.push_back(paquete);
    return paqueteId;
}

bool SistemaEnvios::asignarMensajero(string paqueteId, string mensajeroId, string controladorId)
{
    Usuario *controlador = buscarUsuario(controladorId);
    if (!controlador || (controlador->getTipo() != "Controlador" && controlador->getTipo() != "Administrador"))
    {
        return false;
    }

    Mensajero *mensajero = dynamic_cast<Mensajero *>(buscarUsuario(mensajeroId));
    if (!mensajero)
    {
        return false;
    }

    auto it = find_if(paquetes.begin(), paquetes.end(),
                      [paqueteId](const Paquete &p)
                      { return p.getId() == paqueteId; });

    if (it != paquetes.end() && it->getEstado() == SOLICITADO)
    {
        it->setMensajeroId(mensajeroId);
        it->setEstado(EN_TRANSITO);
        mensajero->setEnRuta(true);

        enviarMensaje(mensajeroId, "Nuevo paquete asignado: " + paqueteId);
        return true;
    }
    return false;
}

bool SistemaEnvios::actualizarEstadoPaquete(string paqueteId, EstadoPaquete estado)
{
    auto it = find_if(paquetes.begin(), paquetes.end(),
                      [paqueteId](const Paquete &p)
                      { return p.getId() == paqueteId; });

    if (it != paquetes.end())
    {
        it->setEstado(estado);
        if (estado == ENTREGADO)
        {
            it->setFechaEntrega(time(0));

            Mensajero *mensajero = dynamic_cast<Mensajero *>(buscarUsuario(it->getMensajeroId()));
            if (mensajero)
            {
                mensajero->setEnRuta(false);
            }
        }
        return true;
    }
    return false;
}

void SistemaEnvios::enviarMensaje(string destinatario, string mensaje)
{
    mensajes[destinatario] += mensaje + "\n";
}

vector<string> SistemaEnvios::obtenerMensajes(string usuarioId)
{
    vector<string> resultado;
    if (mensajes.find(usuarioId) != mensajes.end())
    {
        stringstream ss(mensajes[usuarioId]);
        string linea;
        while (getline(ss, linea))
        {
            if (!linea.empty())
            {
                resultado.push_back(linea);
            }
        }
        mensajes[usuarioId] = "";
    }
    return resultado;
}

double SistemaEnvios::calcularTarifa(double peso, time_t fechaSolicitud)
{
    double tarifa = tarifaBase * peso;

    if (!esHorarioNormal(fechaSolicitud))
    {
        tarifa *= 2.0;
    }

    return tarifa;
}

void SistemaEnvios::actualizarTarifaBase(double nuevaTarifa)
{
    tarifaBase = nuevaTarifa;
}

vector<Paquete> SistemaEnvios::obtenerPaquetesPorEstado(EstadoPaquete estado)
{
    vector<Paquete> resultado;
    for (const Paquete &p : paquetes)
    {
        if (p.getEstado() == estado)
        {
            resultado.push_back(p);
        }
    }
    return resultado;
}

void SistemaEnvios::generarReporte()
{
    cout << "\n=== REPORTE DE ENVIOS ===" << endl;
    cout << "Solicitudes recibidas: " << obtenerPaquetesPorEstado(SOLICITADO).size() << endl;
    cout << "Paquetes en transito: " << obtenerPaquetesPorEstado(EN_TRANSITO).size() << endl;
    cout << "Paquetes entregados: " << obtenerPaquetesPorEstado(ENTREGADO).size() << endl;
    cout << "Total de paquetes: " << paquetes.size() << endl;
}

string SistemaEnvios::generarId()
{
    return "ID" + to_string(contadorId++);
}

bool SistemaEnvios::esHorarioNormal(time_t fecha)
{
    struct tm *timeinfo = localtime(&fecha);
    int hora = timeinfo->tm_hour;
    int diaSemana = timeinfo->tm_wday;

    return (diaSemana >= 1 && diaSemana <= 6) && (hora >= 8 && hora <= 18);
}