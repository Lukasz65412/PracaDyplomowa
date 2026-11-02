

#include "DBUtility.h"

#include <format>

dbu::record* dbu::gen(size_t n) {
    dbu::record* r = new dbu::record[n];
    for (size_t i=0; i<n; i++) {
        r[i].key = i;
        r[i].value = std::format("{:08d}",i%100);
    }
    return r;
}
