#ifndef ACTIVATION_H
#define ACTIVATION_H

#include "matrix.hpp"
#include <cmath>
#include <functional>
#include <unordered_map>

struct ActivationFunction {
    std::function<void(Matrix&)>          activate;   //Pointer to activation function
    std::function<void(Matrix&, Matrix&)> derivative; //Pointer to activation function's derivative function
};

class Activation {
public:
    std::unordered_map<std::string, ActivationFunction> functions;
    
    Activation() {
        //Map the activation functions
        functions["sigmoid"] = {sigmoid, sigmoid_derivative};    
        functions["relu"]    = {relu, relu_derivative};    
        functions["tanh"]    = {tanh, tanh_derivative};    
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

};

#endif