//
// Created by Malthe on 24/05/2022.
//

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "countMin.h"
#include "countSketch.h"
#include <math.h>
#include <iostream>
#include "HeavyHitterCormode.h"
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
    fstream file("../../ProcessedData/query_train.csv", ios::in);
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
    fstream file2("../../ProcessedData/checkData.csv", ios::in);
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
    map<string, int> query_dict = {};
    map<int, string> reverse_dict = {};
    int id_count = 1;
    for (int j = 0; j < content.size(); j++) {
        if (query_dict.find(content[j]) == query_dict.end()) {
            query_dict[content[j]] = id_count;
            reverse_dict[id_count] = content[j];
            id_count += 1;
        }
    }

    int sum_freq = 0;

    for (const auto& [key, value] : freq_dict) {
        sum_freq += value;
    }
    cout << sum_freq << endl;

    // Creating the data structure and updating each element
    vector<float> epsilon_vec = {0.000333};
    vector<float> u_vec = {100, 200, 1000};

    // Vectors corresponding to number of combinations of above vectors
    vector<float> epsilons;
    vector<float> us;

    vector<int> query_time;
    vector<int> update_time;
    vector<uint64_t> memory_used;

    vector<int> true_positives;
    vector<int> false_positives;
    vector<int> false_negatives;

    // Creating the data structure and updating each element
    for (float u: u_vec) {
        for (float eps: epsilon_vec) {
            HeavyHitterCormode HH(eps, u);
            epsilons.push_back(eps);
            us.push_back(u);
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

            vector<int> estimated_list = HH.query();

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
            vector<int> heavy_list;
            vector<int> allowed_list;
            for (int i = 1; i <= reverse_dict.size(); i++) {
                uint64_t true_freq = freq_dict[reverse_dict[i]];  // reverse dict, as we need a query to index freq_dict by (not a value)
                if (true_freq >= eps * sum_freq) {
                    heavy_list.push_back(i);
                }
                if (true_freq >= (eps/2) * sum_freq) {
                    allowed_list.push_back(i);
                }
            }
            // Find true positives and false positives
            for (int i: estimated_list) {
                if (any_of(allowed_list.begin(), allowed_list.end(), compare(i))) {
                    n_true_positives += 1;
                } else {
                    n_false_positives += 1;
                }
            }

            // Find false negatives
            for (int i: heavy_list) {
                if (!any_of(estimated_list.begin(), estimated_list.end(), compare(i))) {
                    n_false_negatives += 1;
                }
            }
            cout << "The estimates are: " << endl;
            for (int i: estimated_list) {
                cout << i << ",  ";
            }
            cout << " " << endl;
            cout << "The heavy list is: " << endl;
            for (int i: heavy_list) {
                cout << i << ",   ";
            }



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
        }
    }
    ofstream output_file;
    output_file.open ("../../ResultData/heavyHitterCormode.csv");
    output_file << "epsilon,u,memory,true_positives,false_positives,false_negatives,queryTime,updateTime" << endl;
    for (int i = 0; i < epsilons.size(); i++) {
        output_file << epsilons[i] << "," << us[i] << "," << memory_used[i]<< "," << true_positives[i] << ","
                    << false_positives[i] << "," << false_negatives[i]  << "," << query_time[i] << "," << update_time[i] << endl;
    }
    output_file.close();
}