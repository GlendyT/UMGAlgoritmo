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
        +setActivo(bool)
        +setDireccion(string)
        +setTelefono(string)
        +getTipo()* string
    }

    class Cliente {
        +Cliente(string, string, string, string, string)
        +getTipo() string
    }

    class Administrador {
        +Administrador(string, string, string, string, string)
        +getTipo() string
    }

    class Controlador {
        +Controlador(string, string, string, string, string)
        +getTipo() string
    }

    class Mensajero {
        -bool enRuta
        +Mensajero(string, string, string, string, string)
        +getTipo() string
        +isEnRuta() bool
        +setEnRuta(bool)
    }

    class Paquete {
        -string id
        -string clienteId
        -string direccionOrigen
        -string direccionDestino
        -double peso
        -string descripcion
        -EstadoPaquete estado
        -time_t fechaSolicitud
        -time_t fechaEntrega
        -double costo
        -string mensajeroId
        +Paquete(string, string, string, string, double, string)
        +getId() string
        +getClienteId() string
        +getDireccionOrigen() string
        +getDireccionDestino() string
        +getPeso() double
        +getDescripcion() string
        +getEstado() EstadoPaquete
        +getFechaSolicitud() time_t
        +getFechaEntrega() time_t
        +getCosto() double
        +getMensajeroId() string
        +setEstado(EstadoPaquete)
        +setFechaEntrega(time_t)
        +setCosto(double)
        +setMensajeroId(string)
        +getEstadoString() string
    }

    class SistemaEnvios {
        -vector~Usuario*~ usuarios
        -vector~Paquete~ paquetes
        -map~string, string~ mensajes
        -double tarifaBase
        -int contadorId
        +SistemaEnvios()
        +~SistemaEnvios()
        +registrarUsuario(Usuario*) bool
        +eliminarUsuario(string) bool
        +buscarUsuario(string) Usuario*
        +obtenerUsuarios() vector~Usuario*~
        +solicitarEnvio(string, string, string, double, string) string
        +asignarMensajero(string, string, string) bool
        +actualizarEstadoPaquete(string, EstadoPaquete) bool
        +enviarMensaje(string, string)
        +obtenerMensajes(string) vector~string~
        +calcularTarifa(double, time_t) double
        +actualizarTarifaBase(double)
        +obtenerPaquetesPorEstado(EstadoPaquete) vector~Paquete~
        +generarReporte()
        -generarId() string
        -esHorarioNormal(time_t) bool
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
