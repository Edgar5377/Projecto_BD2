#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
using namespace std;

class Airbnb2{
private:
    char id[8];
    char name[51];
    float lat;
    float lon;
    int price;
    int reviews;
public:
    int nextDel;
    char reference;
    Airbnb2() = default;
    Airbnb2(string id, string name, string lat, string lon,string price, int reviews){
        if(id.size() > 7 ) id = id.substr(0, 7);
        if(name.size() > 50) name = name.substr(0, 50);
        strcpy(this->id, id.c_str());
        strcpy(this->name, name.c_str());
        this->lat = stof(lat);
        this->lon = stof(lon);
        this->price = stoi(price);
        this->reviews = reviews;
        this->nextDel = 0;
    }
    void showData(){
        cout << "ID          : " << id << endl;
        cout << "Nombre      : " << name << endl;
        cout << "Latitud     : " << lat << endl;
        cout << "Longitud    : " << lon << endl;
        cout << "Precio      : " << price << endl;
        cout << "Reviews     : " << reviews << endl;
    }
    bool operator < (Airbnb2& other){
        return strcmp(this->id, other.id) < 0;
    }

    bool operator == (Airbnb2& other){
        return strcmp(this->id, other.id) == 0;
    }

    const char *getPrimaryKey() const {
        return id;
    }
    bool equalToKey(const char* key){
        return strcmp(this->id, key) == 0;
    }

    bool lessThanToKey(const char* key){
        return strcmp(this->id, key) < 0;
    }

    bool greaterThanToKey(const char* key){
        return strcmp(this->id, key) > 0;
    }

    bool lessThanOrEqualToKey(const char* key){
        return strcmp(this->id, key) <= 0;
    }

    bool greaterThanOrEqualToKey(const char* key){
        return strcmp(this->id, key) >= 0;
    }

    string getCodigo()const {
        return this->id;
    }

    string getNombre()const {
        return this->name;
    }

//    string getApellidos()const {
//        return this->apellidos;
//    }
//
//    string getCarrera()const {
//        return this->carrera;
//    }

    int getCiclo()const {
        return this->price;
    }

    float getMensualidad()const {
        return this->reviews;
    }

    void setCodigo(string id){
        strcmp(this->id, id.c_str());
    }

    void setNombre(string name){
        strcmp(this->id, name.c_str());
    }

//    void setApellidos(string apellido){
//        strcmp(this->apellidos, apellido.c_str());
//    }
//
//    void setCarrera(string carrera){
//        strcmp(this->carrera, carrera.c_str());
//    }

    void setCiclo(string price){
        this->price = stoi(price);
    }

    void setMensualidad(string reviews){
        this->reviews = stof(reviews);
    }

};

struct Airbnb2Hash{
    int operator()(const char* key){
        string str(key);
        hash<string> myHash;
        return myHash(str);
    }
};