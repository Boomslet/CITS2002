#if defined(__linux__)
# define _POSIX_SOURCE
#endif

#include "myshell.h"
#include <sys/types.h>

/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */

// handles catching of the SIGCHLD signal when child process terminates
void sighandler(int signum)
{
    int pid = getpid();

    if (signum == SIGCHLD)
    {
        fprintf(stderr, "Process %d terminated\n", pid);
        exit(EXIT_SUCCESS);
    }
    if (signum == SIGUSR1)
    {
        fprintf(stderr, "Terminating process %d\n", pid);
    }
}

void sigh(int signum)
{
    int pid = getpid();

    fprintf(stderr, "Process %d terminated\n", pid);
    // exit(0);
}

/**
 * Function: run_background
 * ----------------------------
 * @brief    Runs a process in the background and signals the parent process
 *           when it finishes it's execution
 *
 * @param *t    SHELLCMD pointer to shell command to run in background
 *
 * @return status    Integer exit status of background process creation
 */

int run_background(SHELLCMD *t)
{
    wait(NULL);
    int status = EXIT_SUCCESS;
    int pid;

    switch (pid = fork())
    {
        // failed to fork
        case -1:
            perror("fork()");
            status = EXIT_FAILURE;
            break;

        case 0:
            fprintf(stderr, "Started %i\n", getpid());
            // register the SIGCHLD signal with sighandler
            signal(SIGCHLD, sighandler);
            // register the SIGUSR1 signal with sighandler
            signal(SIGUSR1, sighandler);

            status = execute_shellcmd(t);
        // _exit(status);

        default:
            break;
    }

    return status;
}
