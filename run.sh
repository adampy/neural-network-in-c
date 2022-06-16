#!/bin/bash

RESET='\e[0m';
YELLOW='\e[33m';
LIGHTRED='\e[1;31m';

# Ensure all 3 extra arguments are given
if [[ $# -ne 3 ]]; then
    echo -e "${LIGHTRED}Usage: ./run.sh learningRate epochs miniMatchSize${RESET}";
    exit;
fi

echo -e "${YELLOW}Running make...${RESET}";
make clean;
make;
made=$?;
if [[ made -ne 0 ]]; then
    echo -e "${LIGHTRED}Error occured when running make, see above for details${RESET}";
    exit
fi

echo -e "${YELLOW}Solution made!${RESET}";

echo -e "${YELLOW}Downloading MNIST dataset...${RESET}";
./downloadMNIST.sh
echo -e "${YELLOW}MNIST dataset downloaded!${RESET}";

./main dataset/train-images-idx3-ubyte dataset/train-labels-idx1-ubyte dataset/t10k-images-idx3-ubyte dataset/t10k-labels-idx1-ubyte $@;
