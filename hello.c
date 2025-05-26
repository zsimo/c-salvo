#include <stdio.h>

int sum (int a, int b) {
    int result = a + b;
    return result;
}

int main (void) {
    printf("Hello\n");
    printf("%d\n", sum(1, 2));
    return 0;
}


