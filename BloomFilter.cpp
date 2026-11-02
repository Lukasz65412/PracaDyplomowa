
#include "BloomFilter.h"

#include <iostream>

#include "MurmurHash3.h"

BloomFilter::BloomFilter(size_t s, int k, uint32_t* arr, size_t len ,size_t startArr, size_t endArr) {
    m = s;
    k_hashes = k;
    array = arr;
    start = startArr;
    end = endArr;
    array_len = len;
        //new std::vector<uint32_t>((s-1)/32 + 1);

}
BloomFilter::~BloomFilter() = default;
void BloomFilter::insert(const std::string& key) {
    uint32_t res;
    for (int i=0; i<k_hashes; i++) {
        MurmurHash3_x86_32(key.c_str(), key.size(), i, &res);
        uint32_t bitT = res%m;
        array[start + bitT/32] |= (1 << (bitT%32));
    }
}

bool BloomFilter::find(std::string key) {
    bool found = true;
    uint32_t res;
    for (int i=0; i<k_hashes; i++) {
        MurmurHash3_x86_32(key.c_str(), key.size(), i, &res);
        uint32_t bitT = res%m;
        found &= (array[start+bitT/32] & (1 << bitT%32)) != 0;
    }
    return found;
}
bool BloomFilter::find(int * keyHashes) {
    bool found = true;
    for (int i=0; i<(m-1)/32 + 1; i++) {
        found &= (array[start+i] & keyHashes[i]) != keyHashes[i];
    }
    return found;
}

void BloomFilter::merge(BloomFilter* other) {
    for (size_t i=0; i<array_len; i++) {
        array[start+i] = array[start+i] | other->array[other->start+i];
    }
}


//10111010001111011
//00001000000010000
//11100111010111001