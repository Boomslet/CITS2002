#include "myshell.h"

/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */

//  THREE INTERNAL VARIABLES (SEE myshell.h FOR EXPLANATION)
char *HOME, *PATH, *CDPATH;

char *argv0        = NULL; // the program's name
bool interactive   = false;
int lastcmd_status = 0; // exit status of the last executed command

// ------------------------------------------------------------------------

void check_allocation0(void *p, char *file, const char *func, int line)
{
    if (p == NULL)
    {
        fprintf(stderr, "%s, %s() line %i: unable to allocate memory\n",
          file, func, line);
        exit(2);
    }
}

static void print_redirection(SHELLCMD *t)
{
    if (t->infile != NULL)
        printf("< %s ", t->infile);
    if (t->outfile != NULL)
    {
        if (t->append == false)
            printf(">");
        else
            printf(">>");
        printf(" %s ", t->outfile);
    }
}

void print_shellcmd0(SHELLCMD *t)
{
    if (t == NULL)
    {
        printf("<nullcmd> ");
        return;
    }

    switch (t->type)
    {
        case CMD_COMMAND:
            for (int a = 0; a < t->argc; a++)
                printf("%s ", t->argv[a]);
            print_redirection(t);
            break;

        case CMD_SEMICOLON:
            print_shellcmd0(t->left); printf("; "); print_shellcmd0(t->right);
            break;

        case CMD_AND:
            print_shellcmd0(t->left); printf("&& "); print_shellcmd0(t->right);
            break;

        case CMD_OR:
            print_shellcmd0(t->left); printf("|| "); print_shellcmd0(t->right);
            break;

        case CMD_SUBSHELL:
            printf("( "); print_shellcmd0(t->left); printf(") ");
            print_redirection(t);
            break;

        case CMD_PIPE:
            print_shellcmd0(t->left); printf("| "); print_shellcmd0(t->right);
            break;

        case CMD_BACKGROUND:
            print_shellcmd0(t->left); printf("& "); print_shellcmd0(t->right);
            break;

        default:
            fprintf(stderr, "%s: invalid CMDTYPE in print_shellcmd0()\n", argv0);
            exit(EXIT_FAILURE);
            break;
    }
}
