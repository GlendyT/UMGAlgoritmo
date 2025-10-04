#include "SistemaEnvios.h"
#include <iostream>
#include <string>
using namespace std;

void mostrarMenu() {
    cout << "\n=== SISTEMA DE ENVIOS GARANTIZADOS ===" << endl;
    cout << "1. Registrar Cliente" << endl;
    cout << "2. Registrar Mensajero" << endl;
    cout << "3. Solicitar Envio" << endl;
    cout << "4. Asignar Mensajero (Controlador)" << endl;
    cout << "5. Actualizar Estado Paquete" << endl;
    cout << "6. Ver Mensajes" << endl;
    cout << "7. Generar Reporte" << endl;
    cout << "8. Actualizar Tarifa Base (Admin)" << endl;
    cout << "9. Listar Usuarios" << endl;
    cout << "0. Salir" << endl;
    cout << "Opcion: ";
}

int main() {
    SistemaEnvios sistema;
    
    // Crear usuarios por defecto
    sistema.registrarUsuario(new Administrador("Admin", "Oficina Central", "123-456", "admin@envios.com", "ADM001"));
    sistema.registrarUsuario(new Controlador("Control", "Centro Control", "123-457", "control@envios.com", "CTL001"));
    
    int opcion;
    string nombre, direccion, telefono, email, id;
    
    do {
        mostrarMenu();
        cin >> opcion;
        cin.ignore();
        
        switch(opcion) {
            case 1: {
                cout << "Nombre: "; getline(cin, nombre);
                cout << "Direccion: "; getline(cin, direccion);
                cout << "Telefono: "; getline(cin, telefono);
                cout << "Email: "; getline(cin, email);
                cout << "ID: "; getline(cin, id);
                
                if(sistema.registrarUsuario(new Cliente(nombre, direccion, telefono, email, id))) {
                    cout << "Cliente registrado exitosamente!" << endl;
                } else {
                    cout << "Error: ID ya existe" << endl;
                }
                break;
            }
            
            case 2: {
                cout << "Nombre: "; getline(cin, nombre);
                cout << "Direccion: "; getline(cin, direccion);
                cout << "Telefono: "; getline(cin, telefono);
                cout << "Email: "; getline(cin, email);
                cout << "ID: "; getline(cin, id);
                
                if(sistema.registrarUsuario(new Mensajero(nombre, direccion, telefono, email, id))) {
                    cout << "Mensajero registrado exitosamente!" << endl;
                } else {
                    cout << "Error: ID ya existe" << endl;
                }
                break;
            }
            
            case 3: {
                string clienteId, origen, destino, desc;
                double peso;
                
                cout << "ID Cliente: "; getline(cin, clienteId);
                cout << "Direccion Origen: "; getline(cin, origen);
                cout << "Direccion Destino: "; getline(cin, destino);
                cout << "Peso (max 15 lbs): "; cin >> peso;
                cin.ignore();
                cout << "Descripcion: "; getline(cin, desc);
                
                string paqueteId = sistema.solicitarEnvio(clienteId, origen, destino, peso, desc);
                if(!paqueteId.empty()) {
                    cout << "Envio solicitado. ID: " << paqueteId << endl;
                } else {
                    cout << "Error en la solicitud" << endl;
                }
                break;
            }
            
            case 4: {
                string paqueteId, mensajeroId, controladorId;
                cout << "ID Paquete: "; getline(cin, paqueteId);
                cout << "ID Mensajero: "; getline(cin, mensajeroId);
                cout << "ID Controlador: "; getline(cin, controladorId);
                
                if(sistema.asignarMensajero(paqueteId, mensajeroId, controladorId)) {
                    cout << "Mensajero asignado exitosamente!" << endl;
                } else {
                    cout << "Error en la asignacion" << endl;
                }
                break;
            }
            
            case 5: {
                string paqueteId;
                int estado;
                cout << "ID Paquete: "; getline(cin, paqueteId);
                cout << "Estado (0=Solicitado, 1=En Transito, 2=Entregado): "; cin >> estado;
                
                if(sistema.actualizarEstadoPaquete(paqueteId, (EstadoPaquete)estado)) {
                    cout << "Estado actualizado!" << endl;
                } else {
                    cout << "Error al actualizar estado" << endl;
                }
                break;
            }
            
            case 6: {
                string usuarioId;
                cout << "ID Usuario: "; getline(cin, usuarioId);
                
                vector<string> mensajes = sistema.obtenerMensajes(usuarioId);
                cout << "Mensajes para " << usuarioId << ":" << endl;
                for(const string& msg : mensajes) {
                    cout << "- " << msg << endl;
                }
                break;
            }
            
            case 7: {
                sistema.generarReporte();
                break;
            }
            
            case 8: {
                double nuevaTarifa;
                cout << "Nueva tarifa base: "; cin >> nuevaTarifa;
                sistema.actualizarTarifaBase(nuevaTarifa);
                cout << "Tarifa actualizada!" << endl;
                break;
            }
            
            case 9: {
                vector<Usuario*> usuarios = sistema.obtenerUsuarios();
                cout << "\n=== USUARIOS REGISTRADOS ===" << endl;
                for(Usuario* u : usuarios) {
                    cout << "ID: " << u->getId() << " | Nombre: " << u->getNombre() 
                         << " | Tipo: " << u->getTipo() << endl;
                }
                break;
            }
        }
    } while(opcion != 0);
    
    cout << "Gracias por usar el Sistema de Envios Garantizados!" << endl;
    return 0;
}