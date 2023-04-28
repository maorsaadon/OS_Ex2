#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>

/*
    clear_screen() -
    This function clears the terminal screen by printing escape sequences "\033[H\033[J".
*/
void clear_screen()
{
    printf("\033[H\033[J");
}

/*
    signal_handler(int signal) -
    This function is a signal handler that takes an integer signal as input and prints
    "chiled process terminated" if the input signal is SIGINT.
*/
void signal_handler(int signal)
{
    if (signal == SIGINT)
        printf("\nchiled process terminated\n");
}

/*
    exec(int start, int end, char *argv[10]) -
    This function takes in a starting index 'start', ending index 'end', and an array of strings 'argv'.
    It copies the strings between 'start' and 'end' from the input 'argv' array to a new 'argv_new' array and sets the last element of this new array to NULL.
    The function then calls the execvp system call with the first element of 'argv_new' as the command and 'argv_new' as the argument list.
*/
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

/*
    rediract(int argc, int single_redirect, int double_redirect, char *argv[10]) -
    This function takes in the number of arguments 'argc',
    indices of single and double redirection operators 'single_redirect' and 'double_redirect', and an array of strings 'argv'.
    If a redirection operator is present, it opens a file with appropriate flags and redirects stdout to that file using dup2 system call.
    The function returns the value of 'end' which is used in subsequent parts of the program.
*/
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

/*
    error_handler(int error) -
    This function takes in an integer error code and prints appropriate error messages based on the error code.
    This function is used for handling errors that occur during program execution.
*/
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

/*
    This is a C program that implements a simple shell. It allows the user to input commands and execute them.
    The shell supports the following features:
        1.Basic command execution
        2.I/O redirection
        3.Piping
    The program starts with the main function that first ignores the SIGINT signal by setting it to SIG_IGN (ignore signal) and then initializes some variables:
        1.status: an integer variable that stores the status of the child process
        2.argc: an integer variable that stores the number of command-line arguments
        3.argv: a character array that stores the command-line arguments
        4.command: a character array that stores the command entered by the user
        5.token: a character pointer used to tokenize the input command
    The program then enters an infinite loop where it repeatedly prompts the user to enter a command and then processes the command.
    The prompt is printed using ANSI escape codes to color the text blue.
    The input command is read using the fgets function, which reads a line of text from the user.
    The newline character at the end of the input is replaced with a null character.
    If the user enters the "clear" command, the clear_screen function is called to clear the screen, and the loop continues.
    If the user enters the "exit" command, the program exits.
    The input command is then tokenized using the strtok function. The tokenized strings are stored in the argv array, and the number of arguments is stored in the argc variable.
    The program also checks for the presence of I/O redirection symbols and piping symbols in the command.
    The program then forks a child process to execute the command. If the fork fails, an error is raised.
    If the command does not include I/O redirection or piping, the execvp function is called to execute the command.
    If the command includes piping, a series of child processes are forked to handle the piping.
    If the command includes I/O redirection, the rediract function is called to handle the redirection.
    After the command is executed, the child process exits, and the parent process continues the loop.
    If the user presses Ctrl+C, the SIGINT signal is ignored and the program does not exit.
*/
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
