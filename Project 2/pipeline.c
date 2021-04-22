#include "myshell.h"

// define pipe ends for clarity
#define READ  0
#define WRITE 1


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: open_pipeline
 * ----------------------------
 * @brief    Pipes the stdout of shell command c1 to the standard input
 *           of shell command c2
 *
 * @param *c1    SHELLCMD pointer to the shell command writing to pipe
 *
 * @param *c2    SHELLCMD pointer to shell command reading from pipe
 *
 * @return status   Integer exit status of last executed command
 */

int open_pipeline(SHELLCMD *c1, SHELLCMD *c2)
{
    int status;
    int pid;

    int pipefd[2];

    pipe(pipefd);

    // failed to fork
    if ((pid = fork()) == -1)
    {
        perror("fork()");
        status = EXIT_FAILURE;
    }

    if (pid == 0)
    {
        // close unused file descriptor
        close(pipefd[READ]);
        // duplicate write-end descriptor to stdout
        dup2(pipefd[WRITE], fileno(stdout));
        // close duplicate file descriptor
        close(pipefd[WRITE]);

        _exit(execute_shellcmd(c1));
    }

    else
    {
        wait(&status);

        // failed to fork
        if ((pid = fork()) == -1)
        {
            perror("fork()");
            status = EXIT_FAILURE;
        }

        if (pid == 0)
        {
            // close unused file descriptor
            close(pipefd[WRITE]);
            // duplicate read-end descriptor to stdin
            dup2(pipefd[READ], fileno(stdin));
            // close duplicate file descriptor
            close(pipefd[READ]);

            _exit(execute_shellcmd(c2));
        }
        else
        {
            // close both duplicate file descriptors
            close(pipefd[WRITE]);
            close(pipefd[READ]);

            wait(&status);
        }
    }

    if (WIFEXITED(status))
    {
        status = WEXITSTATUS(status);
    }

    return status;
}
