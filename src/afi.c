#include "afi.h"

void afi_exec(const char *buf, size_t buflen) {
}

afi_Entry *afi_find(afi_Dict *dict, const char *name) {
	afi_Dict *curr_node = dict;
	while(curr_node->entry != NULL) {
		afi_Entry *curr_entry = curr_node->entry;
		if(strncmp(curr_entry->name,name,AFI_NAME_LEN) == 0) {
			return curr_entry;
		}
		curr_node = curr_node->prev;
	}
	return NULL;
}

afi_Entry *afi_defEntry(const char *name, afi_Word codeword,
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

afi_Dict *afi_addEntry(afi_Dict *dict, afi_Entry *entry){
	afi_Dict *head = malloc(sizeof(afi_Dict));
	head->prev = dict;
	head->entry = entry;
	return head;
}

afi_Dict *afi_newDict() {
	afi_Dict *head = malloc(sizeof(afi_Dict));
	head->prev = NULL;
	head->entry = NULL;
	return head;
}
