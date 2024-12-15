#ifndef ACTIVATION_H
#define ACTIVATION_H

#include "matrix.hpp"
#include <cmath>
#include <functional>
#include <map>

struct ActivationFunction {
    std::function<void(Matrix&)>          activate;   //Pointer to activation function
    std::function<void(Matrix&, Matrix&)> derivative; //Pointer to activation function's derivative function
};

class Activation {
private:
    Activation() = delete;

public:
    //User can add their own custom activation function
    //Do note that when saving, loading or training the model, with added custom activation functions,
    //to ensure that the same Activation object is used so indexing is preserved.
    static void add_function(const std::string& name, 
                             std::function<void(Matrix&)> activate,
                             std::function<void(Matrix&, Matrix&)> derivative) {
        if (functions.find(name) != functions.end()) {
            std::cerr << "[-] ERROR: Activation function '" << name << "' already exists." << std::endl;
            return;
        }
        functions[name] = {activate, derivative};
    }

    //Get the ActivationFunction from function name
    static const ActivationFunction& get_function(const std::string& name) {
        auto it = functions.find(name);
        if (it == functions.end()) {
            throw std::runtime_error("[-] ERROR: Activation function '" + name + "' not found.");
        }
        return it->second;
    }

    //Get the function name from specified numerical index from the functions hashmap
    static std::string get_function_name(uint8_t func_index) {
        uint8_t current_index = 0;
        for (const auto& pair : functions) {
            if (current_index == func_index) return pair.first;
            ++current_index;
        }
        return "sigmoid"; //Default to sigmoid if index is out of range
    }

    //Get the index of the function name from the functions hashmap
    static uint8_t get_function_index(const std::string& activation_func) {
        uint8_t func_index = 0;
        for (const auto& pair : functions) {
            if (pair.first == activation_func) return func_index;
            ++func_index;
        }
        throw std::runtime_error("[-] ERROR: Activation function not found: " + activation_func);
    }

    //Check if activation function is available in the functions hashmap
    static bool has_function(const std::string& name) {
        return functions.find(name) != functions.end();
    }

private:
    //Sigmoid activation function
    static void sigmoid(Matrix& mat) {
        for (int i = 0; i < mat.get_rows(); i++) {
            for (int j = 0; j < mat.get_columns(); j++) {
                mat(i, j) = 1.0 / (1.0 + exp(-mat(i, j)));
            }
        }
    }

    //Derivative of Sigmoid function 
    //Might need to modify since 'Matrix& output' might be outputted from a different activation function.
    static void sigmoid_derivative(const Matrix& output, Matrix& derivative) {
        for (int i = 0; i < output.get_rows(); i++) {
            for (int j = 0; j < output.get_columns(); j++) {
                derivative(i, j) = output(i, j) * (1 - output(i, j));
            }
        }
    }

    //ReLU activation function
    static void relu(Matrix& mat) {
        for (int i = 0; i < mat.get_rows(); i++) {
            for (int j = 0; j < mat.get_columns(); j++) {
                mat(i, j) = (mat(i, j) > 0) ? mat(i, j) : 0;
            }
        }
    }

    //Derivative of ReLU function
    static void relu_derivative(const Matrix& output, Matrix& derivative) {
        for (int i = 0; i < output.get_rows(); i++) {
            for (int j = 0; j < output.get_columns(); j++) {
                derivative(i, j) = (output(i, j) > 0) ? 1 : 0;
            }
        }
    }

    //Tanh activation function
    static void tanh(Matrix& mat) {
        for (int i = 0; i < mat.get_rows(); i++) {
            for (int j = 0; j < mat.get_columns(); j++) {
                mat(i, j) = std::tanh(mat(i, j));
            }
        }
    }

    //Derivative of Tanh function
    static void tanh_derivative(const Matrix& output, Matrix& derivative) {
        for (int i = 0; i < output.get_rows(); i++) {
            for (int j = 0; j < output.get_columns(); j++) {
                derivative(i, j) = 1 - std::pow(output(i, j), 2);
            }
        }
    }

    //Main functions map. It initially stores the default activation functions
    static inline std::map<std::string, ActivationFunction> functions = {
        {"tanh",    {tanh, tanh_derivative}},
        {"relu",    {relu, relu_derivative}},
        {"sigmoid", {sigmoid, sigmoid_derivative}}
    };
};

#endif