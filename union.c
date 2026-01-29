#include <stdio.h>


struct foo {

    // i and a sono sovrapposte, partono dallo stesso offset
    union {
        int i;
        unsigned char a[4];
    };
};

int main(void) {

    struct foo f;



    return 0;
}
