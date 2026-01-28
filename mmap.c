#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

int main (void) {

    int fileDescriptor = open("mmap.c", O_RDONLY);
    // mem is a memory address not typed (non tipizzato)
    void *mem = mmap(NULL, 100, PROT_READ, MAP_FILE|MAP_SHARED, fileDescriptor, 0);
    printf("File mapped at %p", mem);

    // get access to the file via the s pointer
    char *s = mem;
    for (int j = 0; j < 10; j++) {
        printf("s[%d] = %c\n", j, s[j]);
    }
    return 0;
}


