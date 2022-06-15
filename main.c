#include <stdio.h>
#include <stdlib.h>
#include <time.h> // For srand
#include "utils.h" // For printing of images, matrices, etc
#include "image.h"
#include "neuralNetwork.h"
#include "imageInput.h"
#include "err.h"

#define LEARNING_RATE 3
#define EPOCHS 50
#define MINI_BATCH_SIZE 10 // 1 is SGD, anything else is mini-batch gradient descent
#define HIDDEN_LAYERS 2
unsigned int neurons[4] = {784, 30, 30, 10};

/**
 * argv = {main, trainingDatasetFilename, trainingLabelsFilename,
 *         testDatasetFilename, testLabelsFilename}
 */
int main(int argc, char** argv) {
    // Check all arguments given
    if (argc == 1) {
        printf("Usage: ./main trainingDatasetFilename trainingLabelsFilename testDatasetFilename testLabelsFilename\n");
        return SUCCESS;
    }
    if (argc != 5) {
        return reportError(BAD_ARGUMENT_COUNT, "");
    }

    // Initialise training dataset
    int numberOfTrainingImages = 0;
    Image** trainingImages = NULL;
    int returnCode = readMNIST(argv[1], argv[2], &trainingImages, &numberOfTrainingImages);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    // Initialise testing dataset
    int numberOfTestingImages = 0;
    Image** testingImages = NULL;
    returnCode = readMNIST(argv[3], argv[4], &testingImages, &numberOfTestingImages);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // Set up NN
    NeuralNetwork* network = NULL;
    returnCode = makeNetwork(HIDDEN_LAYERS, neurons, LEARNING_RATE, &network);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    network->trainingImages = trainingImages;
    network->numberOfTrainingImages = numberOfTrainingImages;
    network->testingImages = testingImages;
    network->numberOfTestingImages = numberOfTestingImages;

    // --- Evaluate once ---
    returnCode = evaluateNetwork(network, "Initial");
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    returnCode = trainNetworkMiniBatches(network, EPOCHS, MINI_BATCH_SIZE);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // Cleanup and exit execution
    cleanUp:
        freeNetwork(network);
        freeAllImages(trainingImages, numberOfTrainingImages);
        freeAllImages(testingImages, numberOfTestingImages);
        return returnCode;
}
