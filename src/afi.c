#include "afi.h"
#include <string.h>

// NOP word. Only useful when ending a branch.
void do_nop(afi_Entry *self, afi_State *state) {
	return;
}

// Interpret a compound word.
// Also used to interpret an already parsed line.
void docol(afi_Entry *self, afi_State *state) {
	if(self->type != AFI_T_COMP) return;
	for(int i=0; i<self->numwords; i++) {
		afi_Entry *curr_word = self->words[i];
		switch(curr_word->type) {

			// If literal, push onto stack.
		case AFI_T_LITR:
			PUSH(state->args,curr_word->literal);
			break;

			// If primitive, execute codeword.
		case AFI_T_PRIM:
			curr_word->codeword(curr_word,state);
			break;
			
			// If another compound word, recurse.
		case AFI_T_COMP:
			docol(curr_word,state);
			break;

			// If unconditional branch, skip to the offset.
		case AFI_T_UBRN:
			i += curr_word->offset;
			break;

			// If conditional branch, check if the top of the stack is zero.
			//    if so, skip to the offset.
		case AFI_T_CBRN:
			if(POP(state->args) == 0) i+= curr_word->offset;
			break;

			// If type unknown, silently ignore.
		default:
			break;
		}
	}
}

// Parse and execute a buffer of text.
int afi_exec(afi_State *state, const char *buf, size_t buflen) {
	char toks[buflen];
	size_t num_toks = 0;
	int word_start = 0;
	int word_end = 0;
	for(int wr = 0, rd=0; rd<buflen+1; rd++) // Parse and tokenize.
	{
		if((buf[rd] == ' ' || buf[rd] == 0)) // If whitespace found
		{
			// If first whitespace after word end, save token.
			if(word_end == rd)
			{
				toks[wr++] = '\0'; // Mark end of token.
				num_toks += 1;
			}
			word_start = rd+1;
		}
		else // If non-whitespace, add it to the word name.
		{
			// If word is too long, error.
			if(rd - word_start >= AFI_NAME_LEN) return rd;

			// Otherwise, copy the character to token list.
			toks[wr++] = buf[rd];
			word_end = rd+1;
		}
	}
	if(toks[0] == '\0') return 0; // Trivial success if empty string.

	afi_Stack *branches = afi_newStack(num_toks);
	afi_Entry *line_word = afi_defCompound("", num_toks);
	char *cur_tok = toks;
	for(afi_int_t t = 0; t < num_toks; t++) // Symbol Lookup
	{
		// check if word exists.
		afi_Entry *word = afi_find(state->dict, cur_tok);

		// Check for special case words.
		if(strncmp(cur_tok,"BRANCH",6) == 0)
		{
			// (internal) Add unconditional branch with specified offset.
			word = afi_defBranch(strtol(cur_tok+6,NULL,10),false);
			state->dict = afi_addEntry(state->dict, word);
		}
		else if(strncmp(cur_tok,"ZBRANCH",7) == 0)
		{
			// (internal) Add conditional branch with specified offset.
			word = afi_defBranch(strtol(cur_tok+7,NULL,10),true);
			state->dict = afi_addEntry(state->dict, word);
		}
		// WHILE .. DO .. LOOP loop
		else if(strcmp(cur_tok,"WHILE") == 0)
		{
			// Push location of WHILE onto stack.
			PUSH(branches, t);
			// Insert placeholder NOP.
			word = afi_find(state->dict,"NOP");
		}
		else if(strcmp(cur_tok,"DO") == 0)
		{
			// Push location of DO onto stack.
			PUSH(branches, t);
			// Insert placeholder cond. branch.
			word = afi_defBranch(0,true);
			state->dict = afi_addEntry(state->dict, word);
		}
		else if(strcmp(cur_tok,"LOOP") == 0)
		{
			// Modify placeholder cond. branch to have proper offset.
			if(SIZE(branches) < 1)
			{
				free(line_word);
				free(branches);
				return -(t+1);
			}
			afi_int_t do_loc = POP(branches);
			afi_int_t while_loc = POP(branches);
			line_word->words[do_loc]->offset = (t+1) - do_loc;
			word = afi_defBranch((while_loc - t),false);
			state->dict = afi_addEntry(state->dict, word);
		}
		// IF .. ELSE .. END branch
		else if(strcmp(cur_tok,"IF") == 0)
		{
			// Push location of IF onto stack.
			PUSH(branches, t);
			// Insert placeholder cond. branch in line_word.
			word = afi_defBranch(0,true);
			state->dict = afi_addEntry(state->dict, word);
		}
		else if(strcmp(cur_tok,"ELSE") == 0)
		{
			// Modify placeholder cond. branch to have proper offset.
			if(SIZE(branches) < 1)
			{
				free(line_word);
				free(branches);
				return -(t+1);
			}
			afi_int_t if_loc = POP(branches);
			line_word->words[if_loc]->offset = t - if_loc;
			// Push location of ELSE onto stack.
			PUSH(branches, t);
			// Insert placeholder uncond. branch in line_word.
			word = afi_defBranch(0,false);
			state->dict = afi_addEntry(state->dict, word);
		}
		else if(strcmp(cur_tok,"END") == 0)
		{
			// Modify placeholder cond. branch to have proper offset.
			if(SIZE(branches) < 1)
			{
				free(line_word);
				free(branches);
				return -(t+1);
			}
			afi_int_t else_loc = POP(branches);
			line_word->words[else_loc]->offset = t - else_loc;
			// Insert NOP to keep token number consistent.
			word = afi_find(state->dict,"NOP");
		}
		else if(word == NULL) // If word not found, assume literal.
		{
			// Attempt to convert to literal.
			char *end;
			afi_int_t literal = strtol(cur_tok,&end,10);
			if(end == cur_tok) // If invalid token
			{
				free(line_word);
				free(branches);
				return -(t+1);
			}

			// If literal, store word in dictionary.
			word = afi_defLiteral(literal);
			state->dict = afi_addEntry(state->dict, word);
		}

		// Store word in compound word, and advance to next token.
		line_word->words[t] = word;
		cur_tok += strlen(cur_tok) + 1;
	}

	// If there are still unclosed branches, error.
	if(SIZE(branches) > 0)
	{
		free(line_word);
		free(branches);
		return -(num_toks+1);
	}
	free(branches);

	docol(line_word, state); // Execute
	free(line_word);
	return 0;
}

// Check if a word exists in the dictionary.
afi_Entry *afi_find(afi_Node *dict, const char *name) {
	// Initialize current node to head of dictionary.
	afi_Node *curr_node = dict;

	// Iterate through dictionary until root node reached.
	while(curr_node->entry != NULL)
	{
		// Get the current word from the dictionary.
		afi_Entry *curr_entry = curr_node->entry;
		// Only check primitive and compound words.
		if(curr_entry->type == AFI_T_PRIM || curr_entry->type == AFI_T_COMP)
		{
			// Compare word name to name in query.
			if(strncmp(curr_entry->name,name,AFI_NAME_LEN) == 0)
			{
				// Match found, return word.
				return curr_entry;
			}
		}
		// Advance to next word.
		curr_node = curr_node->prev;
	}
	// No match found, return NULL.
	return NULL;
}

afi_Entry *afi_defPrimitive(const char *name, afi_Word *codeword) {
	// Initialize a new entry.
	afi_Entry *entry = malloc(sizeof(afi_Entry));

	// Copy word name to new entry.
	strncpy(entry->name, name, AFI_NAME_LEN);

	entry->type = AFI_T_PRIM;

	// Set codeword pointer to a C function.
	entry->codeword = codeword;

	return entry;
}

afi_Entry *afi_defCompound(const char *name, size_t num_words) {
	// Initialize entry header + array of words
	afi_Entry *entry = malloc(sizeof(afi_Entry) +
							  (sizeof(afi_Entry*) * num_words));

	// Copy word name to new word.
	strncpy(entry->name, name, AFI_NAME_LEN);

	entry->numwords = num_words;

	// Set codeword type.
	entry->type = AFI_T_COMP;

	return entry;
}

afi_Entry *afi_defBranch(size_t offset, bool cond) {
	// malloc entry header
	afi_Entry *entry = malloc(sizeof(afi_Entry));

	// Set if the branch is conditional or not.
	if(cond)
	{
		entry->type = AFI_T_CBRN;
	}
	else
	{
		entry->type = AFI_T_UBRN;
	}

	// Set the offset of the branch.
	entry->offset = offset;

	return entry;
}

afi_Entry *afi_defLiteral(afi_int_t literal) {
	// malloc entry header
	afi_Entry *entry = malloc(sizeof(afi_Entry));

	// Set type of word to literal.
	entry->type = AFI_T_LITR;

	// Store literal in word.
	entry->literal = literal;

	return entry;
}

afi_Node *afi_addEntry(afi_Node *dict, afi_Entry *entry){
	// Malloc new Node in dictionary.
	afi_Node *head = malloc(sizeof(afi_Node));

	// Link new entry and set it as the head.
	head->prev = dict;
	head->entry = entry;
	return head;
}

afi_Node *afi_newNode() {
	// Initialize a new dictionary.
	afi_Node *head = malloc(sizeof(afi_Node));

	// Set root node to be NULL, and point to NULL.
	head->prev = NULL;
	head->entry = NULL;
	return head;
}

afi_Stack *afi_newStack(size_t stack_size) {
	// Initialize new stack header, and stack space.
	afi_Stack *stack = malloc(sizeof(afi_Stack)
							  + (sizeof(afi_int_t) * stack_size));
	
	// Set stack initial metadata.
	stack->top = stack->data;
	stack->size = stack_size;
	return stack;
}

afi_State *afi_initState(size_t stack_size) {
	// Initialize a new AFI state object
	afi_State *state = malloc(sizeof(afi_State));
	
	// Add an args stack to the state.
	state->args = afi_newStack(stack_size);
	// Add a dictionary to the state.
	state->dict = afi_newNode();

	// Include a single default word, nop. Needed for branching.
	afi_Entry *nop = afi_defPrimitive("NOP",do_nop);
	state->dict = afi_addEntry(state->dict, nop);

	return state;
}

void afi_freeState(afi_State *state) {
	// Free an unused state, starting with the args stack.
	free(state->args);
	// Free the dictionary.
	afi_freeDict(state->dict);
	// Finally, free the state object itself.
	free(state);
}

void afi_freeDict(afi_Node *head) {
	// Free an unused dictionary, and all the words inside it.
	while(head != NULL)
	{
		afi_Node *curr = head;
		head = head->prev;
		free(curr->entry);
		free(curr);
	}
}
