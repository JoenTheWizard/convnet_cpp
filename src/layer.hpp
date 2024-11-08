#ifndef LAYER_H
#define LAYER_H

#include "matrix.hpp"
#include <vector>
#include <cmath>

class Layer {
private:
    Matrix weights; //Weights matrix for this layer
    Matrix outputs; //Outputs from this layer
    Matrix inputs;  //Inputs to this layer

public:
    Layer(int input_size, int output_size) 
        : weights(input_size, output_size), outputs(output_size, 1), inputs(input_size, 1) {
        //Initialize weights randomly
        weights.set_random_weights();
    }

    const Matrix& get_outputs() const {
        return outputs;
    }

    const Matrix& get_weights() const {
        return weights;
    }

    void forward(const Matrix& input) {
        inputs = input;
        Matrix net_input = inputs * weights;
        sigmoid(net_input);
        outputs = net_input;
    }

    void backward(const Matrix& error, double learning_rate) {
        //Sigmoid derivative
        Matrix sig_derivative(outputs.get_rows(), outputs.get_columns());
        for (int i = 0; i < outputs.get_rows(); i++) {
            for (int j = 0; j < outputs.get_columns(); j++) {
                sig_derivative(i, j) = error(i, j) * outputs(i, j) * (1 - outputs(i, j));
            }
        }

        //Update weights
        Matrix delta_weights = inputs.transpose() * sig_derivative;
        for (int i = 0; i < weights.get_rows(); i++) {
            for (int j = 0; j < weights.get_columns(); j++) {
                weights(i, j) -= learning_rate * delta_weights(i, j);
            }
        }
    }

private:
    void sigmoid(Matrix& mat) {
        for (int i = 0; i < mat.get_rows(); i++) {
            for (int j = 0; j < mat.get_columns(); j++) {
                mat(i, j) = 1.0 / (1.0 + exp(-mat(i, j)));
            }
        }
    }
};

#endif