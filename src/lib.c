#include "afi.h"
#include "lib.h"

void do_add(afi_Entry *self, afi_State *state) {
	afi_int_t a = POP(state->args);
	afi_int_t b = POP(state->args);

	PUSH(state->args, a+b);
}

void do_ident(afi_Entry *self, afi_State *state) {
	puts(self->name);
	PUSH(state->args, 2);
}

void do_size(afi_Entry *self, afi_State *state) {
	char c[] = {SIZE(state->args)+'0','\0'};
	puts(c);
}

void do_emit(afi_Entry *self, afi_State *state) {
	char c[] = {POP(state->args)+'0','\0'};
	puts(c);
}

afi_Node *lib_initDict(afi_Node *dict) {
	dict = afi_addEntry(dict, afi_defEntry("TEST", do_ident, 0));
	dict = afi_addEntry(dict, afi_defEntry("TEST2", do_ident, 0));
	dict = afi_addEntry(dict, afi_defEntry("ADD", do_add, 0));
	dict = afi_addEntry(dict, afi_defEntry("EMIT", do_emit, 0));
	dict = afi_addEntry(dict, afi_defEntry("SIZE", do_size, 0));
	return dict;
}
