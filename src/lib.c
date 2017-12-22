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
	AFI_PUTS(str);
}

void do_dotd(afi_Entry *self, afi_State *state) {
	char str[16];
	sprintf(str, "%d", PEEK(state->args));
	AFI_PUTS(str);
}

void do_drop(afi_Entry *self, afi_State *state) {
	POP(state->args);
}

afi_Node *lib_initDict(afi_Node *dict) {
	afi_Entry *add  = afi_defPrimitive("+",    do_add);
	afi_Entry *sub  = afi_defPrimitive("-",    do_sub);
	afi_Entry *mul  = afi_defPrimitive("*",    do_mul);
	afi_Entry *div  = afi_defPrimitive("/",    do_div);
	afi_Entry *size = afi_defPrimitive("SIZE", do_size);
	afi_Entry *dot  = afi_defPrimitive(".",    do_dotd);
	afi_Entry *dotd = afi_defPrimitive(".D",   do_dotd);
	afi_Entry *peek = afi_defPrimitive("PEEK",   do_dotd);
	afi_Entry *drop = afi_defPrimitive("DROP",   do_drop);
	afi_Entry *pop  = afi_defCompound("POP",  2);

	pop->words[0] = dot;
	pop->words[1] = drop;

	dict = afi_addEntry(dict, add);
	dict = afi_addEntry(dict, sub);
	dict = afi_addEntry(dict, mul);
	dict = afi_addEntry(dict, div);
	dict = afi_addEntry(dict, size);
	dict = afi_addEntry(dict, dot);
	dict = afi_addEntry(dict, dotd);
	dict = afi_addEntry(dict, peek);
	dict = afi_addEntry(dict, drop);
	dict = afi_addEntry(dict, pop);

	return dict;
}
