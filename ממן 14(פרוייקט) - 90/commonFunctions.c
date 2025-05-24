#include <string.h>
#include <stdlib.h>
#include "commonFunctions.h"


char* myStrcat(const char *firstString, const char *secondString)
{
    unsigned int firstLength = strlen(firstString);
    unsigned int secondLength = strlen(secondString);
    unsigned int combinedLength = firstLength + secondLength + 1; /* added 1 more for the null terminator */
    char *concatenatedString = (char*)calloc(1, combinedLength);

    strcpy(concatenatedString, firstString);
    strcat(concatenatedString,secondString);

    return concatenatedString;
}
