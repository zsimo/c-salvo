#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct pls {
    uint32_t len;
    uint32_t refcount;
    uint32_t magic;
    char str[];
};

/* Initialize a prefixed length string with the specified string 'init' of length 'len'.
 * The created strings have the following layout:
 *
 * +----+------------------------
 * |LLLL|My strings here..
 * +----+-----------------------
 * Thus this strings are binary safe: zero bytes are permitted in the middle.
 * Warning: this function does not check for buffer overflows.
*/
char *ps_create(char *init, int len) {
    struct pls *p = malloc(sizeof(struct pls)+len+1);
    p->len = len;
    p->refcount = 1;
    p->magic = 0xDEADBEEF;

    for (int j = 0; j < len; j++) {
        p->str[j] = init[j];
    }
    p->str[len] = 0;
    return p->str;
}

/* Display the string 's' on the screen. */
void ps_print(char *s) {
    struct pls *p = (struct pls*)(s - sizeof(*p));
    for (uint32_t j = 0; j < p->len; j++) {
        putchar(p->str[j]);
    }
    printf("\n");
}

/* Free a previously created PS string.*/
void ps_free(char *s) {
    free(s-sizeof(struct pls));
}
/* Validate that a PS string looks valid. */
void ps_validate (struct pls *p) {
    if (p->magic != 0xDEADBEEF) {
        printf("Invalid string\n");
        exit(1);
    }
}
/* Drop the reference count of the string object by one
 * and frees the object if refcount reached 0. */
void ps_release(char *s) {
    struct pls *p = (struct pls*)(s - sizeof(*p));
    ps_validate(p);

    p->refcount--;
    if (p->refcount == 0) {
        p->magic = 0;
        ps_free(s);
    }
}
/* Increase the reference count of the string object. */
void ps_retain(char *s) {
    struct pls *p = (struct pls*)(s - sizeof(*p));;
    if (p->refcount == 0) {
        printf("Aborted on retain of illegal string\n");
        exit(1);
    }
    p->refcount++;
}

/* Return the length of the string in O(1) time. */
uint32_t ps_len(char *s) {
    struct pls *p = (struct pls*)(s - sizeof(*p));;
    return p->len;
}

char *global_string;

int main (void) {
    char *mystr = ps_create("Hello World", 11);
    global_string = mystr;
    ps_retain(mystr);
    ps_print(mystr);
    ps_release(mystr);
    printf("%s %d\n", mystr, (int)ps_len(mystr));
    ps_release(mystr);
    return 0;
}
