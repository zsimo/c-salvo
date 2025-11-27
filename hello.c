#include <stdio.h>

int sum (int a, int b) {
    int result = a + b;
    return result;
}

void incr (void) {
    int x = 1;
    x += 1;
    printf("%d\n", x);
    printf("sizeof %d bytes %d\n", x, (int)sizeof(x));
}

void array () {
    int a[2] = {1, 2};
    // in c le stringhe sono array ti tipo char
    char str[] = "simo";
     printf("%s\n%d\n", str, a[0]);
}

int main (void) {
    printf("Hello\n");
    printf("%d\n", sum(1, 2));
    incr();
    array();
    return 0;
}


