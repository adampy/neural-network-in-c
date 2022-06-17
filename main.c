#include <stdio.h>
#include <stdlib.h>
#include <time.h> // For srand
#include "utils.h" // For printing of images, matrices, etc
#include "image.h"
#include "neuralNetwork.h"
#include "imageInput.h"
#include "err.h"

//#define LEARNING_RATE 3
//#define EPOCHS 50
//#define MINI_BATCH_SIZE 10 // 1 is SGD, anything else is mini-batch gradient descent
#define HIDDEN_LAYERS 1

/**
 * argv = {main, trainingDatasetFilename, trainingLabelsFilename,
 *         testDatasetFilename, testLabelsFilename}
 */
int main(int argc, char** argv) {
    // Check all arguments given
    if (argc == 1) {
        printf("Usage: ./main trainingDatasetFilename trainingLabelsFilename testDatasetFilename testLabelsFilename learningRate epochs miniBatchSize\n");
        return SUCCESS;
    }
    if (argc != 8) {
        return reportError(BAD_ARGUMENT_COUNT, "");
    }
    double learningRate;
    if (!sscanf(argv[5], "%lf", &learningRate)) {
        return reportError(MISC, "Conversion of learning rate argument error");
    }

    // --- TRAINING DATASET ---
    int numberOfTrainingImages = 0;
    Image** trainingImages = NULL;
    int returnCode = readMNIST(argv[1], argv[2], &trainingImages, &numberOfTrainingImages);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    // --- TESTING DATASET ---
    int numberOfTestingImages = 0;
    Image** testingImages = NULL;
    returnCode = readMNIST(argv[3], argv[4], &testingImages, &numberOfTestingImages);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // --- MAKE NEURAL NETWORK ---
    NeuralNetwork* network = NULL;
    /*unsigned int* neurons = calloc(sizeof(unsigned int), HIDDEN_LAYERS + 2);
    neurons[0] = 784;
    neurons[1] = 30;
    neurons[2] = 10;
    returnCode = makeNetwork(HIDDEN_LAYERS, neurons, learningRate, &network);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }*/
    
    returnCode = loadNetwork(&network, "network", learningRate);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    network->trainingImages = trainingImages;
    network->numberOfTrainingImages = numberOfTrainingImages;
    network->testingImages = testingImages;
    network->numberOfTestingImages = numberOfTestingImages;

    // --- EVALUATION ---
    returnCode = evaluateNetwork(network, "Initial");
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // --- TRAINING --- 
    returnCode = trainNetworkMiniBatches(network, atoi(argv[6]), atoi(argv[7]));
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // --- SAVING ---
    returnCode = saveNetwork(network, "network");
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
