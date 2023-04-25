#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

// clear stshell screen
void clear_screen()
{
    printf("\033[H\033[J");
}

int main()
{
    int argc;
    char *argv[10];
    char command[1024];
    char *token;

    signal(SIGINT, SIG_IGN); // cancelling Ctrl+C
    while (1)
    {
        printf("\033[1;34mstshell> \033[0m"); // Blue
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0

        // clear the screen
        if (strcmp(command, "clear") == 0)
        {
            clear_screen();
            continue;
        }

        // exit the program
        if (strcmp(command, "exit") == 0)
        {
            exit(0);
        }

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
        {
            continue;
        }

        int single_rediract_exist = -1;
        int double_rediract_exist = -1;
        int pipe_exist = 0;
        int pipes[] = {-1, -1};

        /* for commands not part of the shell command language */
        pid_t id1 = fork();
        if (id1 == 0)
        {
            signal(SIGINT, SIG_DFL); // exist Ctrl+C
            for (int i = 0; i < argc; i++)
            {
                if (strcmp(argv[i], ">") == 0)
                {
                    single_rediract_exist = i;
                }
                if (strcmp(argv[i], ">>") == 0)
                {
                    double_rediract_exist = i;
                }
                if (strcmp(argv[i], "|") == 0)
                {
                    pipes[pipe_exist] = i;
                    pipe_exist++;
                }
            }

            if (single_rediract_exist != -1)
            {
                if (freopen(argv[single_rediract_exist + 1], "w", stdout) == NULL)
                {
                    printf("freopen failed\n");
                    return 1;
                }
                argv[single_rediract_exist] = NULL;
            }
            if (double_rediract_exist != -1)
            {
                if (freopen(argv[double_rediract_exist + 1], "a", stdout) == NULL)
                {
                    printf("freopen failed\n");
                    return 1;
                }
                argv[double_rediract_exist] = NULL;
            }
            if (pipes[0] != -1)
            {
                argv[pipes[0]] = NULL;

                // execute the first command
                if (fork() == 0)
                {
                    // redirect stdout into a text file
                    if (freopen("pipes.txt", "w", stdout) == NULL)
                    {
                        printf("freopen failed\n");
                        return 1;
                    }

                    // execute the first command so its output goes to the file
                    if (execvp(argv[0], argv) == -1)
                    {
                        printf("%s: command not found\n", argv[0]);
                        return 1;
                    }
                }
                wait(NULL);

                // two pipelines
                if (pipes[1] != -1)
                {
                    argv[pipes[1]] = NULL;

                    if (fork() == 0)
                    {
                        // redirect stdin into a text file
                        if (freopen("pipes1.txt", "r", stdin) == NULL)
                        {
                            printf("freopen failed\n");
                            return 1;
                        }

                        // redirect stdout into a text file
                        if (freopen("pipes2.txt", "w", stdout) == NULL)
                        {
                            printf("freopen failed\n");
                            return 1;
                        }

                        // execute the second command so its input is from the file
                        if (execvp(argv[pipes[0] + 1], argv + pipes[0] + 1) == -1)
                        {
                            printf("%s: command not found\n", argv[0]);
                            return 1;
                        }
                    }
                    wait(NULL);

                    // redirect stdin into a text file
                    if (freopen("pipes2.txt", "r", stdin) == NULL)
                    {
                        printf("freopen failed\n");
                        return 1;
                    }

                    // execute the third command so its input is from the file
                    if (execvp(argv[pipes[1] + 1], argv + pipes[1] + 1) == -1)
                    {
                        printf("%s: command not found\n", argv[0]);
                        return 1;
                    }
                }

                // one pipe
                else
                {
                    // redirect stdin into a text file
                    if (freopen("pipes1.txt", "r", stdin) == NULL)
                    {
                        printf("freopen failed\n");
                        return 1;
                    }

                    // execute the second command so its input is from the file
                    if (execvp(argv[pipes[0] + 1], argv + pipes[0] + 1) == -1)
                    {
                        printf("%s: command not found\n", argv[0]);
                        return 1;
                    }
                }
            }
            else if (execvp(argv[0], argv) == -1)
            {
                printf("%s: command not found\n", argv[0]);
                return 1;
            }
        }
        wait(NULL);
    }
}
