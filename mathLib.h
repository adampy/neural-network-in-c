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

int addMatricesInto(Matrix* m1, Matrix* m2, Matrix* result);
int multiplyMatricesInto(Matrix* m1, Matrix* m2, Matrix* result);
int transposeMatrix(Matrix* m1, Matrix** result);
int hadamardProduct(Matrix* m1, Matrix* m2, Matrix** result);

void randomiseMatrix(Matrix* m);
void zeroMatrix(Matrix* m);

// --- Activation functions ---
void reluInto(Matrix* m, Matrix* output);
double drelu(int x);
void sigmoidInto(Matrix* m, Matrix* output);
double dsigmoid(int x);

// --- Backpropagation functions ---
int meanSquaredError(Matrix* actual, int expected);
#endif // MATH_LIB

// --- Helper functions ---
int indexOfMaxValue(Matrix* m);