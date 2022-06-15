#include <stdlib.h>
#include <time.h> // For random number generation
#include "neuralNetwork.h" // TODO: Remove all includes and put them in headers
#include "imageInput.h" // Used for implementation of evaluateNetwork
#include "mathLib.h" // For zeroMatrix in gradient descent
#include "utils.h" // For shuffle

#include <stdio.h>

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
    (*network)->a = malloc((hiddenLayers + 2) * sizeof(Matrix));
    (*network)->z = malloc((hiddenLayers + 2) * sizeof(Matrix));
    if ((*network)->a == NULL || (*network)->z == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }

    // Make weight and bias matrix arrays
    int returnCode = SUCCESS;
    for (int i = 0; i < hiddenLayers + 1; i++) {
        Matrix* weights = NULL;
        Matrix* biases = NULL;
        returnCode = makeMatrix(neurons[i+1], neurons[i], &weights);
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

    // Make activation and sum arrays
    for (int i = 0; i < hiddenLayers + 2; i++) {
        // Make each activation and sum matrix
        Matrix* a = NULL;
        Matrix* z = NULL;
        returnCode = makeMatrix(neurons[i], 1, &a);
        if (returnCode != SUCCESS) {
            return returnCode;
        }
        returnCode = makeMatrix(neurons[i], 1, &z);
        if (returnCode != SUCCESS) {
            return returnCode;
        }
        (*network)->a[i] = a;
        (*network)->z[i] = z;
    }
    return SUCCESS;
}

void freeNetwork(NeuralNetwork* network) {
    // Free weight and bias matrix arrays
    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        freeMatrix(network->weights[i]);
        freeMatrix(network->biases[i]);
    }
    // Free activation and sum arrays
    for (int i = 0; i < network->hiddenLayers + 2; i++) {
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

    
    // Move input values to network->z[0]
    zeroMatrix(network->z[0]); // TODO: Add error checking here
    addMatricesInto(network->z[0], input, network->z[0]);
    // Set activations of network->a[0]
    //sigmoidInto(network->z[0], network->a[0]);
    zeroMatrix(network->a[0]);
    addMatricesInto(network->a[0], input, network->a[0]);
    

    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        Matrix* weights = network->weights[i];
        Matrix* biases = network->biases[i];
        
        // Use input matrix on first iteration, else use the prev. activations
        errorCode = multiplyMatricesInto(weights, network->a[i], network->z[i+1]);
        if (errorCode != SUCCESS) {
            return errorCode;
        }
        errorCode = addMatricesInto(network->z[i+1], biases, network->z[i+1]);
        if (errorCode != SUCCESS) {
            return errorCode;
        }

        // Perform activation function on the column matrix
        sigmoidInto(network->z[i+1], network->a[i+1]);
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
                    char* string) {
    int outputNeurons = network->neurons[network->hiddenLayers + 1];
    int correctImages = 0;
    double cost = 0;
    int* o = calloc(outputNeurons, sizeof(int)); // Stores number of correct outputs for each digit
    int* e = calloc(outputNeurons, sizeof(int)); // Stores expected outputs TODO: Make dynamic
    if (o == NULL || e == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }

    for (int i = 0; i < numberOfImages; i++) {
        // Feedforward
        Image* img = images[i];
        int returnCode = feedForwardNetworkImage(network, img);
        if (returnCode != SUCCESS) {
            goto cleanUp; // TODO: Change all errorCode vars to returnCode
        }
        Matrix* networkOutput = network->a[network->hiddenLayers + 1];
        
        // Check result
        int output = indexOfMaxValue(networkOutput);
        int expected = (int) img->label; // Convert char to int
        e[expected]++;
        if (output == expected) {
            o[output]++;
            correctImages++;
        }

        // Work out cost
        returnCode = costFunction(networkOutput, (int) img->label, &cost);
        if (returnCode != SUCCESS) {
            goto cleanUp;
        }
    }
    cost /= numberOfImages;

    printf("----NETWORK EVALUATION (%s)----\n", string);
    printf("%.3lf%% testing accuracy\n", (double) 100*correctImages/numberOfImages);
    printf("%.3lf cost\n", cost);

    // For each output neuron, print its accuracy
    for (int i = 0; i < outputNeurons; i++) {
        printf("Neuron %i accuracy: %3.lf%%\n", i, (double) 100 * o[i] / e[i]);
    }

    cleanUp:
        free(o);
        free(e);
        return SUCCESS;
}

int trainNetworkMiniBatches(NeuralNetwork* network, int epochs, int miniBatchSize,
                             Image** trainingImages, int numberOfTrainingImages,
                             Image** testingImages, int numberOfTestingImages) {
    
    if (numberOfTrainingImages % miniBatchSize != 0) {
        return reportError(MISC, "miniBatchSize must equally divide numberOfTrainingImages");
    }
    int numberOfMiniBatches = numberOfTestingImages / miniBatchSize;
    int returnCode = SUCCESS;
    int H = network->hiddenLayers;

    // For each epoch
    for (int e = 0; e < epochs; e++) {
        // For each mini batch
        shuffle(trainingImages, numberOfTrainingImages);
        for (int x = 0; x < numberOfMiniBatches; x++){ // TODO: Line limits of 80 chars
            // Initialise sum matrices - nablaB and nablaW have same shape of network->weights
            Matrix** nablaB = malloc((H + 1) * sizeof(Matrix));
            Matrix** nablaW = malloc((H + 1) * sizeof(Matrix));
            if (nablaB == NULL || nablaW == NULL) {
                return reportError(IMAGE_MALLOC_FAILED, "");
            }
            for (int i = 0; i < H + 1; i++) {
                returnCode = makeMatrix(network->neurons[i+1], network->neurons[i], &nablaW[i]);
                if (returnCode != SUCCESS) {
                    return returnCode;
                }
                returnCode = makeMatrix(network->neurons[i+1], 1, &nablaB[i]);
                if (returnCode != SUCCESS) {
                    return returnCode;
                }
            }

            for (int i = 0; i < miniBatchSize; i++) {
                // Forward prop, storing activations and outputs
                Image* img = trainingImages[miniBatchSize * x + i];
                returnCode = feedForwardNetworkImage(network, img);
                if (returnCode != SUCCESS) {
                    return returnCode;
                }

                // For each layer
                Matrix* delta = NULL;
                for (int l = H; l >= 0; l--) {
                    // Calculate error of output layer
                    Matrix* sum = network->z[l + 1];
                    Matrix* firstTerm = NULL;
                    // If output layer, set first term of delta to cost derivative
                    if (l == H) {
                        Matrix* output = network->a[l + 1];
                        returnCode = costDerivative(output, (int) img->label, &firstTerm);
                        if (returnCode != SUCCESS) {
                            return returnCode;
                        }
                        
                    } else {
                        Matrix* transposed = NULL;
                        returnCode = transposeMatrix(network->weights[l+1], &transposed);
                        if (returnCode != SUCCESS) {
                            return returnCode;
                        }
                        returnCode = makeMatrix(sum->rows, 1, &firstTerm);
                        if (returnCode != SUCCESS) {
                            return returnCode;
                        }
                        returnCode = multiplyMatricesInto(transposed, delta, firstTerm);
                        if (returnCode != SUCCESS) {
                            return returnCode;
                        }
                        freeMatrix(transposed);
                    }

                    //delta = hadamardProduct(firstTerm, sigmoidPrime(sum));
                    Matrix* sumD = NULL;
                    returnCode = makeMatrix(sum->rows, 1, &sumD);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }
                    dsigmoidInto(sum, sumD);
                    
                    // If delta has already been initialised (if not on the output layer)
                    if (l != H) {
                        freeMatrix(delta);
                        delta = NULL;
                    }
                    
                    returnCode = hadamardProduct(firstTerm, sumD, &delta);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }

                    //nablaB[outputLayer] += delta
                    returnCode = addMatricesInto(nablaB[l], delta, nablaB[l]);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }

                    //toAddToNablaW = delta dotted w/ a[outputLayer - 1]^T; (where ^T means transpose)
                    Matrix* toAddToNablaW = NULL;
                    returnCode = makeMatrix(nablaW[l]->rows, nablaW[l]->columns, &toAddToNablaW);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }

                    Matrix* transposed = NULL; // TODO: Make these allocations consistent
                    returnCode = transposeMatrix(network->a[l], &transposed);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }
                    
                    returnCode = multiplyMatricesInto(delta, transposed, toAddToNablaW);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }

                    //nablaW[outputLayer] += toAddToNablaW
                    returnCode = addMatricesInto(nablaW[l], toAddToNablaW, nablaW[l]);
                    if (returnCode != SUCCESS) {
                        return returnCode;
                    }

                    // Free used matrices
                    freeMatrix(firstTerm);
                    freeMatrix(sumD);
                    freeMatrix(transposed);
                    freeMatrix(toAddToNablaW);
                }
                freeMatrix(delta);
            }
            
            // For each layer change the weights and biases
            for (int l = 0; l < H + 1; l++) {
                // Change nablaW to represent -deltaW and nablaB to represent -deltaB
                multiplyScalarInto(nablaW[l], (double) -network->learningRate/miniBatchSize, nablaW[l]);
                multiplyScalarInto(nablaB[l], (double) -network->learningRate/miniBatchSize, nablaB[l]);
                // Add deltaW and deltaB to weights and biases
                addMatricesInto(network->weights[l], nablaW[l], network->weights[l]);
                addMatricesInto(network->biases[l], nablaB[l], network->biases[l]);
            }
            
            // Free nablaW and nablaB
            for (int l = 0; l < H + 1; l++) {
                freeMatrix(nablaB[l]);
                freeMatrix(nablaW[l]);
            }
            // Free pointer arrays
            free(nablaB);
            free(nablaW);
        }

        char string[128] = "";
        sprintf(string, "End of epoch %d", e);
        returnCode = evaluateNetwork(network, testingImages, numberOfTestingImages,
                            string);
        if (returnCode != SUCCESS) {
            return SUCCESS;
        }
    }
    return SUCCESS;
}

int costDerivative(Matrix* a, int y, Matrix** output) {
    // Allocate expected output vector
    Matrix* m = NULL;
    int returnCode = makeMatrix(a->rows, 1, &m);
    if (returnCode != SUCCESS) {
        return returnCode;
    }
    m->values[y] = -1;

    // Allocate output vector
    returnCode = makeMatrix(a->rows, 1, output);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    returnCode = addMatricesInto(a, m, *output);
    if (returnCode != SUCCESS) {
        goto cleanUp;
    }
    
    cleanUp:
        freeMatrix(m);
        return returnCode;
}

int costFunction(Matrix* networkOutput, int correctIndex, double* cost) {
    // Allocate expected output vector
    Matrix* y = NULL;
    int returnCode = makeMatrix(networkOutput->rows, 1, &y);
    if (returnCode != SUCCESS) {
        return returnCode;
    }
    y->values[correctIndex] = 1;

    // Cost = correctMatrix - networkOutput
    negateMatrix(networkOutput); // This changes the value of networkOutput - need to change back afterwards
    // Add cost and get MSE
    for (int i = 0; i < networkOutput->rows; i++) {
        *cost += (networkOutput->values[i] - y->values[i]) * (networkOutput->values[i] - y->values[i]) / 2;
    }
    negateMatrix(networkOutput);
    freeMatrix(y);
    return returnCode;
}