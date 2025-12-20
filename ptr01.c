#include <stdio.h>


int main (void) {
    int x = 5;

    // create pointer of type int
    int *y = NULL;
    printf("%d\n", x);
    printf("%p\n", y);

    // assign the address of x to the y pointer
    y = &x;
    printf("%p\n", y);

    // write 10 to the y memory address (which is stored the x address)
    *y = 10;
    printf("%d\n", x);

    return 0;
}


