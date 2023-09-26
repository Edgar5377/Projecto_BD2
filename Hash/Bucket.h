#include "../Util/GlobalConstants.h"
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

template <class Register, class Key>
class Bucket{
private:
    Register records[BUCKETSIZE];
    int nextBucket;
    int count;
public:
    int nextDel;
    Bucket(): nextBucket(-1), nextDel(0), count(0) {}
    explicit Bucket(int nextBucket): nextBucket(nextBucket), nextDel(0), count(0) {}

    bool isFull(){
        return count == BUCKETSIZE;
    }

    void setNextBucket(int nextPosition){
        this->nextBucket = nextPosition;
    }

    int getNextBucket() const {
        return nextBucket;
    }

    void add(Register record){
        if(count < BUCKETSIZE) {
            records[count] = record;
            ++count;
        }else throw out_of_range("BUCKET LLENO");
    }

    void sortBucket(){
        sort(records, records+count, compareRecords);
    }

    static bool compareRecords(Register& r1, Register& r2){
        return r1 < r2;
    }
    void printAll(){
        cout << "******************" << endl;
        for(int i = 0; i < count; ++i){
            records[i].showData();
            cout << "******************" << endl;
        }
    }
    vector<Register> getRecords(){
        return vector<Register>(records, records+count);
    }

    void setRecords(vector<Register> newRecords){
        count = 0;
        for(auto& record : newRecords){
            records[count] = record;
            count++;
        }
    }

    bool empty(){
        return count == 0;
    }

    vector<Register> getAllDifferentRecords(Key key){
        vector<Register> output;
        for(auto& record : this->getRecords()){
            if(!record.equalToKey(key))
                output.push_back(record);
        }
        return output;
    }
};