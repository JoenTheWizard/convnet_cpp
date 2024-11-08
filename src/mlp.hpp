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

    void train(const Matrix& inputs, const Matrix& targets, double learning_rate, int epochs) {
        for (int e = 0; e < epochs; e++) {
            forward(inputs);
            backpropagate(targets, learning_rate);
        }
    }

    const Matrix predict(const Matrix& input) {
        forward(input);
        return layers.back().get_outputs();
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
                //Calculate error for hidden layers
                Matrix hidden_error = error * layers[i + 1].get_weights().transpose();
                layers[i].backward(hidden_error, learning_rate);
                error = hidden_error;
            }
        }
    }
};

#endif