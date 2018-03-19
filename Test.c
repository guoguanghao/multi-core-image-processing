#include "stdio.h"

int main()
{
    unsigned int *intP;
    unsigned char *charP,arr[5]={1,2,3,4,5};
    charP = arr ;
    intP = charP;
    printf("%x\n",*intP++);
    printf("%x",*intP);
}