#if defined(__linux__)
# define _BSD_SOURCE
#endif

#include "myshell.h"
#include <sys/time.h>
#include <time.h>
#include <errno.h>


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: run_exit
 * ----------------------------
 * @brief    Terminates the myshell process with provided exit status if
 *           present, otherwise exits with the exit status of the last
 *           executed command, lastcmd_status
 *
 * @param *t    SHELLCMD pointer to shell command that may contain exit status
 *
 * @return void
 */

void run_exit(SHELLCMD *t)
{
    if (t->argc > 2)
    {
        fprintf(stderr, "exit: too many arguments\n");
        return;
    }

    // terminate early if stdin or stdout is piped
    if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
    {
        return;
    }

    // parent ignores SIGTERM
    signal(SIGUSR1, SIG_IGN);

    // terminate any still-running background processes
    kill(0, SIGUSR1);

    // wait for any background processes to finish
    while (waitpid(-1, NULL, 0) > 0)
    {
        if (errno == ECHILD)
        {
            break;
        }
    }

    // exit status provided
    if (t->argc == 2)
    {
        exit(atoi(t->argv[1]));
    }
    // default
    else
    {
        exit(lastcmd_status);
    }
}

/**
 * Function: run_cd
 * ----------------------------
 * @brief    Changes the current working directory to a provided directory
 *           path if present, otherwise defaults to the HOME directory
 *
 * @param *t    SHELLCMD pointer to shell command that may contain
 *              directory path
 *
 * @return status    Integer exit status of chdir
 */

int run_cd(SHELLCMD *t)
{
    int status = EXIT_SUCCESS;

    // directory provided
    if (t->argc > 1)
    {
        // increment the t->argv pointer so we don't pass 'cd' forward
        t->argv++;
        t->argc--;

        // check if the first character is '/'
        if (t->argv[0][0] != '/')
        {
            char *path_start = strdup(t->argv[0]);

            // if the supplied path contains '/', tokenise and search for
            // only the first portion of the directory path
            if (strchr(path_start, '/') != NULL)
            {
                path_start = strtok(path_start, "/");
            }

            char *cmd_path = search_path(path_start, CDPATH);

            update_shellcmd(t, cmd_path);
        }

        // failed to change directory
        if (chdir(t->argv[0]) == -1)
        {
            perror("chdir()");
            status = EXIT_FAILURE;
        }

        // decrement the pointer to prepare for free_shellcmd()
        t->argv--;
        t->argc++;
    }
    // default
    else
    if (chdir(HOME) == -1)
    {
        perror("chdir()");
        status = EXIT_FAILURE;
    }

    return status;
}

/**
 * Function: run_time
 * ----------------------------
 * @brief    Prints the millisecond execution time of the following
 *           shell command to stderr
 *
 * @param *t    SHELLCMD pointer to shell command to be timed
 *
 * @return status    Integer exit status of timed shell command
 */

int run_time(SHELLCMD *t)
{
    int status = EXIT_FAILURE;

    if (t->argc > 1)
    {
        // increment the t->argv pointer so we don't pass 'time' forward
        t->argv++;
        t->argc--;

        struct timeval begin, end, result;

        // capture start time
        gettimeofday(&begin, NULL);

        status = execute_shellcmd(t);

        // capture finish time
        gettimeofday(&end, NULL);

        // subtract begin from end
        timersub(&end, &begin, &result);

        // convert to milliseconds
        long elapsed = (1000 * result.tv_sec)
          + (result.tv_usec / 1000.0);

        fprintf(stderr, "%lims\n", elapsed);

        // decrement the pointer to prepare for free_shellcmd()
        t->argv--;
        t->argc++;
    }
    // no shell command received
    else
    {
        fprintf(stderr, "Usage: time command...\n");
    }

    return status;
}
