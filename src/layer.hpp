#ifndef LAYER_H
#define LAYER_H

#include "matrix.hpp"
#include "activation.hpp"
#include <vector>
#include <cmath>

class Layer {
private:
    Matrix weights; //Weights matrix for this layer
    Matrix outputs; //Outputs from this layer
    Matrix inputs;  //Inputs to this layer

    //Activation function
    ActivationFunction activation_function;

public:
    Layer(int input_size, int output_size, const std::string& activation_name = "sigmoid") 
        : weights(input_size, output_size), outputs(output_size, 1),
          inputs(input_size, 1) {
        //Initialize weights randomly
        weights.set_random_weights();

        //Initialize the activation function
        Activation activation;
        if (activation.functions.find(activation_name) == activation.functions.end()) {
            std::cerr << "[*] WARNING: Invalid activation function '" << activation_name 
                      << "' specified. Defaulting to sigmoid." << std::endl;
            activation_function = activation.functions["sigmoid"];
        }
        else {
            activation_function = activation.functions[activation_name];
        }
    }

    const Matrix& get_outputs() const {
        return outputs;
    }

    const Matrix& get_weights() const {
        return weights;
    }

    void set_weights(const Matrix& new_weights) {
        if (new_weights.get_rows() != weights.get_rows() || new_weights.get_columns() != weights.get_columns()) {
            throw std::invalid_argument("[-] ERROR: New weights dimensions do not match existing weights.");
        }
        weights = new_weights;
    }

    void forward(const Matrix& input) {
        inputs = input;
        Matrix net_input = inputs * weights;
        activation_function.activate(net_input);
        outputs = net_input;
    }

    void backward(const Matrix& error, double learning_rate) {
        //Calculate derivative
        Matrix derivative(outputs.get_rows(), outputs.get_columns());
        activation_function.derivative(outputs, derivative);

        for (int i = 0; i < outputs.get_rows(); i++) {
            for (int j = 0; j < outputs.get_columns(); j++) {
                derivative(i, j) = error(i, j) * derivative(i, j);
            }
        }

        //Update weights
        Matrix delta_weights = inputs.transpose() * derivative;
        for (int i = 0; i < weights.get_rows(); i++) {
            for (int j = 0; j < weights.get_columns(); j++) {
                weights(i, j) -= learning_rate * delta_weights(i, j);
            }
        }
    }
};

#endif