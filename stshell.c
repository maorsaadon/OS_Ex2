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

// Define a signal handler function to handle signals received by the process
void signal_handler(int signal)
{
    // If the signal received is SIGINT (interrupt signal), print a message to the console
    if (signal == SIGINT)
        printf("\nchiled process terminated\n");
}

// Define a function to execute a command with arguments using the execvp() function
void exec(int start, int end, char *argv[10])
{
    char *argv_new[10]; // Create a new array to store the command and its arguments
    int index = 0;      // Initialize an index variable to keep track of the current position in the new array
    // Copy the arguments from the original array to the new array
    while (index < end)
    {
        argv_new[index] = argv[start];
        index++;
        start++;
    }
    // Set the last element of the new array to NULL (required by execvp())
    argv_new[index] = NULL;
    // Call execvp() to execute the command with its arguments
    execvp(argv_new[0], argv_new);
}

// Define a function to handle single output redirection
void single_rediract(int start, int argc, int redirect, char *argv[10])
{
    // Check if there are enough arguments to perform the redirection
    if (argc <= redirect)
        exit(3); // If not, exit the program with an error code 3
    else
    {
        // If there are enough arguments
        char *argv_new[10]; // Create a new array to store the command and its arguments
        int index = 0;      // Initialize an index variable to keep track of the current position in the new array
        // Copy the arguments before the redirect symbol to the new array
        while (index < redirect - 1)
        {
            argv_new[index] = argv[start];
            start++;
            index++;
        }
        // Set the last element of the new array to NULL (required by execvp())
        argv_new[index] = NULL;
        // Open the file specified by the redirect symbol for writing
        FILE *fd = fopen(argv[redirect], "w");
        // Redirect stdout to the file
        dup2(fileno(fd), 1);
        // Close the file
        fclose(fd);
        // Call execvp() to execute the command with its arguments
        execvp(argv_new[0], argv_new);
    }
}

// Define a function to handle double output redirection
void double_rediract(int start, int argc, int redirect, char *argv[10])
{
    // Check if there are enough arguments to perform the redirection
    if (argc <= redirect)
        exit(4); // If not, exit the program with an error code 4
    else
    {
        // If there are enough arguments
        char *argv_new[10]; // Create a new array to store the command and its arguments
        int index = 0;      // Initialize an index variable to keep track of the current position in the new array
        // Copy the arguments before the redirect symbol to the new array
        while (index < redirect - 1)
        {
            argv_new[index] = argv[start];
            start++;
            index++;
        }
        // Set the last element of the new array to NULL (required by execvp())
        argv_new[index] = NULL;
        // Open the file specified by the redirect symbol for append
        FILE *fd = fopen(argv[redirect], "a");
        // Redirect stdout to the file
        dup2(fileno(fd), 1);
        // Close the file
        fclose(fd);
        // Call execvp() to execute the command with its arguments
        execvp(argv_new[0], argv_new);
    }
}

// Define a function to handle errors
void error_handler(int error)
{
    // Print an error message in red
    printf("\033[0;31merror: \033[0m");
    // Use a switch statement to determine which error occurred and print an appropriate error message
    if (error == 1)
        printf("Pipes part failed\n");
    if (error == 2)
        printf("Error with dup2\n");
    if (error == 3)
        printf("single rediract - wrong command\n");
    if (error == 4)
        printf("double rediract - wrong command\n");
    if (error == 5)
        printf("Signal error\n");
}

int main()
{
    int status;
    int argc;
    char *argv[10];
    char command[1024];
    char *token;

    // loop indefinitely
    while (1)
    {
        // print shell prompt
        printf("\033[1;34mstshell> \033[0m"); // Blue

        // get input command from user
        fgets(command, 1024, stdin);

        // replace \n with \0
        command[strlen(command) - 1] = '\0'; // replace \n with \0

        // handle 'clear' command
        if (strcmp(command, "clear") == 0)
        {
            clear_screen();
            continue;
        }

        // handle 'exit' command
        if (strcmp(command, "exit") == 0)
            exit(0);

        // set signal handler for SIGINT
        if (signal(SIGINT, signal_handler) == SIG_ERR)
            error_handler(5);

        // parse command line
        argc = 0;
        token = strtok(command, " ");

        // initialize variables to track I/O redirection and pipes
        int single_rediract_exist = -1;
        int double_rediract_exist = -1;
        int pipe_exist = 0;
        int pipes[] = {-1, -1};
        int simple = 0;

        // loop through each token and add it to argv array
        while (token != NULL)
        {
            argv[argc] = token;

            // check for I/O single redirection
            if (strcmp(token, ">") == 0)
            {
                if (single_rediract_exist == -1)
                    single_rediract_exist = argc;
            }
            // check for I/O double redirection
            else if (strcmp(token, ">>") == 0)
            {
                if (double_rediract_exist == -1)
                    double_rediract_exist = argc;
            }
            // check for pipes
            else if (strcmp(token, "|") == 0)
            {
                if (pipe_exist < 2)
                {
                    if (pipes[pipe_exist] == -1)
                    {
                        pipes[pipe_exist] = argc;
                        pipe_exist++;
                    }
                }
            }
            // get next token
            token = strtok(NULL, " ");
            argc++;
        }
        // set last element of argv array to NULL
        argv[argc] = NULL;

        // if command is empty, continue to next iteration of loop
        if (argv[0] == NULL)
            continue;

        /* for commands not part of the shell command language */
        // fork a child process to execute the command
        pid_t id1 = fork();
        if (id1 == 0)
        {
            // check if I/O redirection or pipes are needed
            if (single_rediract_exist == -1 && double_rediract_exist == -1 && pipe_exist == 0)
                execvp(argv[0], argv);
            else if (pipes[0] != -1 && pipes[1] == -1)
            {
                // create pipe for single pipe
                int fd[2];
                if (pipe(fd) == -1)
                    exit(1);

                // fork child process to execute first command
                pid_t id2 = fork();
                // If this is the child process (id2 == 0), then redirect its standard output to the write end of the pipe (fd[1])
                if (id2 == 0)
                {
                    // Close the read end of the pipe, since we won't be using it in this process
                    close(fd[0]);
                    // Duplicate the write end of the pipe to standard output (file descriptor 1)
                    // If this fails, exit the process with status code 2
                    if (dup2(fd[1], 1) == -1)
                        exit(2);
                    // Close the original file descriptor for the write end of the pipe (since we've duplicated it to standard output)
                    close(fd[1]);
                    // Execute the command specified by argv, reading from the read end of the pipe (pipes[0])
                    // The first argument (0) is not used by this implementation of exec, but is required by convention
                    exec(0, pipes[0], argv);
                }
                else
                {
                    // fork another child process to execute second command
                    pid_t id3 = fork();
                    // check if the current process is the child process
                    if (id3 == 0)
                    {
                        waitpid(id2, NULL, 0); // wait for the second child process to finish

                        // Close the read end of the pipe, since we won't be using it in this process
                        close(fd[1]);
                        // redirect the standard input to the read end of the pipe
                        if (dup2(fd[0], 0) == -1)
                            exit(2);
                        // Close the original file descriptor for the write end of the pipe (since we've duplicated it to standard output)
                        close(fd[0]);

                        // check if there is a single redirection
                        // redirect the standard output to the specified file in write mode
                        if (single_rediract_exist != -1)
                            single_rediract(pipes[0] + 1, argc, single_rediract_exist + 1, argv);
                        // check if there is a double redirection
                        // redirect the standard output to the specified file in append mode
                        else if (double_rediract_exist != -1)
                            single_rediract(pipes[0] + 1, argc, double_rediract_exist + 1, argv);
                        // execute the second command
                        else
                            exec(pipes[0] + 1, argc, argv);
                    }
                    // current process is the parent process
                    else
                    {
                        // close both ends of the pipe
                        close(fd[0]);
                        close(fd[1]);
                        // wait for the second child process to finish
                        waitpid(id3, NULL, 0);
                    }
                }
            }
            else if (pipes[1] != -1)
            {
                // Create two pipes to connect the three child processes
                int fd1[2], fd2[2];
                if (pipe(fd1) == -1 || pipe(fd2) == -1)
                    exit(1);

                // Fork a child process to execute the first command
                pid_t id2 = fork();
                // check if the current process is the child process
                if (id2 == 0)
                {
                    // Close unused pipes
                    close(fd2[0]);
                    close(fd2[1]);

                    close(fd1[0]);
                    // Redirect stdout to the write end of the first pipe
                    if (dup2(fd1[1], 1) == -1)
                        exit(2);
                    close(fd1[1]);

                    // Execute the first command
                    exec(0, pipes[0], argv);
                }
                else
                {
                    // Fork another child process to execute the second command
                    pid_t id3 = fork();
                    // check if the current process is the child process
                    if (id3 == 0)
                    {
                        // Wait for the first command to finish
                        waitpid(id2, NULL, 0);

                        // Close unused pipes
                        close(fd1[1]);
                        // Redirect stdin to the read end of the first pipe
                        if (dup2(fd1[0], 0) == -1)
                            exit(2);
                        close(fd1[0]);

                        close(fd2[0]);
                        // Redirect stdout to the write end of the second pipe
                        if (dup2(fd2[1], 1) == -1)
                            exit(2);
                        close(fd2[1]);

                        // Execute the second command
                        exec(pipes[0] + 1, pipes[1], argv);
                    }
                    else
                    {
                        // Fork another child process to execute the third command
                        pid_t id4 = fork();
                        // check if the current process is the child process
                        if (id4 == 0)
                        {
                            // Wait for the second command to finish
                            waitpid(id3, NULL, 0);
                            // Close unused pipes
                            close(fd1[0]);
                            close(fd1[1]);

                            close(fd2[1]);
                            // Redirect stdin to the read end of the second pipe
                            if (dup2(fd2[0], 0) == -1)
                                exit(2);
                            close(fd2[0]);

                            // check if there is a single redirection
                            // redirect the standard output to the specified file in write mode
                            if (single_rediract_exist != -1)
                                single_rediract(pipes[1] + 1, argc, single_rediract_exist + 1, argv);
                            // check if there is a double redirection
                            // redirect the standard output to the specified file in append mode
                            if (double_rediract_exist != -1)
                                single_rediract(pipes[1] + 1, argc, double_rediract_exist + 1, argv);
                            // execute the third command
                            else
                                exec(pipes[1] + 1, argc, argv);
                        }
                        else
                        {
                            // Close all pipes and wait for the third child process to finish
                            close(fd1[0]);
                            close(fd1[1]);
                            close(fd2[1]);
                            close(fd2[0]);
                            waitpid(id4, NULL, 0);
                        }
                    }
                }
            }
            // check if there is a single redirection
            // redirect the standard output to the specified file in write mode
            else if (single_rediract_exist != -1)
                single_rediract(0, argc, single_rediract_exist + 1, argv);
            // check if there is a double redirection
            // redirect the standard output to the specified file in append mode
            else if (double_rediract_exist != -1)
                double_rediract(0, argc, double_rediract_exist + 1, argv);

            wait(NULL);
            exit(0);
        }
        // If the parent process is running, wait for the child to finish
        // and check the exit status of the child process.
        else
        {
            wait(&status);
            if (WIFEXITED(status))
            {
                // Get the exit code of the child process
                int exit_code = WEXITSTATUS(status);
                // Handle errors if any
                error_handler(exit_code);
            }
        }
    }
}
