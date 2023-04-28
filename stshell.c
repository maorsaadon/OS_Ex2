#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

void clear_screen()
{
    printf("\033[H\033[J");
}

void signal_handler(int signal)
{
    if (signal == SIGINT)
        printf("\nchiled process terminated\n");
}

void exec(int start, int end, char *argv[10])
{
    char *argv_new[10];
    int index = 0;
    while (index < end)
    {
        argv_new[index] = argv[start];
        index++;
        start++;
    }
    argv_new[index] = NULL;
    execvp(argv_new[0], argv_new);
}

int rediract(int argc, int single_redirect, int double_redirect, char *argv[10])
{
    int end = -1;
    if (single_redirect != -1)
    {
        if (argc <= single_redirect + 1)
            exit(3);
        else
        {
            end = single_redirect;
            int fd = open(argv[single_redirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dup2(fd, 1) == -1)
                exit(2);
            close(fd);
        }
    }
    else if (double_redirect != -1)
    {
        if (argc <= double_redirect + 1)
            exit(3);
        else
        {
            end = double_redirect;
            int fd = open(argv[double_redirect + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (dup2(fd, 1) == -1)
                exit(2);
            close(fd);
        }
    }

    return end;
}

void error_handler(int error)
{
    printf("\n\033[0;31merror: \033[0m\n");
    if (error == 1)
        printf("Pipes part failed\n");
    else if (error == 2)
        printf("Error with dup2\n");
    else if (error == 3)
        printf("single rediract - wrong command\n");
    else if (error == 4)
        printf("double rediract - wrong command\n");
    else if (error == 5)
        printf("Signal error\n");
    else if (error == 6)
        printf("More thet one > / >>\n");
    else if (error == 7)
        printf("More then two |\n");
}

int main()
{

    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
        error_handler(5);

    int status;
    int argc;
    char *argv[10];
    char command[1024];
    char *token;

    while (1)
    {
        printf("\033[1;34mstshell> \033[0m"); // Blue

        fgets(command, 1024, stdin);

        command[strlen(command) - 1] = '\0';
        if (!strcmp(command, "clear"))
        {
            clear_screen();
            continue;
        }

        if (!strcmp(command, "exit"))
            exit(0);

        argc = 0;
        token = strtok(command, " ");
        int single_rediract_exist = -1;
        int double_rediract_exist = -1;
        int pipes[] = {-1, -1};
        int redirect_exist = 0;

        while (token != NULL)
        {
            argv[argc] = token;

            if (!strcmp(token, ">"))
            {
                redirect_exist = 1;
                if (single_rediract_exist == -1)
                    single_rediract_exist = argc;
                else
                    exit(6);

                argv[argc] = NULL;
            }
            else if (!strcmp(token, ">>"))
            {
                redirect_exist = 1;
                if (double_rediract_exist == -1)
                    double_rediract_exist = argc;
                else
                    exit(6);

                argv[argc] = NULL;
            }
            else if (!strcmp(token, "|"))
            {
                if (pipes[0] == -1)
                    pipes[0] = argc;
                else if (pipes[1] == -1)
                    pipes[1] = argc;
                else
                    exit(7);

                argv[argc] = NULL;
            }
            token = strtok(NULL, " ");
            argc++;
        }
        argv[argc] = NULL;

        if (argv[0] == NULL)
            continue;

        pid_t id1 = fork();
        if (id1 == 0)
        {

            if (signal(SIGINT, SIG_DFL) == SIG_ERR)
                error_handler(5);

            if (single_rediract_exist == -1 && double_rediract_exist == -1 && pipes[0] == -1 && pipes[1] == -1)
                execvp(argv[0], argv);
            else if (pipes[0] != -1 && pipes[1] == -1)
            {
                int fd[2];
                if (pipe(fd) == -1)
                    exit(1);

                pid_t id2 = fork();
                if (id2 == 0)
                {
                    close(fd[0]);
                    if (dup2(fd[1], 1) == -1)
                        exit(2);
                    close(fd[1]);
                    exec(0, pipes[0], argv);
                }
                else
                {
                    pid_t id3 = fork();
                    if (id3 == 0)
                    {
                        waitpid(id2, NULL, 0);

                        close(fd[1]);
                        if (dup2(fd[0], 0) == -1)
                            exit(2);
                        close(fd[0]);

                        int end = argc;
                        if (redirect_exist == 1)
                            end = rediract(argc, single_rediract_exist, double_rediract_exist, argv);

                        exec(pipes[0] + 1, end, argv);
                    }
                    else
                    {
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
                    exit(1);

                pid_t id2 = fork();
                if (id2 == 0)
                {
                    close(fd2[0]);
                    close(fd2[1]);

                    close(fd1[0]);
                    if (dup2(fd1[1], 1) == -1)
                        exit(2);
                    close(fd1[1]);

                    exec(0, pipes[0], argv);
                }
                else
                {
                    pid_t id3 = fork();
                    if (id3 == 0)
                    {
                        waitpid(id2, NULL, 0);

                        close(fd1[1]);
                        if (dup2(fd1[0], 0) == -1)
                            exit(2);
                        close(fd1[0]);

                        close(fd2[0]);
                        if (dup2(fd2[1], 1) == -1)
                            exit(2);
                        close(fd2[1]);

                        exec(pipes[0] + 1, pipes[1], argv);
                    }
                    else
                    {
                        pid_t id4 = fork();
                        if (id4 == 0)
                        {
                            waitpid(id3, NULL, 0);

                            close(fd1[0]);
                            close(fd1[1]);

                            close(fd2[1]);
                            if (dup2(fd2[0], 0) == -1)
                                exit(2);
                            close(fd2[0]);

                            int end = argc;
                            if (redirect_exist == 1)
                                end = rediract(argc, single_rediract_exist, double_rediract_exist, argv);
                            exec(pipes[1] + 1, end, argv);
                        }
                        else
                        {
                            close(fd1[0]);
                            close(fd1[1]);
                            close(fd2[1]);
                            close(fd2[0]);
                            waitpid(id4, NULL, 0);
                        }
                    }
                }
            }
            else if (redirect_exist == 1)
            {
                int end = rediract(argc, single_rediract_exist, double_rediract_exist, argv);
                exec(0, end, argv);
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
                if (1 <= exit_code && exit_code <= 7)
                    error_handler(exit_code);
            }
        }
    }
}
