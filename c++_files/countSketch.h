//
// Created by Malthe on 15/03/2022.
//

#ifndef BACHELOR_COUNTSKETCH_H
#define BACHELOR_COUNTSKETCH_H


#include <stdint.h>


class countSketch {
private:
    int t, k, l;
    uint64_t *as; // Array to store random odd numbers for hash function
    int **array; // Array of size t, to store arrays of size k
    float eps, delta; // To specify the accuracy and precision
    uint64_t *gas;
    uint64_t *gbs;
    int *estimate_array;
public:
    countSketch(float eps, float delta);
    void update(uint64_t i, int delta_change);
    int estimate(uint64_t i);
    uint64_t hash_h(uint64_t x, uint64_t l, uint64_t a);
    uint64_t hash_g(uint64_t x, uint64_t a, uint64_t b);
    uint64_t test(int i);
    uint64_t getSize();
};

#endif //BACHELOR_COUNTSKETCH_H

