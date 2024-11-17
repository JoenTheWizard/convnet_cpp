#ifndef DATASET_READER_H
#define DATASET_READER_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "matrix.hpp"
#include <string>
#include <fstream>
#include <filesystem>
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
    //Specified number of output columns, will be set in the constructor
    int num_outputs;

public:
    DatasetReader(int num_outputs = 1) : features(0, 0), labels(0, 0) {
        this->num_outputs = num_outputs;
    }

    //Read CSV file (default delimiter is comma and ignore first row 'header' by default)
    //This will read the data as double types
    //NOTE: Might need to improve this function by removing use of 'data' vector vector double
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

        features = Matrix(rows, cols - num_outputs);
        labels   = Matrix(rows, num_outputs);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols - num_outputs; j++) {
                features(i, j) = data[i][j];
            }
            for (int j = 0; j < num_outputs; j++) {
                labels(i, j) = data[i][cols - num_outputs + j];
            }
        }
    }

    //Read CSV file for classification. Will map outputs accordingly to the label matrix
    //Ensure that the label_map input is in the same orientation for the output model
    //NOTE: Might need to improve this function by removing use of 'data' vector vector string
    void read_csv_classification(const std::string& filename, std::vector<std::string> label_map, bool has_header = true, char delimiter = ',') {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("[-] ERROR Dataset_reader.cpp: Unable to open file: " + filename);
        }

        std::string line;
        std::vector<std::vector<std::string>> data;

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
            std::vector<std::string> row;
            while (std::getline(iss, token, delimiter)) {
                row.push_back(token);
            }
            data.push_back(row);
        }

        int rows = data.size();
        int cols = data[0].size();

        size_t label_size = label_map.size();

        //Set num_outputs to the number of classes
        this->num_outputs = label_size;
        
        features = Matrix(rows, cols - 1);
        labels   = Matrix(rows, label_size);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols - 1; j++) {
                features(i, j) = std::stod(data[i][j]);
            }
            for (int j = 0; j < label_size; j++) {
                if (label_map[j] == data[i][cols - 1]) {
                    labels(i, j) = 1;
                }
            }
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
        Matrix y_train(train_samples, num_outputs);
        Matrix X_test(test_samples, features.get_columns());
        Matrix y_test(test_samples, num_outputs);

        //Fill in the train set matricies 
        for (int i = 0; i < train_samples; ++i) {
            for (int j = 0; j < features.get_columns(); ++j) {
                X_train(i, j) = features(indices[i], j);
            }
            for (int j = 0; j < num_outputs; ++j) {
                y_train(i, j) = labels(indices[i], j);
            }
        }

        //Fill in the test set matricies 
        for (int i = 0; i < test_samples; ++i) {
            for (int j = 0; j < features.get_columns(); ++j) {
                X_test(i, j) = features(indices[train_samples + i], j);
            }
            for (int j = 0; j < num_outputs; ++j) {
                y_test(i, j) = labels(indices[train_samples + i], j);
            }
        }

        //Return train and test sets
        return {X_train, X_test, y_train, y_test};
    }

    //Reads images from a folder structure where subfolders represent different classifications. 
    //This function will read the images and produce a CSV file with pixel values as features and subfolder names as labels
    void generate_image_dataset_csv(const std::string& folder_path, const std::string& output_csv) {
        //Write to data file
        std::ofstream data_file;
        data_file.open(output_csv);

        for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
            if (entry.is_directory()) {
                std::string label = entry.path().filename().string();
                for (const auto& img : std::filesystem::directory_iterator(entry.path())) {
                    if (img.is_regular_file()) {

                        int width, height, channels;
                        unsigned char* img_data = stbi_load(img.path().string().c_str(), &width, &height, &channels, 0);

                        if (img_data) {
                            //Start writing the classifications to the CSV file
                            // data_file << "'" << label << "'"<< std::endl;
                            for (int i = 0; i < width * height * channels; i++) {
                                //Write the value
                                data_file << static_cast<double>(img_data[i]) / 255 << ",";
                                // double t = static_cast<double>(img_data[i]) / 255;
                                // int val = t > 0.5 ? 1 : 0;
                                // data_file << val;
                                // data_file << (((i+1) % width == 0) ? "\n" : " ");
                            }
                            data_file << label << "\n";
                            stbi_image_free(img_data);
                        }

                    }
                }
            }
        }

    }

    //Get methods
    const Matrix& get_features() const { return features; }
    const Matrix& get_labels()   const { return labels; }  
    const std::vector<std::string>& get_header() const { return header; }
};

#endif