#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

int main()
{
    int argc;
    char *argv[10];
    char command[1024];
    char *token;

    while (1)
    {
        printf("hello: ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0

        /* parse command line */
        argc = 0;
        token = strtok(command, " ");
        while (token != NULL)
        {
            argv[argc] = token;
            token = strtok(NULL, " ");
            argc++;
        }
        argv[argc] = NULL;

        /* Is command empty */
        if (argv[0] == NULL)
            continue;

        signal(SIGINT, SIG_IGN); // cancling Ctrl+C

        int rediract_exist = 0;
        int pipe_exist = 0;
        int pipes[] = {-1, -1};

        /* for commands not part of the shell command language */
        pid_t id1 = fork();
        if (id1 == 0)
        {
            signal(SIGINT, SIG_DFL); // exist Ctrl+C
            for(int i = 0; i < argc; i++)
            {
                if (strcmp(argv[i], ">") == 0)
                {
                    rediract_exist = 1;
                }
                if (strcmp(argv[i], ">>") == 0)
                {
                    rediract_exist = 2;
                }
                if (strcmp(argv[i], "|") == 0)
                {
                    pipes[pipe_exist] = i;
                    pipe_exist++;
                }
            }

            if(rediract_exist == 0 && pipe_exist == 0){
                printf("command not found\n");
                return 1;
            }
            pid_t id2 = fork();
            if(id2 == 0){
                
            }

            execvp(argv[0], argv);
            wait(NULL);
        }
    }
}
