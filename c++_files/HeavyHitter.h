//
// Created by Laura Nilsson on 03-05-2022.
//

#ifndef BACHELOR_HEAVYHITTER_H
#define BACHELOR_HEAVYHITTER_H
#include "countMin.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <tuple>

class HeavyHitter {
private:
    uint64_t u, precision;
    float eps;
    int tree_size;
    uint64_t total_frequency;
    std::vector<countMin> sketches;
    std::vector<uint64_t> list;
    std::map<uint64_t, int>  heavy_dict;
public:
    HeavyHitter(float eps, uint64_t u, uint64_t precision);
    void update(uint64_t index, int delta_change);
    std::tuple<std::vector<uint64_t>, std::map<uint64_t, int>> query(); // query prints a list of heavy hitters and their frequency
    void query_recursive(int layer, uint64_t index);
    uint64_t getSize();
};

#endif //BACHELOR_HEAVYHITTER_H
