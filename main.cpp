#include "SistemaEnvios.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <limits>
#include <iomanip>
#include <ctime>
using namespace std;

// Modo prueba: si es true, omitimos la validacion Luhn para tarjetas (usar solo en desarrollo)
bool modoPrueba = false;

bool esNumerico(const string &str)
{
    if (str.empty())
        return false;
    for (char c : str)
    {
        if (!isdigit(c) && c != '-' && c != ' ')
        {
            return false;
        }
    }
    return true;
}

// Función para validar email
bool validarEmail(const string &email)
{
    return email.find('@') != string::npos && !email.empty();
}

// Función para verificar si una cadena está vacía o solo espacios
bool isBlank(const string &s)
{
    for (char c : s)
    {
        if (!isspace(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

// Comprueba que la cadena contiene solo dígitos
bool isDigits(const string &s)
{
    if (s.empty())
        return false;
    for (char c : s)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

// Comprueba que la cadena contiene solo letras y espacios
bool isAlphaSpace(const string &s)
{
    if (s.empty())
        return false;
    for (char c : s)
    {
        if (!isalpha(static_cast<unsigned char>(c)) && !isspace(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

// Validacion Luhn para numero de tarjeta
bool luhnCheck(const string &num)
{
    if (modoPrueba)
        return true;
    int sum = 0;
    bool alt = false;
    for (int i = (int)num.length() - 1; i >= 0; --i)
    {
        char c = num[i];
        if (!isdigit(static_cast<unsigned char>(c)))
            continue;
        int n = c - '0';
        if (alt)
        {
            n *= 2;
            if (n > 9)
                n -= 9;
        }
        sum += n;
        alt = !alt;
    }
    return (sum % 10) == 0;
}

// Función para obtener email válido
string obtenerEmailValido()
{
    string email;
    do
    {
        cout << "Email (debe contener @): ";
        getline(cin, email);
        if (!validarEmail(email))
        {
            cout << "Error: El email debe contener el simbolo @." << endl;
        }
    } while (!validarEmail(email));
    return email;
}

// Obtener email validado y único en el sistema
string obtenerEmailUnico(SistemaEnvios &sistema)
{
    string email;
    do
    {
        email = obtenerEmailValido();
        if (sistema.emailExiste(email))
        {
            cout << "Este correo electronico ya existe, ingrese uno nuevo." << endl;
            email.clear();
        }
    } while (email.empty());
    return email;
}

// Función para obtener telefono valido (solo digitos y guiones/espacios permitidos)
string obtenerTelefonoValido()
{
    string telefono;
    do
    {
        cout << "Telefono (solo digitos, puede incluir - o espacios): ";
        getline(cin, telefono);
        if (!esNumerico(telefono))
        {
            cout << "Telefono invalido. Intente de nuevo." << endl;
        }
    } while (!esNumerico(telefono));
    return telefono;
}

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

void guardarPaquetes(SistemaEnvios &sistema)
{
    ofstream archivo("paquetes.txt");
    if (archivo.is_open())
    {
        // Obtener paquetes por estado y escribirlos
        vector<Paquete> solic = sistema.obtenerPaquetesPorEstado(SOLICITADO);
        vector<Paquete> transit = sistema.obtenerPaquetesPorEstado(EN_TRANSITO);
        vector<Paquete> entreg = sistema.obtenerPaquetesPorEstado(ENTREGADO);

        auto formatTime = [&](time_t t)
        {
            if (t == 0)
                return string("");
            char buf[64];
            struct tm *ti = localtime(&t);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ti);
            return string(buf);
        };

        auto write = [&](const Paquete &p)
        {
            string fecha = formatTime(p.getFechaSolicitud());
            archivo << p.getId() << "|"
                    << p.getClienteId() << "|"
                    << p.getDireccionOrigen() << "|"
                    << p.getDireccionDestino() << "|"
                    << p.getPeso() << "|"
                    << p.getDescripcion() << "|"
                    << p.getEstadoString() << "|"
                    << p.getMensajeroId() << "|"
                    << fecha << "|"
                    << p.getCosto() << "\n";
        };

        for (const Paquete &p : solic)
            write(p);
        for (const Paquete &p : transit)
            write(p);
        for (const Paquete &p : entreg)
            write(p);

        archivo.close();
        cout << "Paquetes guardados en archivo paquetes.txt" << endl;
    }
    else
    {
        cout << "Error al abrir archivo paquetes.txt para escribir" << endl;
    }
}

// Registro simple para representar líneas en paquetes.txt con fecha y costo
struct PaqueteRegistro
{
    string id;
    string clienteId;
    string origen;
    string destino;
    double peso;
    string descripcion;
    string estadoStr;
    string mensajeroId;
    string fechaStr;
    double costo;
};

vector<PaqueteRegistro> leerPaquetesDesdeArchivo()
{
    vector<PaqueteRegistro> resultado;
    ifstream archivo("paquetes.txt");
    if (!archivo.is_open())
        return resultado;

    string linea;
    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;
        vector<string> campos;
        size_t pos = 0;
        while ((pos = linea.find("|")) != string::npos)
        {
            campos.push_back(linea.substr(0, pos));
            linea.erase(0, pos + 1);
        }
        campos.push_back(linea);
        if (campos.size() < 10)
            continue; // id|cliente|origen|destino|peso|desc|estado|mensajero|fecha|costo

        PaqueteRegistro r;
        r.id = campos[0];
        r.clienteId = campos[1];
        r.origen = campos[2];
        r.destino = campos[3];
        try
        {
            r.peso = stod(campos[4]);
        }
        catch (...)
        {
            r.peso = 0.0;
        }
        r.descripcion = campos[5];
        r.estadoStr = campos[6];
        r.mensajeroId = campos[7];
        r.fechaStr = campos[8];
        try
        {
            r.costo = stod(campos[9]);
        }
        catch (...)
        {
            r.costo = 0.0;
        }

        resultado.push_back(r);
    }
    archivo.close();
    return resultado;
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
    cout << "\n===  SISTEMA DE ENVIOS GARANTIZADOS  ===" << endl;
    cout << "Bienvenido! Si ya tienes una cuenta, puedes iniciar sesion o crear una nueva." << endl;
    cout << endl;
    cout << "1 Iniciar Sesion" << endl;
    cout << "2 Crea una cuenta" << endl;
    cout << "0 Salir" << endl;
    cout << "Opcion: ";
}

void mostrarMenuCrearCuenta()
{
    cout << "\n=== CREAR UNA CUENTA ===" << endl;
    cout << "1. Crear usuario como Cliente" << endl;
    cout << "2. Crear usuario como Mensajero" << endl;
    cout << "3. Crear usuario como Administrador" << endl;
    cout << "4. Crear usuario como Controlador" << endl;
    cout << "0. Regresar a menu principal" << endl;
    cout << "Opcion: ";
}

void mostrarMenuIniciarSesion()
{
    cout << "\n=== INICIAR SESION ===" << endl;
    cout << "1. Ingresar como Cliente" << endl;
    cout << "2. Ingresar como Mensajero" << endl;
    cout << "3. Ingresar como Administrador" << endl;
    cout << "4. Ingresar como Controlador" << endl;
    cout << "0. Regresar a menu principal" << endl;
    cout << "Opcion: ";
}

void mostrarMenuCliente()
{
    cout << "\n=== MENU CLIENTE ===" << endl;
    cout << "1. Solicitar Recoleccion de Paquete" << endl;
    cout << "2. Ver Ordenes/Paquetes" << endl;
    cout << "0. Volver al menu principal" << endl;
    cout << "Opcion: ";
}

void mostrarMenuMensajero()
{
    cout << "\n=== MENU MENSAJERO ===" << endl;
    cout << "1. Ver Paquetes Asignados" << endl;
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
    cout << "5. Ver Ordenes" << endl;
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
        if (!(cin >> opcion))
        {
            cout << "Entrada invalida. Por favor ingrese una opcion numerica." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (opcion)
        {
        case 1:
        {
            string origen, destino, desc;
            double peso;
            // Preguntar si desea usar la direccion registrada como origen
            Usuario *usuario = sistema.buscarUsuario(clienteId);
            string direccionRegistrada = "";
            if (usuario)
            {
                direccionRegistrada = usuario->getDireccion();
            }

            int usarRegistro = 0;
            do
            {
                cout << "Quieres utilizar tu direccion actual para la recoleccion del paquete?" << endl;
                cout << "1. Si" << endl;
                cout << "2. No" << endl;
                cout << "Opcion: ";
                if (!(cin >> usarRegistro))
                {
                    cout << "Entrada invalida. Por favor ingrese 1 o 2." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore();
                if (usarRegistro == 1)
                {
                    if (isBlank(direccionRegistrada))
                    {
                        cout << "No existe una direccion registrada." << endl;
                        do
                        {
                            cout << "Ingresa la direccion de recoleccion de paquete: ";
                            getline(cin, origen);
                            if (isBlank(origen))
                                cout << "La direccion no puede estar vacia. Intente de nuevo." << endl;
                        } while (isBlank(origen));
                    }
                    else
                    {
                        origen = direccionRegistrada;
                        cout << "Direccion actual: " << origen << endl;
                    }
                }
                else if (usarRegistro == 2)
                {
                    do
                    {
                        cout << "Ingresa la direccion de recoleccion de paquete: ";
                        getline(cin, origen);
                        if (isBlank(origen))
                            cout << "La direccion no puede estar vacia. Intente de nuevo." << endl;
                    } while (isBlank(origen));
                }
                else
                {
                    cout << "Seleccione 1 o 2." << endl;
                }
            } while (usarRegistro != 1 && usarRegistro != 2);
            cout << "Direccion Destino: ";
            getline(cin, destino);

            // Pedir peso en bucle hasta que sea numérico y <= 15
            while (true)
            {
                cout << "Peso (max 15 lbs): ";
                if (!(cin >> peso))
                {
                    cout << "Error: Debe ingresar un valor numérico para el peso." << endl;
                    cin.clear();                                         // Limpiar el estado de error
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descartar entrada incorrecta
                    continue;                                            // pedir de nuevo
                }
                // consumir el newline restante
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (peso > 15.0)
                {
                    cout << "Error: No se puede agregar un paquete de mas de 15 libras. Peso ingresado: " << peso << " lbs" << endl;
                    cout << "Por favor ingrese un peso valido (<= 15 lbs)." << endl;
                    continue; // pedir de nuevo
                }

                if (peso <= 0)
                {
                    cout << "Error: El peso debe ser mayor que 0." << endl;
                    continue;
                }

                break; // peso válido
            }

            cout << "Descripcion: ";
            getline(cin, desc);

            // Mostrar precio fijo consultando el sistema (tarifa fija por paquete)
            double tarifaBaseSistema = sistema.getTarifaBase();
            cout << "El precio de mandar un paquete es de Q." << fixed << setprecision(2) << tarifaBaseSistema << endl;

            // Seleccionar metodo de pago con validacion
            int metodo = 0;
            do
            {
                cout << "Seleccione metodo de pago:" << endl;
                cout << "1: Pago contra entrega (cash)" << endl;
                cout << "2: Pago con tarjeta de credito/debito" << endl;
                cout << "3: Pago con transferencia" << endl;
                cout << "Opcion: ";
                if (!(cin >> metodo))
                {
                    cout << "Entrada invalida. Ingrese 1, 2 o 3." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (metodo < 1 || metodo > 3)
                {
                    cout << "Opcion invalida. Intente de nuevo." << endl;
                }
            } while (metodo < 1 || metodo > 3);

            string pagoDetalle;
            if (metodo == 1)
            {
                pagoDetalle = "Pago contra entrega (cash)";
            }
            else if (metodo == 2)
            {
                pagoDetalle = "Pago con tarjeta";
                string tarjetaNum, tarjetaNombre, vigencia, cvv;
                // Numero de tarjeta: solo digitos, hasta 16
                do
                {
                    cout << "Ingrese numero de tarjeta (maximo 16 digitos, puede incluir espacios o guiones): ";
                    getline(cin, tarjetaNum);
                    string tarjetaDigits2;
                    for (char c : tarjetaNum)
                        if (isdigit(static_cast<unsigned char>(c)))
                            tarjetaDigits2.push_back(c);
                    if (tarjetaDigits2.empty() || tarjetaDigits2.size() > 16)
                    {
                        cout << "Numero de tarjeta invalido. Debe contener entre 1 y 16 digitos." << endl;
                        tarjetaNum.clear();
                    }
                    else
                    {
                        tarjetaNum = tarjetaDigits2;
                        break;
                    }
                } while (true);

                do
                {
                    cout << "Nombre y apellido en la tarjeta: ";
                    getline(cin, tarjetaNombre);
                    if (!isAlphaSpace(tarjetaNombre))
                        cout << "El nombre debe contener solo letras y espacios." << endl;
                } while (!isAlphaSpace(tarjetaNombre));

                do
                {
                    cout << "Fecha de vigencia (MM/AA): ";
                    getline(cin, vigencia);
                    bool okFormat = vigencia.size() == 5 && vigencia[2] == '/';
                    int mm = 0, yy = 0;
                    if (okFormat)
                    {
                        try
                        {
                            mm = stoi(vigencia.substr(0, 2));
                            yy = stoi(vigencia.substr(3, 2));
                        }
                        catch (...)
                        {
                            okFormat = false;
                        }
                    }
                    if (!okFormat || mm < 1 || mm > 12)
                    {
                        cout << "Formato de vigencia invalido. Use MM/AA (ej: 09/25)." << endl;
                        continue;
                    }
                    time_t ahora = time(nullptr);
                    struct tm *ti = localtime(&ahora);
                    int anioActual = ti->tm_year + 1900;
                    int mesActual = ti->tm_mon + 1;
                    int anioTarjeta = 2000 + yy;
                    if (anioTarjeta < anioActual || (anioTarjeta == anioActual && mm < mesActual))
                    {
                        cout << "La tarjeta esta vencida. Ingrese una fecha valida." << endl;
                        continue;
                    }
                    break;
                } while (true);

                do
                {
                    cout << "Codigo de seguridad (CVV, 3 digitos): ";
                    getline(cin, cvv);
                    if (!isDigits(cvv) || cvv.size() != 3)
                        cout << "CVV invalido. Debe contener 3 digitos." << endl;
                } while (!isDigits(cvv) || cvv.size() != 3);

                pagoDetalle += ": **** **** **** " + (tarjetaNum.size() > 4 ? tarjetaNum.substr(tarjetaNum.size() - 4) : tarjetaNum);
            }
            else
            {
                pagoDetalle = "Pago con transferencia";
                string banco, referencia;
                cout << "Ingrese nombre del banco: ";
                getline(cin, banco);
                cout << "Ingrese referencia/numero de transaccion: ";
                getline(cin, referencia);
                pagoDetalle += ": " + banco + " ref:" + referencia;
            }

            // Calcular costo final (tarifa fija, duplicar solo fuera de horario)
            double costoFinal = sistema.calcularTarifa(peso, time(0));

            // Mostrar resumen y confirmar
            cout << "\n--- Resumen de recoleccion ---" << endl;
            cout << "Cliente ID: " << clienteId << endl;
            cout << "Direccion origen: " << origen << endl;
            cout << "Direccion destino: " << destino << endl;
            cout << "Peso: " << peso << " lbs" << endl;
            cout << "Descripcion: " << desc << endl;
            cout << "Precio base: Q." << fixed << setprecision(2) << tarifaBaseSistema << endl;
            cout << "Costo final aplicado: Q." << fixed << setprecision(2) << costoFinal << endl;
            cout << "Metodo de pago: " << pagoDetalle << endl;
            cout << "Confirmar recoleccion y envio? (s/n): ";
            char confirmar;
            cin >> confirmar;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (confirmar == 's' || confirmar == 'S')
            {
                string paqueteId = sistema.solicitarEnvio(clienteId, origen, destino, peso, desc);
                if (!paqueteId.empty())
                {
                    cout << "Envio solicitado. ID: " << paqueteId << endl;
                    // Mostrar hora exacta de confirmacion
                    time_t ahora = time(nullptr);
                    struct tm *ti = localtime(&ahora);
                    char buf[64];
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ti);
                    cout << "Hora de confirmacion: " << buf << endl;

                    int hora = ti->tm_hour;
                    if (hora < 8 || hora > 18)
                    {
                        cout << "Nota: La recoleccion es fuera del horario (8:00-18:00). La tarifa puede duplicarse." << endl;
                    }
                    // Guardar historial de paquetes en archivo
                    guardarPaquetes(sistema);
                }
                else
                {
                    cout << "Error en la solicitud" << endl;
                }
            }
            else
            {
                cout << "Solicitud cancelada." << endl;
            }
            break;
        }

        case 2:
        {
            // Mostrar historial de paquetes desde el archivo para este cliente
            vector<PaqueteRegistro> historial = leerPaquetesDesdeArchivo();
            bool any = false;
            for (const PaqueteRegistro &r : historial)
            {
                if (r.clienteId == clienteId)
                {
                    any = true;
                    cout << "ID: " << r.id << " | Estado: " << r.estadoStr
                         << " | Mensajero: " << (r.mensajeroId.empty() ? "(No asignado)" : r.mensajeroId)
                         << " | Fecha: " << (r.fechaStr.empty() ? "-" : r.fechaStr)
                         << " | Costo: Q" << fixed << setprecision(2) << r.costo
                         << " | Origen: " << r.origen << " | Destino: " << r.destino
                         << " | Peso: " << r.peso << " lbs" << endl;
                }
            }
            if (!any)
                cout << "No hay ordenes registradas para este cliente." << endl;
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
        if (!(cin >> opcion))
        {
            cout << "Entrada invalida. Por favor ingrese una opcion numerica." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (opcion)
        {
        case 1:
        {
            // Buscar paquetes asignados al mensajero en cualquier estado relevante
            vector<Paquete> enTransito = sistema.obtenerPaquetesPorEstado(EN_TRANSITO);
            vector<Paquete> entregados = sistema.obtenerPaquetesPorEstado(ENTREGADO);
            vector<Paquete> asignados;
            for (const Paquete &p : enTransito)
                if (p.getMensajeroId() == mensajeroId)
                    asignados.push_back(p);
            for (const Paquete &p : entregados)
                if (p.getMensajeroId() == mensajeroId)
                    asignados.push_back(p);

            if (asignados.empty())
            {
                cout << "No hay asignaciones para este mensajero." << endl;
            }
            else
            {
                cout << "Paquetes asignados a " << mensajeroId << ":" << endl;
                for (const Paquete &p : asignados)
                {
                    cout << "ID paquete: " << p.getId() << " | Estado: " << p.getEstadoString()
                         << " | Cliente: " << p.getClienteId() << " | Origen: " << p.getDireccionOrigen()
                         << " | Destino: " << p.getDireccionDestino() << endl;
                }
            }

            // Mostrar mensajes en bandeja (si los hay)
            vector<string> mensajes = sistema.obtenerMensajes(mensajeroId);
            if (!mensajes.empty())
            {
                cout << "\nMensajes para " << mensajeroId << ":" << endl;
                for (const string &msg : mensajes)
                    cout << "- " << msg << endl;
            }
            break;
        }

        case 2:
        {
            // Buscar paquetes asignados al mensajero y en transito
            vector<Paquete> asignados = sistema.obtenerPaquetesPorEstado(EN_TRANSITO);
            vector<string> misPaquetes;
            for (const Paquete &p : asignados)
            {
                if (p.getMensajeroId() == mensajeroId)
                {
                    misPaquetes.push_back(p.getId());
                }
            }

            if (misPaquetes.empty())
            {
                cout << "No tienes ningun paquete asignado" << endl;
                break;
            }

            cout << "Paquetes asignados:\n";
            for (const string &pid : misPaquetes)
            {
                cout << "- " << pid << endl;
            }

            string paqueteId;
            // Pedir y validar que el paqueteId ingresado pertenezca a la lista de paquetes asignados
            while (true)
            {
                cout << "ID Paquete (elige uno de la lista) (Enter para cancelar): ";
                getline(cin, paqueteId);
                if (paqueteId.empty())
                {
                    cout << "Operacion cancelada por el mensajero." << endl;
                    paqueteId = "";
                    break;
                }

                bool encontrado = false;
                for (const string &pid : misPaquetes)
                {
                    if (pid == paqueteId)
                    {
                        encontrado = true;
                        break;
                    }
                }
                if (!encontrado)
                {
                    cout << "ID de paquete no encontrado en su lista. Intente nuevamente." << endl;
                    continue; // pedir de nuevo
                }
                break; // paqueteId valido
            }

            // Si el mensajero canceló la operacion, volver al menu
            if (paqueteId.empty())
                break;

            int estado;
            // Validar estado con reintentos
            while (true)
            {
                cout << "Estado (0=Solicitado, 1=En Transito, 2=Entregado): ";
                if (!(cin >> estado))
                {
                    cout << "Entrada invalida. Por favor ingrese 0, 1 o 2." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (estado < 0 || estado > 2)
                {
                    cout << "Estado invalido. Ingrese 0, 1 o 2." << endl;
                    continue;
                }
                break;
            }

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
            // Mostrar solicitudes y mensajeros disponibles (igual que Controlador)
            vector<Paquete> solicitudes = sistema.obtenerPaquetesPorEstado(SOLICITADO);
            if (solicitudes.empty())
            {
                cout << "No hay solicitudes pendientes." << endl;
            }
            else
            {
                cout << "Solicitudes pendientes:" << endl;
                for (const Paquete &p : solicitudes)
                {
                    cout << "ID: " << p.getId() << " | Cliente: " << p.getClienteId()
                         << " | Origen: " << p.getDireccionOrigen() << " | Destino: " << p.getDireccionDestino()
                         << " | Peso: " << p.getPeso() << " lbs" << endl;
                }
            }

            vector<Mensajero *> disponibles = sistema.obtenerMensajerosDisponibles();
            if (disponibles.empty())
            {
                cout << "No hay mensajeros disponibles en este momento." << endl;
                break;
            }
            else
            {
                cout << "\nMensajeros disponibles:" << endl;
                for (Mensajero *m : disponibles)
                {
                    cout << "ID: " << m->getId() << " | Nombre: " << m->getNombre() << endl;
                }
            }

            // Separador antes de los prompts para mayor claridad
            cout << endl;
            string paqueteId, mensajeroId;

            // Validar paqueteId: debe existir en solicitudes
            while (true)
            {
                cout << "ID Paquete a asignar (Enter para cancelar): ";
                getline(cin, paqueteId);
                if (paqueteId.empty())
                {
                    cout << "Operacion cancelada." << endl;
                    break;
                }
                bool found = false;
                for (const Paquete &p : solicitudes)
                {
                    if (p.getId() == paqueteId)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    cout << "ID de paquete incorrecto. Intente de nuevo." << endl;
                    continue;
                }
                break;
            }
            if (paqueteId.empty())
                break;

            // Validar mensajeroId: debe existir y estar disponible
            while (true)
            {
                cout << "ID Mensajero (Enter para cancelar): ";
                getline(cin, mensajeroId);
                if (mensajeroId.empty())
                {
                    cout << "Operacion cancelada." << endl;
                    break;
                }
                Usuario *u = sistema.buscarUsuario(mensajeroId);
                Mensajero *m = nullptr;
                if (u && u->getTipo() == "Mensajero")
                    m = dynamic_cast<Mensajero *>(u);
                if (!m)
                {
                    cout << "ID de mensajero invalido. Intente de nuevo." << endl;
                    continue;
                }
                if (m->isEnRuta())
                {
                    cout << "El mensajero no esta disponible (en ruta). Elija otro mensajero." << endl;
                    continue;
                }
                break;
            }
            if (mensajeroId.empty())
                break;

            if (sistema.asignarMensajero(paqueteId, mensajeroId, adminId))
            {
                cout << "Mensajero asignado exitosamente!" << endl;
            }
            else
            {
                cout << "Error en la asignacion (verifique IDs o estado del paquete)" << endl;
            }
            break;
        }

        case 2:
        {
            // Generar reporte completo: usuarios y paquetes
            cout << "\n=== REPORTE COMPLETO ===" << endl;
            cout << "Tarifa actual (Q por paquete): Q" << fixed << setprecision(2) << sistema.getTarifaBase() << endl;
            vector<Usuario *> usuarios = sistema.obtenerUsuarios();
            cout << "\n-- Usuarios registrados --" << endl;
            for (Usuario *u : usuarios)
            {
                cout << "Tipo: " << u->getTipo() << " | ID: " << u->getId() << " | Nombre: " << u->getNombre() << endl;
            }

            cout << "\n-- Paquetes (todos los estados) --" << endl;
            vector<Paquete> solic = sistema.obtenerPaquetesPorEstado(SOLICITADO);
            vector<Paquete> transit = sistema.obtenerPaquetesPorEstado(EN_TRANSITO);
            vector<Paquete> entreg = sistema.obtenerPaquetesPorEstado(ENTREGADO);
            auto printP = [&](const Paquete &p)
            {
                cout << "ID: " << p.getId() << " | Cliente: " << p.getClienteId()
                     << " | Estado: " << p.getEstadoString() << " | Mensajero: " << (p.getMensajeroId().empty() ? "(No asignado)" : p.getMensajeroId())
                     << " | Origen: " << p.getDireccionOrigen() << " | Destino: " << p.getDireccionDestino()
                     << " | Peso: " << p.getPeso() << " lbs" << endl;
            };
            for (const Paquete &p : solic)
                printP(p);
            for (const Paquete &p : transit)
                printP(p);
            for (const Paquete &p : entreg)
                printP(p);

            // Guardar paquetes en archivo
            guardarPaquetes(sistema);
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
            // Mostrar todas las ordenes realizadas (todos los paquetes)
            vector<Paquete> solic = sistema.obtenerPaquetesPorEstado(SOLICITADO);
            vector<Paquete> transit = sistema.obtenerPaquetesPorEstado(EN_TRANSITO);
            vector<Paquete> entreg = sistema.obtenerPaquetesPorEstado(ENTREGADO);

            if (solic.empty() && transit.empty() && entreg.empty())
            {
                cout << "No hay ordenes registradas." << endl;
            }
            else
            {
                cout << "Todas las ordenes:" << endl;
                auto printP = [&](const Paquete &p)
                {
                    cout << "ID: " << p.getId() << " | Cliente: " << p.getClienteId()
                         << " | Estado: " << p.getEstadoString() << " | Mensajero: " << (p.getMensajeroId().empty() ? "(No asignado)" : p.getMensajeroId())
                         << " | Origen: " << p.getDireccionOrigen() << " | Destino: " << p.getDireccionDestino()
                         << " | Peso: " << p.getPeso() << " lbs" << endl;
                };
                for (const Paquete &p : solic)
                    printP(p);
                for (const Paquete &p : transit)
                    printP(p);
                for (const Paquete &p : entreg)
                    printP(p);
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

// Nuevo: menú específico para Controlador
void menuControlador(SistemaEnvios &sistema, string controladorId)
{
    int opcion;
    do
    {
        cout << "\n=== MENU CONTROLADOR ===" << endl;
        cout << "1. Ver solicitudes (paquetes solicitados)" << endl;
        cout << "2. Asignar mensajero a paquete" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "Opcion: ";

        cin >> opcion;
        cin.ignore();

        switch (opcion)
        {
        case 1:
        {
            vector<Paquete> solicitudes = sistema.obtenerPaquetesPorEstado(SOLICITADO);
            if (solicitudes.empty())
            {
                cout << "No hay solicitudes pendientes." << endl;
            }
            else
            {
                cout << "Solicitudes pendientes:" << endl;
                for (const Paquete &p : solicitudes)
                {
                    cout << "ID: " << p.getId() << " | Cliente: " << p.getClienteId()
                         << " | Origen: " << p.getDireccionOrigen() << " | Destino: " << p.getDireccionDestino()
                         << " | Peso: " << p.getPeso() << " lbs" << endl;
                }
            }

            // Mostrar mensajeros disponibles
            vector<Mensajero *> disponibles = sistema.obtenerMensajerosDisponibles();
            if (disponibles.empty())
            {
                cout << "No hay mensajeros disponibles en este momento." << endl;
            }
            else
            {
                cout << "\nMensajeros disponibles:" << endl;
                for (Mensajero *m : disponibles)
                {
                    cout << "ID: " << m->getId() << " | Nombre: " << m->getNombre() << endl;
                }
            }
            break;
        }

        case 2:
        {
            // Revisar paquetes asignados y sin asignar
            vector<Paquete> todos = sistema.obtenerPaquetesPorEstado(SOLICITADO);
            // obtener tambien paquetes en otros estados para resumen
            vector<Paquete> enTransito = sistema.obtenerPaquetesPorEstado(EN_TRANSITO);
            vector<Paquete> entregados = sistema.obtenerPaquetesPorEstado(ENTREGADO);

            vector<Paquete> asignadosResumen;
            vector<Paquete> sinAsignar;

            // Paquetes sin asignar: estado SOLICITADO
            for (const Paquete &p : todos)
            {
                sinAsignar.push_back(p);
            }

            // Paquetes asignados: buscar en transito y entregados
            for (const Paquete &p : enTransito)
                asignadosResumen.push_back(p);
            for (const Paquete &p : entregados)
                asignadosResumen.push_back(p);

            if (sinAsignar.empty())
            {
                cout << "Todos los paquetes estan asignados." << endl;
                if (asignadosResumen.empty())
                {
                    cout << "No hay paquetes asignados para mostrar." << endl;
                }
                else
                {
                    cout << "Resumen de paquetes asignados:" << endl;
                    for (const Paquete &p : asignadosResumen)
                    {
                        string mid = p.getMensajeroId();
                        if (mid.empty())
                            mid = "(No asignado)";
                        cout << "Paquete: " << p.getId() << " | Mensajero Asignado: " << mid << endl;
                    }
                }
                break;
            }

            // Si hay paquetes sin asignar, mostrarlos y listar mensajeros disponibles
            cout << "Paquetes sin asignar:" << endl;
            for (const Paquete &p : sinAsignar)
            {
                cout << "ID: " << p.getId() << " | Cliente: " << p.getClienteId()
                     << " | Origen: " << p.getDireccionOrigen() << " | Destino: " << p.getDireccionDestino()
                     << " | Peso: " << p.getPeso() << " lbs" << endl;
            }

            vector<Mensajero *> disponibles = sistema.obtenerMensajerosDisponibles();
            if (disponibles.empty())
            {
                cout << "\nNo hay mensajeros disponibles en este momento." << endl;
                break;
            }
            else
            {
                cout << "\nMensajeros disponibles:" << endl;
                for (Mensajero *m : disponibles)
                {
                    cout << "ID: " << m->getId() << " | Nombre: " << m->getNombre() << endl;
                }
            }

            // Permitir asignar uno de los paquetes sin asignar con validacion interactiva
            cout << endl;
            string paqueteId, mensajeroId;

            // Validar paqueteId (debe estar en sinAsignar)
            while (true)
            {
                cout << "ID Paquete a asignar (Enter para cancelar): ";
                getline(cin, paqueteId);
                if (paqueteId.empty())
                {
                    cout << "Operacion cancelada." << endl;
                    break;
                }
                bool encontrado = false;
                for (const Paquete &p : sinAsignar)
                {
                    if (p.getId() == paqueteId)
                    {
                        encontrado = true;
                        break;
                    }
                }
                if (!encontrado)
                {
                    cout << "ID de paquete incorrecto o no esta en la lista de sin asignar. Intente de nuevo." << endl;
                    continue;
                }
                break;
            }
            if (paqueteId.empty())
                break;

            // Validar mensajeroId (debe existir y estar disponible)
            while (true)
            {
                cout << "ID Mensajero (Enter para cancelar): ";
                getline(cin, mensajeroId);
                if (mensajeroId.empty())
                {
                    cout << "Operacion cancelada." << endl;
                    break;
                }
                Usuario *u = sistema.buscarUsuario(mensajeroId);
                Mensajero *m = nullptr;
                if (u && u->getTipo() == "Mensajero")
                    m = dynamic_cast<Mensajero *>(u);
                if (!m)
                {
                    cout << "ID de mensajero invalido. Intente de nuevo." << endl;
                    continue;
                }
                if (m->isEnRuta())
                {
                    cout << "El mensajero no esta disponible (en ruta). Elija otro mensajero." << endl;
                    continue;
                }
                break;
            }
            if (mensajeroId.empty())
                break;

            if (sistema.asignarMensajero(paqueteId, mensajeroId, controladorId))
            {
                cout << "Mensajero asignado y notificado correctamente." << endl;
            }
            else
            {
                cout << "Error: No se pudo asignar el mensajero (verifique IDs o estado del paquete)." << endl;
            }
            break;
        }
        }
    } while (opcion != 0);
}

int main()
{
    SistemaEnvios sistema;

    // modoPrueba permanece en false por defecto; no preguntar al usuario en inicio

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
        case 1: // Iniciar Sesion (submenu)
        {
            int sub;
            do
            {
                mostrarMenuIniciarSesion();
                if (!(cin >> sub))
                {
                    cout << "Entrada invalida. Por favor ingrese una opcion numerica." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore();
                switch (sub)
                {
                case 1: // Ingresar Cliente
                {
                    cout << "=== INGRESO DE CLIENTE ===" << endl;
                    cout << "ID Cliente: ";
                    getline(cin, nombre);
                    Usuario *usuario = sistema.buscarUsuario(nombre);
                    if (usuario && usuario->getTipo() == "Cliente")
                    {
                        cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                        menuCliente(sistema, nombre);
                    }
                    else
                    {
                        cout << "Cliente no encontrado o ID invalido!" << endl;
                    }
                    break;
                }
                case 2: // Ingresar Mensajero
                {
                    cout << "=== INGRESO DE MENSAJERO ===" << endl;
                    cout << "ID Mensajero: ";
                    getline(cin, nombre);
                    Usuario *usuario = sistema.buscarUsuario(nombre);
                    if (usuario && usuario->getTipo() == "Mensajero")
                    {
                        cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                        menuMensajero(sistema, nombre);
                    }
                    else
                    {
                        cout << "Mensajero no encontrado o ID invalido!" << endl;
                    }
                    break;
                }
                case 3: // Ingresar Administrador
                {
                    cout << "=== INGRESO DE ADMINISTRADOR ===" << endl;
                    cout << "ID Administrador: ";
                    getline(cin, nombre);
                    Usuario *usuario = sistema.buscarUsuario(nombre);
                    if (usuario && usuario->getTipo() == "Administrador")
                    {
                        cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                        menuAdministrador(sistema, nombre);
                    }
                    else
                    {
                        cout << "Administrador no encontrado o ID invalido!" << endl;
                    }
                    break;
                }
                case 4: // Ingresar Controlador
                {
                    cout << "=== INGRESO DE CONTROLADOR ===" << endl;
                    cout << "ID Controlador: ";
                    getline(cin, nombre);
                    Usuario *usuario = sistema.buscarUsuario(nombre);
                    if (usuario && usuario->getTipo() == "Controlador")
                    {
                        cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                        menuControlador(sistema, nombre);
                    }
                    else
                    {
                        cout << "Controlador no encontrado o ID invalido!" << endl;
                    }
                    break;
                }
                case 0:
                    break;
                default:
                    cout << "Opcion invalida en el submenu." << endl;
                }
            } while (sub != 0);
            break;
        }

        case 2: // Crear cuenta (submenu)
        {
            int sub;
            do
            {
                mostrarMenuCrearCuenta();
                if (!(cin >> sub))
                {
                    cout << "Entrada invalida. Por favor ingrese una opcion numerica." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore();
                switch (sub)
                {
                case 1: // Registrar Cliente
                {
                    cout << "=== REGISTRO DE CLIENTE ===" << endl;
                    cout << "Nombre: ";
                    getline(cin, nombre);
                    cout << "Direccion: ";
                    getline(cin, direccion);
                    telefono = obtenerTelefonoValido();
                    email = obtenerEmailUnico(sistema);
                    string id = sistema.generarIdUsuario("CLI");
                    if (sistema.registrarUsuario(new Cliente(nombre, direccion, telefono, email, id)))
                    {
                        cout << "Cliente registrado exitosamente! ID asignado: " << id << endl;
                    }
                    else
                    {
                        cout << "Error en el registro" << endl;
                    }
                    break;
                }
                case 2: // Registrar Mensajero
                {
                    cout << "=== REGISTRO DE MENSAJERO ===" << endl;
                    cout << "Nombre: ";
                    getline(cin, nombre);
                    cout << "Direccion: ";
                    getline(cin, direccion);
                    telefono = obtenerTelefonoValido();
                    email = obtenerEmailUnico(sistema);
                    string id = sistema.generarIdUsuario("MSG");
                    if (sistema.registrarUsuario(new Mensajero(nombre, direccion, telefono, email, id)))
                    {
                        cout << "Mensajero registrado exitosamente! ID asignado: " << id << endl;
                    }
                    else
                    {
                        cout << "Error en el registro" << endl;
                    }
                    break;
                }
                case 3: // Registrar Administrador
                {
                    cout << "=== REGISTRO DE ADMINISTRADOR ===" << endl;
                    cout << "Nombre: ";
                    getline(cin, nombre);
                    cout << "Direccion: ";
                    getline(cin, direccion);
                    telefono = obtenerTelefonoValido();
                    email = obtenerEmailUnico(sistema);
                    string id = sistema.generarIdUsuario("ADM");
                    if (sistema.registrarUsuario(new Administrador(nombre, direccion, telefono, email, id)))
                    {
                        cout << "Administrador registrado exitosamente! ID asignado: " << id << endl;
                    }
                    else
                    {
                        cout << "Error en el registro" << endl;
                    }
                    break;
                }
                case 4: // Registrar Controlador
                {
                    cout << "=== REGISTRO DE CONTROLADOR ===" << endl;
                    cout << "Nombre: ";
                    getline(cin, nombre);
                    cout << "Direccion: ";
                    getline(cin, direccion);
                    telefono = obtenerTelefonoValido();
                    email = obtenerEmailUnico(sistema);
                    string id = sistema.generarIdUsuario("CTL");
                    if (sistema.registrarUsuario(new Controlador(nombre, direccion, telefono, email, id)))
                    {
                        cout << "Controlador registrado exitosamente! ID asignado: " << id << endl;
                    }
                    else
                    {
                        cout << "Error en el registro" << endl;
                    }
                    break;
                }
                case 0:
                    break;
                default:
                    cout << "Opcion invalida en el submenu." << endl;
                }
            } while (sub != 0);
            break;
        }
        case 6:
        { // Ingresar Mensajero
            cout << "=== INGRESO DE MENSAJERO ===" << endl;
            cout << "ID Mensajero: ";
            getline(cin, nombre); // reutilizando variable nombre para el id

            Usuario *usuario = sistema.buscarUsuario(nombre);
            if (usuario && usuario->getTipo() == "Mensajero")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuMensajero(sistema, nombre);
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
            getline(cin, nombre); // reutilizando variable nombre para el id

            Usuario *usuario = sistema.buscarUsuario(nombre);
            if (usuario && usuario->getTipo() == "Administrador")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuAdministrador(sistema, nombre);
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
            getline(cin, nombre); // reutilizando variable nombre para el id

            Usuario *usuario = sistema.buscarUsuario(nombre);
            if (usuario && usuario->getTipo() == "Controlador")
            {
                cout << "Bienvenido " << usuario->getNombre() << "!" << endl;
                menuControlador(sistema, nombre);
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