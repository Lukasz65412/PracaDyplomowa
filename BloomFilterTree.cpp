
#include "BloomFilterTree.h"

#include <iostream>
#include <ostream>

//Zakladamy ze klucze nie maja brakujaych wartosci

void BloomFilterTree::build(dbu::record table[], size_t tabSize, size_t offset) {
    leafs.resize((tabSize-1)/offset+1);
    size_t bfs = (mSize-1)/32 +1;
    uint32_t* bfarr = new uint32_t[leafs.size()*bfs]{0};
    for (size_t i=0; i<leafs.size(); i++) {
        leafs[i] = new Node(new BloomFilter(mSize,num_hashes,bfarr, leafs.size()*bfs,i*bfs,i*bfs+(bfs-1)),i*offset,i*offset+offset-1);
        leafs[i]->filter->isLeaf = true;
        for (size_t j=0; j<offset; j++) {
            leafs[i]->filter->insert(table[i*offset+j].value);
        }
    }
    buildLevel(leafs);
}

void BloomFilterTree::buildLevel(std::vector<Node *> nodes) {
    if (nodes.size() == 1) {
        root = nodes.front();
        return;
    }

    std::vector<Node *> parentLevel;
    size_t bs = (mSize-1)/32 +1;
    uint32_t* bfarr = new uint32_t[((nodes.size() - 1)/d + 1)*bs]{0};
    for (size_t i=0; i<nodes.size(); i+=d) {
        size_t end = std::min(nodes.size(), i+d);
        Node *p = new Node(new BloomFilter(mSize, num_hashes,bfarr, ((nodes.size() - 1)/d + 1)*bs ,i/d*bs, i/d*bs + bs - 1) ,nodes[i]->startKey, nodes[end-1]->endKey);

        for (size_t j=i; j<end; j++) {
            if (p->startKey > nodes[j]->startKey) {p->startKey = nodes[j]->startKey;}
            if (p->endKey < nodes[j]->endKey) {p->endKey = nodes[j]->endKey;}
            p->filter->merge(nodes[j]->filter);
            p->children.push_back(nodes[j]);
        }
        parentLevel.push_back(p);
    }
    buildLevel(parentLevel);
}

void BloomFilterTree::addLeaf(BloomFilter* leaf, uint32_t skey, uint32_t ekey) {
    leafs.push_back(new Node(leaf, skey,ekey));
}

void BloomFilterTree::deleteTree(std::vector<Node *> nodes) {
    for ( auto node : nodes) {
        std::vector<Node *> children = node->children;
        delete node;
        deleteTree(children);
    }
}
