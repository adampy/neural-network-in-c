#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "image.h"
#include "mathLib.h"
#include "neuralNetwork.h"

void printImage(Image* img) {
    for (int i = 0; i < img->rows; i++) {
        for (int j = 0; j < img->columns; j++) {
            printf("%i\t", img->imageData[i][j]);
        }
        printf("\n");
    }
    printf("Label: %i\n", img->label);
}

void printMatrix(Matrix* m) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            printf("%.10lf\t", m->values[i * m->columns + j]);
        }
        printf("\n");
    }
}

void printNetwork(NeuralNetwork* network) {
    printf("Dimensions are printed in row*column format...\n");
    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        printf("w_%i: %i*%i, b_%i: %i*%i\n", i, network->weights[i]->rows,
                                             network->weights[i]->columns, i,
                                             network->biases[i]->rows,
                                             network->biases[i]->columns);
    }
}

void shuffle(Image** array, int n) {
    srand(time(NULL));

    if (n > 1) {
        for (int i = n - 1; i > 0; i--) {
            int j = (unsigned int) (((double)rand()/RAND_MAX)*(i+1));
            Image* t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}