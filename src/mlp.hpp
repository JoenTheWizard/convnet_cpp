#ifndef MLP_H
#define MLP_H

#include "layer.hpp"
#include <vector>
#include <cmath>

class MLP {
private:
    std::vector<Layer> layers;

public:
    MLP(const std::vector<int>& layer_sizes) {
        for (size_t i = 0; i < layer_sizes.size() - 1; ++i) {
            layers.emplace_back(layer_sizes[i], layer_sizes[i + 1]);
        }
    }

    MLP(const std::vector<int>& layer_sizes, const std::vector<std::string>& activation_funcs) {
        if (layer_sizes.size() - 1 != activation_funcs.size()) {
            throw std::invalid_argument("[-] ERROR: Number of activation functions must match number of layers - 1");
        }
        for (size_t i = 0; i < layer_sizes.size() - 1; ++i) {
            layers.emplace_back(layer_sizes[i], layer_sizes[i + 1], activation_funcs[i]);
        }
    }

    MLP(const std::vector<Layer>& input_layers) : layers(input_layers) {}

    MLP(const std::string& filename) {
        load_model_binary(filename);
    }

    void train(const Matrix& inputs, const Matrix& targets, double learning_rate, int epochs) {
        for (int e = 0; e < epochs; e++) {
            forward(inputs);
            backpropagate(targets, learning_rate);
        }
    }

    //Call this method with batched sized inputs
    void train(const Matrix& inputs, const Matrix& targets, double learning_rate, int epochs, int batch_size) {
        int num_samples = inputs.get_rows();

        for (int e = 0; e < epochs; e++) {
            for (int start = 0; start < num_samples; start += batch_size) {
                //Determine where the batch will end
                int end = std::min(start + batch_size, num_samples);

                Matrix input_batch  = inputs.get_submatrix(start, end);
                Matrix target_batch = targets.get_submatrix(start, end);

                forward(input_batch);
                backpropagate(target_batch, learning_rate);
            }
        }
    }

    const Matrix predict(const Matrix& input) {
        forward(input);
        return layers.back().get_outputs();
    }

    void print_weights(void) {
        for (size_t i = 0; i < layers.size(); ++i) {
            std::cout << "Layer " << i << " Activation: " << layers[i].get_activation_function_name() << std::endl;
            layers[i].get_weights().print_matrix();
            std::cout << "=============" << std::endl;
        }
    }

    //Might refactor for better performance maybe?
    void save_model_binary(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("[-] ERROR: Unable to open file '"+ filename + "' to save model");
        }

        //Write number of layers (int format)
        int num_layers = layers.size();
        file.write(reinterpret_cast<const char*>(&num_layers), sizeof(int));
        
        for (const Layer& layer : layers) {
            const Matrix& weights = layer.get_weights();
            int rows = weights.get_rows(), cols = weights.get_columns();

            //Write number of rows and columns (int format)
            file.write(reinterpret_cast<const char*>(&rows), sizeof(int));
            file.write(reinterpret_cast<const char*>(&cols), sizeof(int));

            //Write the weights (double format)
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    double weight = weights(i, j);
                    file.write(reinterpret_cast<const char*>(&weight), sizeof(double));
                }
            }

            //Write the activation function index (uint8_t format)
            //NOTE: Might modify it to compare with function pointer rather than string for better performance
            const std::string& activation_func = layer.get_activation_function_name();
            uint8_t func_index = Activation::get_function_index(activation_func);
            file.write(reinterpret_cast<const char*>(&func_index), sizeof(uint8_t));
        }

        file.close();
    }
    
    //Might refactor for better performance maybe?
    void load_model_binary(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("[-] ERROR: Unable to open file '"+ filename + "' to load model");
        }

        int num_layers;
        file.read(reinterpret_cast<char*>(&num_layers), sizeof(int));

        layers.clear();
        for (int l = 0; l < num_layers; ++l) {
            int rows, cols;
            file.read(reinterpret_cast<char*>(&rows), sizeof(int));
            file.read(reinterpret_cast<char*>(&cols), sizeof(int));
            
            Matrix weights(rows, cols);
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    double weight;
                    file.read(reinterpret_cast<char*>(&weight), sizeof(double));
                    weights(i, j) = weight;
                }
            }
            
            uint8_t func_index;
            file.read(reinterpret_cast<char*>(&func_index), sizeof(uint8_t));
            std::string activation_name = Activation::get_function_name(func_index);

            layers.emplace_back(rows, cols, activation_name);
            layers.back().set_weights(weights);
        }
        
        file.close();
    }

private:
    void forward(const Matrix& input) {
        layers[0].forward(input);
        for (size_t i = 1; i < layers.size(); ++i) {
            layers[i].forward(layers[i - 1].get_outputs());
        }
    }

    void backpropagate(const Matrix& targets, double learning_rate) {
        //Calculate error at output layer
        Matrix error = layers.back().get_outputs() - targets;

        //Backpropagation through layers by iterating backwards
        for (size_t i = layers.size(); i-- > 0;) {
            if (i == layers.size() - 1) {
                //Output layer
                layers[i].backward(error, learning_rate);
            }
            else { 
                //Hidden layers

                //Calculate error for hidden layers.
                //NOTE: hidden_error is calculated with 'layers[i + 1].get_weights()' which is 
                //using the updated new weights from the previous layer's backward() pass rather 
                //than the old weights. Might modify, but still functions.

                Matrix hidden_error = error * layers[i + 1].get_weights().transpose();
                layers[i].backward(hidden_error, learning_rate);
                error = hidden_error;
            }
        }
    }
};

#endif