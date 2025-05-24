#include <stdio.h>

int count_bits(unsigned long);

int main(void)
{
    unsigned long num;
    printf("Please enter an unsigned long number: \n");
    scanf("%lu", &num);
    printf("The number of bits that lit in even positions is %d \n", count_bits(num));
    return 0;
}

int count_bits(unsigned long num)
{
    int size = sizeof(num)*8;
    unsigned long mask = 1UL;
    int even_counter = 0;
    int i;
    for (i = 0 ; i < size ; i+=2)
    {
        if (num & mask)
            even_counter++;
        mask <<= 2;
    }
    return even_counter;
}
