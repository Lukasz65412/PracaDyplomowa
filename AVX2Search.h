
#ifndef PRACADYPLOMOWA_AVX2SEARCH_H
#define PRACADYPLOMOWA_AVX2SEARCH_H
#include "node.h"
#include "BloomFilterTree.h"

namespace avx2 {
        std::vector<Node *>* disjunctionSearch(std::vector<Node *> BFTroots[], const std::string key[], size_t len);
        std::vector<Node *>* conjunctionSearch(std::vector<Node *> nodes[], const std::string key[], size_t len);
        std::vector<Node *>* levelSearch(std::vector<Node *>* nodes, const std::string &key);
        int* MakeMask(std::string key, size_t BFSize, int k_hashes);
        std::vector<Node *> treeSearch(Node * BFTroots[], const std::string key[], size_t len);
}

#endif //PRACADYPLOMOWA_AVX2SEARCH_H
