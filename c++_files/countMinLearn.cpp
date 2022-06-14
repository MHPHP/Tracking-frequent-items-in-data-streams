#include <stdint.h> //defines uint64_t as unsigned 64-bit integer.
#include "countMinLearn.h"
#include <random>
#include <iostream>
#include <cmath>
#include <map>
#include <vector>
using namespace std;

countMinLearn::countMinLearn(float eps, float delta, float threshold) {
    this->eps = eps;
    this->delta = delta;
    this->threshold = threshold;
    if ((0 < eps && eps < 1) && (0 < delta && delta < 1)) {
        this->t = ceil(log2(1/delta));
        cout << "t: " << t << endl;
        this->k = pow(2, ceil(log2(2/eps))+1);
        cout << "k: " << k << endl;
        this->l = (int) ceil(log2(ceil(2/eps)))+1;
        cout << "l: " << l << endl;
    }
    cout << "the threshold is:" << threshold << endl;

    //cout << "Test constructor" << endl;
    this->as = new uint64_t [t];  // Array of a's for hash function
    this->array = new int *[t];   // t copies

    //Create vector to store items with predicted high freq
    std::vector<int> predict_high_freq;
    this->predict_high_freq = predict_high_freq;
    // Create map to find index of predicted high frequency items
    map<uint64_t, int> predict_high_dict = {};
    this->predict_high_dict = predict_high_dict;


    random_device rd;     // Get a random seed from the OS entropy device
    mt19937_64 eng(rd()); // Use the 64-bit Mersenne Twister 19937 generator
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

void countMinLearn::update(uint64_t item, int delta_change, float prediction) {
    //cout << "im here with item: " << item << " and the prediction is: " << prediction << endl;
    if (prediction > threshold) {
        if (predict_high_dict.find(item) == predict_high_dict.end()) {  // create new bucket and update
            //cout << "item " << item << " with prediction " << prediction << " and it will get key: " << predict_high_dict.size()  <<endl;
            predict_high_dict[item] = predict_high_dict.size() ;
            predict_high_freq.push_back(delta_change);
        } else {  // update existing bucket
            predict_high_freq[predict_high_dict[item]] += delta_change;
        }
    }
    else {  // feed to sketch algorithm if below threshold
        for (int array_index = 0; array_index < t; array_index++) {
            array[array_index][hash(item, l, as[array_index])] += delta_change;
        }
    }
}

int countMinLearn::estimate(uint64_t i, float prediction) {
    //cout << "estimate " << i << " With prediction " << prediction << endl;
    if (prediction > threshold) {
        //cout << "item " << i  << " has a prediction of:  " << prediction << " the data stored is :" << predict_high_freq[predict_high_dict[i]] << endl;
        return predict_high_freq[predict_high_dict[i]];
    }
    else {
        int min_estimate = array[0][hash(i, l, as[0])]; // set min estimate as first estimate
        for (int array_index = 0; array_index < t; array_index++) {
            //cout << "hash is: " << hash(i, l, as[array_index]) << endl;
            int estimate_j = array[array_index][hash(i, l, as[array_index])]; // find the j'th estimate
            if (estimate_j < min_estimate) {
                min_estimate = estimate_j; // if the j'th estimate is smaller; replace the min estimate.
            }
        }
        return min_estimate;
    }
}


uint64_t countMinLearn::hash(uint64_t x, uint64_t l, uint64_t a) {
// hashes x universally into l<=64 bits using random odd seed a.
    int kk = (a*x) >> (64-l);
    //cout << "hash:" << kk << endl;
    return (a*x) >> (64-l);
}

uint64_t countMinLearn::getSize() {
    // return number of bytes used
    // t*k*8 for sketches, t*8 for as for hash function
    // vector (4 bytes for int) * predict_high_freq.size() + predict_high_freq.size() * (8 for uint64 + 4 for int)
    uint64_t bytesUsed = t * k * 8 + t * 8 + predict_high_freq.size() * 4 * 4;
    return bytesUsed;
}
