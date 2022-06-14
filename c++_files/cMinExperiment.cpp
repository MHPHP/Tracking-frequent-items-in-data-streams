#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "countMin.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

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

    // Creating the data structure and updating each element
    // Create the different epsilons for memory plot
    vector<float> delta_vec = {0.00001 ,0.002, 0.03125, 0.125};
    vector<float> epsilon_vec = {};
    for (int i = 2; i < 20; i++) {
        float one = 1.1;
        float eps_value = (float) one/(pow(2,i)) ;
        epsilon_vec.push_back(eps_value);
    }

    // create data for constant epsilons and different delta_vec

//    vector<float> delta_vec = {};
//    vector<float> epsilon_vec = {0.02, 0.002, 0.05};
//    for (int i = 2; i < 20; i++) {
//        float delta =  1/pow(2,i);
//        delta_vec.push_back(delta);
//    }

    // some code to run without dictonary look up.
    vector<int> intContent;
    for (string query:content) {
        intContent.push_back(query_dict.at(query));
    }

    // Vectors corresponding to number of combinations of above vectors
    vector<float> epsilons;
    vector<float> deltas;

    vector<int> query_time;
    vector<int> update_time;
    vector<uint64_t> memory_used;
    vector<double> average_relative_error;
    vector<double> average_error;

    // Create vvectro with integers to run though, when doing updates.
    // This helps getting a time that is easier to compare to update
    vector<int> queries_ints;
    for (int i = 1; i <= reverse_dict.size(); i++) {
        queries_ints.push_back(i);
    }


    int times = 1;
    for (int timesRun = 0 ; timesRun < times; timesRun++) {
        for (float delta: delta_vec) {
            for (float eps: epsilon_vec) {

                countMin cMin(eps, delta);
                cout << "test " << endl;
                cout << "eps:" << eps << endl;
                cout << "delta: " << delta << endl;

                // start time, update
                auto startTime = high_resolution_clock::now();
                epsilons.push_back(eps);
                deltas.push_back(delta);

                for (int queryInt: intContent) {
                    cMin.update(queryInt,1);
                }
//                for (string query: content) {
//
//                    cMin.update(query_dict.at(query), 1);
//                }

                // end time, update
                auto endTime = high_resolution_clock::now();
                auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);
                int elapsedTimeUpdate = elapsedTime.count(); // convert to integer
                cout << "update time: " << elapsedTimeUpdate << endl;

                // Querying elements from the data structure

                // start time, query all items in data stream
                auto startTime2 = high_resolution_clock::now();
                for (int i : queries_ints) {
                    // Do dummy look up to get compareable times
                    //string t =  reverse_dict.at(i);
                    cMin.estimate(i);
                }
                cout << "dictionary size is: " << query_dict.size() << endl;

                // end time, query all items in data stream
                auto endTime2 = high_resolution_clock::now();
                auto elapsedTime2 = duration_cast<milliseconds>(endTime2 - startTime2);
                int elapsedTimeQuery = elapsedTime2.count();
                cout << "query time: " << elapsedTimeQuery << endl;

                // space usage for cMin object
                cout << "The Memory used in bytes is: " << cMin.getSize() << endl;

                // error calculations
                double totalError = 0;
                double totalError2 = 0;
                for (int i = 1; i <= reverse_dict.size(); i++) {
                    uint64_t f = freq_dict[reverse_dict[i]];  // reverse dict, as we need a query to index freq_dict by (not a value)
                    uint64_t fHat = cMin.estimate(i);
                    double error = (double) llabs(fHat - f) / f;
                    totalError += error;
                    double error2 = (double) llabs(fHat - f);
                    totalError2 += error2;
                }
                cout << "average relative error is: " << totalError / reverse_dict.size() << endl;
                cout << "average total error is: " << totalError2 / reverse_dict.size() << endl;

                average_relative_error.push_back(totalError / reverse_dict.size());
                average_error.push_back(totalError2 / reverse_dict.size());
                memory_used.push_back(cMin.getSize());
                update_time.push_back(elapsedTimeUpdate);
                query_time.push_back(elapsedTimeQuery);
            }
        }
    }
    ofstream output_file;
    output_file.open ("../../ResultData/countMinSketchConstantDelta_11.csv");
    output_file << "epsilon,delta,memory,relativeError,averageError,queryTime,updateTime" << endl;
    for (int i = 0; i < epsilons.size(); i++) {
        output_file << epsilons[i] << "," << deltas[i] << "," << memory_used[i]<< "," << average_relative_error[i] << ","
        << average_error[i] << "," << query_time[i] << "," << update_time[i] << endl;
    }
    output_file.close();
}