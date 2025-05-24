#include "my_set.h"

int main()
{
    set *s = get_set();
    print_set(s);
    free(s->members); /* releasing the memory we allocated for set members */
    s->members = NULL; /* now the pointer members is no more a dangling pointer */
    free(s); /* releasing the memory we allocated for s */
    s = NULL; /* now the pointer s is no more a dangling pointer */
    return 0;
}

/**
 * The function not_member checks whether a given number exists in a given set.
 * @param s a pointer to set.
 * @param number a number to check if he doesn't exist in the set.
 * @return 1 if it doesn't exist, 0 if it already exists.
 */
int not_member(set *s, int number)
{
    int *p;
    for (p = s->members ; p < s->members + s->length ; p++)
    {
        if (*p == number)
            return 0;
    }
    return 1;
}

/**
 * The function get_set builds the set using scanf to input numbers and allocate memory for them if needed.
 * @return pointer to a built set.
 */
set *get_set()
{
    int num;
    int new_size; /* in bytes */
    int *temp_set = NULL; /* temp_set will be use later for realloc function */

    set *s = (set *)malloc(sizeof(set));
    if (s == NULL)
    {
        printf("Could not allocate enough memory for set structure");
        exit(1);
    }

    s->members = (int *)malloc(sizeof(int));
    if (s->members == NULL)
    {
        printf("Could not allocate memory for set members");
        free(s); /* releasing the memory we allocated for s */
        s =  NULL; /* now the pointer s is no more a dangling pointer */
        exit(1);
    }

    s->length = 0;
    s->size_left = sizeof(int); /* we already allocated memory for one integer */

    printf("Please enter numbers to add to the set: \n");
    while(scanf("%d", &num) != EOF)
    {
        if (not_member(s, num) && s->size_left < sizeof(int)) /* there's not enough space for a new number */
        {
            new_size = (s->length + ENLARGE_SIZE) * sizeof(int); /* the new size(in bytes) of our set with the addition of 'ENLARGE_SIZE' more integers */
            temp_set = (int *)realloc(s->members , new_size);
            if (temp_set == NULL)
            {
                printf("Could not allocate memory for more set members");
                free(s->members); /* releasing the memory we allocated for set members */
                s->members = NULL; /* now the pointer members is no more a dangling pointer */
                free(s); /* releasing the memory we allocated for s */
                s = NULL; /* now the pointer s is no more a dangling pointer */
                exit(1);
            }
            s->members = temp_set;
            *(s->members + s->length) = num;
            s->length++;
            s->size_left += ENLARGE_SIZE*sizeof(int) - sizeof(int); /* we added bytes for 'ENLARGE_SIZE' more integers and added one integer to the set */
        }
        else if (not_member(s, num) && s->size_left >= sizeof(int)) /* no need to allocate more memory */
        {
            *(s->members + s->length) = num;
            s->length++;
            s->size_left -= sizeof(int);
        }
    }
    return s;
}

/**
 * The function print_set will print a given set.
 * @param s a pointer to a set.
 */
void print_set(set *s)
{
    int *p;
    printf("The set you created is: \n");
    for (p = s->members ; p < s->members + s->length ; p++)
    {
        printf("%d ", *(p));
    }
}

