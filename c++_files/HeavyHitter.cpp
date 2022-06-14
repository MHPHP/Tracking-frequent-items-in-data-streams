//
// Created by Laura Nilsson on 03-05-2022.
//
#include <stdint.h>
#include "HeavyHitter.h"
#include <cmath>
#include "countMin.h"
#include <vector>
#include <iostream>
#include <map>
#include <tuple>

using namespace std;

HeavyHitter::HeavyHitter(float eps, uint64_t u, uint64_t precision) {
    this-> eps = eps;
    this-> u = u;
    this-> precision = precision;
    this->total_frequency = 0;
    // find the size of the tree
    this-> tree_size = ceil(log2(u)); // ensure that three size is power of 2 (+1, as layers = logU+1)
    cout << "tree_size " << tree_size << endl;
    cout << "pres is" << precision << endl;

    // indexing of CMS array based on indexing of tree -> 0 to log U
    vector<countMin> sketches;

    cout << "u is:" << u << endl;

    sketches.push_back(countMin(eps/2, (float) 1/(precision))); // layer 0

    // layers above 0
    for (int i = 1; i <= tree_size; i++) {
        sketches.push_back(countMin(eps / 2, 0.25));
    }
    this->sketches = sketches;
    this->list = list;
    map<uint64_t, int> heavy_Dict = {};
    this->heavy_dict = heavy_Dict;
}

void HeavyHitter::update(uint64_t index, int delta_change) {
    for (int i = 0; i <= tree_size; i++) {
        // Updating each sketch. index >> i is equal to floor(index/pow(2,i)).
        sketches[i].update( (index >> i),delta_change);
    }
    total_frequency += delta_change;
}

std::tuple<std::vector<uint64_t>, std::map<uint64_t, int>> HeavyHitter::query() {
    cout << "tree size is also " << tree_size << endl;
    query_recursive(tree_size, 0);
    return make_tuple(list, heavy_dict);
}

void HeavyHitter::query_recursive(int layer, uint64_t index) {
    if (layer > 0) {
        if (sketches[layer-1].estimate(index*2) >= eps * total_frequency) {
            query_recursive(layer-1, index*2);
        }
        if (sketches[layer-1].estimate(index*2+1) >= eps * total_frequency) {
            query_recursive(layer-1, index*2+1);
        }
    } else {
        list.push_back(index);
        heavy_dict[index] = sketches[0].estimate(index);
        //cout << "Heavy hitter index: " << index << " has a frequency of: " << sketches[0].estimate(index) << endl;
    }
}

uint64_t HeavyHitter::getSize() {
    // Return number of bytes used
    uint64_t bytesUsed = 0;
    for (countMin sketch : sketches) {
        bytesUsed += sketch.getSize();
    }
    return bytesUsed;
}