#!/bin/bash

make clean;
make;
./main dataset/train-images-idx3-ubyte dataset/train-labels-idx1-ubyte dataset/t10k-images-idx3-ubyte dataset/t10k-labels-idx1-ubyte;
