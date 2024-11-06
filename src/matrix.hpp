#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <random>
#include <iostream>
#include <cmath>

class Matrix {
private:
    int rows, columns;

    //One-dimensional vector, treated as 2D
    std::vector<double> data;

    //Index obtain from 1D data vector
    inline size_t index(int i, int j) const { return i * columns + j; }

public:
    Matrix(int rows, int columns) : rows(rows), columns(columns), data(rows * columns, 0.0) {};

    //operator() for matrix. Example: 
    //Matrix mat(3,3);       //Define 3 x 3
    //double val = mat(0,0); //Gets 0th row, 0th column 
    double&       operator()(int i, int j)       { return data[index(i, j)]; }
    const double& operator()(int i, int j) const { return data[index(i, j)]; }

    //Mathematical matrix operations (* for dot product, - for subtraction )
    Matrix operator-(const Matrix& other) const { return subtract(other); }
    Matrix operator*(const Matrix& other) const { return dot_product(other); }

    //Get methods
    int get_rows()    const;
    int get_columns() const;

    //Initialise random weights to the matrix
    //Pseudo-RNG w/ Mersenne Twister 
    void set_random_weights();

    //Print the formatted matrix
    void print_matrix() const;

    //Dot product, returns matrix
    Matrix dot_product(const Matrix& other) const;

    //Subtract matricies, returns matrix
    Matrix subtract(const Matrix& other) const;

    //Transpose matrix, returns matrix
    Matrix transpose() const;
};

#endif