#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

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
void *xrealloc (void *oldptr, size_t size) {
    void *ptr = realloc(oldptr, size);
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
    o->str.ptr = xmalloc(len + 1);
    o->str.len = len;
    memcpy(o->str.ptr, s, len);
    o->str.ptr[len] = 0;
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
    l->list.ele = xrealloc(l->list.ele, sizeof(tfobj*) * (l->list.len + 1));
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

/* Return true if the character 'c' if one of the chars acceptable for our symbols. */
int is_symbol_char(int c) {
    char symchars[] = "+-*/%";
    if (isalpha(c)) {
        return 1;
    } else if (strchr(symchars, c) != NULL) {
        return 1;
    } else {
        return 0;
    }
}

tfobj *parseSymbol(tfparser *parser) {

    char *start = parser->p;
    while (parser->p[0] && is_symbol_char(parser->p[0])) {
       parser->p++;
    }
    int len = parser->p - start;
    return createSymbolObject(start, len);
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

        if (isdigit(parser.p[0]) || (parser.p[0] == '-' && isdigit(parser.p[1]))) {
            o = parseNumbers(&parser);
        } else if (is_symbol_char(parser.p[0])) {
            o = parseSymbol(&parser);
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
void printObj (tfobj *o) {
    switch (o->type) {
        case TFOBJ_TYPE_INT:
            printf("%d", o->i);
        break;
        case TFOBJ_TYPE_LIST:
            printf("[");
            for (size_t j = 0; j < o->list.len; j++) {
                tfobj *ele = o->list.ele[j];
                printObj(ele);
                if (j != o->list.len - 1) {
                    printf(" ");
                }
            }
            printf("]");
        break;
        case TFOBJ_TYPE_SYMBOL:
            printf("%s", o->str.ptr);
        break;
        default:
            printf("?");
        break;
    }

}
/* ================================ Execution and context =====================================*/

tfctx *createContext(void) {
    tfctx *ctx = xmalloc(sizeof(*ctx));
    ctx->stack = createListObject();
    return ctx;
}

void exec (tfctx *ctx, tfobj *prg) {
    assert(prg->type == TFOBJ_TYPE_LIST);

    for (size_t j = 0; j < prg->list.len; j++) {
        tfobj *word = prg->list.ele[j];
//        printObj(word);
//        if (j != o->list.len - 1) {
//            printf(" ");
//        }
        switch (word->type) {
            case TFOBJ_TYPE_SYMBOL:
            break;
            default:
            listPush(ctx->stack, word);
            break;
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
    tfobj *prg = compile(prgtext);
    printObj(prg);
    printf("\n");
    tfctx *ctx = createContext();
    exec(ctx, prg);

    printf("Stack content at end:");
    printObj(ctx->stack);
    printf("\n");
    return 0;
}
