#include "myshell.h"


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: execute_command
 * ----------------------------
 * @brief    Forks the myshell process and executes the input shell command
 *           using the execv() system call
 *
 * @param *t    SHELLCMD pointer to shell command to be executed
 *
 * @return status    Integer exit status of executed shell command
 */

int execute_command(SHELLCMD *t)
{
    signal(SIGUSR1, SIG_IGN);
    int status;
    int pid;

    switch (pid = fork())
    {
        // fork failed
        case -1:
            perror("fork()");
            status = EXIT_FAILURE;
            break;

        case 0:
            // check for file redirection
            check_redirects(t);

            if ((execv(t->argv[0], t->argv)) == -1)
            {
                // execv() failed, check for shellscript
                status = check_shellscript(t);

                if (status != EXIT_SUCCESS)
                {
                    // command not found
                    perror("execv()");
                    status = EXIT_FAILURE;
                }
                _exit(status);
            }
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
