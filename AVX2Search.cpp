#include "AVX2Search.h"
#include <immintrin.h>
#include <iostream>

#include "MurmurHash3.h"

alignas(32) static uint32_t operationArr[8];
alignas(32) static uint32_t testArr[8];
alignas(32) static uint32_t operationMaskArr[8];
static size_t iter;
static __m256i oper;
static __m256i operMask;

//zakladamy ze kluzce sa posortowane

int* avx2::MakeMask(std::string key, size_t BFSize, int k_hashes) {
    int* ret = new int[(BFSize-1)/32 + 1]{0};
    uint32_t res;
    for (int i=0; i<k_hashes; i++) {
        MurmurHash3_x86_32(key.c_str(), key.size(), i, &res);
        uint32_t bitT = res%BFSize;
        ret[bitT/32] |= (1 << bitT%32);
    }
    return ret;
}
//Koniunkcja i dysujnckja na razie dla dwoch kluczy - musze pomyslec jak dla wiekszej liczby kluczy to optymalnie zaladowac do AVX
//Zakladamy tez ze przeszukiwane tabele maja takie same rozmiary i klucze
std::vector<Node *>* avx2::disjunctionSearch(std::vector<Node *> BFTroots[], const std::string key[], size_t len = 2) {
    int * masks[len];
    for (size_t i=0; i<len; i++) {
        masks[i] = MakeMask(key[i], BFTroots[i].at(0)->filter->m ,BFTroots[i].at(0)->filter->k_hashes);
    }
    std::cout<<"Mask Created"<<std::endl;
    std::vector<Node *> * result = new std::vector<Node *>[2];

    size_t BF_Array_Size = (BFTroots[0].at(0)->filter->m-1)/32 + 1;

    size_t offset = 0;
    size_t maskSize = (BFTroots[0].at(0)->filter->m-1)/32 + 1;
    uint32_t isFound[2] = {0xFFFFFFFF,0xFFFFFFFF};


    for (size_t i=0; i<std::max(BFTroots[0].at(0)->filter->array_len, BFTroots[1].at(0)->filter->array_len); i+=4) {
        //size_t end = std::min(len,i+4);

        offset = i % maskSize;
        for (size_t j=0; j<4; j++) {
            operationMaskArr[j] = masks[0][(j+offset)%maskSize];
            operationMaskArr[j+4] = masks[1][(j+offset)%maskSize];
        }

        oper = _mm256_loadu2_m128i(reinterpret_cast<__m128i const *>(BFTroots[0].at(0)->filter->array+i), reinterpret_cast<__m128i const *>(BFTroots[1].at(0)->filter->array+i));
        _mm256_store_si256(reinterpret_cast<__m256i *>(testArr), oper);
        operMask = _mm256_loadu2_m128i(reinterpret_cast<__m128i const *>(operationMaskArr),reinterpret_cast<__m128i const *>(operationMaskArr+4));
        oper = _mm256_and_si256(oper, operMask);
        oper = _mm256_cmpeq_epi32(oper, operMask);
        _mm256_store_si256(reinterpret_cast<__m256i *>(operationArr), oper);
        for (size_t j=0; j<4; j++) {
            std::cout << operationArr[j] << " - " << operationMaskArr[j] << " - "<< testArr[j] << " - " << std::endl;
            std::cout << operationArr[j+4] << " - " << operationMaskArr[j+4] << " - "<< testArr[j+4] << " - " << std::endl;
            isFound[0] &= operationArr[j];
            isFound[1] &= operationArr[j+4];
            if ((j+offset)%maskSize == maskSize - 1) {
                std::cout<<"Check"<<std::endl;
                if (isFound[0] != 0 && i+j < BF_Array_Size) {
                    result[0].push_back(BFTroots[0].at((i+j)/BF_Array_Size));
                    std::cout<<"Found"<<std::endl;
                }
                if (isFound[1] != 0 && i+j < BF_Array_Size) {
                    result[1].push_back(BFTroots[1].at((i+j)/BF_Array_Size));
                    std::cout<<"Found"<<std::endl;
                }
                isFound[0] = 0xFFFFFFFF;
                isFound[1] = 0xFFFFFFFF;
            }
        }
    }
    std::cout<<"LevelSearchCompleted"<<std::endl;

    if (result[0].empty() && result[1].empty()) {
        std::cout<<"Not Found"<<std::endl;
        return nullptr;
    }
    std::vector<Node *> * result1 = new std::vector<Node *>[2];
    if (result[0].empty() || result[0][0]->filter->isLeaf) {
        result1[0] = result[0];
    }
    else {
        for (size_t j=0; j<result[0].size(); j++) {
            result1[0].insert(result1[0].end(),result[0].at(j)->children.begin(),result[0].at(j)->children.end());
        }
    }
    if (result[1].empty() || result[1][0]->filter->isLeaf) {
        result1[1] = result[1];
    }
    else {
        for (size_t j=0; j<result[1].size(); j++) {
            result1[1].insert(result1[1].end(),result[1].at(j)->children.begin(),result[1].at(j)->children.end());
        }
    }
    if (result[1][0]->filter->isLeaf && result[0][0]->filter->isLeaf) {
        return result1;
    }

    std::cout<<"Recursion"<<std::endl;
    return avx2::disjunctionSearch(result1, key, len);
}

std::vector<Node *>* avx2::conjunctionSearch(std::vector<Node *> BFTroots[], const std::string key[], size_t len = 2) {
    int * masks[len];
    for (size_t i=0; i<len; i++) {
        masks[i] = MakeMask(key[i], BFTroots[i].at(0)->filter->m ,BFTroots[i].at(0)->filter->k_hashes);
    }
    std::cout<<"Mask Created"<<std::endl;
    std::vector<Node *> * result = new std::vector<Node *>[2];

    size_t BF_Array_Size = (BFTroots[0].at(0)->filter->m-1)/32 + 1;

    size_t offset = 0;
    size_t maskSize = (BFTroots[0].at(0)->filter->m-1)/32 + 1;
    uint32_t isFound[2] = {0xFFFFFFFF,0xFFFFFFFF};

    std::cout<<"ARL : "<<BFTroots[0].at(0)->filter->array_len<<" - "<< BFTroots[1].at(0)->filter->array_len << std::endl;
    for (size_t i=0; i<std::max(BFTroots[0].at(0)->filter->array_len, BFTroots[1].at(0)->filter->array_len); i+=4) {
        //size_t end = std::min(len,i+4);

        offset = i % maskSize;
        for (size_t j=0; j<4; j++) {
            operationMaskArr[j] = masks[0][(j+offset)%maskSize];
            operationMaskArr[j+4] = masks[1][(j+offset)%maskSize];
        }
        _mm256_store_si256(reinterpret_cast<__m256i *>(testArr), oper);
        std::cout << operationArr[0] << " - " << operationMaskArr[0] << " - "<< testArr[0] << " - " << std::endl;
        std::cout << operationArr[4] << " - " << operationMaskArr[4] << " - "<< testArr[4] << " - " << std::endl;
        oper = _mm256_loadu2_m128i(reinterpret_cast<__m128i const *>(BFTroots[0].at(0)->filter->array+i), reinterpret_cast<__m128i const *>(BFTroots[1].at(0)->filter->array+i));
        _mm256_store_si256(reinterpret_cast<__m256i *>(testArr), oper);
        operMask = _mm256_loadu2_m128i(reinterpret_cast<__m128i const *>(operationMaskArr),reinterpret_cast<__m128i const *>(operationMaskArr+4));
        oper = _mm256_and_si256(oper, operMask);
        oper = _mm256_cmpeq_epi32(oper, operMask);
        _mm256_store_si256(reinterpret_cast<__m256i *>(operationArr), oper);
        for (size_t j=0; j<4; j++) {
            std::cout << operationArr[j] << " - " << operationMaskArr[j] << " - "<< testArr[j] << " - " << std::endl;
            std::cout << operationArr[j+4] << " - " << operationMaskArr[j+4] << " - "<< testArr[j+4] << " - " << std::endl;
            isFound[0] &= operationArr[j];
            isFound[1] &= operationArr[j+4];

            if ((j+offset)%maskSize == maskSize - 1) {
                std::cout<<"Check"<<std::endl;
                if (isFound[0] != 0 && i+j < BF_Array_Size) {
                    result[0].push_back(BFTroots[0].at((i+j)/BF_Array_Size));
                    std::cout<<"Found"<<std::endl;
                }
                if (isFound[1] != 0 && i+j < BF_Array_Size) {
                    result[1].push_back(BFTroots[1].at((i+j)/BF_Array_Size));
                    std::cout<<"Found"<<std::endl;
                }
                isFound[0] = 0xFFFFFFFF;
                isFound[1] = 0xFFFFFFFF;
            }
        }
    }
    std::cout<<"LevelSearchCompleted"<<std::endl;
    std::cout<<"SIZ0 "<<result[0].size()<<" - "<<result[1].size()<<std::endl;
    auto fresult = new bool[std::max(result[0].size(),result[1].size())][2]{0};
    size_t iter1=0,iter2=0;
    while (iter1 < result[0].size() && iter2 < result[1].size()) {
        if (result[0].at(iter1)->startKey <= result[1].at(iter2)->endKey && result[1].at(iter2)->startKey <= result[0].at(iter1)->endKey) {
            fresult[iter1][0] = 1;
            fresult[iter2][1] = 1;
            std::cout<<"R1 "<<result[0].at(iter1)->startKey << " - "<< result[0].at(iter1)->endKey<<std::endl;
            std::cout<<"R2 "<<result[1].at(iter2)->startKey << " - "<< result[1].at(iter2)->endKey<<std::endl;
        }
        if (result[1].at(iter2)->endKey < result[0].at(iter1)->endKey) {
            iter2 ++;
        }
        else {
            iter1++;
        }
    }
    if (result[0].empty() || result[1].empty()) {
        std::cout<<"Not Found"<<std::endl;
        return nullptr;
    }
    std::cout<<"Aggregated"<<std::endl;
    std::cout<<result[0].at(0)->filter->isLeaf<<std::endl;
    std::cout<<result[1].at(0)->filter->isLeaf<<std::endl;
    if (result[0].at(0)->filter->isLeaf) {
        std::vector<Node *> * res1 = new std::vector<Node *>();
        for (size_t j=0; j<result[0].size(); j++) {
            if (fresult[j][0]) {
                res1->push_back(result[0].at(j));
            }
        }
        return res1;
    }
    if (result[1].at(0)->filter->isLeaf) {
        std::vector<Node *> * res1 = new std::vector<Node *>();
        for (size_t j=0; j<result[1].size(); j++) {
            if (fresult[j][1]) {
                res1->push_back(result[1].at(j));
            }
        }
        return res1;
    }
    std::cout<<"Not leaf"<<std::endl;
    std::vector<Node *> * result1 = new std::vector<Node *>[2];
    std::cout<<"SIZ1 "<<result[0].size()<<" - "<<result[1].size()<<std::endl;
    for (size_t j=0; j<std::max(result[0].size(),result[1].size()); j++) {
        if (fresult[j][0]) {
            result1[0].insert(result1[0].end(),result[0].at(j)->children.begin(),result[0].at(j)->children.end());
        }
        if (fresult[j][1]) {
            result1[1].insert(result1[1].end(),result[1].at(j)->children.begin(),result[1].at(j)->children.end());
        }
    }
    std::cout<<"Recursion"<<std::endl;
    return avx2::conjunctionSearch(result1, key, len);
}



std::vector<Node *>* avx2::levelSearch(std::vector<Node *>* nodes, const std::string &key) {
    int* mask = MakeMask(key, nodes->at(0)->filter->m, nodes->at(0)->filter->k_hashes);
    size_t BF_Array_Size = (nodes->at(0)->filter->m-1)/32 + 1;
    std::vector<Node *> *result = new std::vector<Node *>();
    size_t offset = 0;
    size_t maskSize = (nodes->at(0)->filter->m-1)/32 + 1;
    uint32_t isFound = 0xFFFFFFFF;
    for (size_t i=0; i<nodes->at(0)->filter->array_len; i+=8) {
        offset = i % maskSize;
        for (size_t j=0; j<8; j++) {
            operationMaskArr[j] = mask[(j+offset)%maskSize];
        }
        oper = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(nodes->at(0)->filter->array+i));
        _mm256_store_si256(reinterpret_cast<__m256i *>(testArr), oper);
        operMask = _mm256_load_si256(reinterpret_cast<__m256i const *>(operationMaskArr));
        oper = _mm256_and_si256(oper, operMask);
        oper = _mm256_cmpeq_epi32(oper, operMask);
        _mm256_store_si256(reinterpret_cast<__m256i *>(operationArr), oper);
        _mm256_store_si256(reinterpret_cast<__m256i *>(operationMaskArr), operMask);
        for (size_t j=0; j<8; j++) {
            //std::cout << operationArr[j] << " - " << operationMaskArr[j] << " - "<< testArr[j] << " - " << (j<5 ? nodes->at(j)->filter: nullptr ) <<std::endl;
            isFound &= operationArr[j];
            if ((j+offset)%maskSize == maskSize - 1) {
                if (isFound != 0 && i+j < nodes->at(0)->filter->array_len) {
                    result->push_back(nodes->at((i+j)/BF_Array_Size));
                }
                isFound = 0xFFFFFFFF;
            }
        }

    }
    /*
    for (size_t i = 0; i < nodes.size();i++) {
        for (size_t j = 0; j < BF_Array_Size; j++) {
            iter = (i*BF_Array_Size+j)%8;
            operationArr[iter] = nodes[i]->filter.array->at(j);
            operationMaskArr[iter] = mask[j];

            if (iter == 7) {
                oper = _mm256_load_si256(reinterpret_cast<__m256i const *>(operationArr));
                operMask = _mm256_load_si256(reinterpret_cast<__m256i const *>(operationMaskArr));
                oper = _mm256_and_si256(oper, operMask);
                oper = _mm256_cmpeq_epi32(oper, operMask);
                _mm256_store_si256(reinterpret_cast<__m256i *>(operationArr), oper);
                for (size_t kl = 0; kl <= iter; kl++) {
                    if (operationArr[kl] != 0) {
                        result.push_back(nodes[i]);
                    }
                }
            }
        }

        if (nodes[i]->filter.find(key)) {
            result.push_back(nodes[i]);
        }
    }
    */
    delete mask;
    return result;
}

static bool exists(BloomFilter bf, std::string key) {
    bool found = true;
    uint32_t res;
    for (int i=0; i<bf.k_hashes; i++) {
        MurmurHash3_x86_32(key.c_str(), key.size(), i, &res);
        uint32_t bitT = res%bf.m;
        found &= (bf.array[bitT/32] & (1 << bitT%32)) != 0;
    }
    return found;
}