#ifndef IMAGE
#define IMAGE

typedef struct _Image {
    unsigned int rows;
    unsigned int columns;
    unsigned char** imageData;
    unsigned int hasImageDataAllocated;
    unsigned int numberOfRowsAllocated;
    char label;
} Image;

/**
 * Allocates memory to a null pointer for an image
 */
int makeImage(Image** outputVector);

/**
 * Allocates memory for `img->imageData`, and the rows inside it.
 * It requires that columns and rows is set, and not 0.
 */
int allocateImageData(Image* img);

/**
 * Free all memory used by this `img`.
 */
int freeImageData(Image* img);

/**
 * Used to free all memory used by all images `images`. The total
 * number of images is `numberOfImages`.
 */
int freeAllImages(Image* images[], int numberOfImages);

#endif // IMAGE