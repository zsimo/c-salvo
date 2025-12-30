#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
    char *s = malloc(4+len+1);
    uint32_t *lenptr = (uint32_t*)s;
    *lenptr = len;

    s += 4;
    for (int j = 0; j < len; j++) {
        s[j] = init[j];
    }
    s[len] = 0;
    return s;
}

/* Display the string 's' on the screen. */
void ps_print(char *s) {
    uint32_t *lenptr = (uint32_t*)(s-4);
    for (uint32_t j = 0; j < *lenptr; j++) {
        putchar(s[j]);
    }
    printf("\n");
}

/* Free a previously created PS string.*/
void ps_free(char *s) {
    free(s-4);
}

/* Return the length of the string in O(1) time. */
uint32_t ps_len(char *s) {
    uint32_t *lenptr = (uint32_t*)(s-4);
    return *lenptr;
}

int main (void) {
    char *mystr = ps_create("Hello World", 11);
    ps_print(mystr);
    ps_print(mystr);
    printf("%s %d\n", mystr, (int)ps_len(mystr));
    ps_free(mystr);
    return 0;
}
