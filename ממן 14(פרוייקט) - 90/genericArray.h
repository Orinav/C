#ifndef GENERICARRAY_H
#define GENERICARRAY_H
#define GenericArrayLoop(i,p,s,ga) for(i=0,p=getItems(ga),s=getItemCounter(ga);i<s;i++)


struct GenericArray;

struct GenericArray* createGenericArray(unsigned int itemSize);
void freeGenericArray(struct GenericArray **array);
void *addItem(struct GenericArray *array, const void *item);
unsigned int getItemCounter(const struct GenericArray *array);
void* getItems(const struct GenericArray *array);


#endif
