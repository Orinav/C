#include <stdio.h>
#define MAX_SIZE 80 /* The maximum length a string can be */

int my_strcmp(char [], char []);
int my_strncmp(char [], char [], int);
int my_strchr(char [], char);

int main(void)
{
    char str1[MAX_SIZE];
    char str2[MAX_SIZE];
    char c;
    int n;
    int choice = 0;
    printf("Please choose your desired function to activate: \n");
    printf("Enter 1 in order to activate my_strcmp \n");
    printf("Enter 2 in order to activate my_strncmp \n");
    printf("Enter 3 in order to activate my_strchr \n");
    scanf("%d", &choice);
    getchar(); /* to clean the input buffer from '\n' that scanf() provided */
    switch (choice)
    {
        case 1:
            printf("You chose to use my_strcmp function \n");
            printf("Enter your first String:");
            fgets(str1, MAX_SIZE, stdin);
            printf("Enter your second String:");
            fgets(str2, MAX_SIZE, stdin);
            printf("result: %d \n", my_strcmp(str1, str2));
            break;
        case 2:
            printf("You chose to use my_strncmp function \n");
            printf("Enter your first String:");
            fgets(str1, MAX_SIZE, stdin);
            printf("Enter your second String:");
            fgets(str2, MAX_SIZE, stdin);
            printf("Enter a number that represents the amount of chars you want to compare:");
            scanf("%d", &n);
            printf("result: %d \n", my_strncmp(str1, str2, n));
            break;
        case 3:
            printf("You chose to use my_strchr function \n");
            printf("Enter a String:");
            fgets(str1, MAX_SIZE, stdin);
            printf("Enter a character:");
            scanf("%c", &c);
            printf("result: %d \n", my_strchr(str1, c));
            break;
        default:
            printf("You entered an invalid number \n");
    }
    return 0;
}



int my_strcmp(char str1[], char str2[])
{
    int i;
    for (i = 0 ; str1[i]!='\0' ; i++)
    {
        if (str1[i] == str2[i])
            continue;
        else if (str1[i] > str2[i])
            return 1;
        else
            return -1;
    }
    return 0; /* if we reached the end of the string */
}

int my_strncmp(char str1[], char str2[], int n)
{
    int i;
    for (i = 0 ; str1[i]!='\0' && i<n ; i++)
    {
        if (str1[i] == str2[i])
            continue;
        else if (str1[i] > str2[i])
            return 1;
        else
            return -1;
    }
    return 0; /* if we reached the end of the string or i equals to n */
}

int my_strchr(char str[], char c)
{
    int i;
    for (i = 0 ; str[i]!='\0' ; i++)
    {
        if (str[i] == c)
            return i;
        else
            continue;
    }
    return -1; /* if the desired character is not found in the given string */
}
