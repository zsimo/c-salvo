#include <stdio.h>


int main (void) {
    char mystr[] = "ABCD";
    char *p = mystr;
    printf("At the beginning is %p\n", p);
    while (*p != 0) {
        putchar(*p);
        p ++;
    }
    printf("\n");
    printf("At the end is is %p\n", p);
    return 0;
}


