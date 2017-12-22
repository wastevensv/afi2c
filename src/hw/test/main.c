#include "afi.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

#define STACK_SIZE 32

int afi_assert(afi_State *state, char *line, int exp_return,
				afi_int_t *exp_stack, size_t exp_stack_size) {
	STACK_RESET(state->args);
	printf("Testing input: %s\n",line);

	int err = afi_exec(state, line, strlen(line));

	if(err != 0) {
		if(err != exp_return) {
			fprintf(stderr, "Return code mismatch:\n Expected: %d\n Actual: %d\n",
					exp_return, err);	
			return 1;
		}
		else
		{
			printf("Test passed.\n");
			return 0;
		}
	}
		

	if(exp_stack_size != SIZE(state->args))
	{
		fprintf(stderr, "Size Mismatch:\n Expected: %ld\n Actual:   %ld\n",
				exp_stack_size, SIZE(state->args));	
		return 1;
	}
	
	for(int i = 0; i < SIZE(state->args); i++)
	{
		if(exp_stack[i] != state->args->data[i])
		{
			fprintf(stderr, "Data Mismatch at index %d:\n Expected: %d\n Actual:   %d\n",
					i, exp_stack[i], state->args->data[i]);	
			return 1;
		}
	}
	printf("Test Passed.\n");
	return 0;
}

int run_tests(afi_State *state) {
	int fails = 0;

	afi_int_t exp_stack[33] = {};
	fails += afi_assert(state,
						"",
						0,exp_stack,0);
	for(int i = 0; i < (state->args->size); i++) exp_stack[i] = i+1;
	fails += afi_assert(state,
						"1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33",
						0,exp_stack,32);
	exp_stack[0] = 12;
	fails += afi_assert(state,
						"2 2 + 3 *",
						0,exp_stack,1);
	exp_stack[0] = 0;
	fails += afi_assert(state,
						"4 4 - ZBRANCH2 1 BRANCH1 0",
						0,exp_stack,1);
	exp_stack[0] = 1;
	fails += afi_assert(state,
						"4 2 - ZBRANCH2 1 BRANCH1 0",
						0,exp_stack,1);
	return fails;
}

int main(int argc, char *argv) {
	afi_State *state = afi_initState(STACK_SIZE);

	state->dict = lib_initDict(state->dict);
	int fails = run_tests(state);
	fprintf(stderr,"%d tests failed.\n",fails);
	afi_freeState(state);
	return 0;
}
