
#ifndef PRACADYPLOMOWA_BLOOMFILTER_H
#define PRACADYPLOMOWA_BLOOMFILTER_H
#include <cstdint>
#include <string>
#include <vector>

class BloomFilter {
    public:
        uint32_t id = 0;
        bool isLeaf = false;
        size_t m;
        int k_hashes;
        size_t start;
        size_t end;
        uint32_t* array;
        size_t array_len;
        BloomFilter(size_t m, int k, uint32_t* arr, size_t len ,size_t startArr, size_t endArr);
        ~BloomFilter();
        void insert(const std::string& key);
        bool find(std::string key);
        bool find(int* keyHashes);
        void merge(BloomFilter* other);
};

#endif //PRACADYPLOMOWA_BLOOMFILTER_H
