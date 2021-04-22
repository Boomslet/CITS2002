#include "myshell.h"


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: create_subshell
 * ----------------------------
 * @brief   Forks a sub-shell of the myshell process and executes the input
 *          shell commands within the created sub-shell
 *
 * @param *t    SHELLCMD pointer to shell commands to be executed in a sub-shell
 *
 * @return status   Integer exit-status of executed shell commands
 */

int create_subshell(SHELLCMD *t)
{
    int status;
    int pid;

    switch (pid = fork())
    {
        case -1:
            perror("fork()");
            status = EXIT_FAILURE;
            break;

        case 0:
            signal(SIGUSR1, SIG_IGN);
            // check for file redirection
            check_redirects(t);


            status = execute_shellcmd(t->left);
            _exit(status);
            break;

        default:
            wait(&status);
            break;
    }

    if (WIFEXITED(status))
    {
        status = WEXITSTATUS(status);
    }

    return status;
}
