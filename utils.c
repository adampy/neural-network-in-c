#include <stdio.h>
#include <time.h>
#include "image.h"
#include "mathLib.h"

void printImage(Image* img) {
    for (int i = 0; i < img->rows; i++) {
        for (int j = 0; j < img->columns; j++) {
            printf("%i\t", img->imageData[i][j]);
        }
        printf("\n");
    }
    printf("Label: %i\n", img->label);
}

void printMatrix(Matrix* m) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            printf("%.3lf\t", m->values[i * m->columns + j]);
        }
        printf("\n");
    }
}