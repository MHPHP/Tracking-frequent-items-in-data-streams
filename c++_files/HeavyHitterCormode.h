//
// Created by Malthe on 24/05/2022.
//

#ifndef BACHELOR_HEAVYHITTERCORMODE_H
#define BACHELOR_HEAVYHITTERCORMODE_H
#include "countMin.h"
#include <stdint.h>
#include <vector>

class HeavyHitterCormode {
private:
    uint64_t u, precision;
    float eps;
    int tree_size;

    uint64_t total_frequency;
    std::vector<countMin> sketches;
    std::vector<uint64_t> list;
public:
    HeavyHitterCormode(float eps, uint64_t u, uint64_t precision);
    void update(uint64_t index, int delta_change);
    std::vector<uint64_t> query(); // query prints a list of heavy hitters and their frequency
    void query_recursive(int layer, uint64_t index);
    uint64_t getSize();
};

#endif //BACHELOR_HEAVYHITTERCORMODE_H