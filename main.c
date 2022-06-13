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
    unsigned int neurons[4] = {784, 50, 20, 10};
    returnCode = makeNetwork(2, neurons, LEARNING_RATE, &network);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    // Choose random image
    /*srand(time(NULL));
    Image* img = testingImages[rand() % numberOfTestingImages];
    //printImage(img);
    Matrix* m = NULL;
    returnCode = getMatrixFromImage(img, &m);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    feedForwardNetworkImage(network, img);
    printf("---z[3]---\n");
    printMatrix(network->z[3]);
    printf("---a[3]---\n");
    printMatrix(network->a[3]);
    Matrix* costD = NULL;
    costDerivative(network->a[3], (int)img->label - 1, &costD);
    printf("---Cost derivative---\n");
    printMatrix(costD);*/

    // --- Evaluate once ---
    int correctImages = 0;
    returnCode = evaluateNetwork(network, testingImages, numberOfTestingImages,
                        &correctImages);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }

    printf("Initial NN evaluation:\t\t\t%.3lf%% testing accuracy.\n", (double) 100*correctImages/numberOfTestingImages);
    returnCode = trainNetworkMiniBatches(network, EPOCHS, MINI_BATCH_SIZE, trainingImages,
                                         numberOfTrainingImages,testingImages,
                                         numberOfTestingImages);
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
