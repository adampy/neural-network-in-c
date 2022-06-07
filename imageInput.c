#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include "image.h"
#include "err.h"
#include "imageInput.h"

#define IMAGE_MAGIC_NUMBER 2051
#define LABEL_MAGIC_NUMBER 2049

int isLittleEndian() {
        /* Get value of a single byte pointer at the lowest byte of x,
        if 1 then little endian */
        int x = 0;
        return *(char*)(&x) == 1;
}

int byteSwap(int num) { // TODO: This is not guaranteed to be 32 bits
    int swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
              ((num<<8)&0xff0000) | // move byte 1 to byte 2
              ((num>>8)&0xff00) | // move byte 2 to byte 1
              ((num<<24)&0xff000000); // byte 0 to byte 3
    return swapped;
}

int readMNIST(char* datasetFilename, char* labelsFilename, image*** images, 
              int* numberOfImages) {
    int returnCode = 0; // Set to 0
    // Open dataset
    FILE* dataset = fopen(datasetFilename, "rb");
    if (dataset == NULL) {
        return reportError(BAD_FILE_NAME, datasetFilename);
    }
    // Open labels
    FILE* labels = fopen(labelsFilename, "rb");
    if (labels == NULL) {
        return reportError(BAD_FILE_NAME, labelsFilename);
        fclose(dataset);
    }

    // Read headers
    int rows = 0;
    int columns = 0;
    int headers = readHeaders(datasetFilename, dataset, labelsFilename, labels,
                              numberOfImages, &rows, &columns); // This outputs to numberOfImages
    if (headers != SUCCESS) {
        returnCode = headers;
        goto cleanUp;
    }

    // Allocate output vector, and read complete images
    int data = batchReadImagesWithLabels(datasetFilename, dataset,
                                         labelsFilename, labels, rows, columns,
                                         *numberOfImages, images);
    if (data != SUCCESS) {
        returnCode = data;
        goto cleanUp;
    }

    cleanUp:
        fclose(dataset);
        fclose(labels);
        return returnCode;
}

// --- Batch read functions ---
int batchReadImagesWithLabels(char* datasetFilename, FILE* dataset, char* labelsFilename,
                    FILE* labels, unsigned int rows, unsigned int columns,
                    unsigned int numberOfImages, image*** images) {

    // Initialise the output vector
    *images = calloc(numberOfImages, sizeof(image*));
    if (images == NULL) {
        return reportError(IMAGE_MALLOC_FAILED, "");
    }

    // Read in all images and give them labels
    for (int i = 0; i < numberOfImages; i++) {
        // Make image
        image* img = NULL;
        int allocated = makeImage(&img);
        if (allocated != SUCCESS) {
            return allocated;
        }

        // Give it rows and columns
        img->rows = rows;
        img->columns = columns;

        // Allocate image data
        allocated = allocateImageData(img);
        if (allocated != SUCCESS) {
            return allocated;
        }

        // Read label
        int gotLabel = readNextLabel(labelsFilename, labels, img);
        if (gotLabel != SUCCESS) {
            return gotLabel;
        }

        // Read pixels
        int gotPixels = readNextImage(datasetFilename, dataset, img, rows,
                                      columns);
        if (gotPixels != SUCCESS) {
            return gotPixels;
        }

        // Add to image collection
        (*images)[i] = img;
    }
    return SUCCESS;
}

// --- Single read functions ---
int readNextImage(char* filename, FILE* file, image* img, unsigned int rows,
               unsigned int columns) {
    
    // Read each pixel into image
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int scanCount = fread(&img->imageData[i][j], 1, 1, file);
            if (scanCount != 1) {
                return reportError(BAD_DATA, filename);
            }
        }
    }
    return SUCCESS;
}

int readNextLabel(char* filename, FILE* file, image* img) {
    unsigned char label;
    int scanCount = fread(&label, 1, 1, file);
    if (scanCount != 1) {
        return reportError(BAD_DATA, filename);
    }

    // Set label
    img->label = label;
    return SUCCESS;
}

// --- Header read functions ---
int readHeaders(char* datasetFilename, FILE* dataset, char* labelsFilename,
                FILE* labels, int* numberOfImages, int* rows, int* columns) {
    int header = readImageFileHeader(datasetFilename, dataset, numberOfImages,
                                     rows, columns);
    if (header != SUCCESS) {
        return header;
    }
    
    // Read header of label file
    int numberOfLabels = 0;
    header = readLabelsFileHeader(labelsFilename, labels, &numberOfLabels);
    if (header != SUCCESS) {
        return header;
    }

    if (*numberOfImages != numberOfLabels) { // Data contains discrepencies
        return reportError(BAD_DATA, labelsFilename);
    }
    return SUCCESS;
}

int readImageFileHeader(char* filename, FILE* file, int* numberOfImages,
                        int* rows, int* columns) {
    // headerData = {magicNumber, images, rows, columns}
    int headerData[4] = {};

    int scanCount = fread(headerData, 4, 4, file);
    if (scanCount != 4) {
        return reportError(BAD_DATA, filename);
    }

    // File = big endian, linux + intel = little
    if (0==0) { //if (isLittleEndian()) { TODO: Fix this - !always need swap
        for (int i = 0; i < 4; i++) {
            headerData[i] = byteSwap(headerData[i]);
        }
    }

    // Validate magic number
    if (headerData[0] != IMAGE_MAGIC_NUMBER) {
        return reportError(BAD_MAGIC_NUMBER, filename);
    }

    *numberOfImages = headerData[1];
    *rows = headerData[2];
    *columns = headerData[3];
    return SUCCESS;
}

int readLabelsFileHeader(char* filename, FILE* file, int* numberOfLabels) {
    // headerData = {magicNumber, labels}
    int headerData[2] = {};
    
    int scanCount = fread(headerData, 4, 2, file);
    if (scanCount != 2) {
        return reportError(BAD_DATA, filename);
    }
    
    // File = big endian, linux + intel = little
    if (0==0) { //if (isLittleEndian()) { TODO: Fix this - !always need swap
        for (int i = 0; i < 2; i++) {
            headerData[i] = byteSwap(headerData[i]);
        }
    }

    // Validate magic number
    if (headerData[0] != LABEL_MAGIC_NUMBER) {
        return reportError(BAD_MAGIC_NUMBER, filename);
    }

    *numberOfLabels = headerData[1];
    return SUCCESS;
}