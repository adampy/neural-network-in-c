#ifndef MATH_LIB
#define MATH_LIB

typedef struct _matrix {
    double* values; // Stores all the values in a 1D matrix
    unsigned int rows;
    unsigned int columns;
} matrix;

// --- Matrix functions ---
int makeMatrix(unsigned int rows, unsigned int columns, matrix** m);
int freeMatrix(matrix* m);

int addMatrices(matrix* m1, matrix* m2);
int multiplyMatrices(matrix* m1, matrix* m2);
int transposeMatrix(matrix* m1);
int hadamardProduct(matrix* m1, matrix* m2);

// --- Activation functions ---
double relu(int x);
double sigmoid(int x);
double drelu(int x);
double dsigmoid(int x);

// --- Backpropagation functions ---
int meanSquaredError(matrix* actual, int expected);
#endif // MATH_LIB