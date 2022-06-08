#ifndef MATH_LIB
#define MATH_LIB

typedef struct _Matrix {
    double* values; // Stores all the values in a 1D matrix
    unsigned int rows;
    unsigned int columns;
} Matrix;

// --- Matrix functions ---
int makeMatrix(unsigned int rows, unsigned int columns, Matrix** m);
int freeMatrix(Matrix* m);

int addMatrices(Matrix* m1, Matrix* m2, Matrix** result);
int multiplyMatrices(Matrix* m1, Matrix* m2, Matrix** result);
int transposeMatrix(Matrix* m1, Matrix** result);
int hadamardProduct(Matrix* m1, Matrix* m2, Matrix** result);

// --- Activation functions ---
double relu(int x);
double sigmoid(int x);
double drelu(int x);
double dsigmoid(int x);

// --- Backpropagation functions ---
int meanSquaredError(Matrix* actual, int expected);
#endif // MATH_LIB