
#ifndef PRACADYPLOMOWA_DBUTILITY_H
#define PRACADYPLOMOWA_DBUTILITY_H
#include <cstdint>
#include <string>

namespace dbu {
    typedef struct record {
        uint32_t key;
        std::string value;
    } record;

    record* gen(size_t n);
}

#endif //PRACADYPLOMOWA_DBUTILITY_H
