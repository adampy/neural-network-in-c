# Set up defaults for implicit rules
CC = gcc -g
CFLAGS = -std=c99 -Wall -Werror # TODO: Remove debugging flag

# Define source code and object code macro
SRC = main.c err.c image.c imageInput.c mathLib.c utils.c neuralNetwork.c
MODULES = err.c image.c imageInput.c mathLib.c utils.c neuralNetwork.c
OBJ = $(SRC:.c=.o)
CLN = $(OBJ) $(SRC:.c=)

# Make object code of each file
all: $(OBJ) main

# Static rule to convert each .c file into a .o file
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

main: main.o $(MODULES)
	$(CC) main.o $(MODULES) -o main -lm

# Clean target
clean:
	rm -f $(CLN)

# Dependencies
main.o: main.c main.h
err.o: err.c err.h
image.o: image.c image.h
imageInput.o: imageInput.c imageInput.h
mathLib.o: mathLib.c mathLib.h
utils.o: utils.c utils.h
neuralNetwork.o: neuralNetwork.c neuralNetwork.h