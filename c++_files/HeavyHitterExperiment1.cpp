#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "countMin.h"
#include "countSketch.h"
#include <math.h>
#include <iostream>
#include "HeavyHitter.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <algorithm>

using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

struct compare
{
    int key;
    compare(int const &i): key(i) {}

    bool operator()(int const &i) {
        return (i == key);
    }
};

int main() {

    // Loading the clean, original data
    vector<string> content;
    string line, word;
    fstream file("../../ProcessedData/experiment_data_03_25_clean_words_no_freq.csv", ios::in);
    if (file.is_open()) {
        cout << "The file is open" << endl;
        while (getline(file, line)) {
            stringstream str(line);
            while (getline(str, word, ',')) {
                content.push_back(word.substr(1, word.size() - 2));
            }
        }
    }

    // Loading the true frequencies
    vector<vector<string>> freq_vector;
    vector<string> row;
    string line2, elm;
    fstream file2("../../ProcessedData/experiment_data_03_25_clean_words_with_freq.csv", ios::in);
    if (file.is_open()) {
        cout << "The file is open" << endl;
        while (getline(file2, line2)) {
            row.clear();
            stringstream str(line2);
            while (getline(str, elm, ',')) {
                row.push_back(elm);
            }
            freq_vector.push_back(row);
        }
    }

    map<string, int> freq_dict = {};
    for (int row = 1; row < freq_vector.size(); row++) {
        int val;
        stringstream ss;
        ss << freq_vector[row][1];
        ss >> val;
        freq_dict[freq_vector[row][0].substr(1, freq_vector[row][0].size() - 2)] = val;
    }

    // Create dictionary of queries
    map<string, uint64_t> query_dict = {};
    map<uint64_t , string> reverse_dict = {};

    // set the size of the universe
    uint64_t u = (uint64_t) pow(2,64);
    //u = (uint64_t) pow(10,5);

    // Code to make random number generator
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator
    //and seed it with entropy.

    //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
    //or what have you.
    std::uniform_int_distribution<unsigned long long> distr(0, u);

    cout << "We are here 1" <<endl;

    for (int j = 0; j < content.size(); j++) {
        if (query_dict.find(content[j]) == query_dict.end()) {
            // Generate a random number
            uint64_t random_number = distr(eng);
            while (reverse_dict.find(random_number) != reverse_dict.end()) {
                random_number = distr(eng);
            }
            query_dict[content[j]] = random_number;
            reverse_dict[random_number] = content[j];
        }
    }

    cout << "We are here 2" <<endl;
    int sum_freq = 0;

    for (const auto& [key, value] : freq_dict) {
        sum_freq += value;
    }
    cout << sum_freq << endl;

    // Creating the data structure and updating each element
    vector<float> epsilon_vec = {0.01, 0.005, 0.001, 0.0005, 0.0001};
    vector<uint64_t> precision_vec = {(uint64_t) pow(2,64)};
//    for (int i = 2; i < 3; i++) {
//        uint64_t pres = pow(2,i);
//        precision_vec.push_back(pres);
//    }
    // Vectors corresponding to number of combinations of above vectors
    vector<float> epsilons;
    vector<float> us;
    vector<float> precisions;

    vector<int> query_time;
    vector<int> update_time;
    vector<uint64_t> memory_used;

    vector<int> true_positives;
    vector<int> false_positives;
    vector<int> false_negatives;
    vector<int> number_of_heavy;
    vector<double> average_error_list;
    vector<double> relativ_error_list;

    // Creating the data structure and updating each element
    for (float precision: precision_vec) {
        for (float eps: epsilon_vec) {
            HeavyHitter HH(eps, u, precision);
            epsilons.push_back(eps);
            us.push_back(u);
            precisions.push_back(precision);
            // start time, update
            auto startTime = high_resolution_clock::now();

            for (string query: content) {
                HH.update(query_dict.at(query), 1);
            }

            // end time, update
            auto endTime = high_resolution_clock::now();
            auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);
            int elapsedTimeUpdate = elapsedTime.count(); // convert to integer
            cout << "update time: " << elapsedTimeUpdate << endl;

            // Querying elements from the data structure

            // start time, query all heavy hitters from data (i.e. query list L)
            auto startTime2 = high_resolution_clock::now();

            vector<uint64_t> estimated_list = get<0>(HH.query());
            std::map<uint64_t, int> estimated_dict = get<1>(HH.query());
            //cout << "dictionary size is: " << query_dict.size() << endl;

            // end time, query all items in data stream
            auto endTime2 = high_resolution_clock::now();
            auto elapsedTime2 = duration_cast<milliseconds>(endTime2 - startTime2);
            int elapsedTimeQuery = elapsedTime2.count();
            cout << "query time: " << elapsedTimeQuery << endl;

            // space usage
            cout << "The Memory used in bytes is: " << HH.getSize() << endl;

            // error calculations
            int n_true_positives = 0;
            int n_false_positives = 0;
            int n_false_negatives = 0; // should be 0

            // Create a list with the true epsilon-heavy hitters and a list with the allowed items
            // True heavy hitters: estimate >= epsilon * sum(freqs)
            // Allowed, but not epsilon-heavy hitter: estimate >= (epsilon/2) *> sum(freqs)
            vector<uint64_t> heavy_list;
            vector<uint64_t> allowed_list;
            for (const auto& elem : reverse_dict) {
                uint64_t true_freq = freq_dict[reverse_dict[elem.first]];  // reverse dict, as we need a query to index freq_dict by (not a value)
                if (true_freq >= eps * sum_freq) {
                    heavy_list.push_back(elem.first);
                }
                if (true_freq >= (eps/2) * sum_freq) {
                    allowed_list.push_back(elem.first);
                }
            }
            // Find true positives and false positives
            for (uint64_t i: estimated_list) {
                if (any_of(allowed_list.begin(), allowed_list.end(), compare(i))) {
                    n_true_positives += 1;
                } else {
                    n_false_positives += 1;
                }
            }


            // Find false negatives
            for (uint64_t i: heavy_list) {
                if (!any_of(estimated_list.begin(), estimated_list.end(), compare(i))) {
                    n_false_negatives += 1;
                }
            }
            cout << "The estimates are: " << endl;
            for (uint64_t i: estimated_list) {
                //cout << i << ",  ";
            }
            cout << " " << endl;
            cout << "The heavy list is: " << endl;
            for (uint64_t i: heavy_list) {
                //cout << i << ",   ";
            }

            // Calculate the average error for estimates
            int total_error = 0;
            double total_error2 = 0;
            for (uint64_t index: estimated_list) {
                int f_true = freq_dict[reverse_dict[index]];
                int f_hat = estimated_dict[index];
                cout << "the true was: " << f_true << " the  estimate was " << f_hat << endl;
                total_error += llabs(f_true - f_hat);

                // Relativ error calculations
                if (f_true == 0) {
                    cout << "we got zero at the query: " << index  << reverse_dict[index] << endl;
                    cout << "f_hta is:" << f_hat << endl;
                    f_true = 1;
                }
                total_error2 += (double) llabs(f_true-f_hat) / f_true;
                //cout << "index is" << index << endl;
                //cout << "f_true is: " << f_true << "   F_hat is: " << f_hat << endl;
                //cout << total_error2 << endl;
            }
            // Get the average relativ error and average error
            double average_error = (double) total_error/estimated_list.size();
            double relative_error = (double) total_error2/estimated_list.size();
            cout << "epsilon is: " << eps << endl;
            cout << "Average error is: " << average_error << endl;
            cout << endl;
            // based on the threshold set by epsilon, we define:
            // true positive = both in freq_dict and L
            // false positive = not in freq_dict but in L
            // false negative = not in L but in freq_dict
            cout << "number of true positives in L is: " << n_true_positives << endl;
            cout << "number of false positives in L is: " << n_false_positives << endl;
            cout << "number of false negatives in L is: " << n_false_negatives << endl;

            true_positives.push_back(n_true_positives);
            false_positives.push_back(n_false_positives);
            false_negatives.push_back(n_false_negatives);
            memory_used.push_back(HH.getSize());
            update_time.push_back(elapsedTimeUpdate);
            query_time.push_back(elapsedTimeQuery);
            number_of_heavy.push_back(heavy_list.size());
            average_error_list.push_back(average_error);
            relativ_error_list.push_back(relative_error);
        }
    }
    ofstream output_file;
    output_file.open ("../../ResultData/heavyHitter.csv");
    output_file << "epsilon,u,memory,true_positives,false_positives,false_negatives,queryTime,"
                   "updateTime, precision, numberOfHeavy,average_error,relative_error" << endl;
    for (int i = 0; i < epsilons.size(); i++) {
        output_file << epsilons[i] << "," << us[i] << "," << memory_used[i]<< "," << true_positives[i] << ","
        << false_positives[i] << "," << false_negatives[i]  << "," << query_time[i] << "," << update_time[i]
        <<"," << precisions[i] << "," << number_of_heavy[i]<< "," << average_error_list[i] << ","
        << relativ_error_list[i]  << endl;
    }
    output_file.close();
}
