#include <stdio.h>
#include <stdlib.h>
#include <time.h> // For srand
#include "utils.h" // For printing of images, matrices, etc
#include "image.h"
#include "neuralNetwork.h"
#include "imageInput.h"
#include "err.h"


/**
 * argv = {main, datasetFilename, labelsFilename}
 */
int main(int argc, char** argv) {
    // Check all arguments given
    if (argc == 1) {
        printf("Usage: ./main datasetFilename, labelsFilename\n");
        return SUCCESS;
    }
    if (argc != 3) {
        return reportError(BAD_ARGUMENT_COUNT, "");
    }

    // Error code
    int returnCode = SUCCESS;

    // Initialise image output vector
    int numberOfImages = 0;
    Image** images = NULL;
    int read = readMNIST(argv[1], argv[2], &images, &numberOfImages);
    if (read != SUCCESS) {
        returnCode = read;
        goto cleanUp;
    }

    // Choose random image
    srand(time(NULL));
    Image* img = images[rand() % numberOfImages];
    printImage(img);
    Matrix* m = NULL;
    returnCode = getMatrixFromImage(img, &m);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // Set up NN
    NeuralNetwork* network = NULL;
    unsigned int neurons[4] = {784, 20, 20, 10};
    returnCode = makeNetwork(2, neurons, 1, &network);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    printNetwork(network);
    //printMatrix(network->weights[1]);

    // Feedforward result
    Matrix* result = NULL;
    returnCode = feedForwardNetwork(network, m, &result);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    printf("---Feedforward result---\n");
    printMatrix(result);

    // Cleanup and exit execution
    cleanUp:
        freeNetwork(network);
        freeMatrix(m);
        freeMatrix(result);
        returnCode = freeAllImages(images, numberOfImages);
        return returnCode;
}
