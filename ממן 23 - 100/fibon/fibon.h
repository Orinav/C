#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    long value; /* we defined value as long in order to give it the ability to hold big numbers */
    struct Node *next;
}
        Node;

long getValue(int);
Node* createNode(int);
Node* createCircularList(int);
void printList(Node *, int);
void argCheck(int);
void negativeNCheck(int n);
void scanfCheck(int);
void fileHandling(Node *, char *[], int);
void freeCircularList(Node *, int);
