
#ifndef BACHELOR_COUNTMIN_H
#define BACHELOR_COUNTMIN_H

#include <stdint.h>


class countMin {
private:
    int t, k, l;
    uint64_t *as; // Array to store random odd numbers for hash function
    int **array; // Array of size t, to store arrays of size k
    float eps, delta; // To specify the accuracy and precision
public:
    countMin(float eps, float delta);
    void update(uint64_t i, int delta_change);
    int estimate(uint64_t i);
    uint64_t hash(uint64_t x, uint64_t l, uint64_t a);
    uint64_t test(int i);
    uint64_t getSize();
};

#endif //BACHELOR_COUNTMIN_H