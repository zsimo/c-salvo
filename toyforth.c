#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define TFOBJ_TYPE_INT 0
#define TFOBJ_TYPE_STR 1
#define TFOBJ_TYPE_BOOL 2
#define TFOBJ_TYPE_LIST 3
#define TFOBJ_TYPE_SYMBOL 4

/* =========================== Data structures ===============================*/

typedef struct tfobj {
    int refcount;
    int type; // TFOBJ_TYPE_*
    union {
        int i;
        struct {
            char *ptr;
            size_t len;
        } str;
        struct {
            struct tfobj **ele;
            size_t len;
        } list;
    };
} tfobj;


typedef struct tfparser {
    char *prg; // the program to compile into a list
    char *p;   // next token to parse
} tfparser;


typedef struct tfctx {
    tfobj *stack;
} tfctx;

/* ========================= Allocation wrappers =============================*/
void *xmalloc (size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory allocating %zu bytes\n", size);
        exit(1);
    }
    return ptr;
}

/* ====================== Object related functions ===========================
 * The following functions allocate Toy Forth objects of different types.
*/
/* Allocate and initialize a new Toy Forth object */
tfobj * createObject (int type) {
    tfobj *o = xmalloc(sizeof(tfobj));
    o->type = type;
    o->refcount = 1;
    return o;
}
tfobj * createStringObject (char *s, size_t len) {
    tfobj *o = createObject(TFOBJ_TYPE_STR);
    o->str.ptr = s;
    o->str.len = len;
    return o;
}
tfobj * createSymbolObject (char *s, size_t len) {
    tfobj *o = createStringObject(s, len);
    o->type = TFOBJ_TYPE_SYMBOL;
    return o;
}
tfobj * createIntObject (int i) {
    tfobj *o = createObject(TFOBJ_TYPE_INT);
    o->i = i;
    return o;
}
tfobj * createBoolObject (int i) {
    tfobj *o = createObject(TFOBJ_TYPE_BOOL);
    o->i = i;
    return o;
}

/* =============================== List object ===============================*/
tfobj * createListObject (void) {
    tfobj *o = createObject(TFOBJ_TYPE_LIST);
    o->list.ele = NULL;
    o->list.len = 0;
    return o;
}
/* Add the new element at the end of the list 'list'
 * It is up to the caller to increment the reference count of the element added if needed. */
void listPush (tfobj *l, tfobj *ele) {
    l->list.ele = realloc(l->list.ele, sizeof(tfobj*) * (l->list.len + 1));
    l->list.ele[l->list.len] = ele;
    l->list.len++;
}
/* ===================== Turn program into toy forth  list ===================*/

void parseSpaces(tfparser *parser) {

    while (isspace(parser->p[0])) {
         parser->p++;
    }

}

#define MAX_NUM_LEN 128
tfobj *parseNumbers(tfparser *parser) {

    char buf[MAX_NUM_LEN];
    char *start = parser->p;
    char *end;
    if (parser->p[0] == '-') {
        parser->p++;
    }

    while (parser->p[0] && isdigit(parser->p[0])) {
         parser->p++;
    }

   end = parser->p;
   int numlen = end - start;
   if (numlen >= MAX_NUM_LEN) {
        return NULL;
   }

   memcpy(buf, start, numlen);
   buf[numlen] = 0;

   tfobj *o = createIntObject(atoi(buf));
   return o;
}

tfobj *compile(char *prg) {
    tfparser parser;
    parser.prg = prg;
    parser.p = prg;

    tfobj *parsed = createListObject();

    while (parser.p) {
        tfobj *o;
        char *token_start = parser.p;

        parseSpaces(&parser);

        if (parser.p[0] == 0) {
            break; // end of program reached
        }

        if (isdigit(parser.p[0]) || parser.p[0] == '-') {
            o = parseNumbers(&parser);
        } else {
            o = NULL;
        }

        // check if the current token produced a parsing error.
        if (o == NULL) {
            // FIXME: release parsed here
            printf("Syntax error near: %32s ... \n", token_start);
            return NULL;
        } else {
            listPush(parsed, o);
        }
    }

    return parsed;
}

/* ======================== Execute the program ==============================*/
void exec (tfobj *prg) {
    printf("[");
    for (size_t j = 0; j < prg->list.len; j++) {
        tfobj *o = prg->list.ele[j];
        switch (o->type) {
            case TFOBJ_TYPE_INT:
                printf("%d", o->i);
                break;
            default:
                printf("?");
                break;
        }
        printf(" ");
    }
    printf("]\n");
}

/* ================================ Main =====================================*/

int main (int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    printf("%s %d\n", argv[0], argc);

    /* Read the program in memory for later parsing. */
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Opening Toy Forth program");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    char *prgtext = xmalloc(file_size+1);
    fseek(fp, 0, SEEK_SET);
    fread(prgtext, file_size, 1, fp);
    prgtext[file_size] = 0;
    fclose(fp);

    printf("Program text: %s\n", prgtext);
    tfobj *prg = compile(prgtext);
    exec(prg);

    return 0;
}
