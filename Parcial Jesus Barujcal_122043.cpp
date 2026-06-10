#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cctype>
#include <locale>

using namespace std;

/*
	AUTOR : JESÚS BARÚJCAL 
*/

struct Usuario {
    int idUsuario;
    string nombre;
    string correoElectronico;
    string contrasena;
    string direccion;
    string metodoDePago;
    bool tieneBono50; 
};

struct Producto {
    int idProducto;
    string nombre;
    string descripcion;
    double precio;
    int stock;
};

struct Comentario {
    int idComentario;
    string productoNombre;
    string usuarioNombre;
    string texto;
    string fecha;
};

struct ItemCarrito {
    Producto producto;
    int cantidad;
};

struct Carrito {
    int idCarrito;
    int idUsuario;
    vector<ItemCarrito> items;
    bool pagado;
};

struct OrdenDeCompra {
    int idOrden;
    Usuario usuario;
    vector<ItemCarrito> productos;
    double subtotal;
    double descuentoBono; 
    double impuestos;
    double envio;
    double total;
};


vector<Usuario> usuarios;
vector<Producto> productos;
vector<Comentario> comentarios;
vector<Carrito> carritosGlobales;
vector<OrdenDeCompra> ordenes;

int proximoIdCarrito = 1;
int proximoIdOrden = 1;


int toInt(string s) {
    stringstream ss(s);
    int x = 0;
    ss >> x;
    return x;
}


double toDouble(string s) {
    stringstream ss(s);
    double x = 0;
    ss >> x;
    return x;
}


string trim(string s) {
    while (!s.empty() && isspace((unsigned char)s[0])) s.erase(0, 1);
    while (!s.empty() && isspace((unsigned char)s[s.size() - 1])) s.erase(s.size() - 1, 1);
    return s;
}


string aMayusculas(string texto) {
    for (size_t i = 0; i < texto.size(); i++) {
        texto[i] = toupper((unsigned char)texto[i]);
    }
    return texto;
}


string enteroAString(int numero) {
    ostringstream ss;
    ss << numero;
    return ss.str();
}


double calcularSubtotal(const vector<ItemCarrito>& items) {
    double subtotal = 0.0;
    for (size_t i = 0; i < items.size(); i++) {
        subtotal += items[i].producto.precio * items[i].cantidad;
    }
    return subtotal;
}


Usuario* buscarUsuarioPorId(int id) {
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (usuarios[i].idUsuario == id) {
            return &usuarios[i];
        }
    }
    return NULL;
}

Producto* buscarProductoPorId(int id) {
    for (size_t i = 0; i < productos.size(); i++) {
        if (productos[i].idProducto == id) {
            return &productos[i];
        }
    }
    return NULL;
}


void cargarUsuarios() {
    ifstream file("datosusuarios.txt");
    if (!file.is_open()) {
        cout << "[!] Advertencia: No se pudo abrir datosusuarios.txt\n";
        return;
    }
    string linea;
    getline(file, linea); 

    while (getline(file, linea)) {
        if (linea.empty()) continue;
        stringstream ss(linea);
        Usuario u;
        string temp;

        getline(ss, temp, ','); u.idUsuario = toInt(trim(temp));
        getline(ss, u.nombre, ','); u.nombre = trim(u.nombre);
        getline(ss, u.correoElectronico, ','); u.correoElectronico = trim(u.correoElectronico);
        getline(ss, u.contrasena, ','); u.contrasena = trim(u.contrasena);
        getline(ss, u.direccion, ','); u.direccion = trim(u.direccion);
        getline(ss, u.metodoDePago); u.metodoDePago = trim(u.metodoDePago);
        
        u.tieneBono50 = false; 
        usuarios.push_back(u);
    }
    file.close();
}

void cargarProductos() {
    ifstream file("listaproductos.txt");
    if (!file.is_open()) {
        cout << "[!] Advertencia: No se pudo abrir listaproductos.txt\n";
        return;
    }
    string linea;

    while (getline(file, linea)) {
        if (linea.empty()) continue;
        stringstream ss(linea);
        Producto p;
        string temp;

        getline(ss, temp, ','); p.idProducto = toInt(trim(temp));
        getline(ss, p.nombre, ','); p.nombre = trim(p.nombre);
        getline(ss, p.descripcion, ','); p.descripcion = trim(p.descripcion);
        getline(ss, temp, ','); p.precio = toDouble(trim(temp));
        getline(ss, temp); p.stock = toInt(trim(temp));

        productos.push_back(p);
    }
    file.close();
}

void cargarComentarios() {
    ifstream file("comentariosusuarios.txt");
    if (!file.is_open()) {
        cout << "[!] Advertencia: No se pudo abrir comentariosusuarios.txt\n";
        return;
    }
    string linea;

    while (getline(file, linea)) {
        if (linea.empty()) continue;
        stringstream ss(linea);
        Comentario c;
        string temp;

        getline(ss, temp, ','); c.idComentario = toInt(trim(temp));
        getline(ss, c.productoNombre, ','); c.productoNombre = trim(c.productoNombre);
        getline(ss, c.usuarioNombre, ','); c.usuarioNombre = trim(c.usuarioNombre);
        getline(ss, c.texto, ','); c.texto = trim(c.texto);
        getline(ss, c.fecha); c.fecha = trim(c.fecha);

        comentarios.push_back(c);
    }
    file.close();
}

void mostrarCatalogo() 
{
    cout << "\n===== CATALOGO DE PRODUCTOS =====\n";
    cout << left << setw(4) << "ID" << setw(22) << "Nombre" << setw(12) << "Precio" << setw(7) << "Stock" << endl;
    cout << string(50, '-') << endl;

    for (size_t i = 0; i < productos.size(); i++) {
        cout << left << setw(4) << productos[i].idProducto << setw(22) << productos[i].nombre
             << "$" << setw(11) << fixed << setprecision(2) << productos[i].precio << setw(7) << productos[i].stock << endl;
    }
}

int contarCarritosActivos(int idUsuario) 
{
    int cnt = 0;
    for (size_t i = 0; i < carritosGlobales.size(); i++) {
        if (carritosGlobales[i].idUsuario == idUsuario && !carritosGlobales[i].pagado) 
		{
            cnt++;
        }
    }
    return cnt;
}

int buscarCarritoPorIdYUsuario(int idCarrito, int idUsuario) 
{
    for (size_t i = 0; i < carritosGlobales.size(); i++) {
        if (carritosGlobales[i].idCarrito == idCarrito && carritosGlobales[i].idUsuario == idUsuario) 
		{
            return (int)i;
        }
    }
    return -1;
}

int buscarCarritoActivo(int idCarrito, int idUsuario) {
    int idx = buscarCarritoPorIdYUsuario(idCarrito, idUsuario);
    if (idx != -1 && !carritosGlobales[idx].pagado) {
        return idx;
    }
    return -1;
}

void listarCarritosUsuario(int idUsuario) {
    cout << "\n--- CARROS DEL USUARIO ---\n";
    bool hay = false;

    for (size_t i = 0; i < carritosGlobales.size(); i++) {
        if (carritosGlobales[i].idUsuario == idUsuario) {
            hay = true;
            cout << "Carrito #" << carritosGlobales[i].idCarrito
                 << " | items: " << carritosGlobales[i].items.size()
                 << " | estado: " << (carritosGlobales[i].pagado ? "PAGADO" : "ACTIVO") << endl;
        }
    }
    if (!hay) cout << "Este usuario no tiene carritos.\n";
}

int crearCarrito(int idUsuario) {
    if (contarCarritosActivos(idUsuario) >= 2) {
        cout << "[!] El usuario ya tiene 2 carritos activos. Pague uno primero.\n";
        return -1;
    }

    Carrito c;
    c.idCarrito = proximoIdCarrito++;
    c.idUsuario = idUsuario;
    c.pagado = false;

    carritosGlobales.push_back(c);
    cout << "[+] Carrito #" << c.idCarrito << " creado correctamente.\n";
    return c.idCarrito;
}

void listarCarrito(int idCarrito, int idUsuario) {
    int idx = buscarCarritoPorIdYUsuario(idCarrito, idUsuario);
    if (idx == -1) {
        cout << "[!] Carrito no encontrado.\n";
        return;
    }

    Carrito& c = carritosGlobales[idx];
    Usuario* u = buscarUsuarioPorId(idUsuario);

    cout << "\n===== CARRITO #" << c.idCarrito << " =====\n";
    cout << "Usuario: " << (u ? u->nombre : "Desconocido") << endl;
    cout << "Estado : " << (c.pagado ? "PAGADO" : "ACTIVO") << endl;

    if (c.items.empty()) {
        cout << "(Carrito vacio)\n";
        return;
    }

    cout << left << setw(4) << "ID" << setw(22) << "Nombre" << setw(8) << "Cant." << setw(12) << "P.Unit." << "Subtotal\n";
    cout << string(55, '-') << endl;

    double subtotal = calcularSubtotal(c.items);
    for (size_t i = 0; i < c.items.size(); i++) {
        const ItemCarrito& it = c.items[i];
        double sub = it.producto.precio * it.cantidad;
        cout << left << setw(4) << it.producto.idProducto << setw(22) << it.producto.nombre << setw(8) << it.cantidad
             << "$" << setw(11) << fixed << setprecision(2) << it.producto.precio << "$" << fixed << setprecision(2) << sub << endl;
    }

    cout << string(55, '-') << endl;
    cout << right << setw(48) << "SUBTOTAL: $" << fixed << setprecision(2) << subtotal << endl;

    if (u && u->tieneBono50 && !c.pagado) {
        double descEstimado = subtotal * 0.5;
        cout << right << setw(48) << "BONO ACTIVO (50%): -$" << fixed << setprecision(2) << descEstimado << endl;
        cout << right << setw(48) << "SUBTOTAL CON DESCUENTO: $" << fixed << setprecision(2) << (subtotal - descEstimado) << endl;
    }
}

void agregarAlCarrito(int idCarrito, int idUsuario) {
    int idx = buscarCarritoActivo(idCarrito, idUsuario);
    if (idx == -1) {
        cout << "[!] Carrito invalido o ya pagado.\n";
        return;
    }

    mostrarCatalogo();
    int idProducto, cantidad;
    cout << "ID del producto: "; cin >> idProducto;
    cout << "Cantidad: "; cin >> cantidad;

    if (cantidad <= 0) {
        cout << "[!] La cantidad debe ser mayor que cero.\n";
        return;
    }

    Producto* p = buscarProductoPorId(idProducto);
    if (!p) {
        cout << "[!] Producto no encontrado.\n";
        return;
    }

    if (p->stock < cantidad) {
        cout << "[!] Stock insuficiente. Stock disponible: " << p->stock << endl;
        return;
    }

    for (size_t i = 0; i < carritosGlobales[idx].items.size(); i++) {
        if (carritosGlobales[idx].items[i].producto.idProducto == idProducto) {
            carritosGlobales[idx].items[i].cantidad += cantidad;
            p->stock -= cantidad;
            cout << "[+] Se actualizo la cantidad de " << p->nombre << " en el carrito.\n";
            return;
        }
    }

    ItemCarrito item;
    item.producto = *p;
    item.cantidad = cantidad;
    carritosGlobales[idx].items.push_back(item);
    p->stock -= cantidad;

    cout << "[+] Producto agregado correctamente al carrito.\n";
}


void generarOrdenTxt(const OrdenDeCompra& o) {
    ostringstream nombreArchivo;
    nombreArchivo << "orden_" << o.idOrden << "_usuario_" << o.usuario.idUsuario << ".txt";

    ofstream archivo(nombreArchivo.str().c_str());
    if (!archivo.is_open()) {
        cout << "[ERROR] No se pudo crear el archivo de orden.\n";
        return;
    }

    archivo << "========================================\n";
    archivo << "              ORDEN DE COMPRA\n";
    archivo << "========================================\n";
    archivo << "ID ORDEN : " << o.idOrden << "\n";
    archivo << "USUARIO  : " << o.usuario.idUsuario << "\n";
    archivo << "NOMBRE   : " << o.usuario.nombre << "\n";
    archivo << "DIRECCION: " << o.usuario.direccion << "\n";
    archivo << "----------------------------------------\n";
    archivo << "PRODUCTOS\n";
    archivo << left << setw(4) << "ID" << setw(22) << "Nombre" << setw(8) << "Cant." << setw(12) << "P.Unit." << "Subtotal\n";
    archivo << string(55, '-') << "\n";

    for (size_t i = 0; i < o.productos.size(); i++) {
        const ItemCarrito& it = o.productos[i];
        archivo << left << setw(4) << it.producto.idProducto << setw(22) << it.producto.nombre << setw(8) << it.cantidad
                << "$" << setw(11) << fixed << setprecision(2) << it.producto.precio << "$" << fixed << setprecision(2) << (it.producto.precio * it.cantidad) << "\n";
    }

    archivo << string(55, '-') << "\n";
    archivo << right << setw(45) << "SUBTOTAL : $" << fixed << setprecision(2) << o.subtotal << "\n";
    
    if (o.descuentoBono > 0) {
        archivo << right << setw(45) << "DESCUENTO BONO (50%): -$" << fixed << setprecision(2) << o.descuentoBono << "\n";
    }
    
    archivo << right << setw(45) << "IMPUESTOS : $" << fixed << setprecision(2) << o.impuestos << "\n";
    archivo << right << setw(45) << "ENVIO     : $" << fixed << setprecision(2) << o.envio << "\n";
    archivo << right << setw(45) << "TOTAL     : $" << fixed << setprecision(2) << o.total << "\n";
    archivo << "========================================\n";

    archivo.close();
    cout << "[+] Archivo de orden generado: " << nombreArchivo.str() << endl;
}

void pagarCarrito(int idCarrito, int idUsuario) {
    int idx = buscarCarritoActivo(idCarrito, idUsuario);
    if (idx == -1) {
        cout << "[!] Carrito invalido o ya pagado.\n";
        return;
    }

    Carrito& c = carritosGlobales[idx];
    if (c.items.empty()) {
        cout << "[!] El carrito esta vacio.\n";
        return;
    }

    Usuario* u = buscarUsuarioPorId(idUsuario);

    OrdenDeCompra o;
    o.idOrden = proximoIdOrden++;
    o.usuario = *u;
    o.productos = c.items;
    o.subtotal = calcularSubtotal(o.productos);
    
    if (u->tieneBono50) {
        o.descuentoBono = o.subtotal * 0.5;
        u->tieneBono50 = false; 
        cout << "[PROMO] ¡Bono del 50% de descuento aplicado con exito a esta compra!\n";
    } else {
        o.descuentoBono = 0.0;
    }

    double subtotalNeto = o.subtotal - o.descuentoBono;
    o.impuestos = subtotalNeto * 0.19;
    o.envio = (o.subtotal >= 500000.0) ? 0.0 : 15000.0;
    o.total = subtotalNeto + o.impuestos + o.envio;

    ordenes.push_back(o);
    c.pagado = true;

    cout << "\n[=] RESUMEN DE LA ORDEN #" << o.idOrden << "\n";
    cout << "    Subtotal : $" << o.subtotal << "\n";
    if (o.descuentoBono > 0) {
        cout << "    Descuento 50%: -$" << o.descuentoBono << "\n";
    }
    cout << "    Impuestos: $" << o.impuestos << "\n";
    cout << "    Envio    : $" << o.envio << "\n";
    cout << "    TOTAL    : $" << o.total << "\n";

    generarOrdenTxt(o);
}

void listarProductosMasVendidos() {
    cout << "\n===== PRODUCTOS MAS VENDIDOS =====\n";
    cout << left << setw(5) << "ID" << setw(22) << "Nombre" << setw(10) << "Cant.Vend" << "Ordenes en que se vendio\n";
    cout << string(65, '-') << endl;

    bool huboVentas = false;

    for (size_t i = 0; i < productos.size(); i++) {
        int idProd = productos[i].idProducto;
        int totalVendido = 0;
        string listaOrdenes = "";

        for (size_t j = 0; j < ordenes.size(); j++) {
            for (size_t k = 0; k < ordenes[j].productos.size(); k++) {
                if (ordenes[j].productos[k].producto.idProducto == idProd) {
                    totalVendido += ordenes[j].productos[k].cantidad;
                    listaOrdenes += "#" + enteroAString(ordenes[j].idOrden) + " ";
                }
            }
        }

        if (totalVendido > 0) {
            huboVentas = true;
            cout << left << setw(5) << idProd << setw(22) << productos[i].nombre << setw(10) << totalVendido << listaOrdenes << endl;
        }
    }

    if (!huboVentas) {
        cout << "Aun no se han realizado ventas en la plataforma.\n";
    }
}

void reporteComentariosPorFecha() {

    string fechaBuscar;

    cout << "\nIngrese la fecha (AAAA-MM-DD): ";
    cin >> fechaBuscar;

    int contador = 0;

    for (size_t i = 0; i < comentarios.size(); i++) {

        if (comentarios[i].fecha == fechaBuscar) {
            contador++;
        }
    }

    cout << "\n===== REPORTE DE COMENTARIOS =====\n";

    cout << left
         << setw(20) << "Fecha"
         << setw(20) << "Cantidad" << endl;

    cout << string(40, '-') << endl;

    cout << left
         << setw(20) << fechaBuscar
         << setw(20) << contador << endl;
}
void reportePrecioMaximoMinimo() 
{

    if (productos.empty()) {

        cout << "No existen productos cargados.\n";
        return;
    }

    double precioMax = productos[0].precio;
    double precioMin = productos[0].precio;

    string nombreMax = productos[0].nombre;
    string nombreMin = productos[0].nombre;

    for (size_t i = 1; i < productos.size(); i++) {

        if (productos[i].precio > precioMax) {

            precioMax = productos[i].precio;
            nombreMax = productos[i].nombre;
        }
		
        if (productos[i].precio < precioMin) {

            precioMin = productos[i].precio;
            nombreMin = productos[i].nombre;
        }
    }

    cout << "\n===== PRECIO MAXIMO Y MINIMO =====\n";

    cout << "\nProducto con PRECIO MAXIMO\n";
    cout << "Nombre : " << nombreMax << endl;
    cout << "Precio : $" << fixed << setprecision(2)
         << precioMax << endl;

    cout << "\nProducto con PRECIO MINIMO\n";
    cout << "Nombre : " << nombreMin << endl;
    cout << "Precio : $" << fixed << setprecision(2)
         << precioMin << endl;
}
void moduloGananciasYBonos() {
    cout << "\n===== MODULO DE GANANCIAS Y BONOS =====\n";
    
    double totalSubtotales = 0.0;
    double totalImpuestos = 0.0;
    double totalGanancias = 0.0;

    for (size_t i = 0; i < ordenes.size(); i++) {
        totalSubtotales += ordenes[i].subtotal;
        totalImpuestos += ordenes[i].impuestos;
        totalGanancias += ordenes[i].total;
    }

    cout << "[a] Historial Acumulado de la Plataforma:\n";
    cout << "    Subtotal Total  : $" << fixed << setprecision(2) << totalSubtotales << endl;
    cout << "    Impuestos Totales: $" << fixed << setprecision(2) << totalImpuestos << endl;
    cout << "    Total Neto      : $" << fixed << setprecision(2) << totalGanancias << endl;

    double x;
    cout << "\n[b] Ingrese el valor 'x' limite para otorgar bonos de descuento: ";
    cin >> x;

    cout << "\n--- Verificando ordenes que superan el valor $" << x << " ---\n";
    bool seAsignaronBonos = false;

    for (size_t i = 0; i < ordenes.size(); i++) {
        if (ordenes[i].total > x) {
            seAsignaronBonos = true;
            Usuario* u = buscarUsuarioPorId(ordenes[i].usuario.idUsuario);
            if (u) {
                if (!u->tieneBono50) {
                    u->tieneBono50 = true;
                    cout << " -> [BONO ASIGNADO] La Orden #" << ordenes[i].idOrden 
                         << " de " << u->nombre << " (Total: $" << ordenes[i].total 
                         << ") supero el valor X. ¡Se le otorgo 50% de descuento para su proximo carro!\n";
                } else 
				{
                    cout << " -> [INFO] El usuario " << u->nombre << " ya cuenta con un bono activo.\n";
                }
            }
        }
    }

    if (!seAsignaronBonos) {
        cout << "Ninguna orden de compra registrada ha superado el valor de $" << x << ".\n";
    }
}
 
/* void reporteCincoProductosMenorStock() {

    cout << "\n===== 5 PRODUCTOS CON MENOR STOCK =====\n";

    vector<Producto> copia = productos;

    sort (copia.begin(), copia.end(), [](const Producto& a, const Producto& b) 
		{
            return a.stock < b.stock;
        });

    cout << left
         << setw(12) << "ID"
         << setw(25) << "Nombre"
         << setw(10) << "Stock" << endl;

    cout << string(50, '-') << endl;

    int limite = (copia.size() < 5) ? copia.size() : 5;

    for (int i = 0; i < limite; i++) {
        cout << left
             << setw(12) << copia[i].idProducto
             << setw(25) << copia[i].nombre
             << setw(10) << copia[i].stock
             << endl;
    }
}*/

int login() {
    string correo, clave;
    cout << "Correo: "; cin >> correo;
    cout << "Contrasena: "; cin >> clave;

    for (size_t i = 0; i < usuarios.size(); i++) {
        if (usuarios[i].correoElectronico == correo && usuarios[i].contrasena == clave) {
            return usuarios[i].idUsuario;
        }
    }
    return -1;
}

void listarStockBajo() {
    cout << "\n--- PRODUCTOS CON STOCK MENOR A 15 ---\n";
    for (size_t i = 0; i < productos.size(); i++) {
        if (productos[i].stock < 15) {
            cout << productos[i].idProducto << ". " << productos[i].nombre << " - Stock: " << productos[i].stock << endl;
        }
    }
}

void menuCarrito(int idUsuario) {
    Usuario* u = buscarUsuarioPorId(idUsuario);
    if (!u) return;

    int opcion;
    do {
        cout << "\n----- MENU CARRITO | " << u->nombre << " -----\n";
        cout << "Carritos activos: " << contarCarritosActivos(idUsuario) << "/2\n";
        listarCarritosUsuario(idUsuario);

        cout << "\n1. Crear nuevo carrito\n";
        cout << "2. Ver productos de un carrito\n";
        cout << "3. Agregar producto a carrito\n";
        cout << "4. Pagar carrito (genera orden)\n";
        cout << "0. Volver\n";
        cout << "Opcion: "; cin >> opcion;

        if (opcion == 1) { crearCarrito(idUsuario); }
        else if (opcion == 2) { int idC; cout << "Numero de carrito: "; cin >> idC; listarCarrito(idC, idUsuario); }
        else if (opcion == 3) { int idC; cout << "Numero de carrito: "; cin >> idC; agregarAlCarrito(idC, idUsuario); }
        else if (opcion == 4) { int idC; cout << "Numero de carrito a pagar: "; cin >> idC; pagarCarrito(idC, idUsuario); }
    } while (opcion != 0);
}
void menuReportes() {

    int opcion;

    do {

        cout << "\n===== MENU DE REPORTES =====\n";
        cout << "1. Productos mas vendidos\n";
        cout << "2. 5 productos con menor stock\n";
        cout << "3. Cantidad de comentarios por fecha\n";
        cout << "4. Precio maximo y minimo de productos\n";
        cout << "0. Volver\n";
        cout << "Opcion: ";
        cin >> opcion;

        switch(opcion) {

            case 1:
                listarProductosMasVendidos();
                break;

            case 2:
               // reporteCincoProductosMenorStock();
                break;

            case 3:
                reporteComentariosPorFecha();
                break;

            case 4:
                reportePrecioMaximoMinimo();
                break;
        }

    } while(opcion != 0);
}
void menuPrincipal() 
{
    int opcion;
    do {
        cout << "\n========== TIENDA ONLINE ==========\n";
        cout << "1. Ingresar al modulo de compras (Carritos)\n";
        cout << "2. Ver catalogo completo de productos\n";
        cout << "3. Reportes\n";
        cout << "4. Ver productos con stock bajo (<15)\n";
        cout << "5. Ver ganancias y asignar bonos\n";
        cout << "0. Salir\n";
        cout << "Opcion: "; cin >> opcion;

        if (opcion == 1) {
            cout << "\n--- SELECCIONAR USUARIO DE SESION ---\n";
            for (size_t i = 0; i < usuarios.size(); i++) {
                cout << "  [" << usuarios[i].idUsuario << "] " << usuarios[i].nombre << endl;
            }
            cout << "ID de usuario para operar: "; int id; cin >> id;
            if(buscarUsuarioPorId(id) != NULL) {
                menuCarrito(id);
            } else {
                cout << "[!] ID Invalido.\n";
            }
        }
        else if (opcion == 2) { mostrarCatalogo(); }
        else if (opcion == 3) {menuReportes(); }
        else if (opcion == 4) { listarStockBajo(); }
        else if (opcion == 5) { moduloGananciasYBonos(); }
    } while (opcion != 0);
}

int main() {
    
    setlocale(LC_ALL, ""); 

    cargarUsuarios();
    cargarProductos();
    cargarComentarios();

    cout << "===== INICIO DE SESION AUTENTICADO =====\n";
    int idUsuario = login();

    if (idUsuario == -1) {
        cout << "[!] Usuario o contrasenia invalida.\n";
        return 0;
    }

    cout << "\nInicio de sesion exitoso.\n";
    menuPrincipal();

    cout << "\nHasta pronto!\n";
    return 0;
}
