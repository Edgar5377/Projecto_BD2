
#include <functional>
#include <bitset>
#include "FreeList.h"
#include "HashNode.h"
#include "Bucket.h"
using namespace std;

template <class Register, class Key, class Hash = hash<Key> > 
class HashIndex{
private:
    int MAXHEIGHT;
    typedef Bucket<Register, Key> myBucket;
    typedef bitset<LIMIT> myBitset;
    FreeList<myBucket> bucketFile;
    FreeList<HashNode> indexFile;
    Hash myHash;
public:
    HashIndex()= default;
    HashIndex(const string& indexFilePath, const string& bucketFilePath, int MAXHEIGHT_ = 3){
        this->bucketFile.setPath(bucketFilePath);
        this->indexFile.setPath(indexFilePath);
        if(MAXHEIGHT_ > LIMIT - 1) throw invalid_argument("Invalid MAXHEIGHT, must to be < " + to_string(LIMIT));
        this->MAXHEIGHT = MAXHEIGHT_;
    }
    void initializeFirstValues(){
        HashNode root(1, 2), v1(0), v2(1);
        myBucket b1, b2;
        this->bucketFile.add(b1);
        this->bucketFile.add(b2);
        this->indexFile.add(root);
        this->indexFile.add(v1);
        this->indexFile.add(v2);
    }

    void insertAll(vector<Register> records){
        if(bucketFile.getNumberOfRecords() == 0)
            initializeFirstValues();

        for(auto& record : records){
            insert(record);
        }
    }

    AddressType searchNode(AddressType nodePosition, myBitset key, int& height){
        HashNode node = indexFile.readRecord(nodePosition);
        if(node.isLeaf)
            return nodePosition;
        else{
            if(key[height] == 0) return searchNode(node.left, key, ++height);
            else return searchNode(node.right, key, ++height);
        }
    }
    vector<Register> search(Key searchKey){
        myBitset hashKey = myHash(searchKey);
        int height = 0;
        AddressType currentNodePosition = searchNode(ROOT, hashKey, height);
        HashNode currentNode = indexFile.readRecord(currentNodePosition);
        AddressType currentBucketPosition = currentNode.getBucketPosition();
        vector<Register> output;
        while(currentBucketPosition != -1){
            myBucket bucket = bucketFile.readRecord(currentBucketPosition);
            for(auto& r: bucket.getRecords()){
                if(r.equalToKey(searchKey))
                    output.push_back(r);
            }
            currentBucketPosition = bucket.getNextBucket();
        }
        return output;
    }

    vector<Register> searchInRange(Key beginKey, Key endKey){
        int bucketsNumber = bucketFile.getNumberOfRecords();
        vector<Register> output;
        for(int i = 0; i < bucketsNumber; ++i){
            myBucket bucket = bucketFile.readRecord(i);
            for(auto& r : bucket.getRecords()){
                if(r.lessThanOrEqualToKey(endKey) && r.greaterThanOrEqualToKey(beginKey))
                    output.push_back(r);
            }
        }
        sort(output.begin(), output.end(), myBucket::compareRecords);
        return output;
    }

    void insert(Register record){
        if(bucketFile.getNumberOfRecords() == 0)
            initializeFirstValues();
        int height = 0;
        myBitset hashKey = myHash(record.getPrimaryKey());
        AddressType currentNodePosition = searchNode(ROOT, hashKey, height);
        HashNode currentNode = indexFile.readRecord(currentNodePosition);
        AddressType currentBucketPosition = currentNode.getBucketPosition();
        myBucket bucket = bucketFile.readRecord(currentBucketPosition);
        if(!bucket.isFull()){
            bucket.add(record);
            bucket.sortBucket();
            bucketFile.writeRecord(currentBucketPosition, bucket);
        }else
            split(record, height, currentNodePosition, currentNode, currentBucketPosition, bucket);
    }

    void split(Register& record, int height, int currentNodePosition, HashNode& currentNode, int currentBucketPosition,
               myBucket& bucket) {
        if(height == MAXHEIGHT){
            myBucket bucketToLink;
            bucketToLink.add(record);
            bucketToLink.setNextBucket(currentBucketPosition);
            currentNode.bucketPosition = bucketFile.add(bucketToLink);
            indexFile.writeRecord(currentNodePosition, currentNode);
            return;
        }
        vector<Register> records = bucket.getRecords();
        records.push_back(record);
        myBucket bucket1, bucket2;
        bool splitAgain = false;
        bool splitToLeft = false;
        for(auto& r : records){
            myBitset hashKey = myHash(r.getPrimaryKey());
            if(hashKey[height] == 0){
                if(!bucket1.isFull()) bucket1.add(r);
                else{
                    splitAgain = true;
                    splitToLeft = true;
                }
            }
            else{
                if(!bucket2.isFull()) bucket2.add(r);
                else splitAgain = true;
            }
        }
        bucketFile.deleteRecord(currentBucketPosition);
        bucket1.sortBucket();
        bucket2.sortBucket();
        AddressType bucketPosition1 = bucketFile.add(bucket1);
        AddressType bucketPosition2 = bucketFile.add(bucket2);
        HashNode leftNode(bucketPosition1), rightNode(bucketPosition2);
        currentNode.left = indexFile.add(leftNode);
        currentNode.right = indexFile.add(rightNode);
        currentNode.isLeaf = false;
        indexFile.writeRecord(currentNodePosition, currentNode);
        if(splitAgain){
            AddressType splitNodePosition, splitBucketPosition;
            HashNode splitNode;
            if(splitToLeft) {
                splitNodePosition = currentNode.left;
                splitNode = indexFile.readRecord(splitNodePosition);
                split(record, height+1, splitNodePosition, splitNode, bucketPosition1, bucket1);
            }else{
                splitNodePosition = currentNode.right;
                splitNode = indexFile.readRecord(splitNodePosition);
                split(record, height+1, splitNodePosition, splitNode, bucketPosition2, bucket2);
            }
        }
    }
    void remove(Key key){
        int height = 0;
        removeUtil(ROOT, key, myHash(key), height);
    }
    void removeUtil(int nodePosition, Key key, myBitset hashKey, int& height) {
        HashNode currentNode = indexFile.readRecord(nodePosition);
        if (currentNode.isLeaf) {
            deleteRecordInBucket(key, currentNode, nodePosition);
            return;
        }
        if (hashKey[height] == 0) removeUtil(currentNode.left, key, hashKey, ++height);
        else removeUtil(currentNode.right, key, hashKey, ++height);

        if(nodePosition == ROOT) return;
        HashNode left = indexFile.readRecord(currentNode.left);
        HashNode right = indexFile.readRecord(currentNode.right);

        if (left.isLeaf && right.isLeaf) {
            myBucket bucket1 = bucketFile.readRecord(left.bucketPosition);
            myBucket bucket2 = bucketFile.readRecord(right.bucketPosition);
            if (bucket1.empty() && bucket2.empty()) {
                bucketFile.deleteRecord(left.bucketPosition);
                bucketFile.deleteRecord(right.bucketPosition);
                indexFile.deleteRecord(currentNode.left);
                indexFile.deleteRecord(currentNode.right);
                myBucket newBucket;
                currentNode.isLeaf = true;
                currentNode.bucketPosition = bucketFile.add(newBucket);
                indexFile.writeRecord(nodePosition, currentNode);
            }
        }
    }

    void deleteRecordInBucket(Key key, HashNode &currentNode, AddressType nodePosition) {
        AddressType currentBucketPosition = currentNode.bucketPosition;
        myBucket bucket = bucketFile.readRecord(currentBucketPosition);
        auto differentRecords = bucket.getAllDifferentRecords(key);
        bucket.setRecords(differentRecords);
        bucketFile.writeRecord(currentBucketPosition, bucket);
        if(bucket.empty() && bucket.getNextBucket() != -1){
            currentNode.bucketPosition = bucket.getNextBucket();
            bucketFile.deleteRecord(currentBucketPosition);
            indexFile.writeRecord(nodePosition, currentNode);
            deleteRecordInBucket(key, currentNode, nodePosition);
        }
    }

    int getMaxHeight(){
        return MAXHEIGHT;
    }

    long getNumberOfAccessesToSecondaryMemory(){
        return this->bucketFile.getNumberOfAccessesToSecondaryMemory() +
                this->indexFile.getNumberOfAccessesToSecondaryMemory();
    }
};