#include "Usuario.h"

Usuario::Usuario(string n, string dir, string tel, string em, string i)
    : nombre(n), direccion(dir), telefono(tel), email(em), id(i), activo(true) {}

Cliente::Cliente(string n, string dir, string tel, string em, string i)
    : Usuario(n, dir, tel, em, i) {}

Administrador::Administrador(string n, string dir, string tel, string em, string i)
    : Usuario(n, dir, tel, em, i) {}

Controlador::Controlador(string n, string dir, string tel, string em, string i)
    : Usuario(n, dir, tel, em, i) {}

Mensajero::Mensajero(string n, string dir, string tel, string em, string i)
    : Usuario(n, dir, tel, em, i), enRuta(false) {}