#include <stdlib.h>
#include <time.h> // For random number generation
#include "err.h"
#include "mathLib.h"
#include "neuralNetwork.h"

int makeNetwork(unsigned int hiddenLayers, unsigned int* neurons,
                double learningRate, NeuralNetwork** network) {
    // Allocate memory
    (*network) = malloc(sizeof(NeuralNetwork));
    if (*network == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }
    (*network)->hiddenLayers = hiddenLayers;
    (*network)->learningRate = learningRate;
    (*network)->neurons = neurons;  

    // n hidden layers -> n+1 sets of weights & n+1 sets of biases
    (*network)->weights = malloc((hiddenLayers + 1) * sizeof(Matrix));
    (*network)->biases = malloc((hiddenLayers + 1) * sizeof(Matrix));
    if ((*network)->weights == NULL || (*network)->biases == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }

    // Make matrices
    for (int i = 0; i < hiddenLayers + 1; i++) {
        Matrix* weights = NULL;
        Matrix* biases = NULL;
        int returnCode = makeMatrix(neurons[i+1], neurons[i], &weights);
        if (returnCode != SUCCESS) {
            return returnCode;
        }
        returnCode = makeMatrix(neurons[i+1], 1, &biases);
        if (returnCode != SUCCESS) {
            return returnCode;
        }

        // Assign random values to matrices from -2 to 2
        randomiseMatrix(weights);
        randomiseMatrix(biases);

        // Put into neural network
        (*network)->weights[i] = weights;
        (*network)->biases[i] = biases;
    }
    return SUCCESS;
}

void freeNetwork(NeuralNetwork* network) {
    // Free all matrices
    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        Matrix* weights = network->weights[i];
        Matrix* biases = network->biases[i];
        freeMatrix(weights);
        freeMatrix(biases);
    }
    // Free pointer arrays
    free(network->weights);
    free(network->biases);
    // Free network itself
    free(network);
}