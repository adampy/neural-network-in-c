#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "imageInput.h"
#include "err.h"


/**
 * argv = {main, datasetFilename, labelsFilename}
 */
int main(int argc, char** argv) {
    // Check all arguments given
    if (argc == 1) {
        printf("Usage: ./main datasetFilename, labelsFilename\n");
        return SUCCESS;
    }
    if (argc != 3) {
        return reportError(BAD_ARGUMENT_COUNT, "");
    }

    // Error code
    int returnCode = SUCCESS;

    // Initialise image output vector
    int numberOfImages = 0;
    Image** images = NULL;
    int read = readMNIST(argv[1], argv[2], &images, &numberOfImages);
    if (read != SUCCESS) {
        returnCode = read;
        goto cleanUp;
    }

    // Cleanup and exit execution
    cleanUp:
        returnCode = freeAllImages(images, numberOfImages);
        return returnCode;
}
