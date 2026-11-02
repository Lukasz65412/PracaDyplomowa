
#ifndef PRACADYPLOMOWA_NODE_H
#define PRACADYPLOMOWA_NODE_H
#include <cstdint>
#include <vector>

#include "BloomFilter.h"

class Node {
public:
    std::vector<Node*> children;
    BloomFilter* filter;
    uint32_t startKey;
    uint32_t endKey;

    Node(BloomFilter* f, uint32_t start, uint32_t end) : filter(f), startKey(start), endKey(end) {}
    ~Node() {return;};
};

#endif //PRACADYPLOMOWA_NODE_H
