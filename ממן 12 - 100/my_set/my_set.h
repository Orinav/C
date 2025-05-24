#include <stdio.h>
#include <stdlib.h>
#define ENLARGE_SIZE 5  /* A macro that represents the amount of integers we'll want to add to the set */


typedef struct
{
    int *members;
    int length;
    int size_left; /* in bytes */
}set;

int not_member(set* , int);
set* get_set();
void print_set(set*);
