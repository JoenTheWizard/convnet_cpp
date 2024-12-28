#ifndef MLP_H
#define MLP_H

#include "layer.hpp"
#include <vector>
#include <cmath>

//8-bit signed integer used for quantization (currently unused)
using WeightType = int8_t;

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

    /// ================ QUANTIZATION RELATED METHODS ================ ///

    //Return number of bits of integers from largest weight
    uint8_t find_largest_weight() const {
        double max = 0;
        for (const auto& layer : layers) {
            const Matrix& weights = layer.get_weights();
            for (int i = 0; i < weights.get_rows(); ++i) {
                for (int j = 0; j < weights.get_columns(); ++j) {
                    double w = std::abs(weights(i,j));
                    if (w > max) max = w;
                }
            }
        }

        int integerPart = static_cast<int>(max);
        return (integerPart == 0) ? 0 : static_cast<uint8_t>(std::log2(integerPart)) + 1;
    }

    int8_t quantize(double value, uint8_t max) const {
        return static_cast<int8_t>(std::round(value * (1 << (7 - max))));
    }

    double dequantize(int8_t value, uint8_t max) const {
        return static_cast<double>(value) / (1 << (7 - max));
    }

    //Save model via quantization
    void save_model_binary_8bit(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("[-] ERROR: Unable to open file '" + filename + "' to save model");
        }

        int num_layers = layers.size();
        file.write(reinterpret_cast<const char*>(&num_layers), sizeof(int));

        uint8_t max_weight = find_largest_weight();
        file.write(reinterpret_cast<const char*>(&max_weight), sizeof(uint8_t));

        for (const Layer& layer : layers) {
            const Matrix& weights = layer.get_weights();
            int rows = weights.get_rows(), cols = weights.get_columns();

            file.write(reinterpret_cast<const char*>(&rows), sizeof(int));
            file.write(reinterpret_cast<const char*>(&cols), sizeof(int));

            //Write quantized weights
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    int8_t quantized_weight = quantize(weights(i, j), max_weight);
                    file.write(reinterpret_cast<const char*>(&quantized_weight), sizeof(int8_t));
                }
            }

            uint8_t func_index = Activation::get_function_index(layer.get_activation_function_name());
            file.write(reinterpret_cast<const char*>(&func_index), sizeof(uint8_t));
        }

        file.close();
    }

    void load_model_binary_8bit(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("[-] ERROR: Unable to open file '" + filename + "' to load model");
        }

        int num_layers;
        file.read(reinterpret_cast<char*>(&num_layers), sizeof(int));

        uint8_t max_weight;
        file.read(reinterpret_cast<char*>(&max_weight), sizeof(uint8_t));

        layers.clear();
        for (int l = 0; l < num_layers; ++l) {
            int rows, cols;
            file.read(reinterpret_cast<char*>(&rows), sizeof(int));
            file.read(reinterpret_cast<char*>(&cols), sizeof(int));

            Matrix weights(rows, cols);
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    int8_t quantized_weight;
                    file.read(reinterpret_cast<char*>(&quantized_weight), sizeof(int8_t));
                    weights(i, j) = static_cast<double>(quantized_weight); //dequantize(quantized_weight, max_weight);
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