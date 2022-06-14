#include <stdint.h> //defines uint64_t as unsigned 64-bit integer.
#include "countMin.h"
#include <random>
#include <iostream>
#include <cmath>
using namespace std;

countMin::countMin(float eps, float delta) {
    this->eps = eps;
    this->delta = delta;
    if ((0 < eps && eps < 1) && (0 < delta && delta < 1)) {
        this->t = ceil(log2(1/delta));
        //cout << "t: " << t << endl;
        this->k = pow(2, ceil(log2(2/eps))+1);
        //cout << "k: " << k << endl;
        this->l = (int) ceil(log2(ceil(2/eps)))+1;
        //cout << "l: " << l << endl;
    }

    //cout << "Test constructor" << endl;
    this->as = new uint64_t [t];  // Array of a's for hash function
    this->array = new int *[t];   // t copies


    random_device rd;     // Get a random seed from the OS entropy device
    mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator
    //and seed it with entropy.

    //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
    //or what have you.
    uniform_int_distribution<uint64_t> distr;

    // Generating odd uniformly random w-bit integers (a's) for the hash function
    for (int i = 0; i < t; i++) {
        uint64_t random_odd_int = distr(eng);
        random_odd_int |= 1ULL << 0;  // Bit-shifting to make odd
        as[i] = random_odd_int;
    }

    // Array of arrays
    for (int i = 0; i < t; i++) {
        array[i] = new int[k];
        for (int counter = 0; counter < k; counter++) {
            array[i][counter] = 0; // Initializing counters to 0
        }
    }
}

void countMin::update(uint64_t item, int delta_change) {
    for (int array_index = 0; array_index < t; array_index++) {
        array[array_index][hash(item, l, as[array_index])] += delta_change;
    }
}

int countMin::estimate(uint64_t i) {
    int min_estimate = array[0][hash(i, l, as[0])]; // set min estimate as first estimate
    for (int array_index = 0; array_index < t; array_index++) {
        //cout << "hash is: " << hash(i, l, as[array_index]) << endl;
        int estimate_j = array[array_index][hash(i, l, as[array_index])]; // find the j'th estimate
        if(estimate_j < min_estimate ){
            min_estimate = estimate_j; // if the j'th estimate is smaller; replace the min estimate.
        }
    }
    return min_estimate;
}


uint64_t countMin::hash(uint64_t x, uint64_t l, uint64_t a) {
// hashes x universally into l<=64 bits using random odd seed a.
    int kk = (a*x) >> (64-l);
    //cout << "hash:" << kk << endl;
    return (a*x) >> (64-l);
}

uint64_t countMin::getSize() {
    // return number of bytes used
    // t*k*8 for sketches, t*8 for as for hash function
    uint64_t bytesUsed = t * k * 8 + t * 8;
    return bytesUsed;
}

/*
uint64_t countMin::test(int i) {
    int index_i = array[0][i];
    cout << "Index i is: " << index_i << endl;
    cout << "Random numbers are: ";
    for (int i = 0; i < t; i++) {
        cout << as[i] << endl;
    }
}*/
