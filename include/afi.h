#ifndef _AFI_H_
#define _AFI_H_

#include <stdint.h>
#include <stdlib.h>
#include "io.h"

#define AFI_NAME_LEN 31

typedef struct afi_Entry afi_Entry;
typedef struct afi_Stack afi_Stack;
typedef struct afi_State afi_State;
typedef struct afi_Node afi_Node;

typedef void afi_Word(afi_Entry *, afi_State *);

typedef int32_t afi_int_t;

struct afi_State {
	afi_Node *dict;
	afi_Stack *args;
};

struct afi_Entry {
	char name[AFI_NAME_LEN+1];
	afi_int_t literal; // Used for passing literals to docol.
	afi_Word* codeword;
	afi_int_t numwords;
	afi_Entry* words[];
};

struct afi_Stack {
	size_t    size;
	afi_int_t *top;
	afi_int_t data[];
};

struct afi_Node {
	afi_Node *prev;
	afi_Entry *entry;
};

int afi_exec(afi_State *state, const char *buf, size_t buflen);

afi_Entry *afi_find(afi_Node *dict, const char *name);

afi_Entry *afi_defLiteral(afi_int_t literal);
afi_Entry *afi_defEntry(const char *name, afi_Word *codeword,
					   size_t num_words);

afi_Node *afi_addEntry(afi_Node *dict, afi_Entry *entry);

afi_Node *afi_newNode();

afi_State *afi_initState(size_t stack_size);
void afi_freeState(afi_State *state);
void afi_freeDict(afi_Node *head);

void docol(afi_Entry *self, afi_State *state);

#define PUSH(stack,data) {if(SIZE(stack) < stack->size) { *(++stack->top) = data; }}
#define SIZE(stack) (stack->top - stack->data)
#define POP(stack)  ((SIZE(stack) > 0) ? *(stack->top--) : 0)
#define PEEK(stack) ((SIZE(stack) > 0) ? *(stack->top) : 0)

#endif
