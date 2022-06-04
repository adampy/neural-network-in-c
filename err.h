#ifndef ERR
#define ERR

typedef enum _error {
    SUCCESS = 0,
    BAD_ARGUMENT_COUNT = 1,
    BAD_FILE_NAME = 2,
    BAD_MAGIC_NUMBER = 3,
    IMAGE_MALLOC_FAILED = 4,
    BAD_DATA = 5,
    OUTPUT_FAILED = 6,
    MISC = 100
} error;

/**
 * Delegates printing an error to the appropriate function. `string` is
 * provided when necessary but is not guarantee. It returns `errorCode`.
 */
int reportError(int errorCode, char* string);

void badArgumentCount();
void badFileName();
void badMagicNumber(char* string);
void imageMallocFailed();
void badData(char* string);
void outputFailed(char* string);
void miscError(char* string);

#endif // ERR