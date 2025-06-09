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

int main (void) {
    printf("Hello\n");
    printf("%d\n", sum(1, 2));
    incr();
    return 0;
}


