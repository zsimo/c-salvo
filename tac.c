#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct line {
    char *s;
    struct line *next;
};

/**
Print all the parameters for this program
./a.aut uno due
*/
int main (int argc, char **argv) {
    if (argc != 2) {
        printf("Missing file name\n");
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("The file does not exists\n");
        return 1;
    }

    char buf[1024];
    struct line *head = NULL;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        // printf("%s", buf);
        struct line *l = malloc(sizeof(struct line));
        size_t linelen = strlen(buf);
        l->s = malloc(strlen(buf)+1);
        for (size_t j = 0; j <= linelen; j++) {
            l->s[j] = buf[j];
        }
        l->next = head;
        head = l;
    }
    fclose(fp);

    while (head != NULL)  {
        printf("%s", head->s);
        head = head->next;
    }

//    for (int i = 0; i < argc; i++) {
//        printf("%d: %s\n", i, argv[i]);
//    }
    return 0;
}


