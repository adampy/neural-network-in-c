#ifndef MATH_LIB
#define MATH_LIB

#include <stdio.h>

typedef struct _Matrix {
    double* values; // Stores all the values in a 1D matrix
    unsigned int rows;
    unsigned int columns;
} Matrix;

// --- Matrix functions ---
int makeMatrix(unsigned int rows, unsigned int columns, Matrix** m);
int freeMatrix(Matrix* m);

// --- IO Functions ---
int loadMatrixInto(Matrix* m, char* inputFilename);
int saveMatrix(Matrix* m, char* outputFilename);

// --- Operations ---
int addMatricesInto(Matrix* m1, Matrix* m2, Matrix* result);
int multiplyScalarInto(Matrix* m1, double scalar, Matrix* result);
int multiplyMatricesInto(Matrix* m1, Matrix* m2, Matrix* result);
int transposeMatrix(Matrix* m1, Matrix** result);
int hadamardProduct(Matrix* m1, Matrix* m2, Matrix** result);

void randomiseMatrix(Matrix* m);
void zeroMatrix(Matrix* m);
void negateMatrix(Matrix* m);

// --- Activation functions ---
int reluInto(Matrix* m, Matrix* output);
int dreluInto(Matrix* m, Matrix* output);
int sigmoidInto(Matrix* m, Matrix* output);
int dsigmoidInto(Matrix* m, Matrix* output);

// --- Helper functions ---
int indexOfMaxValue(Matrix* m, int* indx);

/**
 * Generates a random number from the standard normal distribution
 * using the Marsaglia polar method. A faster method like the
 * Ziggurat method could be used but this method is easier to implement. // TODO: Try implementing Ziggurat method
 */
double randn();

#endif // MATH_LIB