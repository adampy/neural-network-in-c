#include <stdlib.h>
#include <time.h> // For random number generation
#include "neuralNetwork.h" // TODO: Remove all includes and put them in headers
#include "imageInput.h" // Used for implementation of evaluateNetwork
#include "mathLib.h" // For zeroMatrix in gradient descent

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

    // Allocate activations and sum matrices
    (*network)->a = malloc((hiddenLayers + 1) * sizeof(Matrix));
    (*network)->z = malloc((hiddenLayers + 1) * sizeof(Matrix));
    if ((*network)->a == NULL || (*network)->z == NULL) {
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

        // Make each activation and sum matrix
        Matrix* a = NULL;
        Matrix* z = NULL;
        returnCode = makeMatrix(neurons[i+1], 1, &a);
        if (returnCode != SUCCESS) {
            return returnCode;
        }
        returnCode = makeMatrix(neurons[i+1], 1, &z);
        if (returnCode != SUCCESS) {
            return returnCode;
        }

        // Assign random values to matrices from -2 to 2
        randomiseMatrix(weights);
        randomiseMatrix(biases);

        // Put into neural network
        (*network)->weights[i] = weights;
        (*network)->biases[i] = biases;
        (*network)->a[i] = a;
        (*network)->z[i] = z;
    }
    return SUCCESS;
}

void freeNetwork(NeuralNetwork* network) {
    // Free all matrices
    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        freeMatrix(network->weights[i]);
        freeMatrix(network->biases[i]);
        freeMatrix(network->a[i]);
        freeMatrix(network->z[i]);
    }
    // Free pointer arrays
    free(network->weights);
    free(network->biases);
    free(network->a);
    free(network->z);
    // Free network itself
    free(network);
}

int feedForwardNetwork(NeuralNetwork* network, Matrix* input) {
    // Feed-forward through all layers
    int errorCode = SUCCESS;
    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        Matrix* weights = network->weights[i];
        Matrix* biases = network->biases[i];
        
        // Use input matrix on first iteration, else use the prev. activations
        if (i == 0) {
            errorCode = multiplyMatricesInto(weights, input, network->z[i]);
        } else {
            errorCode = multiplyMatricesInto(weights, network->a[i-1], network->z[i]);
        }
        if (errorCode != SUCCESS) {
            return errorCode;
        }
        errorCode = addMatricesInto(network->z[i], biases, network->z[i]);
        if (errorCode != SUCCESS) {
            return errorCode;
        }

        // Perform activation function on the column matrix
        sigmoidInto(network->z[i], network->a[i]);
        if (errorCode != SUCCESS) {
            return errorCode;
        }
    }
    return SUCCESS;
}

int feedForwardNetworkImage(NeuralNetwork* network, Image* input) {
    // Allocate return vars
    Matrix* m = NULL;
    int returnCode = getMatrixFromImage(input, &m);
    if (returnCode != SUCCESS) {
        freeMatrix(m);
        return returnCode; // TODO: Change all errorCode vars to returnCode
    }

    // Feedforward
    returnCode = feedForwardNetwork(network, m);
    if (returnCode != SUCCESS) {
        freeMatrix(m);
        return returnCode; // TODO: Change all errorCode vars to returnCode
    }
    freeMatrix(m);
    return SUCCESS; 
}

int evaluateNetwork(NeuralNetwork* network, Image** images, int numberOfImages,
                    int* correctImages) {
    for (int i = 0; i < numberOfImages; i++) {
        // Feedforward
        Image* img = images[i];
        int returnCode = feedForwardNetworkImage(network, img);
        if (returnCode != SUCCESS) {
            return returnCode; // TODO: Change all errorCode vars to returnCode
        }
        
        // Check result
        int output = indexOfMaxValue(network->a[network->hiddenLayers]);
        int expected = (int) img->label; // Convert char to int
        if (output + 1 == expected) {
            (*correctImages)++;
        }
    }
    return SUCCESS;
}

        // Free image matrix
        freeMatrix(m);
    }
    return SUCCESS;
}
    }
    return SUCCESS;
}