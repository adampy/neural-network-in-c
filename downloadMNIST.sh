#!/bin/bash

URL="http://yann.lecun.com/exdb/mnist/";
trainingImages="train-images-idx3-ubyte";
trainingLabels="train-labels-idx1-ubyte";
testingImages="t10k-images-idx3-ubyte";
testingLabels="t10k-labels-idx1-ubyte";
datasets=("${trainingImages}" "${trainingLabels}" "${testingImages}" "${testingLabels}");

# Make dataset directory and enter it
if [ ! -d "dataset" ]; then
    mkdir dataset;
fi

# Check every data piece is present, download it if not
cd dataset;
for i in {0..3}; do
    dataPiece="${datasets[i]}";
    if [ ! -f "${dataPiece}" ]; then
        wget "${URL}${dataPiece}.gz";
        gunzip "${dataPiece}.gz";
    fi
done
cd ..;
