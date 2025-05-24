#include "fibon.h"

int main(int argc, char *argv[])
{
    int n, num;
    Node *head = NULL;
    argCheck(argc);
    printf("Please enter the number n: ");
    num = scanf("%d", &n);
    scanfCheck(num);
    negativeNCheck(n);
    head = createCircularList(n);
    printList(head,n);
    fileHandling(head, argv, n);
    freeCircularList(head, n);
    return 0;
}

long getValue(int n)
{
    int i;
    long left = 1;
    long right = 1;
    long result;
    if (n == 0 || n == 1)
        return 1;
    for (i = 2 ; i <= n ; i++)
    {
        result = left + right;
        left = right;
        right = result;
    }
    return result;
}

Node* createNode(int n)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    if (temp == NULL)
    {
        fprintf(stderr, "Couldn't allocate enough memory for a new node \n");
        exit(1);
    }
    temp->value = getValue(n);
    temp->next = NULL;
    return temp;
}

Node* createCircularList(int n)
{
    int i;
    Node *head = NULL;
    Node *current = NULL;
    if (n == 0)
    {
        head = createNode(0);
        head->next = head;
        return head;
    }
    head = createNode(n);
    current = head;
    for (i = n-1 ; i >= 0 ; i--)
    {
        Node *newNode = createNode(i);
        current->next = newNode;
        current = newNode;
    }
    current->next = head;
    return head;
}

void printList(Node *head, int n)
{
    int i;
    for (i = 0 ; i <= n ; i++)
    {
        printf("%ld \n", head->value);
        head = head->next;
    }
}

void argCheck(int argumentsNumber)
{
    if (argumentsNumber != 2) /* expecting argc[0] to be the run file name and argc[1] to be the file name which we want to add the nodes values to it */
    {
        fprintf(stderr, "Arguments number is invalid \n");
        exit(1);
    }
}

void negativeNCheck(int n)
{
    if (n < 0)
    {
        fprintf(stderr, "You've entered a negative number \n");
        exit(1);
    }
}

void scanfCheck(int num)
{
    if (num != 1)
    {
        fprintf(stderr, "Failed to input n \n");
        exit(1);
    }
}

void fileHandling(Node *head, char *argv[], int n)
{
    int i;

    FILE *fptr = fopen(argv[1], "w");
    if (fptr == NULL)
    {
        fprintf(stderr, "Failed to open a new file \n");
        exit(1);
    }
    fprintf(fptr, "This program creates circular linked list of fibonacci sequence. \n");
    fprintf(fptr, "n used to represent the number of nodes(there are n+1 nodes). \n");
    fprintf(fptr, "In addition the node which use the value n is the node with the highest value. \n\n");
    fprintf(fptr, "The nodes values in descending order are: \n");

    for (i = 0; i <= n; i++)
    {
        fprintf(fptr,"%ld \n", head->value);
        head = head->next;
    }
    fclose(fptr);
}

void freeCircularList(Node *head, int n)
{
    int i;
    Node *current = NULL;
    Node *temp = NULL;
    if (head == NULL)
        return;
    else if (n == 0)
    {
        free(head);
        head = NULL; /* head is no more a dangling pointer */
        return;
    }
    current = head;
    for (i = 0; i <= n; i++)
    {
        if (i != n)
            temp = current->next;
        else
            temp = NULL;
        free(current);
        current = temp;
    }
}
