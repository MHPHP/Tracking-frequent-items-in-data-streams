
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "countMinLearn.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <tuple>
using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

int main() {
    // Loading the data with the predictions
    cout << "hey 1" << endl;
    vector<vector<string>> pred_vector;
    vector<string> row;
    string line, elm;
    fstream file("../../ProcessedData/experiment_data_03_25_clean_words_with_predictions2.csv", ios::in);
    if (file.is_open()) {
        cout << "The file is open" << endl;
        while (getline(file, line)) {
            row.clear();
            stringstream str(line);
            while (getline(str, elm, ',')) {
                row.push_back(elm);
            }
            pred_vector.push_back(row);
        }
    }
    //.substr(1, pred_vector[row][0].size() - 2)
    // create a vector with tuples (element, prediction), one for each element in the query stream.

    vector<tuple<string, float>> query_predict_vector;
    for (int row = 1; row < pred_vector.size(); row++) {
        float val;
        stringstream ss;
        ss << pred_vector[row][1];
        ss >> val;
        auto new_tuple = make_tuple(pred_vector[row][0].substr(1, pred_vector[row][0].size() - 2), val);
        query_predict_vector.push_back(new_tuple);
    }

    // Test if it works (Can be deleted)
    cout << "hey 3" << endl;
    for (int i = 0; i< 10; i++) {
        cout << get<0>(query_predict_vector[i])<< " and " << get<1>(query_predict_vector[i]) << endl;
    }

    // Loading the true frequencies
    vector<vector<string>> freq_vector;
    vector<string> row2;
    string line2, elm2;
    fstream file2("../../ProcessedData/experiment_data_03_25_clean_words_with_freq.csv", ios::in);
    if (file2.is_open()) {
        cout << "The file is open" << endl;
        while (getline(file2, line2)) {
            row2.clear();
            stringstream str(line2);
            while (getline(str, elm2, ',')) {
                row2.push_back(elm2);
            }
            freq_vector.push_back(row2);
        }
    }



    // Create dictionary of queries
    map<string, int> query_dict = {};
    map<int, string> reverse_dict = {};
    int id_count = 0;
    cout << "hey 4" << endl;
    map<string, int> freq_dict = {};
    for (int row = 1; row < freq_vector.size(); row++) {
        int val2;
        stringstream ss2;
        ss2 << freq_vector[row][1];
        ss2 >> val2;
        freq_dict[freq_vector[row][0].substr(1, freq_vector[row][0].size() - 2)] = val2;
        string query = freq_vector[row][0].substr(1, freq_vector[row][0].size() - 2);
        if (query_dict.find(query) == query_dict.end()) {
            query_dict[query] = id_count;
            reverse_dict[id_count] = query;
            id_count += 1;
        }
    }
    map<int, float> threshold_dict = {};
    for (tuple query: query_predict_vector) {
        if (threshold_dict.find(query_dict[get<0>(query)]) == threshold_dict.end()) {
            threshold_dict[query_dict[get<0>(query)]] = get<1>(query);
        }
    }


//    int id_count = 1;
//    for (int j = 0; j < pred_vector.size(); j++) {
//        if (query_dict.find(pred_vector[j][0]) == query_dict.end()) {
//            query_dict[pred_vector[j][0]] = id_count;
//            reverse_dict[id_count] = pred_vector[j][0];
//            id_count += 1;
//            threshold_dict[query_dict[pred_vector[j][0]]] = get<1>(query_predict_vector[j]);
//        }
//    }
    // vector<float> delta_vec = {0.001, 0.03125, 0.125};
    // Create the different epsilons for memory plot
    vector<float> threshold_vec = {0.1, 0.01,0.001,  0.0001, 0.00001};
    vector<float> delta_vec = {0.00001, 0.002, 0.03125, 0.125};
    vector<float> epsilon_vec = {};
    for (int i = 2; i < 20; i++) {
        float one = 1.1;
        float eps_value = (float) one/(pow(2,i)) ;
        epsilon_vec.push_back(eps_value);
    }

    // create data for constant epsilons and different delta_vec
//    vector<float> threshold_vec = {0.001, 0.0001, 0.005, 0.00005, 0.00001};
//    vector<float> delta_vec = {};
//    vector<float> epsilon_vec = {0.02, 0.002, 0.05};
//    for (int i = 5; i < 15; i++) {
//        float delta =  1/pow(2,i);
//        delta_vec.push_back(delta);
//    }

    // Vectors corresponding to number of combinations of above vectors
    vector<float> epsilons;
    vector<float> deltas;
    vector<float> thresholds;
    vector<int> query_time;
    vector<int> update_time;
    vector<uint64_t> memory_used;
    vector<double> average_relative_error;
    vector<double> average_error;


    int times = 1;
    for (int timesRun = 0 ; timesRun <= times; timesRun++) {
        for (float delta: delta_vec) {
            for (float eps: epsilon_vec) {
                for (float threshold: threshold_vec) {

                    countMinLearn cMinLearn(eps, delta, threshold);

                    cout << "test " << endl;
                    cout << "eps:" << eps << endl;
                    cout << "delta: " << delta << endl;
                    cout << "threshold: " << threshold << endl;

                    // start time, update
                    epsilons.push_back(eps);
                    deltas.push_back(delta);
                    thresholds.push_back(threshold);

                    auto startTime = high_resolution_clock::now();
                    // tuple query: query_predict_vector
                    for (auto const& query : query_predict_vector) {
                        //cout << get<0>(query) << endl;
                        cMinLearn.update(query_dict.at(get<0>(query)), 1,  get<1>(query));
                    }

                    // end time, update
                    auto endTime = high_resolution_clock::now();
                    auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);
                    int elapsedTimeUpdate = elapsedTime.count(); // convert to integer
                    cout << "update time: " << elapsedTimeUpdate << endl;

                    // Querying elements from the data structure
                    // start time, query all items in data stream
                    auto startTime2 = high_resolution_clock::now();
                    for (int i = 1; i <= query_dict.size(); i++) {
                        cMinLearn.estimate(i,threshold_dict[i]);
                        //string t =  reverse_dict.at(i);
                    }
                    cout << "dictionary size is: " << query_dict.size() << endl;


                    // end time, query all items in data stream
                    auto endTime2 = high_resolution_clock::now();
                    auto elapsedTime2 = duration_cast<milliseconds>(endTime2 - startTime2);
                    int elapsedTimeQuery = elapsedTime2.count();
                    cout << "query time: " << elapsedTimeQuery << endl;

                    // space usage for cMin object
                    cout << "The Memory used in bytes is: " << cMinLearn.getSize() << endl;


                    // error calculations
                    double totalError = 0;
                    double totalError2 = 0;
                    cout << query_dict.size() << endl;
                    int query_size = query_dict.size();
                    for (int i = 1; i <= query_size; i++) {
                        uint64_t f = freq_dict[reverse_dict[i]];  // reverse dict, as we need a query to index freq_dict by (not a value)
                        uint64_t fHat = cMinLearn.estimate(i, threshold_dict[i]);
                        //auto error = (double) llabs(fHat - f) / f;
                        //totalError += error;
                        double error2 = (double) llabs(fHat - f);
                        totalError2 += error2;
                    }
                    //cout << "average relative error is: " << totalError / query_dict.size() << endl;
                    cout << "average total error is: " << totalError2 / query_dict.size() << endl;
                    //average_relative_error.push_back(totalError / query_dict.size());

                    average_error.push_back(totalError2 / query_dict.size());

                    memory_used.push_back(cMinLearn.getSize());

                    update_time.push_back(elapsedTimeUpdate);

                    query_time.push_back(elapsedTimeQuery);

                }
            }
        }
    }

    ofstream output_file;
    output_file.open ("../../ResultData/countMinLearnSketchConstantDelta_11.csv");
    output_file << "epsilon,delta,memory,averageError,queryTime,updateTime,threshold" << endl;

    for (int i = 0; i < epsilons.size(); i++) {
        output_file << epsilons[i] << "," << deltas[i] << "," << memory_used[i]<< ","
                    << average_error[i] << "," << query_time[i] << "," << update_time[i] << "," << thresholds[i] << endl;
    }
    output_file.close();
}

//  << average_relative_error[i] << ","
