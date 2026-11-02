
#ifndef PRACADYPLOMOWA_BLOOMFILTERTREE_H
#define PRACADYPLOMOWA_BLOOMFILTERTREE_H
#include "node.h"
#include "DBUtility.h"

class BloomFilterTree {
    public:
        Node* root = nullptr;
        std::vector<Node*> leafs;
        BloomFilterTree(int dsize, size_t mS, int num_hashes) : d(dsize), mSize(mS), num_hashes(num_hashes) {};
        ~BloomFilterTree() = default;
        void build(dbu::record table[], size_t tabSize, size_t offset);
        void buildLevel(std::vector<Node *> nodes);
        void addLeaf(BloomFilter* leaf, uint32_t skey, uint32_t ekey);
        void deleteTree(std::vector<Node *> nodes);
    private:
        int d;
        size_t mSize;
        int num_hashes;


};

#endif //PRACADYPLOMOWA_BLOOMFILTERTREE_H
