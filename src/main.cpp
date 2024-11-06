#include <stdio.h>
#include "matrix.hpp"

int main() {
    // == Dot product test ==
    printf("Matrix 1:\n");
    Matrix mat1(3,3);
    mat1.set_random_weights();
    mat1.print_matrix();

    printf("\nMatrix 2:\n");
    Matrix mat2(3,3);
    mat2.set_random_weights();
    mat2.print_matrix();

    //mat3 = mat1 * mat2
    printf("\nMatrix 3 (Dot product result):\n");
    Matrix mat3 = mat1.dot_product(mat2);
    mat3.print_matrix();

    return 0;
}