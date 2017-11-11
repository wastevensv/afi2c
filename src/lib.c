#include "afi.h"
#include <stdio.h>
#include "lib.h"

void do_add(afi_Entry *self, afi_State *state) {
	afi_int_t a = POP(state->args);
	afi_int_t b = POP(state->args);

	PUSH(state->args, a+b);
}

void do_sub(afi_Entry *self, afi_State *state) {
	afi_int_t a = POP(state->args);
	afi_int_t b = POP(state->args);

	PUSH(state->args, b-a);
}

void do_mul(afi_Entry *self, afi_State *state) {
	afi_int_t a = POP(state->args);
	afi_int_t b = POP(state->args);

	PUSH(state->args, a*b);
}

void do_div(afi_Entry *self, afi_State *state) {
	afi_int_t a = POP(state->args);
	afi_int_t b = POP(state->args);

	PUSH(state->args, b/a);
}

void do_size(afi_Entry *self, afi_State *state) {
	char str[16];
	sprintf(str, "%ld", SIZE(state->args));
	puts(str);
}

void do_emit(afi_Entry *self, afi_State *state) {
	char str[16];
	sprintf(str, "%d", PEEK(state->args));
	puts(str);
}

void do_pop(afi_Entry *self, afi_State *state) {
	POP(state->args);
}

afi_Node *lib_initDict(afi_Node *dict) {
	afi_Entry *add  = afi_defEntry("+", do_add, 0);
	afi_Entry *sub  = afi_defEntry("-", do_sub, 0);
	afi_Entry *mul  = afi_defEntry("*", do_mul, 0);
	afi_Entry *div  = afi_defEntry("/", do_div, 0);
	afi_Entry *size = afi_defEntry("SIZE", do_size, 0);
	afi_Entry *emit = afi_defEntry("EMIT", do_emit, 0);
	afi_Entry *pop  = afi_defEntry("POP", do_pop, 0);

	dict = afi_addEntry(dict, add);
	dict = afi_addEntry(dict, sub);
	dict = afi_addEntry(dict, mul);
	dict = afi_addEntry(dict, div);
	dict = afi_addEntry(dict, size);
	dict = afi_addEntry(dict, emit);
	dict = afi_addEntry(dict, pop);

	return dict;
}
