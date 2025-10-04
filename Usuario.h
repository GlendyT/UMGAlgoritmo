#ifndef USUARIO_H
#define USUARIO_H

#include <string>
using namespace std;

class Usuario {
protected:
    string nombre;
    string direccion;
    string telefono;
    string email;
    string id;
    bool activo;

public:
    Usuario(string n, string dir, string tel, string em, string i);
    virtual ~Usuario() = default;
    
    // Getters
    string getNombre() const { return nombre; }
    string getDireccion() const { return direccion; }
    string getTelefono() const { return telefono; }
    string getEmail() const { return email; }
    string getId() const { return id; }
    bool isActivo() const { return activo; }
    
    // Setters
    void setActivo(bool estado) { activo = estado; }
    void setDireccion(string dir) { direccion = dir; }
    void setTelefono(string tel) { telefono = tel; }
    
    virtual string getTipo() const = 0;
};

class Cliente : public Usuario {
public:
    Cliente(string n, string dir, string tel, string em, string i);
    string getTipo() const override { return "Cliente"; }
};

class Administrador : public Usuario {
public:
    Administrador(string n, string dir, string tel, string em, string i);
    string getTipo() const override { return "Administrador"; }
};

class Controlador : public Usuario {
public:
    Controlador(string n, string dir, string tel, string em, string i);
    string getTipo() const override { return "Controlador"; }
};

class Mensajero : public Usuario {
private:
    bool enRuta;
    
public:
    Mensajero(string n, string dir, string tel, string em, string i);
    string getTipo() const override { return "Mensajero"; }
    bool isEnRuta() const { return enRuta; }
    void setEnRuta(bool estado) { enRuta = estado; }
};

#endif