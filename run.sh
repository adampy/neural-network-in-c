#!/bin/bash

RESET='\e[0m'
YELLOW='\e[33m'

echo -e "${YELLOW}Running make...${RESET}";
make clean;
make;
echo -e "${YELLOW}Solution made!${RESET}";

echo -e "${YELLOW}Downloading MNIST dataset...${RESET}";
./downloadMNIST.sh
echo -e "${YELLOW}MNIST dataset downloaded!${RESET}";
./main dataset/train-images-idx3-ubyte dataset/train-labels-idx1-ubyte dataset/t10k-images-idx3-ubyte dataset/t10k-labels-idx1-ubyte;
