#ifndef _AFI_H_
#define _AFI_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "io.h"

#define AFI_NAME_LEN 31
#define AFI_T_PRIM 0 // Primitive Word
#define AFI_T_COMP 1 // Compound Word
#define AFI_T_UBRN 2 // Unconditional Branch
#define AFI_T_CBRN 3 // Conditional Branch
#define AFI_T_LITR 4 // Numeric Literal Word

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
	uint8_t type;
	union {
		afi_Word *codeword; // For Primitive Words
		afi_int_t literal; // For Numeric Literals
		size_t offset; // For Branches
		struct { // For Compound Words
			size_t numwords;
			afi_Entry* words[];
		};
	};
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

afi_Entry *afi_defPrimitive(const char *name, afi_Word *codeword);
afi_Entry *afi_defCompound(const char *name, size_t num_words);
afi_Entry *afi_defLiteral(afi_int_t literal);
afi_Entry *afi_defBranch(size_t offset, bool cond);

afi_Node *afi_addEntry(afi_Node *dict, afi_Entry *entry);

afi_Node *afi_newNode();

afi_State *afi_initState(size_t stack_size);
void afi_freeState(afi_State *state);
void afi_freeDict(afi_Node *head);

#define PUSH(stack,data) {if(SIZE(stack) < stack->size) { *(stack->top++) = data; }}
#define SIZE(stack) (stack->top - stack->data)
#define POP(stack)  ((SIZE(stack) > 0) ? *(--stack->top) : 0)
#define PEEK(stack) ((SIZE(stack) > 0) ? *((stack->top)-1) : 0)
#define STACK_RESET(stack) {stack->top = stack->data;}

#endif
