#include "myshell.h"
#include <sys/stat.h>


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: redirect_outfile
 * ----------------------------
 * @brief    Redirects the shell command's output to attached outfile, with
 *           the append boolean signalling the access mode (append / truncate)
 *
 * @param *t    SHELLCMD pointer to shell command to be redirected
 *
 * @return void
 */

void redirect_outfile(SHELLCMD *t)
{
    int fd_out;

    if (t->append)
    {
        // append is true, open file for appending
        fd_out = open(t->outfile, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    }
    else
    {
        // append is false, open file for truncating
        fd_out = open(t->outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    }

    // failed to open
    if (fd_out == -1)
    {
        perror("open()");
        return;
    }

    // duplicate outfile descriptor to stdout
    dup2(fd_out, fileno(stdout));
    close(fd_out);
}

/**
 * Function: redirect_infile
 * ----------------------------
 * @brief    Redirects the stdin of shell command to the attached infile
 *
 * @param *t    SHELLCMD pointer to shell command to be redirected
 *
 * @return void
 */

void redirect_infile(SHELLCMD *t)
{
    int fd_in;

    // open with read-only permission
    fd_in = open(t->infile, O_RDONLY);

    // failed to open
    if (fd_in == -1)
    {
        perror("open()");
        return;
    }

    // duplicate infile descriptor to stdin
    dup2(fd_in, fileno(stdin));
    close(fd_in);
}

/**
 * Function: check_redirects
 * ----------------------------
 * @brief    Checks the input shell command for non-NULL
 *           infile and outfile fields
 *
 * @param *t    SHELLCMD pointer to shell command to be checked
 *
 * @return void
 */

void check_redirects(SHELLCMD *t)
{
    if (t->outfile != NULL)
    {
        redirect_outfile(t);
    }

    if (t->infile != NULL)
    {
        redirect_infile(t);
    }
}
