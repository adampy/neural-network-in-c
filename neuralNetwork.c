#include <stdlib.h>
#include <time.h> // For random number generation
#include "neuralNetwork.h" // TODO: Remove all includes and put them in headers
#include "imageInput.h" // Used for implementation of evaluateNetwork
#include "mathLib.h" // For zeroMatrix in gradient descent
#include "utils.h" // For shuffle

#include <stdio.h>

//Regular text
#define BLK "\e[1;30m"
#define RED "\e[1;31m"
#define GRN "\e[1;32m"
#define YEL "\e[1;33m"
#define BLU "\e[1;34m"
#define MAG "\e[1;35m"
#define CYN "\e[1;36m"
#define WHT "\e[1;37m"
#define CLR "\e[0;0m"

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
    // Move input values to network->z[0]
    zeroMatrix(network->z[0]);
    int returnCode = addMatricesInto(network->z[0], input, network->z[0]);
    if (returnCode != SUCCESS) {
        return returnCode;
    }
    // Set activations of network->a[0]
    zeroMatrix(network->a[0]);
    returnCode = addMatricesInto(network->a[0], input, network->a[0]);
    if (returnCode != SUCCESS) {
        return returnCode;
    }
    
    // Feed-forward through all layers
    for (int i = 0; i < network->hiddenLayers + 1; i++) {
        Matrix* weights = network->weights[i];
        Matrix* biases = network->biases[i];
        
        // Use input matrix on first iteration, else use the prev. activations
        returnCode = multiplyMatricesInto(weights, network->a[i], network->z[i+1]);
        if (returnCode != SUCCESS) {
            return returnCode;
        }
        returnCode = addMatricesInto(network->z[i+1], biases, network->z[i+1]);
        if (returnCode != SUCCESS) {
            return returnCode;
        }

        // Perform activation function on the column matrix
        returnCode = sigmoidInto(network->z[i+1], network->a[i+1]);
        if (returnCode != SUCCESS) {
            return returnCode;
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
        return returnCode;
    }

    // Feedforward
    returnCode = feedForwardNetwork(network, m);
    if (returnCode != SUCCESS) {
        freeMatrix(m);
        return returnCode;
    }
    freeMatrix(m);
    return SUCCESS; 
}

int evaluateNetwork(NeuralNetwork* network, char* string) {
    int outputNeurons = network->neurons[network->hiddenLayers + 1];
    int correctImages = 0;
    double cost = 0;
    int* o = calloc(outputNeurons, sizeof(int)); // Stores number of correct outputs for each digit
    int* e = calloc(outputNeurons, sizeof(int)); // Stores expected outputs
    if (o == NULL || e == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }

    for (int i = 0; i < network->numberOfTestingImages; i++) {
        // Feedforward
        Image* img = network->testingImages[i];
        int returnCode = feedForwardNetworkImage(network, img);
        if (returnCode != SUCCESS) {
            goto cleanUp;
        }
        Matrix* networkOutput = network->a[network->hiddenLayers + 1];
        
        // Check result
        int output = -1;
        returnCode = indexOfMaxValue(networkOutput, &output);
        if (returnCode != SUCCESS) {
            return returnCode;
        }

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
    cost /= network->numberOfTestingImages;

    printf(RED "----NETWORK EVALUATION (%s)----\n" CLR, string);
    printf(GRN "%.3lf%%" CLR " testing accuracy\n", (double) 100*correctImages/network->numberOfTestingImages);
    printf(GRN "%.3lf" CLR " cost\n", cost);

    // For each output neuron, print its accuracy
    for (int i = 0; i < outputNeurons; i++) {
        printf("Neuron %i accuracy: " BLU "%3.lf%%" CLR "\n", i, (double) 100 * o[i] / e[i]);
    }

    cleanUp:
        free(o);
        free(e);
        return SUCCESS;
}

int trainNetworkMiniBatches(NeuralNetwork* network, unsigned int epochs, unsigned int miniBatchSize) {
    // Check mini batch size
    if (network->numberOfTrainingImages % miniBatchSize != 0) {
        return reportError(MISC, "miniBatchSize must equally divide numberOfTrainingImages");
    }
    
    // Initialise variables
    int numberOfMiniBatches = network->numberOfTestingImages / miniBatchSize;
    int returnCode = SUCCESS;
    int H = network->hiddenLayers;

    // For each epoch
    for (int e = 0; e < epochs; e++) {
        // For each mini batch
        shuffle(network->trainingImages, network->numberOfTrainingImages);
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

            // Train all images
            for (int i = 0; i < miniBatchSize; i++) {
                // Forward prop, storing activations and outputs
                Image* img = network->trainingImages[miniBatchSize * x + i];
                returnCode = trainNetworkSingleImage(network, img, nablaW, nablaB);
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
        returnCode = evaluateNetwork(network, string);
        if (returnCode != SUCCESS) {
            return SUCCESS;
        }
    }
    return SUCCESS;
}

int trainNetworkSingleImage(NeuralNetwork* network, Image* img, Matrix** nablaW, Matrix** nablaB) {
    int returnCode = feedForwardNetworkImage(network, img);
    if (returnCode != SUCCESS) {
        return returnCode;
    }

    // For each layer
    Matrix* delta = NULL;
    for (int l = network->hiddenLayers; l >= 0; l--) {
        // Calculate error of output layer
        Matrix* sum = network->z[l + 1];
        Matrix* firstTerm = NULL;
        // If output layer, set first term of delta to cost derivative
        if (l == network->hiddenLayers) {
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
        returnCode = dsigmoidInto(sum, sumD);
        if (returnCode != SUCCESS) {
            return returnCode;
        }
        
        // If delta has already been initialised (if not on the output layer)
        if (l != network->hiddenLayers) {
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
    return returnCode;
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