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

void signal_handler(int signal)
{
    if (signal == SIGINT)
        printf("process terminated\n");
}

void exec(int start, int end, char* argv[10])
{
    char *argv_new[10];
    int index = 0;
    while(index < end) {
        argv_new[index] = argv[start];
        index++;
        start++;
    }
    argv_new[index] = NULL;
    execvp(argv_new[0], argv_new);
}

void single_rediract(int start, int argc, int redirect, char *argv[10])
{
    if (argc <= redirect) 
    {
        exit(2);
    } else {
        char *argv_new[10];
        int index = 0;
        while(index < redirect-1) {
            argv_new[index] = argv[start];
            start++;
            index++;
        }
        argv_new[index] = NULL;
        FILE *fd = fopen(argv[redirect], "w");
        int fout = fileno(fd);
        dup2(fout, 1);
        fclose(fd);
        execvp(argv_new[0], argv_new);
    }
}
void double_rediract(int start, int argc, int redirect, char *argv[10])
{
    if ( argc <= redirect) {
        exit(3);
    } else {
        char *argv_new[10];
        int index = 0;
        while(index < redirect -1) {
            argv_new[index] = argv[start];
            start++;
            index++;
        }
        argv_new[index] = NULL;
        FILE *fd = fopen(argv[redirect], "a");
        int fout = fileno(fd);
        dup2(fout, 1);
        fclose(fd);
        execvp(argv_new[0], argv_new);

    }
}



int main()
{
    int status;
    int argc;
    char *argv[10];
    char command[1024];
    char *token;

    
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

        if (signal(SIGINT, signal_handler) == SIG_ERR)
        {
            printf("\nSIG_ERR\n");
        }
            
        /* parse command line */
        argc = 0;
        token = strtok(command, " ");

        int single_rediract_exist = -1;
        int double_rediract_exist = -1;
        int pipe_exist = 0;
        int pipes[] = {-1, -1};
        int simple = 0;

        while (token != NULL)
        {
            argv[argc] = token;
            
            if (strcmp(token, ">") == 0)
            {
                if(single_rediract_exist == -1)
                {
                    single_rediract_exist = argc;
                    simple = 1;
                } 
            }
            else if (strcmp(token, ">>") == 0)
            {
                if(double_rediract_exist == -1)
                {
                    double_rediract_exist = argc;
                    simple = 1;
                }
            }
            else if (strcmp(token, "|") == 0)
            {
                if(pipe_exist < 2)
                {
                    if (pipes[pipe_exist] == -1)
                    {
                        pipes[pipe_exist] = argc;
                        pipe_exist++;
                        simple = 1;
                    }
                } 
            }
            token = strtok(NULL, " ");
            argc++;
        }
        argv[argc] = NULL;

        /* Is command empty */
        if (argv[0] == NULL)
        {
            continue;
        }

         /* for commands not part of the shell command language */
        pid_t id1 = fork();
        if (id1 == 0)
        {
            if(simple == 0)
            {
                execvp(argv[0], argv);
            }
            else if(pipes[0] != -1 && pipes[1] == -1)
            {
                int fd[2];
                if (pipe(fd) == -1) {
                    exit(1);
                }
                pid_t id2 = fork();
                if (id2 == 0) {
                    close(fd[0]);
                    dup2(fd[1], 1); 
                    close(fd[1]);
                    exec(0, pipes[0], argv);
                } else {
                    pid_t id3 = fork();
                    if (id3 == 0) {
                        waitpid(id2, NULL, 0);
                        close(fd[1]);
                        dup2(fd[0], 0);
                        close(fd[0]);

                        if(single_rediract_exist != -1)
                        {
                            single_rediract(pipes[0]+1, argc, single_rediract_exist+1, argv);
                        }
                        else if(double_rediract_exist != -1)
                        {
                            single_rediract(pipes[0]+1, argc, double_rediract_exist+1, argv);
                        }
                        else
                        {
                            exec(pipes[0]+1, argc, argv);
                        }
                        
                    } else {
                        close(fd[0]);
                        close(fd[1]);
                        waitpid(id3, NULL, 0);
                    }
                }
            }
            else if (pipes[1] != -1) 
            {
                int fd1[2], fd2[2];
                if (pipe(fd1) == -1 || pipe(fd2) == -1) 
                {
                    exit(1);
                }

                pid_t id2 = fork();
                if (id2 == 0) {
                    close(fd2[0]);
                    close(fd2[1]);

                    close(fd1[0]);
                    dup2(fd1[1], 1);
                    close(fd1[1]);

                    exec(0, pipes[0], argv);
                } 
                else {
                    pid_t id3 = fork();
                    if (id3 == 0) {
                        waitpid(id2, NULL, 0);
                        close(fd1[1]);
                        dup2(fd1[0], 0);
                        close(fd1[0]);

                        close(fd2[0]);
                        dup2(fd2[1], 1);
                        close(fd2[1]);
                        
                        exec(pipes[0]+1, pipes[1], argv);
                    } 
                    else 
                    {
                        pid_t id4 = fork();
                        if (id4 == 0) {
                            waitpid(id3, NULL, 0);
                            close(fd1[0]);
                            close(fd1[1]);

                            close(fd2[1]);
                            dup2(fd2[0], 0);
                            close(fd2[0]);

                            if(single_rediract_exist != -1){
                                single_rediract(pipes[1]+1, argc, single_rediract_exist+1, argv);
                            }
                            if(double_rediract_exist != -1){
                                single_rediract(pipes[1]+1, argc, double_rediract_exist+1, argv);
                            }

                            exec(pipes[1]+1, argc, argv);
                        } 
                        else {
                            close(fd1[0]);
                            close(fd1[1]);
                            close(fd2[1]);
                            close(fd2[0]);
                            waitpid(id4, NULL, 0);
                        }
                    }
                }
            }
            else if((single_rediract_exist != -1) && (pipe_exist == 0))
            {
                single_rediract(0, argc, single_rediract_exist+1, argv);

            }
            else if((double_rediract_exist != -1) && (pipe_exist == 0))
            {
                single_rediract(0, argc, double_rediract_exist+1, argv);
            }
            
            wait(NULL);
            exit(0);
        }
        else
        {
            wait(&status);
            if (WIFEXITED(status)) 
            {
                int exit_code = WEXITSTATUS(status);
                if (exit_code) {
                    printf("\033[0;31merror: \033[0m");
                    if(exit_code == 1){
                        printf("Failed while openning pipes\n");
                    }
                    if(exit_code == 2){
                        printf("Missing file name after `>`\n");
                    }
                    if(exit_code == 3){
                        printf("Missing file name after `>>`\n");
                    }
                }
            }
        }
    }
}


