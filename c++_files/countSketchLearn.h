//
// Created by Malthe on 28/05/2022.
//

#ifndef CHARACTERBASEDLSTM_IPYNB_COUNTSKETCHLEARN_H
#define CHARACTERBASEDLSTM_IPYNB_COUNTSKETCHLEARN_H

#include <vector>
#include <stdint.h>
#include <map>

class countSketchLearn {
private:
    int t, k, l;
    uint64_t *as; // Array to store random odd numbers for hash function
    int **array; // Array of size t, to store arrays of size k
    float eps, delta, threshold; // To specify the accuracy and precision
    uint64_t *gas;
    uint64_t *gbs;
    int *estimate_array;

    std::vector<int> predict_high_freq;
    std::map<uint64_t, int>  predict_high_dict;
public:
    countSketchLearn(float eps, float delta, float threshold);
    void update(uint64_t i, int delta_change, float prediction);
    int estimate(uint64_t i, float prediction);
    uint64_t hash_h(uint64_t x, uint64_t l, uint64_t a);
    uint64_t hash_g(uint64_t x, uint64_t a, uint64_t b);
    uint64_t test(int i);
    uint64_t getSize();
};



#endif //CHARACTERBASEDLSTM_IPYNB_COUNTSKETCHLEARN_H
