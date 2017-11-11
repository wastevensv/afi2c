#include "afi.h"
#include <stdlib.h>
#include <string.h>

void docol(afi_Entry *self, afi_State *state) {
	for(int i=0; i<self->numwords; i++) {
		afi_Entry *curr_word = self->words[i];
		if(curr_word->codeword == NULL)
		{
			PUSH(state->args,curr_word->literal);
		}
		else
		{
			curr_word->codeword(curr_word,state);
		}
	}
}

int afi_exec(afi_State *state, const char *buf, size_t buflen) {
	char tok[AFI_NAME_LEN+1];
	int word_start = 0;
	int word_end = 0;
	for(int i=0; i<buflen+1; i++)
	{
		if((buf[i] == ' ' || buf[i] == 0))
		{
			if(word_end == i)
			{
				tok[i - word_start] = 0;
				afi_Entry *word = afi_find(state->dict, tok);
				if(word == NULL)
				{
					char *end;
					afi_int_t literal = strtol(tok,&end,10);
					if(end == tok)
					{
						return i;
					}
					PUSH(state->args,literal);
				} else {
					word->codeword(word,state);
				}
			}
			word_start = i+1;
		}
		else
		{
			if(i - word_start >= AFI_NAME_LEN)
			{
				return i;
			}
			tok[i - word_start] = buf[i];
			word_end = i+1;
		}
	}
	return 0;
}

afi_Entry *afi_find(afi_Node *dict, const char *name) {
	afi_Node *curr_node = dict;
	while(curr_node->entry != NULL)
	{
		afi_Entry *curr_entry = curr_node->entry;
		if(strncmp(curr_entry->name,name,AFI_NAME_LEN) == 0)
		{
			return curr_entry;
		}
		curr_node = curr_node->prev;
	}
	return NULL;
}

afi_Entry *afi_defEntry(const char *name, afi_Word *codeword,
					   size_t num_words) {
	// malloc entry header + array of words
	afi_Entry *entry = malloc(sizeof(afi_Entry) + (sizeof(afi_Entry*) * num_words));

	// Copy word name.
	strncpy(entry->name, name, AFI_NAME_LEN);

	entry->literal = 0;
	entry->numwords = num_words;

	// Set codeword pointer.
	entry->codeword = codeword;

	return entry;
}

afi_Entry *afi_defLiteral(afi_int_t literal) {
	// malloc entry header + array of words
	afi_Entry *entry = malloc(sizeof(afi_Entry));

	entry->literal = literal;

	// Set codeword pointer.
	entry->codeword = NULL;

	return entry;
}

afi_Node *afi_addEntry(afi_Node *dict, afi_Entry *entry){
	afi_Node *head = malloc(sizeof(afi_Node));
	head->prev = dict;
	head->entry = entry;
	return head;
}

afi_Node *afi_newNode() {
	afi_Node *head = malloc(sizeof(afi_Node));
	head->prev = NULL;
	head->entry = NULL;
	return head;
}

afi_Stack *afi_newStack(size_t stack_size) {
	afi_Stack *stack = malloc(sizeof(afi_Stack) + sizeof(afi_int_t) * stack_size);
	stack->top = stack->data;
	stack->size = stack_size;
	return stack;
}

afi_State *afi_initState(size_t stack_size) {
	afi_State *state = malloc(sizeof(afi_State));
	state->args = afi_newStack(stack_size);
	state->dict = afi_newNode();
	return state;
}

void afi_freeState(afi_State *state) {
	free(state->args);
	afi_freeDict(state->dict);
	free(state);
}

void afi_freeDict(afi_Node *head) {
	while(head != NULL)
	{
		afi_Node *curr = head;
		head = head->prev;
		free(curr->entry);
		free(curr);
	}
}
