#include <stdlib.h>
#include <time.h> // For srand
#include "err.h"
#include "mathLib.h"

int makeMatrix(unsigned int rows, unsigned int columns, Matrix** m) {
    *m = malloc(sizeof(Matrix));
    if (*m == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }

    (*m)->rows = rows;
    (*m)->columns = columns;
    (*m)->values = calloc(rows*columns, sizeof(double));
    return SUCCESS;
}

int freeMatrix(Matrix* m) {
    free(m->values);
    free(m);
    return SUCCESS;
}

int addMatrices(Matrix* m1, Matrix* m2, Matrix** result) {
    // Check dimensions
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return reportError(MISC, "addMatrices error: must have same dimensions");
    }

    // Allocate result matrix if not allocated
    if (*result == NULL) {
        int made = makeMatrix(m1->rows, m1->columns, result);
        if (made != SUCCESS) {
            return made;
        }  
    }
   
    // Move addition into result vector
    for (int i = 0; i < m1->rows * m1->columns; i++) {
       (*result)->values[i] = m1->values[i] + m2->values[i];
    }
    return SUCCESS;
}

int multiplyMatrices(Matrix* m1, Matrix* m2, Matrix** result) {
    // Check dimensions
    if (m1->columns != m2->rows) {
        return reportError(MISC, "multiplyMatrices error:  matrices cannot be multiplied");
    }
    
    // Allocate result matrix if not allocated
    if (*result == NULL) {
        int made = makeMatrix(m1->rows, m2->columns, result);
        if (made != SUCCESS) {
            return made;
        }  
    }

    // Apply matrix multiplication
    // for m2 columns
    // for m1 rows
    // add elements with offset of 0
    // then 

    for (int i = 0; i < m2->columns; i++) {
        for (int j = 0; j < m1->rows; j++) {
            double sum = 0;
            for (int k = 0; k < m1->columns; k++) {
                // Add m1[j][k] * m2[k][i] to sum
                double first_element = m1->values[j * m1->columns + k];
                double second_element = m2->values[k * m2->columns + i];
                sum += (first_element * second_element);
            }
            // Put into result[j][i]
            (*result)->values[j * (*result)->columns + i] = sum;
        }
    }
    return SUCCESS;
}

int transposeMatrix(Matrix* m1, Matrix** result) {
    // Create matrix with dimensions transposed
    if (*result == NULL) {
        int made = makeMatrix(m1->columns, m1->rows, result);
        if (made != SUCCESS) {
            return made;
        }
    }

    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m1->columns; j++) {
            int originalIndex = i * m1->columns + j;
            int toIndex = j * m1 -> rows + i;
            (*result)->values[toIndex] = m1->values[originalIndex];
        }
    }
    return SUCCESS;
}

int hadamardProduct(Matrix* m1, Matrix* m2, Matrix** result) {
    // Check dimensions
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return reportError(MISC, "hadamardProduct error: must have same dimensions");
    }

    // Create matrix
    if (*result == NULL) {
        int made = makeMatrix(m1->rows, m1->columns, result);
        if (made != SUCCESS) {
            return made;
        }
    }

    // Move hadamard products into result vector
    for (int i = 0; i < m1->rows * m1->columns; i++) {
        (*result)->values[i] = m1->values[i] * m2->values[i];
    }
    return SUCCESS;
}

void randomiseMatrix(Matrix* m) {
    // Assign random values to matrices from -2 to 2 TODO: Dynamic range
    srand(time(NULL));
    for (int i = 0; i < m->rows * m->columns; i++) {
        m->values[i] = 4 * ((double) rand() / RAND_MAX) - 2;
    }
}

// --- Activation functions ---
double relu(int x) {
    return (double) x * (x > 0);
}
double drelu(int x) {
    return x > 0;
}

double sigmoid(int x) {
    int abs = x;
    if (abs < 0) {
        abs = -abs;
    }
    return 0.5*(x/(1+abs)) + 0.5;
}
double dsigmoid(int x) {
    double s = sigmoid(x);
    return s*(1-s);
}

// --- Backpropagation functions ---
int meanSquaredError(Matrix* actual, int expected);