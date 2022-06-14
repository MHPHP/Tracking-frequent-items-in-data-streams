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

using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


int main() {
    string testRun = "CountTest";

    // setting working directory
    auto path = std::filesystem::current_path(); // getting path
    cout << "path is: " << path << endl;
    std::filesystem::current_path(path); // setting path

    // Heavy Hitter test section
    float eps = 0.1;
    uint64_t u = 100000;
    //HeavyHitter heavyHitter(eps, u);

//    for (int i = 0; i < u ; i++) {
//        heavyHitter.update(i, 10);
//    }
//    heavyHitter.update(5354,500000);
//    heavyHitter.update(4523,500000);
//    heavyHitter.update(u-1,500000);
//    cout << "update is done" << endl;
//    heavyHitter.query();

//    vector<string> content;
//    string line, word;
//    fstream file ("../../ProcessedData/query_train.csv", ios::in);
//    if(file.is_open()) {
//        cout << "The file is open" << endl;
//        while(getline(file, line)) {
//            stringstream str(line);
//            while(getline(str, word, ',')) {
//                content.push_back(word.substr(1, word.size() - 2));
//            }
//        }
//    }
//
//    for(int i=0;i < 20;i++) {
//        cout << content[i] << endl;
//    }
//
//    map<string, int> query_dict = {};
//    map<int, string> reverse_dict = {};
//    int id_count = 1;
//    for (int j=0; j < content.size();j++) {
//        if (query_dict.find(content[j]) == query_dict.end()) {
//            query_dict[content[j]] = id_count;
//            reverse_dict[id_count] = content[j];
//            id_count += 1;
//        }
//    }
//
//    cout << query_dict.at("googl") << endl;
//
//    // we will now test the heavyhitter function with all the queris
//    float epsilon = 0.003;
//    uint64_t universe_size = content.size();
//    //HeavyHitter HH(epsilon, universe_size);
//    countMin cMin(epsilon, 0.001);
//    countSketch cSketch(epsilon, 0.01);
//    for (string query: content) {
//       //HH.update(query_dict.at(query), 1);
//      // cMin.update(query_dict.at(query), 1);
//       cSketch.update(query_dict.at(query), 1);
//    }
//    // cout << "Google is: " << cMin.estimate(query_dict.at("google"));
//    cout << "Google is: " << cSketch.estimate(query_dict.at("google"));

//    string test11 = "42";
//    int val1 = stoi(test11);
//    cout << typeid(val1).name() << endl;

    if (testRun == "CountMinTest") {

        // Loading the clean, original data
        vector<string> content;
        string line, word;
        fstream file ("../../ProcessedData/query_train.csv", ios::in);
        if(file.is_open()) {
            cout << "The file is open" << endl;
            while(getline(file, line)) {
                stringstream str(line);
                while(getline(str, word, ',')) {
                    content.push_back(word.substr(1, word.size() - 2));
                }
            }
        }


        // Loading the true frequencies
        vector<vector<string>> freq_vector;
        vector<string> row;
        string line2, elm;
        fstream file2 ("../../ProcessedData/checkData.csv", ios::in);
        if(file.is_open()) {
            cout << "The file is open" << endl;
            while(getline(file2, line2)) {
                row.clear();
                stringstream str(line2);
                while(getline(str, elm, ',')) {
                    row.push_back(elm);
                }
                freq_vector.push_back(row);
            }
        }


        map<string, int> freq_dict = {};
        for (int row = 1; row < freq_vector.size() ; row++) {
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
        for (int j=0; j < content.size();j++) {
            if (query_dict.find(content[j]) == query_dict.end()) {
                query_dict[content[j]] = id_count;
                reverse_dict[id_count] = content[j];
                id_count += 1;
            }
        }

        // Creating the data structure and updating each element
        countMin cMin(0.0002, 0.0001);


        // start time, update
        auto startTime = high_resolution_clock::now();

        for (string query: content) {
            cMin.update(query_dict.at(query), 1);
        }

        // end time, update
        auto endTime = high_resolution_clock::now();
        auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);
        int elapsedTimeInt = elapsedTime.count(); // convert to integer
        cout << "update time: " << elapsedTimeInt << endl;

        // Querying elements from the data structure

        // start time, query all items in data stream
        auto startTime2 = high_resolution_clock::now();
        for (int i = 1; i <= query_dict.size(); i++) {
            cMin.estimate(i);
            //string t =  reverse_dict.at(i);
        }
        cout << "dictionary size is: " << query_dict.size() << endl;


        // end time, query all items in data stream
        auto endTime2 = high_resolution_clock::now();
        auto elapsedTime2 = duration_cast<milliseconds>(endTime2 - startTime2);
        int elapsedTimeInt2 = elapsedTime2.count();
        cout <<  "query time: " << elapsedTimeInt2 << endl;

        // space usage
        cout << "The Memory used in bytes is: " << cMin.getSize() <<  endl;


        // error calculations
        double totalError = 0;
        double totalError2 = 0;
        for (int i = 1; i <= reverse_dict.size(); i++) {
            uint64_t f = freq_dict[reverse_dict[i]];  // reverse dict, as we need a query to index freq_dict by (not a value)
            uint64_t fHat = cMin.estimate(i);
            auto error = (double) llabs(fHat - f) / f;
            totalError += error;
            double error2 = (double) llabs(fHat - f);
            totalError2 += error2;
        }
        cout << "average error is: " << totalError/reverse_dict.size() << endl;
        cout << "average total error is: " << totalError2/reverse_dict.size() << endl;
    }

    if (testRun == "CountTest") {

        // Loading the clean, original data
        vector<string> content;
        string line, word;
        fstream file ("../../ProcessedData/query_train.csv", ios::in);
        if(file.is_open()) {
            cout << "The file is open" << endl;
            while(getline(file, line)) {
                stringstream str(line);
                while(getline(str, word, ',')) {
                    content.push_back(word.substr(1, word.size() - 2));
                }
            }
        }


        // Loading the true frequencies
        vector<vector<string>> freq_vector;
        vector<string> row;
        string line2, elm;
        fstream file2 ("../../ProcessedData/checkData.csv", ios::in);
        if(file.is_open()) {
            cout << "The file is open" << endl;
            while(getline(file2, line2)) {
                row.clear();
                stringstream str(line2);
                while(getline(str, elm, ',')) {
                    row.push_back(elm);
                }
                freq_vector.push_back(row);
            }
        }

        map<string, int> freq_dict = {};
        for (int row = 1; row < freq_vector.size() ; row++) {
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
        for (int j=0; j < content.size();j++) {
            if (query_dict.find(content[j]) == query_dict.end()) {
                query_dict[content[j]] = id_count;
                reverse_dict[id_count] = content[j];
                id_count += 1;
            }
        }

        // Creating the data structure and updating each element
        countSketch cSketch(0.005, 0.005);


        // start time, update
        auto startTime = high_resolution_clock::now();

        for (string query: content) {
            cSketch.update(query_dict.at(query), 1);
        }

        // end time, update
        auto endTime = high_resolution_clock::now();
        auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);
        int elapsedTimeInt = elapsedTime.count(); // convert to integer
        cout << "update time: " << elapsedTimeInt << endl;

        // Querying elements from the data structure

        // start time, query all items in data stream
        auto startTime2 = high_resolution_clock::now();
        for (int i = 1; i <= query_dict.size(); i++) {
            cSketch.estimate(i);
            //string t =  reverse_dict.at(i);
        }
        //cout << "dictionary size is: " << query_dict.size() << endl;


        // end time, query all items in data stream
        auto endTime2 = high_resolution_clock::now();
        auto elapsedTime2 = duration_cast<milliseconds>(endTime2 - startTime2);
        int elapsedTimeInt2 = elapsedTime2.count();
        cout <<  "query time: " << elapsedTimeInt2 << endl;

        // space usage
        cout << "The Memory used in bytes is: " << cSketch.getSize() <<  endl;


        // error calculations
        double totalError = 0;
        double totalError2 = 0;
        for (int i = 1; i <= reverse_dict.size(); i++) {
            uint64_t f = freq_dict[reverse_dict[i]];  // reverse dict, as we need a query to index freq_dict by (not a value)
            uint64_t fHat = cSketch.estimate(i);
            auto error = (double) llabs(fHat - f) / f;
            double error2 = (double) llabs(fHat - f);
            totalError += error;
            totalError2 += error2;
        }
        cout << "average relative error is: " << totalError/reverse_dict.size() << endl;
        cout << "average total error is: " << totalError2/reverse_dict.size() << endl;
        cout << "total error is: " << totalError2;
    }


    //std::vector<int> L = HH.query();
//    for (int index: L) {
//        cout << reverse_dict.at(index) << endl;
//    }

    // Count Min Sketch and Count Sketch test section
 /*   srand(time(NULL));
    vector<int> data(1000000);
    // generate(data.begin(), data.end(), rand);
    //countMin cMin(0.03,0.2);
    float eps = 0.1;
    float delta = 0.4;
    countSketch cSketch(eps,delta);
    *//*for (int i = 0; i < 1000; i++) {
        cMin.update(i % 100, 5);  // everything has count 10
    }*//*
    //int m = 50000; // The test size of our universe
    int trueArray[250];
    for (int i = 0; i < 250 ; i++) {
        trueArray[i] = 0;
    }

    // Count Min Sketch
 /*   for (int i: data) {
        int num = (rand() % 500);
        trueArray[num] += 1;
        cMin.update(num, 5);
    }
    // cMin.test(5);
    int total = 0;
    for (int i = 1; i < 10; i++) {
        //cout << "Get the estimate:" << endl;
        cout << i << " is: " << cMin.estimate(i) << endl;
        cout << "The true answer is " << trueArray[i] << endl;
        total += cMin.estimate(i);
    }*/
    //cout << "total: " << total;

    // Count Sketch
//    int total_sum = 0;
//    for (int i: data) {
//        int num = (rand() % 250);
//        int r_delta = rand() % 20;
//        trueArray[num] += r_delta ;
//        cSketch.update(num, r_delta);
//        total_sum += r_delta;
//    }
//    cout << "Total sum is: " << total_sum << " and total sum times epsilon is: " << total_sum*eps << endl;
//    for (int i = 1; i < 10; i++) {
//        cout << i << " is: " << cSketch.estimate(i) << endl;
//        cout << "The true answer is " << trueArray[i] << endl;;
//    }*/



}
