#include "../Util/GlobalConstants.h"
#include <functional>

struct HashNode{
    AddressType left;
    AddressType right;
    AddressType bucketPosition;
    bool isLeaf;
    int nextDel;

    HashNode() = default;;
    HashNode(AddressType left, AddressType right){
        this->left = left;
        this->right = right;
        this->isLeaf = false;
        this->nextDel = 0;
    }
    HashNode(AddressType bucketPosition){
        this->bucketPosition = bucketPosition;
        this->isLeaf = true;
        this->nextDel = 0;
    }

    AddressType getBucketPosition() const {
        if(isLeaf)
            return bucketPosition;
        else throw invalid_argument("No es un nodo hoja");
    }

    void setBucketPosition(AddressType position){
        if(isLeaf)
            this->bucketPosition = position;
        else throw invalid_argument("No es un nodo hoja");
    }
};