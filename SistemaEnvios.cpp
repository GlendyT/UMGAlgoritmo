#include "SistemaEnvios.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Constructor y destructor de Tarifas y SistemaEnvios
SistemaEnvios::SistemaEnvios() : tarifaBase(45.0), contadorId(1000), controladorUsuarios(1) {}

// Destructor para liberar memoria de usuarios
SistemaEnvios::~SistemaEnvios()
{
    for (Usuario *usuario : usuarios)
    {
        delete usuario;
    }
}

// Gestión de usuarios
bool SistemaEnvios::registrarUsuario(Usuario *usuario)
{
    if (buscarUsuario(usuario->getId()) != nullptr)
    {
        return false;
    }
    usuarios.push_back(usuario);
    return true;
}

// Genera un ID único para un nuevo usuario con el prefijo dado
string SistemaEnvios::generarIdUsuario(string prefijo)
{
    // Asegurar que el prefijo sea válido
    if (prefijo.empty())
    {
        prefijo = "USR";
    }

    string id;
    do
    {
        // Convertir el contador a string y rellenar con ceros a la izquierda si es necesario
        string numStr = to_string(controladorUsuarios);
        string zeros(3 - numStr.length(), '0');

        id = prefijo + zeros + numStr;
        controladorUsuarios++;

        // Verificar que no exista un usuario con ese ID
    } while (buscarUsuario(id) != nullptr);

    return id;
}

// Elimina (desactiva) un usuario por su ID
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

// Busca un usuario activo por su ID
Usuario *SistemaEnvios::buscarUsuario(string id)
{
    auto it = find_if(usuarios.begin(), usuarios.end(),
                      [id](Usuario *u)
                      { return u->getId() == id && u->isActivo(); });

    return (it != usuarios.end()) ? *it : nullptr;
}

// Verifica si un email ya está registrado en el sistema (usuarios activos)
bool SistemaEnvios::emailExiste(const string &email)
{
    auto toLower = [](const string &s)
    { string out=s; for (char &c: out) c = tolower(static_cast<unsigned char>(c)); return out; };
    string emailLower = toLower(email);
    for (Usuario *u : usuarios)
    {
        if (!u->isActivo())
            continue;
        if (toLower(u->getEmail()) == emailLower)
            return true;
    }
    return false;
}

// Obtiene la lista de usuarios activos
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

// Gestión de paquetes
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

// Asigna un mensajero a un paquete
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

// Actualiza el estado de un paquete
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

// Obtiene la lista de paquetes
void SistemaEnvios::enviarMensaje(string destinatario, string mensaje)
{
    mensajes[destinatario] += mensaje + "\n";
}

// Obtiene los mensajes para un usuario y los limpia del sistema
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

// Cálculo de tarifas
double SistemaEnvios::calcularTarifa(double peso, time_t fechaSolicitud)
{
    // Ahora la tarifa es FIJA por paquete (tarifaBase). El peso ya no afecta el precio.
    double tarifa = tarifaBase;
    if (!esHorarioNormal(fechaSolicitud))
    {
        tarifa *= 2.0;
    }
    return tarifa;
}

// Actualiza la tarifa base
void SistemaEnvios::actualizarTarifaBase(double nuevaTarifa)
{
    tarifaBase = nuevaTarifa;
}

// Obtiene la tarifa base actual
double SistemaEnvios::getTarifaBase() const
{
    return tarifaBase;
}

// Reportes
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

// Obtener paquetes de un cliente
vector<Paquete> SistemaEnvios::obtenerPaquetesPorCliente(string clienteId)
{
    vector<Paquete> resultado;
    for (const Paquete &p : paquetes)
    {
        if (p.getClienteId() == clienteId)
        {
            resultado.push_back(p);
        }
    }
    return resultado;
}

// Obtener mensajeros disponibles (no en ruta)
vector<Mensajero *> SistemaEnvios::obtenerMensajerosDisponibles()
{
    vector<Mensajero *> resultado;
    for (Usuario *u : usuarios)
    {
        if (u->getTipo() == "Mensajero")
        {
            Mensajero *m = dynamic_cast<Mensajero *>(u);
            if (m && !m->isEnRuta())
                resultado.push_back(m);
        }
    }
    return resultado;
}

// Genera un reporte simple de envíos
void SistemaEnvios::generarReporte()
{
    cout << "\n=== REPORTE DE ENVIOS ===" << endl;
    cout << "Solicitudes recibidas: " << obtenerPaquetesPorEstado(SOLICITADO).size() << endl;
    cout << "Paquetes en transito: " << obtenerPaquetesPorEstado(EN_TRANSITO).size() << endl;
    cout << "Paquetes entregados: " << obtenerPaquetesPorEstado(ENTREGADO).size() << endl;
    cout << "Total de paquetes: " << paquetes.size() << endl;
}

// Genera un ID único para paquetes
string SistemaEnvios::generarId()
{
    return "ID" + to_string(contadorId++);
}

// Verifica si la fecha y hora están dentro del horario normal
bool SistemaEnvios::esHorarioNormal(time_t fecha)
{
    struct tm *timeinfo = localtime(&fecha);
    int hora = timeinfo->tm_hour;
    int diaSemana = timeinfo->tm_wday;

    // Consideramos horario normal de 8:00 (inclusive) a 18:00 (exclusive).
    // La tarifa se duplicará solo si es antes de las 8am (hora < 8)
    // o despues de las 6pm (hora >= 18).
    return (diaSemana >= 1 && diaSemana <= 6) && (hora >= 8 && hora < 18);
}

/*Aqui se declarara que operaciones como registrar Usuario, solicitar envio, asigar mensajero, generar reporte. En resumen aqui es donde se asigna a cada usuario lo que tiene que hacer*/