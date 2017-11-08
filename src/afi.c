#include "afi.h"

int afi_exec(afi_State *state, const char *buf, size_t buflen) {
	char tok[AFI_NAME_LEN+1];
	int word_start = 0;
	int word_end = 0;
	for(int i=0; i<buflen+1; i++)
	{
		if((buf[i] == ' ' || buf[i] == 0))
		{
			if(word_end == i) {
				tok[i - word_start] = 0;
				afi_Entry *word = afi_find(state->dict, tok);
				if(word == NULL)
				{
					return -2;
				}
				word->codeword(word,state);
			}
			word_start = i+1;
		}
		else
		{
			if(i - word_start >= AFI_NAME_LEN)
			{
				return -1;
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
	afi_Entry *entry = malloc(sizeof(afi_Entry) + sizeof(afi_Word) * num_words);
	
	// Copy word name.
	strncpy(entry->name, name, AFI_NAME_LEN);

	entry->flags = 0;
	
	// Set codeword pointer.
	entry->codeword = codeword;

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
