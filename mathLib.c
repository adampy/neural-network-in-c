#include <stdlib.h>
#include <time.h> // For srand
#include <math.h> // For exp()
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
    zeroMatrix(*m);
    return SUCCESS;
}

int freeMatrix(Matrix* m) {
    free(m->values);
    free(m);
    return SUCCESS;
}

int addMatricesInto(Matrix* m1, Matrix* m2, Matrix* result) {
    // Check dimensions
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return reportError(MISC, "addMatricesInto error: must have same dimensions");
    }
    if (m1->rows != result->rows || m1->columns != result->columns) {
        return reportError(MISC, "addMatricesInto error: result matrix doesn't have appropriate dimensions");
    }

    // Move addition into result vector
    for (int i = 0; i < m1->rows * m1->columns; i++) {
       result->values[i] = m1->values[i] + m2->values[i];
    }
    return SUCCESS;
}

int multiplyScalarInto(Matrix* m1, double scalar, Matrix* result) {
    for (int i = 0; i < m1->rows * m1->columns; i++) {
        result->values[i] = scalar * m1->values[i];
    }
    return SUCCESS;
}

int multiplyMatricesInto(Matrix* m1, Matrix* m2, Matrix* result) {
    // Check dimensions
    if (m1->columns != m2->rows) {
        return reportError(MISC, "multiplyMatricesInto error:  matrices cannot be multiplied");
    }
    if (m1->rows != result->rows || m2->columns != result->columns) {
        return reportError(MISC, "multiplyMatricesInto error: result matrix doesn't have appropriate dimensions");
    }

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
            result->values[j * result->columns + i] = sum;
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
            int toIndex = j * m1->rows + i;
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
    srand(time(NULL));
    for (int i = 0; i < m->rows * m->columns; i++) {
        m->values[i] = randn();
    }
}

void zeroMatrix(Matrix* m) {
    for (int i = 0; i < m->rows * m->columns; i++) {
        m->values[i] = 0;
    }
}

void negateMatrix(Matrix* m) {
    for (int i = 0; i < m->rows * m->columns; i++) {
        m->values[i] = -m->values[i];
    }
}

// --- Activation functions ---
int reluInto(Matrix* m, Matrix* output) {
    if (m->rows != output->rows || m->columns != output->columns) {
        return reportError(MISC, "reluInto error: output matrix must have the same dimensions as the input");
    }

    for (int i = 0; i < m->rows * m->columns; i++) {
        double x = m->values[i];
        output->values[i] = (double) x * (x > 0);
    }
    return SUCCESS;
}
int dreluInto(Matrix* m, Matrix* output) {
    if (m->rows != output->rows || m->columns != output->columns) {
        return reportError(MISC, "dreluInto error: output matrix must have the same dimensions as the input");
    }
    
    for (int i = 0 ; i < m->rows * m->columns; i++) {
        output->values[i] = (m->values[i] > 0);
    }
    return SUCCESS;
}

int sigmoidInto(Matrix* m, Matrix* output) {
    if (m->rows != output->rows || m->columns != output->columns) {
        return reportError(MISC, "sigmoidInto error: output matrix must have the same dimensions as the input");
    }

    for (int i = 0; i < m->rows * m->columns; i++) {
        double x = m->values[i];
        output->values[i] = 1/(1+exp(-x));
    }
    return SUCCESS;
}
int dsigmoidInto(Matrix* m, Matrix* output) {
    if (m->rows != output->rows || m->columns != output->columns) {
        return reportError(MISC, "dsigmoidInto error: output matrix must have the same dimensions as the input");
    }

    for (int i = 0 ; i < m->rows * m->columns; i++) {
        double x = m->values[i];
        double s = 1/(1+exp(-x));
        output->values[i] = s*(1-s);
    }
    return SUCCESS;
}

// --- Helper functions ---
int indexOfMaxValue(Matrix* m, int* indx) {
    if (m->columns != 1) {
        return reportError(MISC, "indexOfMaxValue error: input matrix must have 1 column");
    }

    double max = -1;
    for (int i = 0; i < m->rows; i++) {
        if (m->values[i] > max) {
            *indx = i;
            max = m->values[i];
        }
    }
    return SUCCESS;
}

double randn() {
    static double spare;
    static int hasSpare = 0;

    if (hasSpare == 1) {
        hasSpare = 0;
        return spare;
    } else {
        double u, v, s;
        do {
            u = ((double) rand() / RAND_MAX) * 2.0 - 1.0; // [0, 1] -> [-1, 1]
            v = ((double) rand() / RAND_MAX) * 2.0 - 1.0;
            s = u * u + v * v;
        } while (s >= 1.0 || s == 0.0);
        s = sqrt(-2.0 * log(s) / s);
        spare = v * s;
        hasSpare = 1;
        return u * s;
    }
}