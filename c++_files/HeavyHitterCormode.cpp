//
// Created by Laura Nilsson on 03-05-2022.
//
#include <stdint.h>
#include "HeavyHitterCormode.h"
#include <cmath>
#include "countMin.h"
#include <vector>
#include <iostream>

using namespace std;

HeavyHitterCormode::HeavyHitterCormode(float eps, uint64_t u, uint64_t precision) {
    this-> eps = eps;
    this-> u = u;
    this->total_frequency = 0;
    this-> precision = precision;
    // find the size of the tree
    this-> tree_size = ceil(log2(u)); // ensure that three size is power of 2 (+1, as layers = logU+1)
    cout << "tree_size " << tree_size << endl;

    // indexing of CMS array based on indexing of tree -> 0 to log U
    vector<countMin> sketches;

    cout << "u is:" << u << endl;

    // for all layers, set failure probability to 1/U^2
    for (int i = 0; i <= tree_size; i++) {
        sketches.push_back(countMin(eps/2, (float) 1/(precision)));
    }
    this->sketches = sketches;
    this->list = list;
}

void HeavyHitterCormode::update(uint64_t index, int delta_change) {
    for (int i = 0; i <= tree_size; i++) {
        // Updating each sketch. index >> i is equal to floor(index/pow(2,i)).
        sketches[i].update( (index >> i),delta_change);
    }
    total_frequency += delta_change;
}

std::vector<uint64_t> HeavyHitterCormode::query() {
    cout << "tree size is also " << tree_size << endl;
    query_recursive(tree_size, 0);
    return list;
}

void HeavyHitterCormode::query_recursive(int layer, uint64_t index) {
    if (layer > 0) {
        if (sketches[layer-1].estimate(index*2) >= eps * total_frequency) {
            query_recursive(layer-1, index*2);
        }
        if (sketches[layer-1].estimate(index*2+1) >= eps * total_frequency) {
            query_recursive(layer-1, index*2+1);
        }
    } else {
        list.push_back(index);
        //cout << "Heavy hitter index: " << index << " has a frequency of: " << sketches[0].estimate(index) << endl;

    }

}

uint64_t HeavyHitterCormode::getSize() {
    // Return number of bytes used
    uint64_t bytesUsed = 0;
    for (countMin sketch : sketches) {
        bytesUsed += sketch.getSize();
    }
    return bytesUsed;
}