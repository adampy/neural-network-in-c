#ifndef MATRIX_MATH
#define MATRIX_MATH

typedef struct _matrix {
    double* values; // Stores all the values in a 1D matrix
    unsigned int rows;
    unsigned int columns;
} matrix;

#endif // MATRIX_MATH