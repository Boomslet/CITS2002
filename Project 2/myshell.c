#include "myshell.h"


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


int main(int argc, char *argv[])
{
    //  REMEMBER THE PROGRAM'S NAME (TO REPORT ANY LATER ERROR MESSAGES)
    argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
    argc--; // skip 1st command-line argument
    argv++;

    //  INITIALIZE THE THREE INTERNAL VARIABLES
    HOME = getenv("HOME");
    if (HOME == NULL)
    {
        HOME = DEFAULT_HOME;
    }

    PATH = getenv("PATH");
    if (PATH == NULL)
    {
        PATH = DEFAULT_PATH;
    }

    CDPATH = getenv("CDPATH");
    if (CDPATH == NULL)
    {
        CDPATH = DEFAULT_CDPATH;
    }

    //  DETERMINE IF THIS SHELL IS INTERACTIVE
    interactive = (isatty(fileno(stdin)) && isatty(fileno(stdout)));

    int exitstatus = EXIT_SUCCESS;

    //  READ AND EXECUTE COMMANDS FROM stdin UNTIL IT IS CLOSED (with control-D)
    while (!feof(stdin))
    {
        SHELLCMD *t = parse_shellcmd(stdin);

        if (t != NULL)
        {
            //  WE COULD DISPLAY THE PARSED COMMAND-TREE, HERE, BY CALLING:
            //	    print_shellcmd(t);

            exitstatus = execute_shellcmd(t);
            free_shellcmd(t);

            // update the lastcmd_status global variable
            lastcmd_status = exitstatus;
        }
    }
    if (interactive)
    {
        fputc('\n', stdout);
    }
    return exitstatus;
}
