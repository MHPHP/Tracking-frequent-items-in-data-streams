#include <stdint.h> //defines uint64_t as unsigned 64-bit integer.
#include "countSketchLearn.h"
#include <random>
#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
#include <map>
#include <vector>
using namespace std;

countSketchLearn::countSketchLearn(float eps, float delta, float threshold) {
    this-> eps = eps;
    this-> delta = delta;
    this->threshold = threshold;
    if ((0 < eps && eps < 1) && (0 < delta && delta < 1)) {
        t = ceil(log2(1/delta));
        cout << "t: " << t << endl;
        k = pow(2, ceil(log2(4/pow(eps,2)))); //log2(4/eps^2), raised to the power of 2 w.r.t. bitshift
        cout << "k: " << k << endl;
        l = log2(k);  // log2 of k, how many bits we want left w.r.t. the length of the array
        //cout << "l: " << l << endl;
    }
    else {
        //abort
    }
    //cout << "Test constructor" << endl;
    as = new uint64_t [t];  // Array of a's for hash function h
    gas = new uint64_t [t]; // Array of a's for hash function g
    gbs = new uint64_t [t]; // Array of b's for hash function g
    array = new int *[t];   // t copies
    estimate_array = new int [t];

    random_device rd;     // Get a random seed from the OS entropy device
    mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator
    //and seed it with entropy.

    //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
    //or what have you.
    uniform_int_distribution<uint64_t> distr;

    // Generating odd uniformly random w-bit integers (a's and b's) for the hash function
    for (int i = 0; i < t; i++) {
        uint64_t random_odd_int = distr(eng);
        random_odd_int |= 1ULL << 0;  // Bit-shifting to make odd
        as[i] = random_odd_int;
        uint64_t random_int_a = distr(eng);
        uint64_t random_int_b = distr(eng);
        gas[i] = random_int_a;
        gbs[i] = random_int_b;
    }

    // Array of arrays
    for (int i = 0; i < t; i++) {
        array[i] = new int[k];
        for (int counter = 0; counter < k; counter++) {
            array[i][counter] = 0; // Initializing counters to 0
        }
    }
}

void countSketchLearn::update(uint64_t item, int delta_change, float prediction) {
    if (prediction > threshold) {
        if (predict_high_dict.find(item) == predict_high_dict.end()) {  // create new bucket and update
            //cout << "item " << item << " with prediction " << prediction << " and it will get key: " << predict_high_dict.size()  <<endl;
            predict_high_dict[item] = predict_high_dict.size() ;
            predict_high_freq.push_back(delta_change);
        } else {  // update existing bucket
            predict_high_freq[predict_high_dict[item]] += delta_change;
        }
    }
    else {
        // Accessing t arrays, and iterating over counters in each
        for (int array_index = 0; array_index < t; array_index++) {
            array[array_index][hash_h(item, l, as[array_index])] +=
                    delta_change * hash_g(item, gas[array_index], gbs[array_index]);
        }
    }
}

int countSketchLearn::estimate(uint64_t x, float prediction) {
    if (prediction > threshold) {
        //cout << "item " << i  << " has a prediction of:  " << prediction << " the data stored is :" << predict_high_freq[predict_high_dict[i]] << endl;
        return predict_high_freq[predict_high_dict[x]];
    } else {
        for (int array_index = 0; array_index < t; array_index++) {
            int estimate_j = array[array_index][hash_h(x, l, as[array_index])]; // find the j'th estimate
            estimate_array[array_index] = hash_g(x, gas[array_index], gbs[array_index]) * estimate_j;
        }

        // calculating median
        sort(estimate_array, estimate_array + t);
        if (t % 2 != 0) {
            return estimate_array[(int) floor(t / 2)];
        } else {
            return estimate_array[t / 2];
        }
    }
}


uint64_t countSketchLearn::hash_h(uint64_t x, uint64_t l, uint64_t a) {
    // hashes x universally into l<=64 bits using random odd seed a.
    return (a*x) >> (64-l);
}

uint64_t countSketchLearn::hash_g(uint64_t x, uint64_t a, uint64_t b) {
    int r = (a*x)+b >> 63;  // Always bit-shifts 63, which results in either r = 0 or r = 1
    int r_new = (2*r)-1;  // -1 or 1
    return r_new;
}

uint64_t countSketchLearn::getSize() {
    // return number of bytes used
    // t*k*8 for sketches, (t*8)*3 for as (two different arrays) and bs for the two hash functions
    uint64_t bytesUsed = t * k * 8 + (t * 8) * 3 + predict_high_freq.size() * 4 * 4;;
    return bytesUsed;
}

