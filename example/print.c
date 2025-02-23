#include <stdio.h>

int main()
{
    int i = 100000;
    while(i > 0)
    {
        printf("Loop %d\n", i);
        i -= 1;
    }
    return 0;
}

