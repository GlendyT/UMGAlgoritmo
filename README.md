# Sistema de Envíos Garantizados - Documentación Técnica

## Diagrama de Clases UML

```mermaid
classDiagram
    class Usuario {
        <<abstract>>
        #string nombre
        #string direccion
        #string telefono
        #string email
        #string id
        #bool activo
        +Usuario(string, string, string, string, string)
        +getNombre() string
        +getDireccion() string
        +getTelefono() string
        +getEmail() string
        +getId() string
        +isActivo() bool
        +getTipo()* string
    }
    class Cliente {
        +getTipo() string
    }
    class Administrador {
        +getTipo() string
    }
    class Controlador {
        +getTipo() string
    }
    class Mensajero {
        -bool enRuta
        +isEnRuta() bool
        +setEnRuta(bool)
        +getTipo() string
    }
    class Paquete {
        -string id
        -string clienteId
        -double peso
        -EstadoPaquete estado
        +Paquete(string, string, string, string, double, string)
        +getId() string
        +getEstado() EstadoPaquete
        +setEstado(EstadoPaquete)
        +getEstadoString() string
    }
    class SistemaEnvios {
        -vector~Usuario*~ usuarios
        -vector~Paquete~ paquetes
        -double tarifaBase
        +registrarUsuario(Usuario*) bool
        +buscarUsuario(string) Usuario*
        +solicitarEnvio(string, string, string, double, string) string
        +asignarMensajero(string, string, string) bool
        +actualizarEstadoPaquete(string, EstadoPaquete) bool
        +obtenerPaquetesPorEstado(EstadoPaquete) vector~Paquete~
    }
    class EstadoPaquete {
        <<enumeration>>
        SOLICITADO
        EN_TRANSITO
        ENTREGADO
    }

    Usuario <|-- Cliente
    Usuario <|-- Administrador
    Usuario <|-- Controlador
    Usuario <|-- Mensajero
    SistemaEnvios *-- Usuario : manages
    SistemaEnvios *-- Paquete : contains
    Paquete --> EstadoPaquete : uses
```

### Flujo de Estados de un Paquete

El estado de un paquete sigue un ciclo de vida claro, gestionado por el sistema.

    SistemaEnvios *-- Usuario : manages
    SistemaEnvios *-- Paquete : contains
    Paquete --> EstadoPaquete : uses
    Paquete --> Cliente : belongs to
    Paquete --> Mensajero : assigned to

    SistemaEnvios ..> Cliente : creates
    SistemaEnvios ..> Mensajero : creates
    SistemaEnvios ..> Administrador : creates
    SistemaEnvios ..> Controlador : creates
```

## Diagrama de Flujo del Sistema Principal

```mermaid
flowchart TD
    A[Inicio del Sistema] --> B[Cargar Usuarios desde Archivo]
    B --> C{¿Existe Administrador?}
    C -->|No| D[Crear Admin y Controlador por Defecto]
    C -->|Sí| E[Mostrar Menú Principal]
    D --> E
    
    E --> F{Seleccionar Opción}
    F -->|1| G[Registrar Cliente]
    F -->|2| H[Registrar Mensajero]
    F -->|3| I[Registrar Administrador]
    F -->|4| J[Registrar Controlador]
    F -->|5| K[Ingreso Cliente]
    F -->|6| L[Ingreso Mensajero]
    F -->|7| M[Ingreso Administrador]
    F -->|8| N[Ingreso Controlador]
    F -->|0| O[Salir]
    
    G --> P[Validar Datos]
    H --> P
    I --> P
    J --> P
    
    P --> Q{¿ID Único?}
    Q -->|Sí| R[Crear Usuario]
    Q -->|No| S[Error: ID Existe]
    R --> E
    S --> E
    
    K --> T[Validar Cliente]
    L --> U[Validar Mensajero]
    M --> V[Validar Administrador]
    N --> W[Validar Controlador]
    
    T --> X{¿Usuario Válido?}
    U --> X
    V --> X
    W --> X
    
    X -->|Sí| Y[Mostrar Menú Específico]
    X -->|No| Z[Error: Usuario No Encontrado]
    Y --> E
    Z --> E
    
    O --> AA[Guardar Usuarios]
    AA --> BB[Fin del Sistema]
```

## Diagrama de Flujo - Menú Cliente

```mermaid
flowchart TD
    A[Menú Cliente] --> B{Seleccionar Opción}
    B -->|1| C[Solicitar Envío]
    B -->|2| D[Ver Mensajes]
    B -->|0| E[Volver al Menú Principal]
    
    C --> F[Ingresar Dirección Origen]
    F --> G[Ingresar Dirección Destino]
    G --> H[Ingresar Peso del Paquete]
    H --> I{¿Peso ≤ 15 lbs?}
    I -->|No| J[Error: Peso Excede 15 lbs]
    I -->|Sí| K[Ingresar Descripción]
    J --> A
    K --> L[Calcular Costo]
    L --> M[Crear Paquete]
    M --> N[Generar ID Único]
    N --> O[Estado: SOLICITADO]
    O --> P[Mostrar ID del Paquete]
    P --> A
    
    D --> Q[Obtener Mensajes del Cliente]
    Q --> R[Mostrar Mensajes]
    R --> A
```

## Diagrama de Flujo - Menú Mensajero

```mermaid
flowchart TD
    A[Menú Mensajero] --> B{Seleccionar Opción}
    B -->|1| C[Ver Paquetes Asignados]
    B -->|2| D[Actualizar Estado de Paquete]
    B -->|3| E[Ver Mensajes]
    B -->|0| F[Volver al Menú Principal]
    
    C --> G[Filtrar Paquetes por Mensajero]
    G --> H[Mostrar Paquetes EN_TRANSITO]
    H --> A
    
    D --> I[Seleccionar Paquete]
    I --> J{¿Paquete Válido?}
    J -->|Sí| K[Cambiar Estado a ENTREGADO]
    J -->|No| L[Error: Paquete No Encontrado]
    K --> M[Actualizar Fecha de Entrega]
    M --> A
    L --> A
    
    E --> N[Obtener Mensajes del Mensajero]
    N --> O[Mostrar Mensajes]
    O --> A
```

## Diagrama de Flujo - Menú Administrador

```mermaid
flowchart TD
    A[Menú Administrador] --> B{Seleccionar Opción}
    B -->|1| C[Asignar Mensajero]
    B -->|2| D[Generar Reporte]
    B -->|3| E[Actualizar Tarifa Base]
    B -->|4| F[Ver Usuarios Registrados]
    B -->|5| G[Ver Mensajes]
    B -->|6| H[Guardar Usuarios]
    B -->|7| I[Cargar Usuarios]
    B -->|0| J[Volver al Menú Principal]
    
    C --> K[Seleccionar Paquete SOLICITADO]
    K --> L[Seleccionar Mensajero Disponible]
    L --> M[Asignar y Cambiar Estado a EN_TRANSITO]
    M --> A
    
    D --> N[Obtener Todos los Paquetes]
    N --> O[Agrupar por Estado]
    O --> P[Mostrar Estadísticas]
    P --> A
    
    E --> Q[Ingresar Nueva Tarifa]
    Q --> R[Actualizar Tarifa Base]
    R --> A
    
    F --> S[Obtener Lista de Usuarios]
    S --> T[Mostrar Información Detallada]
    T --> A
    
    G --> U[Obtener Mensajes del Admin]
    U --> V[Mostrar Mensajes]
    V --> A
    
    H --> W[Escribir Usuarios a Archivo]
    W --> A
    
    I --> X{¿Confirmar Carga?}
    X -->|Sí| Y[Leer Usuarios desde Archivo]
    X -->|No| Z[Cancelar Operación]
    Y --> A
    Z --> A
```

## Diagrama Entidad-Relación

```mermaid
erDiagram
    USUARIO {
        string id PK
        string nombre
        string direccion
        string telefono
        string email
        bool activo
        string tipo
    }
    
    CLIENTE {
        string id PK
    }
    
    ADMINISTRADOR {
        string id PK
    }
    
    CONTROLADOR {
        string id PK
    }
    
    MENSAJERO {
        string id PK
        bool enRuta
    }
    
    PAQUETE {
        string id PK
        string clienteId FK
        string direccionOrigen
        string direccionDestino
        double peso
        string descripcion
        EstadoPaquete estado
        time_t fechaSolicitud
        time_t fechaEntrega
        double costo
        string mensajeroId FK
    }
    
    MENSAJE {
        string destinatarioId FK
        string contenido
    }
    
    SISTEMA_ENVIOS {
        double tarifaBase
        int contadorId
    }
    
    USUARIO ||--o{ CLIENTE : "es un"
    USUARIO ||--o{ ADMINISTRADOR : "es un"
    USUARIO ||--o{ CONTROLADOR : "es un"
    USUARIO ||--o{ MENSAJERO : "es un"
    
    CLIENTE ||--o{ PAQUETE : "solicita"
    MENSAJERO ||--o{ PAQUETE : "entrega"
    
    USUARIO ||--o{ MENSAJE : "recibe"
    
    SISTEMA_ENVIOS ||--o{ USUARIO : "gestiona"
    SISTEMA_ENVIOS ||--o{ PAQUETE : "contiene"
    SISTEMA_ENVIOS ||--o{ MENSAJE : "almacena"
```

## Estados del Paquete

```mermaid
stateDiagram-v2
    [*] --> SOLICITADO : Cliente solicita envío
    SOLICITADO --> EN_TRANSITO : ADMON. o Controlador asigna mensajero
    EN_TRANSITO --> ENTREGADO : Mensajero confirma entrega
    ENTREGADO --> [*]
```

## Principios de Programación Orientada a Objetos (POO)

- **Abstracción**: La clase `Usuario` es una clase base abstracta que define la interfaz común para todos los tipos de usuarios. El `SistemaEnvios` opera sobre punteros a `Usuario`, desconociendo los detalles concretos de cada rol.
- **Encapsulación**: Los datos de las clases (`Paquete`, `Usuario`, `SistemaEnvios`) están protegidos (privados o protegidos) y solo se puede acceder a ellos a través de métodos públicos (getters y setters), asegurando la integridad de los datos.
- **Herencia**: Las clases `Cliente`, `Mensajero`, `Administrador` y `Controlador` heredan de la clase `Usuario`, reutilizando código y estableciendo una jerarquía de tipos.
- **Polimorfismo**: Se utiliza a través de la función virtual `getTipo()`. Un `vector<Usuario*>` puede almacenar diferentes tipos de usuarios, y el sistema puede invocar `getTipo()` en cada uno para determinar su rol específico en tiempo de ejecución.

## Principios SOLID

- **Principio de Responsabilidad Única (SRP)**: Cada clase tiene un propósito bien definido. `Paquete` modela un paquete, `Usuario` a un usuario, `SistemaEnvios` orquesta la lógica principal y `main.cpp` gestiona la interacción con el usuario.
- **Principio de Abierto/Cerrado (OCP)**: El sistema es extensible. Se puede añadir un nuevo rol (ej. `Supervisor`) creando una nueva clase que herede de `Usuario` sin necesidad de modificar el código existente que maneja usuarios.
- **Principio de Sustitución de Liskov (LSP)**: Las subclases de `Usuario` son completamente sustituibles por su clase base. El sistema lo demuestra al gestionar todos los usuarios a través de un `vector<Usuario*>`.
- **Principio de Segregación de Interfaces (ISP)**: Aunque es más visible en diseños basados en interfaces, el concepto se aplica en la separación de menús. Cada rol tiene su propio menú y solo ve las opciones que le corresponden, evitando que un `Cliente` acceda a funciones de `Administrador`.
- **Principio de Inversión de Dependencias (DIP)**: Los módulos de alto nivel como `SistemaEnvios` dependen de abstracciones (`Usuario`) en lugar de implementaciones concretas (`Cliente`, `Mensajero`), lo que desacopla el sistema.

## Estructura del Proyecto

```
ProjectC++/
├── main.cpp              # Punto de entrada y gestión de menús de la consola.
├── SistemaEnvios.h       # Definición de la clase principal del sistema.
├── SistemaEnvios.cpp     # Implementación de la lógica de negocio.
├── Usuario.h             # Definición de la clase base Usuario y sus derivadas.
├── Usuario.cpp           # Implementación de las clases de usuario.
├── Paquete.h             # Definición de la clase Paquete y el enum EstadoPaquete.
├── Paquete.cpp           # Implementación de la clase Paquete.
├── usuarios.txt          # Archivo de texto para persistencia de usuarios.
├── paquetes.txt          # Archivo de texto para persistencia de paquetes.
└── SYSTEM_DOCUMENTATION.md # Documentación técnica original con diagramas.
```

## Dependencias y Compilación

### Dependencias

El proyecto está construido utilizando C++ estándar y no requiere bibliotecas externas. Solo utiliza las bibliotecas del Standard Template Library (STL) incluidas con cualquier compilador de C++ moderno:
- `<iostream>`
- `<string>`
- `<vector>`
- `<map>`
- `<fstream>`
- `<ctime>`
- `<algorithm>`
- `<iomanip>`
- `<limits>`
- `<cctype>`

### Compilación

Para compilar el proyecto, puedes usar un compilador como g++. Asegúrate de compilar todos los archivos `.cpp` juntos.

```bash
# Comando para compilar con g++
g++ -o main.exe *.cpp

# Después de compilar, ejecuta el programa
.\main.exe
```

## Cómo Usar el Sistema

1.  **Compila y ejecuta** el programa como se indicó anteriormente.
2.  Al iniciar, el sistema cargará los usuarios y paquetes desde `usuarios.txt` y `paquetes.txt`. Si no existen, se creará un Administrador y un Controlador por defecto.
3.  El **Menú Principal** te permitirá `Iniciar Sesión` o `Crear una cuenta`.
4.  **Crear una cuenta**: Puedes registrarte como Cliente, Mensajero, etc. El sistema te asignará un ID único.
5.  **Iniciar Sesión**: Ingresa tu ID de usuario. Serás dirigido al menú correspondiente a tu rol, con las funcionalidades permitidas.
6.  Al salir del programa (opción 0 en el menú principal), todos los datos de usuarios se guardarán automáticamente en `usuarios.txt`. Los paquetes se guardan después de ciertas acciones clave.
    SOLICITADO --> EN_TRANSITO : Administrador asigna mensajero
    EN_TRANSITO --> ENTREGADO : Mensajero confirma entrega
    ENTREGADO --> [*]
    
    note right of SOLICITADO
        Paquete creado
        Esperando asignación
    end note
    
    note right of EN_TRANSITO
        Mensajero asignado
        En proceso de entrega
    end note
    
    note right of ENTREGADO
        Entrega completada
        Fecha registrada
    end note
```

## Arquitectura del Sistema

```mermaid
graph TB
    subgraph "Capa de Presentación"
        UI[Interfaz de Usuario - main.cpp]
        MENU[Menús Específicos por Rol]
    end
    
    subgraph "Capa de Lógica de Negocio"
        SYS[SistemaEnvios]
        USER[Gestión de Usuarios]
        PKG[Gestión de Paquetes]
        MSG[Sistema de Mensajes]
    end
    
    subgraph "Capa de Datos"
        FILE[Archivos de Persistencia]
        MEM[Estructuras en Memoria]
    end
    
    subgraph "Modelos de Dominio"
        USUARIO[Usuario - Clase Abstracta]
        CLIENTE[Cliente]
        MENSAJERO[Mensajero]
        ADMIN[Administrador]
        CTRL[Controlador]
        PAQUETE[Paquete]
        ESTADO[EstadoPaquete]
    end
    
    UI --> MENU
    MENU --> SYS
    SYS --> USER
    SYS --> PKG
    SYS --> MSG
    
    USER --> USUARIO
    USUARIO --> CLIENTE
    USUARIO --> MENSAJERO
    USUARIO --> ADMIN
    USUARIO --> CTRL
    
    PKG --> PAQUETE
    PAQUETE --> ESTADO
    
    SYS --> FILE
    SYS --> MEM
```

## Patrones de Diseño Implementados

### 1. **Patrón Template Method**
- La clase `Usuario` define la estructura común
- Las clases derivadas implementan `getTipo()`

### 2. **Patrón Singleton (Implícito)**
- `SistemaEnvios` actúa como punto central de gestión
- Una sola instancia maneja todos los datos

### 3. **Patrón Factory (Implícito)**
- El sistema crea diferentes tipos de usuarios
- Polimorfismo para manejo uniforme

### 4. **Patrón State**
- `EstadoPaquete` representa los estados del paquete
- Transiciones controladas entre estados

## Funcionalidades Principales

### **Gestión de Usuarios**
- Registro de diferentes tipos de usuarios
- Validación de IDs únicos
- Persistencia en archivos
- Autenticación por ID

### **Gestión de Paquetes**
- Solicitud de envíos por clientes
- Validación de peso máximo (15 libras)
- Cálculo automático de tarifas
- Asignación de mensajeros
- Seguimiento de estados

### **Sistema de Mensajes**
- Comunicación entre usuarios
- Almacenamiento por destinatario
- Consulta de mensajes

### **Reportes y Administración**
- Generación de reportes estadísticos
- Actualización de tarifas
- Gestión de usuarios registrados

## Consideraciones Técnicas

### **Memoria**
- Uso de punteros para polimorfismo
- Gestión manual de memoria (destructores)
- Vectores para colecciones dinámicas

### **Persistencia**
- Archivos de texto para almacenamiento
- Carga automática al inicio
- Guardado automático al salir

### **Validación**
- Verificación de IDs únicos
- Validación de tipos de usuario
- Control de peso máximo de paquetes (15 lbs)
- Control de estados de paquetes

### **Escalabilidad**
- Estructura modular
- Fácil extensión de tipos de usuario
- Separación de responsabilidades
