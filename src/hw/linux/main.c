#include "afi.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

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
		if(err > 0) {
			for(;err>=0;err--)
			{
				printf(" ");
			}
			printf("^\n");
		} else if(err < 0) {
			printf("Invalid token. Number: %d\n", -err);
		} else {
			add_history(line);
		}
		free(line);
	}

	afi_freeState(state);
	return 0;
}
