#ifndef IMAGE
#define IMAGE

typedef struct _image {
    unsigned int rows;
    unsigned int columns;
    unsigned char** imageData;
    unsigned int hasImageDataAllocated;
    unsigned int numberOfRowsAllocated;
    char label;
} image;

/**
 * Allocates memory to a null pointer for an image
 */
int makeImage(image** outputVector);

/**
 * Allocates memory for `img->imageData`, and the rows inside it.
 * It requires that columns and rows is set, and not 0.
 */
int allocateImageData(image* img);

/**
 * Free all memory used by this `img`.
 */
int freeImageData(image* img);

/**
 * Used to free all memory used by all images `images`. The total
 * number of images is `numberOfImages`.
 */
int freeAllImages(image* images[], int numberOfImages);

#endif // IMAGE