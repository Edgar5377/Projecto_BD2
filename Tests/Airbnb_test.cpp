#include "../Entities/Airbnb2.h"
#include "../Hash/HashIndex.h"
#include <cstdio>

#include <fstream>
#include <sstream>
#include <utility>


using namespace std;
string indexFilePath = "indexFile.dat";
string bucketFilePath = "bucketFile.dat";
HashIndex<Airbnb2, const char *, Airbnb2Hash> test(indexFilePath, bucketFilePath);

template <class T>
void showAll(vector<T> &records){
    for(auto& r : records){
        cout << "--------------------------" << endl;
        r.showData();
    }
}

void deleteFiles(){
    remove(indexFilePath.c_str());
    remove(bucketFilePath.c_str());
}

void printTestStart(string s){
    string txt = "******** Test: " + s + " ********";
    cout << txt << endl;
}
void printTestEnd(){
    deleteFiles();
    cout << " ********************************" << endl;
}

void insertFromCSV(string name_csv){
    string nombreArchivo = std::move(name_csv);

    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombreArchivo << endl;
    }
    string linea;

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string id, name;
        string lat, lon,price;
        int  reviews;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, lat, ',');
        getline(ss, lon, ',');
        getline(ss, price, ',');
        ss >> reviews;

        Airbnb2 airbnb(id, name, lat, lon, price, reviews);
        test.insert(airbnb);
    }
//    auto output = test.searchInRange("0000000", "9999999");
//    showAll(output);
//    deleteFiles();
}





void insertTest(Airbnb2 airbnb){
    printTestStart("insert new airbnb");
    test.insert(airbnb);
}

void searchTest(const char *dato){
    printTestStart("search(\"" + std::string(dato) + "\")");
    vector<Airbnb2> output = test.search(dato);
    showAll<Airbnb2>(output);
}

void searchPerRangeTest(const char *dato1, const char *dato2){
    printTestStart("searchPerRange(" + std::string(dato1) + ", " + std::string(dato2) + ")");
    vector<Airbnb2> output = test.searchInRange(dato1, dato2);
    showAll<Airbnb2>(output);
}

void removeTest(const char *dato){
    printTestStart("remove(\"" + std::string(dato) + "\")");
    test.remove(dato);

}

int main(){
    //// Subir los datos
    insertFromCSV("dataset.csv");

    //// Insert
    Airbnb2 airbnb2("1234567", "Este es un nombre algo largo para poder hacer la prueba",  "11111","22222","999", 888);
    insertTest(airbnb2);

    //// Busqueda por KEY
//     searchTest("1007411");

//    //// Busqueda por rango
//    searchPerRangeTest("1004000","1007000");

    //// Remove KEY
    removeTest("1004098");
    cout << "+++++++++++++++++++++++++"<< endl;

//    //// Busqueda por rango
//    searchPerRangeTest("1004000","1007000");

    //// Mostrar todos
    auto output = test.searchInRange("0000000", "9999999");
    showAll(output);

    printTestEnd();
}