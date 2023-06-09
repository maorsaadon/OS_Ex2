#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

int main() {
	int i;
	char *argv[10];
	char command[1024];
	char *token;

	while (1) {
	    printf("hello: ");
	    fgets(command, 1024, stdin);
	    command[strlen(command) - 1] = '\0'; // replace \n with \0

	    /* parse command line */
	    i = 0;
	    token = strtok (command," ");
	    while (token != NULL)
	    {
		argv[i] = token;
		token = strtok (NULL, " ");
		i++;
	    }
	    argv[i] = NULL;

	    /* Is command empty */ 
	    if (argv[0] == NULL)
		continue;

	    /* for commands not part of the shell command language */ 
	    if (fork() == 0) { 
		execvp(argv[0], argv);
		wait(NULL);
	    }    
	}
}
