#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

struct line {
    char *s;
    struct line *next;
};

/**
Print all the parameters for this program
./a.aut uno due
*/
int main (void) {

    int fileDescriptor = open("openFile.c", O_RDONLY);
    printf("Error number is %d\n", errno);
    if (fileDescriptor == -1) {
        perror("Unable to open file");
        return 1;
    }

    char buf[32];
    ssize_t nread;
    while(1) {
        nread = read(fileDescriptor, buf, sizeof(buf));
        if (nread == -1) {
            perror("Reading file");
            return 0;
        }
        if (nread == 0) {
            break;
        }
        printf("%d\n", 1);
    }

    close(fileDescriptor);

    return 0;
}


