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
 * Returns the output of the network when `input` is the input. The resulting
 * activations of the output layer is stored in the `output` output vector.
 */
int feedForwardNetwork(NeuralNetwork* network, Matrix* input, Matrix** output);

/**
 * Changes the weights and biases of `network` in regards to a single `input`
 * which is the input to the network, and `correctNeuron` which is the index
 * of the correct neuron.
 * 
 * For the example of MNIST datasets, an image with label '7' should place 7
 * into `result`.
 */
int backpropSingleInput(NeuralNetwork* network, Matrix* input, char result);

/**
 * Evalutes a neural network using the given array of images, `images`. The
 * neural network's output is taken to be whichever output neuron is the
 * biggest. The number of correctly classified images is stored in
 * `correctImages`.
 */
int evaluateNetwork(NeuralNetwork* network, Image** images, int numberOfImages,
                    int* correctImages);

#endif // NEURAL_NETWORK