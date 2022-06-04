#include <stdio.h>
#include "err.h"

// --- Error strings ---
#define BAD_ARGUMENT_COUNT_ERROR_STRING "ERROR: Bad Argument Count"
#define BAD_FILE_NAME_ERROR_STRING "ERROR: Bad Filename (%s)"
#define BAD_MAGIC_NUMBER_ERROR_STRING "ERROR: Bad Magic Number (%s)"
#define IMAGE_MALLOC_FAILED_ERROR_STRING "ERROR: Image Malloc Failed"
#define BAD_DATA_ERROR_STRING "ERROR: Bad Data (%s)"
#define OUTPUT_FAILED_ERROR_STRING "ERROR: Output Failed (%s)"
#define MISC_ERROR_STRING "ERROR: Miscellaneous (%s)"

int reportError(int errorCode, char* string) {
    // Delegate to appropriate function
    switch (errorCode) {
        case BAD_ARGUMENT_COUNT:
            badArgumentCount();
        case BAD_FILE_NAME:
            badFileName(string);
        case BAD_MAGIC_NUMBER:
            badMagicNumber(string);
        case IMAGE_MALLOC_FAILED:
            imageMallocFailed();
        case BAD_DATA:
            badData(string);
        case OUTPUT_FAILED:
            outputFailed(string);
        case MISC:
            miscError(string);
        default:
            break;
    }
    return errorCode;
}

void badArgumentCount() {
    printf(BAD_ARGUMENT_COUNT_ERROR_STRING);
}
void badFileName(char* string) {
    printf(BAD_FILE_NAME_ERROR_STRING, string);
}
void badMagicNumber(char* string) {
    printf(BAD_MAGIC_NUMBER_ERROR_STRING, string);
}
void imageMallocFailed() {
    printf(IMAGE_MALLOC_FAILED_ERROR_STRING);
}
void badData(char* string) {
    printf(BAD_DATA_ERROR_STRING, string);
}
void outputFailed(char* string) {
    printf(OUTPUT_FAILED_ERROR_STRING, string);
}
void miscError(char* string) {
    printf(MISC_ERROR_STRING, string);
}