#ifndef IMAGE
#define IMAGE

typedef struct _image {
    unsigned int rows;
    unsigned int columns;
    unsigned char** imageData;
} Image;

#endif // IMAGE