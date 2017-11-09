#include "afi.h"
#include "lib.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <readline/readline.h>

#define STACK_SIZE 32

int main(int argc, char *argv) {
	afi_State *state = afi_initState(STACK_SIZE);

	state->dict = lib_initDict(state->dict);

	while(1)
	{
		char *line = readline("> ");
		if(line == NULL)
		{
			puts("DONE");
			break;
		}
		int err = afi_exec(state, line, strlen(line));
		free(line);
		if(err > 0) {
			for(;err>=0;err--)
			{
				printf(" ");
			}
			printf("^\n");
		}
	}

	afi_freeState(state);
	return 0;
}
