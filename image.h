#ifndef IMAGE
#define IMAGE

typedef struct _image {
    unsigned int rows;
    unsigned int columns;
    unsigned char** imageData;
    unsigned int hasImageDataAllocated;
    unsigned int numberOfRowsAllocated;
} image;

/**
 * Allocates memory to a null pointer for an image
 */
int makeImage(image* img);

/**
 * Allocates memory for `img->imageData`, and the rows inside it.
 * It requires that columns and rows is set, and not 0.
 */
int allocateImageData(image* img);

/**
 * Free all memory used by this `img`.
 */
int freeImageData(image* img);

#endif // IMAGE