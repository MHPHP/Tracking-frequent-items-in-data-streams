//
// Created by Malthe on 17/05/2022.
//

#ifndef BACHELOR_COUNTMINLEARN_H
#define BACHELOR_COUNTMINLEARN_H

#include <vector>
#include <stdint.h>
#include <map>

class countMinLearn {
private:
    int t, k, l;
    uint64_t *as; // Array to store random odd numbers for hash function
    int **array; // Array of size t, to store arrays of size k
    float eps, delta, threshold; // To specify the accuracy and precision

    std::vector<int> predict_high_freq;
    std::map<uint64_t, int>  predict_high_dict;
public:
    countMinLearn(float eps, float delta, float threshold);
    void update(uint64_t i, int delta_change, float prediction);
    int estimate(uint64_t i, float prediction);
    uint64_t hash(uint64_t x, uint64_t l, uint64_t a);
    uint64_t test(int i);
    uint64_t getSize();
};

#endif //BACHELOR_COUNTMINLEARN_H
