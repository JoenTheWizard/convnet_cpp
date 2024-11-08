#include <iostream>
#include <fstream>
#include <sstream>
#include "mlp.hpp"

void read_xor_dataset(Matrix& inputs, Matrix& outputs) {
    std::ifstream file("xor_dataset.data");
    if (!file.is_open()) {
        std::cerr << "Error opening the dataset file!" << std::endl;
        return;
    }

    std::string line;
    int index = 0;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float input1, input2, output;
        
        if (!(iss >> input1)) break;
        
        iss.ignore(1);
        if (!(iss >> input2)) break;

        iss.ignore(1);
        if (!(iss >> output)) break;

        inputs(index, 0) = input1;
        inputs(index++, 1) = input2;
        
        outputs(index - 1, 0) = output;
    }
}

int main() {
    int num_inputs = 2;
    int num_hidden = 10; 
    int num_outputs = 1;
    
    double learning_rate = 0.1;
    int epochs = 100000;

    Matrix inputs(4, num_inputs); 
    Matrix outputs(4, num_outputs);

    read_xor_dataset(inputs, outputs);

    MLP mlp({num_inputs, num_hidden, num_outputs});
    
    mlp.train(inputs, outputs, learning_rate, epochs);

    //=== Test predictions ===
    for (int i = 0; i < inputs.get_rows(); ++i) {
        auto prediction = mlp.predict(inputs.get_row_matrix(i));
        std::cout << "Input: (" << inputs(i, 0) << ", " << inputs(i, 1)
                  << ") => Prediction: " << prediction(0, 0) << std::endl;
    }

    return 0;
}