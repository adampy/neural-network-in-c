#include <stdio.h>
#include "err.h"

// --- Error strings ---
#define BAD_ARGUMENT_COUNT_ERROR_STRING "ERROR: Bad Argument Count\n"
#define BAD_FILE_NAME_ERROR_STRING "ERROR: Bad Filename (%s)\n"
#define BAD_MAGIC_NUMBER_ERROR_STRING "ERROR: Bad Magic Number (%s)\n"
#define IMAGE_MALLOC_FAILED_ERROR_STRING "ERROR: Image Malloc Failed\n"
#define BAD_DATA_ERROR_STRING "ERROR: Bad Data (%s)\n"
#define OUTPUT_FAILED_ERROR_STRING "ERROR: Output Failed (%s)\n"
#define MISC_ERROR_STRING "ERROR: Miscellaneous (%s)\n"

int reportError(int errorCode, char* string) {
    // Delegate to appropriate function
    switch (errorCode) {
        case BAD_ARGUMENT_COUNT:
            badArgumentCount();
            break;
        case BAD_FILE_NAME:
            badFileName(string);
            break;
        case BAD_MAGIC_NUMBER:
            badMagicNumber(string);
            break;
        case IMAGE_MALLOC_FAILED:
            imageMallocFailed();
            break;
        case BAD_DATA:
            badData(string);
            break;
        case OUTPUT_FAILED:
            outputFailed(string);
            break;
        case MISC:
            miscError(string);
            break;
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