#ifndef UTILS
#define UTILS

#include "image.h"
#include "mathLib.h"
#include "neuralNetwork.h"

void printImage(Image* img);
void printMatrix(Matrix* m);
void printNetwork(NeuralNetwork* network);
void shuffle(Image** array, int n);

#endif // UTILS