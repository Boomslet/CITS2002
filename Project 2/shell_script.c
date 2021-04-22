#include "myshell.h"


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */

/**
 * Function: check_shellscript
 * ----------------------------
 * @brief    Checks for user-access of the input file, and runs the file as a
 *           shellscript if permitted
 *
 * @param *t    SHELLCMD pointer to file to be checked
 *
 * @return status    Integer exit status of executed shellscript
 */

int check_shellscript(SHELLCMD *t)
{
    // check file accessibility
    int accessible = access(t->argv[0], F_OK);

    int status = EXIT_FAILURE;

    if (accessible == 0)
    {
        FILE *shellscript_fp;
        int pid;

        // signal that we are not connected to an interactive terminal
        interactive = false;

        switch (pid = fork())
        {
            // fork failed
            case -1:
                perror("fork()");
                status = EXIT_FAILURE;
                break;

            case 0:
                shellscript_fp = fopen(t->argv[0], "r");

                // read shellscript_fp to EOF
                while (!feof(shellscript_fp))
                {
                    // create a new shell command for every line of shellscript
                    SHELLCMD *t = parse_shellcmd(shellscript_fp);

                    if (t != NULL)
                    {
                        // execute new shell command
                        execute_shellcmd(t);
                        free_shellcmd(t);
                    }
                }

                fclose(shellscript_fp);

                status = EXIT_SUCCESS;

                _exit(status);
                break;

            default:
                signal(SIGUSR1, SIG_IGN);
                wait(&status);
                break;
        }

        // replace interactive boolean
        interactive = (isatty(fileno(stdin)) && isatty(fileno(stdout)));
    }

    return status;
}
