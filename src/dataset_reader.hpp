#ifndef DATASET_READER_H
#define DATASET_READER_H

#include "matrix.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>

//When splitting data, we can store the train and test sets
//respectively in the SplitData struct to group the inputs and outputs
struct SplitData {
    Matrix X_train;
    Matrix X_test;
    Matrix y_train;
    Matrix y_test;
};

class DatasetReader {
private:
    Matrix features; //X (inputs)
    Matrix labels;   //y (outputs)
    std::vector<std::string> header;

public:
    DatasetReader() : features(0, 0), labels(0, 0) {}

    //Read CSV file (default delimiter is comma and ignore first row 'header' by default)
    //This will read the data as double types
    void read_csv(const std::string& filename, bool has_header = true, char delimiter = ',') {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("[-] ERROR Dataset_reader.cpp: Unable to open file: " + filename);
        }

        std::string line;
        std::vector<std::vector<double>> data;

        //Read header if available
        if (has_header && std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            while (std::getline(iss, token, delimiter)) {
                header.push_back(token);
            }
        }

        //Read data
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            std::vector<double> row;
            while (std::getline(iss, token, delimiter)) {
                row.push_back(std::stod(token));
            }
            data.push_back(row);
        }

        int rows = data.size();
        int cols = data[0].size();

        features = Matrix(rows, cols - 1);
        labels   = Matrix(rows, 1);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                features(i, j) = data[i][j];
            }
            labels(i, 0) = data[i][cols - 1];
        }
    }

    //Split data to train-test (default 80-20)
    //Default seed parameter is set randomly but can be set manually
    SplitData train_test_split(double test_size = 0.2, unsigned seed = std::random_device{}()) {
        int total_samples = features.get_rows();
        int test_samples  = static_cast<int>(total_samples * test_size);
        int train_samples = total_samples - test_samples;

        //Create a vector of indices and then shuffle it
        std::vector<int> indices(total_samples);
        std::iota(indices.begin(), indices.end(), 0);
        std::mt19937 g(seed);
        std::shuffle(indices.begin(), indices.end(), g);

        //Split the data
        Matrix X_train(train_samples, features.get_columns());
        Matrix y_train(train_samples, 1);
        Matrix X_test(test_samples, features.get_columns());
        Matrix y_test(test_samples, 1);

        for (int i = 0; i < train_samples; ++i) {
            for (int j = 0; j < features.get_columns(); ++j) {
                X_train(i, j) = features(indices[i], j);
            }
            y_train(i, 0) = labels(indices[i], 0);
        }

        for (int i = 0; i < test_samples; ++i) {
            for (int j = 0; j < features.get_columns(); ++j) {
                X_test(i, j) = features(indices[train_samples + i], j);
            }
            y_test(i, 0) = labels(indices[train_samples + i], 0);
        }

        //Return train and test sets
        return {X_train, X_test, y_train, y_test};
    }

    //Get methods
    const Matrix& get_features() const { return features; }
    const Matrix& get_labels()   const { return labels; }  
    const std::vector<std::string>& get_header() const { return header; }
};

#endif