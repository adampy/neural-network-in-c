#ifndef NEURAL_NETWORK
#define NEURAL_NETWORK

#include <stdlib.h>
#include "err.h"
#include "mathLib.h"
#include "image.h"

typedef struct _NeuralNetwork {
    unsigned int hiddenLayers;
    unsigned int* neurons;
    double learningRate;
    Matrix** weights;
    Matrix** biases;
    Matrix** z; // Stores the summed inputs of each neuron for each layer
    Matrix** a; // Stores activation of each neuron for each layer
} NeuralNetwork;

/**
 * Creates a `NeuralNetwork` from the given parameters, and stores it in the
 * `network` output vector.
 * hiddenLayers = number of hidden layers
 * neurons[0] = number of neurons in the input layer
 * neurons[n] = number of neurons in the n-1th hidden layer
 * neurons[hiddenLayers+1] = number of neurons in the output layer
 * network = output vector
 */
int makeNetwork(unsigned int hiddenLayers, unsigned int* neurons,
                double learningRate, NeuralNetwork** network);

/**
 * Frees all the memory relating to a given network `network`.
 */
void freeNetwork(NeuralNetwork* network);

/**
 * Returns the output of the network when `input` is the input. Outputs
 * are stored in `network->a` and `network->z` for each layer.
 */
int feedForwardNetwork(NeuralNetwork* network, Matrix* input);

/**
 * Returns the output of the network when the matrix of value from an image
 * `image` is the input. Outputs are stored in `network->a` and `network->z`
 * for each layer.
 */
int feedForwardNetworkImage(NeuralNetwork* network, Image* input);

/**
 * Evalutes a neural network using the given array of images, `images`. The
 * neural network's output is taken to be whichever output neuron is the
 * biggest. The number of correctly classified images is stored in
 * `correctImages`.
 */
int evaluateNetwork(NeuralNetwork* network, Image** images, int numberOfImages,
                    int* correctImages);

/**
 * Changes the weights and biases of `network` in regards to a single `input`
 * which is the input to the network, and `correctNeuron` which is the index
 * of the correct neuron.
 */
int backpropSingleInput(NeuralNetwork* network, Matrix* input);

/**
 * Performs mini-batch gradient descent for a number of epochs `epochs`.
 * The number of training examples in each mini batches is `miniBatcheSize`
 * and the weights and biases are updated at the end of each mini batch
 * completion. Training images are provided for training and testing images
 * are provided for evaluating the network at the end of each epoch.
 */
int trainNetworkMiniBatches(NeuralNetwork* network, int epochs, int miniBatchSize,
                             Image** trainingImages, int numberOfTrainingImages,
                             Image** testingImages, int numberOfTestingImages); // TODO: Change ints to unsigned if necessary

#endif // NEURAL_NETWORK