#include <fstream>
#include <iostream>
#include <vector>
#include "../Util/GlobalConstants.h"
using namespace std;

template<class Register>
class FreeList{
private:
    string path;
    long numberOfAccessesToSecondaryMemory;

    void readHeader(int& header, fstream& file){
        file.seekg(0, ios::beg);
        file.read((char *)& header, sizeof(int));
        numberOfAccessesToSecondaryMemory += 1;
    }
    
    void writeHeader(int& header, fstream& file){
        file.seekp(0, ios::beg);
        file.write((char *)& header, sizeof(int));
        numberOfAccessesToSecondaryMemory += 1;
    }
    
    void readRegister(Register& record, AddressType position, fstream& file){
        file.seekg(position*sizeof(Register)+sizeof(int), ios::beg);
        file.read((char*)& record, sizeof(Register));
        numberOfAccessesToSecondaryMemory += 1;
    }
    
    void writeRegister(Register& record, AddressType position, fstream& file){
        file.seekp(position*sizeof(Register)+sizeof(int), ios::beg);
        file.write((char*)& record, sizeof(Register));
        numberOfAccessesToSecondaryMemory += 1;
    }
    
    int numberOfRecords(fstream& file){
        if(file.is_open()){
            file.seekg(0, ios::end);
            int n = ((int) file.tellg() - sizeof(int)) / sizeof(Register);
            file.seekg(0, ios::beg);
            return n;
        }return 0;
    }
public:
    FreeList() = default;
    explicit FreeList(const string& path) {
        this->path = path;
        this->numberOfAccessesToSecondaryMemory = 0;
    }
    void setPath(string path){
        this->path = path;
    }
    int getNumberOfRecords(){
        fstream file(this->path.c_str(), ios::binary | ios::in);
        return numberOfRecords(file);
    }
    vector<Register> load(){
        vector<Register> records;
        fstream file(this->path.c_str(), ios::binary | ios::in);
        if(file.is_open()) {
            Register record;
            for(int i = 0; i < numberOfRecords(file); ++i){
                readRegister(record, i, file);
                if(record.nextDel == 0) records.push_back(record);
            }
        }else cout << "No se pudo abrir el archivo - LOAD\n";
        file.close();
        return records;
    }

    int add(Register record){
        fstream file(this->path.c_str(), ios::binary | ios::in);
        if(file.is_open()){
            file.close();
            file.open(this->path.c_str(), ios::binary | ios::in | ios::out);
            int header;
            readHeader(header, file);
            int recordPosition;
            if(header == -1) {
                recordPosition = numberOfRecords(file);
                writeRegister(record, recordPosition, file);
            }
            else{ // ESTRATEGIA LIFO
                Register recordDeleted;
                readRegister(recordDeleted, header, file);
                recordPosition = header;
                header = recordDeleted.nextDel;
                writeHeader(header, file);
                writeRegister(record, recordPosition, file);
            }
            file.close();
            return recordPosition;
        }
        else{
            file.close();
            file.open(this->path.c_str(), ios::binary | ios::out);
            int header = -1;
            writeHeader(header, file);
            writeRegister(record, 0, file);
            return 0;
        }
    }

    Register readRecord(int position){
        fstream file(this->path.c_str(), ios::binary | ios::in);
        Register record;
        if(numberOfRecords(file) == 0) {
            cout << "ARCHIVO VACIO - readRecord()" << endl;
            return record;
        }
        if(position >= 0 && position < numberOfRecords(file)){
            readRegister(record, position, file);
            file.close();
            return record;
        }else throw out_of_range("Indice incorrecto");
    }

    void writeRecord(AddressType position, Register& record){
        fstream file(this->path.c_str(), ios::binary | ios::in);
        if(numberOfRecords(file) == 0){
            cout << "archivo vacio - writeRecord()" << endl;
            return;
        }
        if(position >= 0 && position < numberOfRecords(file)){
            file.close();
            file.open(this->path.c_str(), ios::binary | ios::out | ios::in);
            writeRegister(record, position, file);
            file.close();
        }else{
            cout << "indice incorrecto - writeRecord()" << endl;
            return;
        }
    }

    void deleteRecord(int position){
        fstream file(this->path.c_str(), ios::binary | ios::in | ios::out);
        if(numberOfRecords(file) == 0) {
            cout << "Archivo vacio - deleteRecord()" << endl;
            return;
        }

        if(position >= 0 && position < numberOfRecords(file)) {
            Register record;
            int header;
            readHeader(header, file);
            writeHeader(position, file);
            readRegister(record, position, file);
            record.nextDel = header;
            writeRegister(record, position, file);
            file.close();
        }else throw out_of_range("Indice incorrecto");
    }

    long getNumberOfAccessesToSecondaryMemory(){
        return this->numberOfAccessesToSecondaryMemory;
    }
};