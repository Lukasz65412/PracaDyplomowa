#include <format>
#include <iostream>

#include "AVX2Search.h"
#include "BloomFilter.h"
#include "BloomFilterTree.h"
#include "DBUtility.h"


int main() {
    BloomFilterTree tree(2,17,2);
    dbu::record *t = dbu::gen(100);
    tree.build(t,50,10);

    Node * tmp = tree.root;

    while (false && tmp != nullptr) {
        std::cout << tmp->startKey <<"-"<<tmp->endKey<<"-"<<tmp->filter->isLeaf<<"-"<<tmp->filter->array_len<<"-"<<tmp->filter->start<<"-"<<tmp->filter->start << std::endl;
        tmp = tmp->children[0];
    }
    for (int i = 0; i<tree.leafs.size(); i++) {
        std::cout<<i<<" - "<<tree.leafs[i]->startKey<<" - "<<tree.leafs[i]->endKey<<std::endl;
    }

    std::cout<<"FB "<<tree.leafs[1]->filter->array[1]<<std::endl;

    std::string key = "00000029";
    std::string key1 = "00000029";
    std::string keys[2]{key,key1};

    std::vector<Node *> *res;
    /*
    res = avx2::levelSearch(&tree.leafs, key);
    std::cout<<"levelSize: "<<res->size()<<std::endl;
    for (auto a : *res) {
        for (int i = a->startKey; i <= a->endKey; i++) {
            if (!key.compare(t[i].value)) {
                std::cout <<i << " - " << t[i].value << std::endl;
            }
        }
    }
    std::cout<<"FA "<<tree.leafs[1]->filter->array[1]<<std::endl;
    */
    std::vector<Node *> * trees = new std::vector<Node *>[2];
    trees[0].push_back(tree.root);
    trees[1].push_back(tree.root);
    res = avx2::conjunctionSearch(trees, keys,2);
    if (res != nullptr) {
        for (auto a : *res) {
            for (int i = a->startKey; i <= a->endKey; i++) {
                if (!keys[0].compare(t[i].value) || !keys[1].compare(t[i].value)) {
                    std::cout <<i << " - " << t[i].value << std::endl;
                }
            }
        }
    }
    //delete t;
    //tree.deleteTree(tree.root->children);
    //delete tree.root;
    return 0;
}
