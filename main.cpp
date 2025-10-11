#include "SistemaEnvios.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

void guardarUsuarios(SistemaEnvios &sistema)
{
    ofstream archivo("usuarios.txt");
    if (archivo.is_open())
    {
        vector<Usuario *> usuarios = sistema.obtenerUsuarios();
        for (Usuario *u : usuarios)
        {
            archivo << u->getTipo() << "|"
                    << u->getId() << "|"
                    << u->getNombre() << "|"
                    << u->getDireccion() << "|"
                    << u->getTelefono() << "|"
                    << u->getEmail() << endl;
        }
        archivo.close();
        cout << "Usuarios guardados en archivo usuarios.txt" << endl;
    }
    else
    {
        cout << "Error al abrir archivo para escribir" << endl;
    }
}

void cargarUsuarios(SistemaEnvios &sistema)
{
    ifstream archivo("usuarios.txt");
    if (archivo.is_open())
    {
        string linea;
        while (getline(archivo, linea))
        {
            size_t pos = 0;
            vector<string> datos;
            string token;

            // Separar los datos por el delimitador "|"
            while ((pos = linea.find("|")) != string::npos)
            {
                token = linea.substr(0, pos);
                datos.push_back(token);
                linea.erase(0, pos + 1);
            }
            datos.push_back(linea); // Último elemento

            if (datos.size() == 6)
            {
                string tipo = datos[0];
                string id = datos[1];
                string nombre = datos[2];
                string direccion = datos[3];
                string telefono = datos[4];
                string email = datos[5];

                // Crear usuario según el tipo
                if (tipo == "Cliente")
                {
                    sistema.registrarUsuario(new Cliente(nombre, direccion, telefono, email, id));
                }
                else if (tipo == "Mensajero")
                {
                    sistema.registrarUsuario(new Mensajero(nombre, direccion, telefono, email, id));
                }
                else if (tipo == "Administrador")
                {
                    sistema.registrarUsuario(new Administrador(nombre, direccion, telefono, email, id));
                }
                else if (tipo == "Controlador")
                {
                    sistema.registrarUsuario(new Controlador(nombre, direccion, telefono, email, id));
                }
            }
        }
        archivo.close();
        cout << "Usuarios cargados desde archivo usuarios.txt" << endl;
    }
}

void mostrarMenuPrincipal()
{
    cout << "\n=== SISTEMA DE ENVIOS GARANTIZADOS ===" << endl;
    cout << "1. Registrar Cliente" << endl;
    cout << "2. Registrar Mensajero" << endl;
    cout << "3. Registrar Administrador" << endl;
    cout << "4. Registrar Controlador" << endl;
    cout << "5. Ingresar Cliente" << endl;
    cout << "6. Ingresar Mensajero" << endl;
    cout << "7. Ingresar Administrador" << endl;
    cout << "8. Ingresar Controlador" << endl;
    cout << "0. Salir" << endl;
    cout << "Opcion: ";
}

void mostrarMenuCliente()
{
    cout << "\n=== MENU CLIENTE ===" << endl;
    cout << "1. Solicitar Envio" << endl;
    cout << "2. Ver Mensajes" << endl;
    cout << "0. Volver al menu principal" << endl;
    cout << "Opcion: ";
}

void mostrarMenuMensajero()
{
    cout << "\n=== MENU MENSAJERO ===" << endl;
    cout << "1. Ver Mensajes" << endl;
    cout << "2. Actualizar Estado Paquete" << endl;
    cout << "0. Volver al menu principal" << endl;
    cout << "Opcion: ";
}

void mostrarMenuAdministrador()
{
    cout << "\n=== MENU ADMINISTRADOR ===" << endl;
    cout << "1. Asignar Mensajero" << endl;
    cout << "2. Generar Reporte" << endl;
    cout << "3. Actualizar Tarifa Base" << endl;
    cout << "4. Listar Usuarios" << endl;
    cout << "5. Ver Mensajes" << endl;
    cout << "6. Guardar Usuarios en Archivo" << endl;
    cout << "7. Cargar Usuarios desde Archivo" << endl;
    cout << "0. Volver al menu principal" << endl;
    cout << "Opcion: ";
}

void menuCliente(SistemaEnvios &sistema, string clienteId)
{
    int opcion;
    do
    {
        mostrarMenuCliente();
        cin >> opcion;
        cin.ignore();

        switch (opcion)
        {
        case 1:
        {
            string origen, destino, desc;
            double peso;

            cout << "Direccion Origen: ";
            getline(cin, origen);
            cout << "Direccion Destino: ";
            getline(cin, destino);
            cout << "Peso (max 15 lbs): ";
            cin >> peso;
            cin.ignore();

            // Validar el peso del paquete
            if (peso > 15.0)
            {
                cout << "Error: No se puede agregar un paquete de mas de 15 libras. Peso ingresado: " << peso << " lbs" << endl;
                break;
            }

            cout << "Descripcion: ";
            getline(cin, desc);

            string paqueteId = sistema.solicitarEnvio(clienteId, origen, destino, peso, desc);
            if (!paqueteId.empty())
            {
                cout << "Envio solicitado. ID: " << paqueteId << endl;
            }
            else
            {
                cout << "Error en la solicitud" << endl;
            }
            break;
        }

        case 2:
        {
            vector<string> mensajes = sistema.obtenerMensajes(clienteId);
            cout << "Mensajes para " << clienteId << ":" << endl;
            for (const string &msg : mensajes)
            {
                cout << "- " << msg << endl;
            }
            break;
        }
        }
    } while (opcion != 0);
}

void menuMensajero(SistemaEnvios &sistema, string mensajeroId)
{
    int opcion;
    do
    {
        mostrarMenuMensajero();
        cin >> opcion;
        cin.ignore();

        switch (opcion)
        {
        case 1:
        {
            vector<string> mensajes = sistema.obtenerMensajes(mensajeroId);
            cout << "Mensajes para " << mensajeroId << ":" << endl;
            for (const string &msg : mensajes)
            {
                cout << "- " << msg << endl;
            }
            break;
        }

        case 2:
        {
            string paqueteId;
            int estado;
            cout << "ID Paquete: ";
            getline(cin, paqueteId);
            cout << "Estado (0=Solicitado, 1=En Transito, 2=Entregado): ";
            cin >> estado;

            if (sistema.actualizarEstadoPaquete(paqueteId, (EstadoPaquete)estado))
            {
                cout << "Estado actualizado!" << endl;
            }
            else
            {
                cout << "Error al actualizar estado" << endl;
            }
            break;
        }
        }
    } while (opcion != 0);
}

void menuAdministrador(SistemaEnvios &sistema, string adminId)
{
    int opcion;
    do
    {
        mostrarMenuAdministrador();
        cin >> opcion;
        cin.ignore();

        switch (opcion)
        {
        case 1:
        {
            string paqueteId, mensajeroId;
            cout << "ID Paquete: ";
            getline(cin, paqueteId);
            cout << "ID Mensajero: ";
            getline(cin, mensajeroId);

            if (sistema.asignarMensajero(paqueteId, mensajeroId, adminId))
            {
                cout << "Mensajero asignado exitosamente!" << endl;
            }
            else
            {
                cout << "Error en la asignacion" << endl;
            }
            break;
        }

        case 2:
        {
            sistema.generarReporte();
            break;
        }

        case 3:
        {
            double nuevaTarifa;
            cout << "Nueva tarifa base: ";
            cin >> nuevaTarifa;
            sistema.actualizarTarifaBase(nuevaTarifa);
            cout << "Tarifa actualizada!" << endl;
            break;
        }

        case 4:
        {
            vector<Usuario *> usuarios = sistema.obtenerUsuarios();
            cout << "\n=== USUARIOS REGISTRADOS ===" << endl;
            cout << "Total de usuarios: " << usuarios.size() << endl;
            cout << "----------------------------------------" << endl;
            for (Usuario *u : usuarios)
            {
                cout << "Tipo: " << u->getTipo() << endl;
                cout << "ID: " << u->getId() << endl;
                cout << "Nombre: " << u->getNombre() << endl;
                cout << "Direccion: " << u->getDireccion() << endl;
                cout << "Telefono: " << u->getTelefono() << endl;
                cout << "Email: " << u->getEmail() << endl;
                cout << "----------------------------------------" << endl;
            }
            break;
        }

        case 5:
        {
            vector<string> mensajes = sistema.obtenerMensajes(adminId);
            cout << "Mensajes para " << adminId << ":" << endl;
            for (const string &msg : mensajes)
            {
                cout << "- " << msg << endl;
            }
            break;
        }

        case 6:
        { // Guardar Usuarios
            guardarUsuarios(sistema);
            break;
        }

        case 7:
        { // Cargar Usuarios
            cout << "ADVERTENCIA: Esto sobrescribira los usuarios actuales." << endl;
            cout << "¿Desea continuar? (s/n): ";
            char respuesta;
            cin >> respuesta;
            cin.ignore();

            if (respuesta == 's' || respuesta == 'S')
            {
                cargarUsuarios(sistema);
            }
            else
            {
                cout << "Operacion cancelada." << endl;
            }
            break;
        }
        }
    } while (opcion != 0);
}

int main()
{
    SistemaEnvios sistema;

    cargarUsuarios(sistema);

    // Crear usuarios por defecto solo si no se cargaron desde archivo
    vector<Usuario *> usuarios = sistema.obtenerUsuarios();
    bool tieneAdmin = false;
    for (Usuario *u : usuarios)
    {
        if (u->getTipo() == "Administrador")
        {
            tieneAdmin = true;
            break;
        }
    }

    if (!tieneAdmin)
    {
        sistema.registrarUsuario(new Administrador("Admin", "Oficina Central", "123-456", "admin@envios.com", "ADM001"));
        sistema.registrarUsuario(new Controlador("Control", "Centro Control", "123-457", "control@envios.com", "CTL001"));
    }

    int opcion;
    string nombre, direccion, telefono, email, id;

    do
    {
        mostrarMenuPrincipal();
        cin >> opcion;
        cin.ignore();

        switch (opcion)
        {
        case 1:
        { // Registrar Cliente
            cout << "=== REGISTRO DE CLIENTE ===" << endl;
            cout << "Nombre: ";
            getline(cin, nombre);
            cout << "Direccion: ";
            getline(cin, direccion);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Email: ";
            getline(cin, email);
            cout << "ID: ";
            getline(cin, id);

            if (sistema.registrarUsuario(new Cliente(nombre, direccion, telefono, email, id)))
            {
                cout << "Cliente registrado exitosamente!" << endl;
            }
            else
            {
                cout << "Error: ID ya existe" << endl;
            }
            break;
        }

        case 2:
        { // Registrar Mensajero
            cout << "=== REGISTRO DE MENSAJERO ===" << endl;
            cout << "Nombre: ";
            getline(cin, nombre);
            cout << "Direccion: ";
            getline(cin, direccion);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Email: ";
            getline(cin, email);
            cout << "ID: ";
            getline(cin, id);

            if (sistema.registrarUsuario(new Mensajero(nombre, direccion, telefono, email, id)))
            {
                cout << "Mensajero registrado exitosamente!" << endl;
            }
            else
            {
                cout << "Error: ID ya existe" << endl;
            }
            break;
        }

        case 3:
        { // Registrar Administrador
            cout << "=== REGISTRO DE ADMINISTRADOR ===" << endl;
            cout << "Nombre: ";
            getline(cin, nombre);
            cout << "Direccion: ";
            getline(cin, direccion);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Email: ";
            getline(cin, email);
            cout << "ID: ";
            getline(cin, id);

            if (sistema.registrarUsuario(new Administrador(nombre, direccion, telefono, email, id)))
            {
                cout << "Administrador registrado exitosamente!" << endl;
            }
            else
            {
                cout << "Error: ID ya existe" << endl;
            }
            break;
        }
        case 4:
        { // Registrar Controlador
            cout << "=== REGISTRO DE CONTROLADOR ===" << endl;
            cout << "Nombre: ";
            getline(cin, nombre);
            cout << "Direccion: ";
            getline(cin, direccion);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Email: ";
            getline(cin, email);
            cout << "ID: ";
            getline(cin, id);

            if (sistema.registrarUsuario(new Controlador(nombre, direccion, telefono, email, id)))
            {
                cout << "Controlador registrado exitosamente!" << endl;
            }
            else
            {
                cout << "Error: ID ya existe" << endl;
            }
            break;
        }

        case 5:
        { // Ingresar Cliente
            cout << "=== INGRESO DE CLIENTE ===" << endl;
            cout << "ID Cliente: ";
            getline(cin, id);

            Usuario *usuario = sistema.buscarUsuario(id);
            if (usuario && usuario->getTipo() == "Cliente")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuCliente(sistema, id);
            }
            else
            {
                cout << "Cliente no encontrado o ID invalido!" << endl;
            }
            break;
        }

        case 6:
        { // Ingresar Mensajero
            cout << "=== INGRESO DE MENSAJERO ===" << endl;
            cout << "ID Mensajero: ";
            getline(cin, id);

            Usuario *usuario = sistema.buscarUsuario(id);
            if (usuario && usuario->getTipo() == "Mensajero")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuMensajero(sistema, id);
            }
            else
            {
                cout << "Mensajero no encontrado o ID invalido!" << endl;
            }
            break;
        }

        case 7:
        { // Ingresar Administrador
            cout << "=== INGRESO DE ADMINISTRADOR ===" << endl;
            cout << "ID Administrador: ";
            getline(cin, id);

            Usuario *usuario = sistema.buscarUsuario(id);
            if (usuario && usuario->getTipo() == "Administrador")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuAdministrador(sistema, id);
            }
            else
            {
                cout << "Administrador no encontrado o ID invalido!" << endl;
            }
            break;
        }
        case 8:
        { // Ingresar Controlador
            cout << "=== INGRESO DE CONTROLADOR ===" << endl;
            cout << "ID Controlador: ";
            getline(cin, id);

            Usuario *usuario = sistema.buscarUsuario(id);
            if (usuario && usuario->getTipo() == "Controlador")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuAdministrador(sistema, id); // Controladors use same menu as administrators
            }
            else
            {
                cout << "Controlador no encontrado o ID invalido!" << endl;
            }
            break;
        }
        }
    } while (opcion != 0);

    guardarUsuarios(sistema);

    cout << "Gracias por usar el Sistema de Envios Garantizados!" << endl;
    return 0;
}