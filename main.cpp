#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <string.h>

using namespace std;

class Airbnb2 {
private:
    char id[8];
    char name[51];
    long long lat;
    long long lon;
    int price;
    int reviews;

public:
    // Constructor de Airbnb2
    Airbnb2(const string& _id, const string& _name, long long _lat, long long _lon, int _price, int _reviews) {
        strncpy(id, _id.c_str(), sizeof(id));
        strncpy(name, _name.c_str(), sizeof(name));
        lat = _lat;
        lon = _lon;
        price = _price;
        reviews = _reviews;
    }

    // Método para imprimir los detalles de Airbnb2
    void imprimirDetalles() {
        cout << "ID: " << id << endl;
        cout << "Nombre: " << name << endl;
        cout << "Latitud: " << lat << endl;
        cout << "Longitud: " << lon << endl;
        cout << "Precio: " << price << endl;
        cout << "Resenas: " << reviews << endl;
    }
};

int main() {
    // Nombre del archivo CSV a leer
    string nombreArchivo = "dataset.csv";

    // Abre el archivo CSV
    ifstream archivo(nombreArchivo);

    // Verifica si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombreArchivo << endl;
        return 1;
    }

    // Vector para almacenar objetos Airbnb2
    vector<Airbnb2> airbnbs;

    // Variables para almacenar y procesar los datos
    string linea;

    // Lee el archivo línea por línea
    while (getline(archivo, linea)) {
        // Usa un stringstream para dividir la línea en campos separados por comas
        stringstream ss(linea);
        string id, name;
        long long lat, lon;
        int price, reviews;

        // Extrae los campos
        getline(ss, id, ',');
        getline(ss, name, ',');
        ss >> lat;
        ss.ignore(); // Ignora la coma después de lat
        ss >> lon;
        ss.ignore(); // Ignora la coma después de lon
        ss >> price;
        ss.ignore(); // Ignora la coma después de price
        ss >> reviews;

        // Crea un objeto Airbnb2 y agrégalo al vector
        Airbnb2 airbnb(id, name, lat, lon, price, reviews);
        airbnbs.push_back(airbnb);
    }

    // Cierra el archivo
    archivo.close();

    // Muestra los detalles de Airbnb2
    for (Airbnb2 airbnb : airbnbs) {
        airbnb.imprimirDetalles();
        cout << endl;
    }

    return 0;
}
