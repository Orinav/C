#include "genericArray.h"
#include <stdlib.h>
#include <string.h>

typedef struct GenericArray
{
    void *items;
    unsigned int itemCounter;
    unsigned int itemSize;
    unsigned int arrayLength;
}GenericArray;


GenericArray* createGenericArray(unsigned int itemSize)
{
    GenericArray *array = (GenericArray*)calloc(1, sizeof(GenericArray));
    array->arrayLength = 2;
    array->itemSize = itemSize;
    array->items = calloc(array->arrayLength, itemSize);
    return array;
}


unsigned int getItemCounter(const GenericArray *array)
{
    return array->itemCounter;
}

void* getItems(const GenericArray *array)
{
    return array->items;
}


void freeGenericArray(GenericArray **array)
{
    if (array == NULL || *array == NULL)
        return;

    free((*array)->items);
    free(*array);
    *array = NULL;
}


void *addItem(GenericArray *array, const void *item)
{
    void *temp = NULL;
    unsigned int offset;

    if (array->itemCounter == array->arrayLength)
    {
        array->arrayLength *= 2;
        temp = realloc(array->items, array->arrayLength * array->itemSize);
        if (temp == NULL)
        {
            array->arrayLength /= 2;
            return NULL;
        }
        array->items = temp;
    }
    offset = array->itemCounter * array->itemSize;
    memcpy((char*)array->items + offset, item, array->itemSize);
    array->itemCounter++;
    return (char*)array->items + offset;
}
