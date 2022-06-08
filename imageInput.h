#ifndef IMAGE_INPUT
#define IMAGE_INPUT

int isLittleEndian();

int byteSwap(int num);

int readMNIST(char* datasetFilename, char* labelsFilename, Image*** images, 
              int* numberOfImages);

// --- Batch read functions ---
int batchReadImagesWithLabels(char* datasetFilename, FILE* dataset, char* labelsFilename,
                    FILE* labels, unsigned int rows, unsigned int columns,
                    unsigned int numberOfImages, Image*** images);

// --- Single read functions ---
int readNextImage(char* filename, FILE* file, Image* img, unsigned int rows,
               unsigned int columns);

int readNextLabel(char* filename, FILE* file, Image* img);

// --- Header read functions ---
int readHeaders(char* datasetFilename, FILE* dataset, char* labelsFilename,
                FILE* labels, int* numberOfImages, int* rows, int* columns);

int readImageFileHeader(char* filename, FILE* file, int* numberOfImages,
                        int* rows, int* columns);

int readLabelsFileHeader(char* filename, FILE* file, int* numberOfLabels);

#endif // IMAGE_INPUT