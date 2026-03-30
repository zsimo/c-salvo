#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

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
/* Add the new element at the end of the list 'list' */
void listPush (tfobj *l, tfobj ele) {

}
/* ===================== Turn program into toy forth  list ===================*/

void parseSpaces(tfparser *parser) {

    while (isspace(parser->p[0])) {
         parser->p ++;
    }

}
void parseNumbers(tfparser *parser) {

    while (isspace(parser->p[0])) {
         parser->p ++;
    }

}

tfobj *compile(char *prg) {
    tfparser parser;
    parser.prg = prg;
    parser.p = prg;

    tfobj *parsed = createListObject();

    while (parser.p) {
        tfobj *o;
        char token_start = parser.p;

        parserSkipSpaces($parser);

        if (parser.p[0] == 0) {
            break; // end of program reached
        }

        if (isdigit(parser.p[0]) || parser.p[0] == '-') {
            o = parseNumbers($parser);
        } else {
            o = NULL;
        }

        // check if the current token produced a parsing error.
        if (o == NULL) {
            printf("Syntax error near: %32s ... \n", token_start);
            return NULL;
        } else {
            listPush(parsed, o);
        }
    }
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
//    tfobj *prg = compile(argv[1]);
//    exec(prftext);

    return 0;
}
