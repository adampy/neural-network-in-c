#include <stdlib.h>
#include "image.h"
#include "err.h"

#include <stdio.h>

int makeImage(image** img) {
    *img = malloc(sizeof(image));
    if (img == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }
    
    // Set attributes to 0
    (*img)->columns = 0;
    (*img)->rows = 0;
    (*img)->hasImageDataAllocated = 0;
    (*img)->numberOfRowsAllocated = 0;
    (*img)->label = '\0';
    return SUCCESS;
}

int allocateImageData(image* img) {
    // Check rows and columns
    if (img == NULL) {
        return reportError(MISC, "Image must be initialised before allocating data to it");
    }
    if (img->rows == 0 || img->columns == 0) {
        return reportError(MISC, "Rows and columns must be set before malloc");
    }

    // Allocate image data 2D array
    img->imageData = calloc(img->rows, sizeof(unsigned char *));
    if (img->imageData == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }
    img->hasImageDataAllocated = 1;

    // Allocate all image data rows
    for (int i = 0; i < img->rows; i++) {
        img->imageData[i] = calloc(img->columns, sizeof(unsigned char));
        if (img->imageData[i] == NULL) {
            return reportError(IMAGE_MALLOC_FAILED, "");
        }
        img->numberOfRowsAllocated++;
    }

    return SUCCESS;
}

int freeImageData(image* img) {
    if (img->hasImageDataAllocated) {
        // Free all rows allocated
        for (int i = img->numberOfRowsAllocated - 1; i >= 0; i--) {
            free(img->imageData[i]);
        }
        free(img->imageData);
    }
    return SUCCESS;
}

int freeAllImages(image** images, int numberOfImages) {
    for (int i = numberOfImages - 1; i >= 0; i--) {
        freeImageData(images[i]);
        free(images[i]);
    }
    free(images);
    return SUCCESS;
}