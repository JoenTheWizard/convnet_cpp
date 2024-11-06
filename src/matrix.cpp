#include "matrix.hpp"

//Get the rows specified from constructor
int Matrix::get_rows() const {
    return rows;
}

//Get the columns specified from constructor
int Matrix::get_columns() const {
    return columns;
}

//Initialise random weights to the matrix
//Pseudo-RNG w/ Mersenne Twiser (https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution)
void Matrix::set_random_weights() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);

    //Set each value from distribution
    for (double& val : data) {
        val = dis(gen);
    }
}

//Print the formatted matrix
void Matrix::print_matrix() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            std::cout << (*this)(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

Matrix Matrix::dot_product(const Matrix& other) const {
    if (columns != other.rows) {
        throw std::invalid_argument("[-] ERROR Matrix.cpp: Number of columns (mat1) is not the same as number of rows (mat2) for dot product");
    }

    Matrix result(rows, other.columns);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < other.columns; j++) {
            for (int k = 0; k < columns; k++) {
                result(i, j) += (*this)(i, k) * other(k, j);
            }
        }
    }

    return result;
}

Matrix Matrix::subtract(const Matrix& other) const {
    if (rows != other.rows || columns != other.columns) {
        throw std::invalid_argument("[-] ERROR Matrix.cpp: Dimensions of mat1 is not the same as the dimensions of mat2 for matrix subtraction");
    }

    Matrix result(rows, columns);

    for (size_t i = 0; i < data.size(); i++) {
        result.data[i] = data[i] - other.data[i];
    }

    return result;
}