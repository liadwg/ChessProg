#ifndef CHESS_UTILS_
#define CHESS_UTILS_

//#ifndef CHESS_
//#include "ChessProg.h"
//#endif CHESS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

#define perror_message(func_name) (perror("Error: standard function %s has failed", func_name));

typedef enum { WHITE = 0, BLACK = 1 } COLOR;

typedef struct pos{
	int col;
	int row;
} Pos;

typedef struct move{
	Pos piece;
	Pos dest;
	char promote; // value 0 means no promotion
	int score;
	struct move* next;
} Move;


extern void* mem_list[5000];
extern int mem_count;
extern int fail_safe;

// Pointer list management
void add_to_list(void* mem);
void remove_from_list(void* mem);

// safe_funcs verifies that that the original functions succeeded
void * safe_malloc(size_t size);
void * safe_realloc(void *old_pointer, size_t size);
int safe_fgetc(FILE *stream);
void safe_free(void * mem);

#endif CHESS_UTILS_