#ifndef _AFI_H_
#define _AFI_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define AFI_NAME_LEN 31

typedef struct afi_Entry afi_Entry;
typedef struct afi_Stack afi_Stack;
typedef struct afi_State afi_State;
typedef struct afi_Dict afi_Dict;

typedef void (*afi_Word)(afi_Entry *, afi_State *);

typedef int32_t afi_int_t;

struct afi_State {
	afi_Stack *args;
	afi_Dict *dict;
};

struct afi_Entry {
	char name[AFI_NAME_LEN+1];
	uint8_t flags; // Unused.
	afi_Word codeword;
	afi_Entry* words[];
};

struct afi_Stack {
	afi_int_t *top;
	afi_int_t data[];
};

struct afi_Dict {
	afi_Dict *prev;
	afi_Entry *entry;
};

void afi_exec(const char *buf, size_t buflen);

afi_Entry *afi_find(afi_Dict *dict, const char *name);

afi_Entry *afi_defEntry(const char *name, afi_Word codeword,
					   size_t num_words);

afi_Dict *afi_addEntry(afi_Dict *dict, afi_Entry *entry);

afi_Dict *afi_newDict();

#define AFI_PUSH(stack,data) {stack.top = data;}
#define AFI_POP(stack) (*stack.top++);

#endif
