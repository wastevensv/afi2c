#include "afi.h"
#include "lib.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define STACK_SIZE 32

int main(int argc, char *argv) {
	// Will be moved into afi_initState(size_t stack_size) eventually.
	afi_State *state = malloc(sizeof(afi_State));
	state->dict = afi_newNode();
	state->args = malloc(sizeof(afi_Stack) + sizeof(afi_int_t) * STACK_SIZE);
	state->args->top = state->args->data;
	
	afi_Entry *entry = afi_defEntry("TEST", do_ident, 0);
	state->dict = afi_addEntry(state->dict, entry);
	state->dict = afi_addEntry(state->dict, afi_defEntry("TEST2", do_ident, 0));

	afi_Entry *found_entry = afi_find(state->dict, "TEST");

	assert(entry == found_entry);
	puts(found_entry->name);

	char line[] =  "TEST     TEST2  TEST";
	assert(afi_exec(state, line, strlen(line)) == 0);
	
	return 0;
}
