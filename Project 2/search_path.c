#include "myshell.h"
#include <dirent.h>


/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: search_path
 * ----------------------------
 * @brief    Searches for the directory path in which the command to be
 *           executed is located
 *
 * @param *cmd    char pointer to character array containing the command
 *                name be searched for
 *
 * @param *path    char pointer to character array containing a colon ':'
 *                 separated list of directory names
 *
 * @return *check_path    char pointer to a character array containing
 *                        the directory path of the command
 */

char *search_path(char *cmd, char *path)
{
    struct dirent *dp;
    DIR *dirp;

    // make a copy of path to tokenise
    char *check_path = strdup(path);

    check_path = strtok(check_path, ":");

    while (check_path != NULL)
    {
        dirp = opendir(check_path);

        // failed to open directory
        if (dirp == NULL)
        {
            // could report error but will be caught in execute_command
            // perror("opendir()");
            return "";
        }

        while ((dp = readdir(dirp)) != NULL)
        {
            // compare command name to directory name
            if (!strcmp(cmd, dp->d_name))
            {
                // match found
                closedir(dirp);
                return check_path;
            }
        }

        check_path = strtok(NULL, ":");

        closedir(dirp);
    }

    return "";
}

/**
 * Function: update_shellcmd
 * ----------------------------
 * @brief    Updates the input shell command to contain the full directory path
 *           of the command to be executed
 *
 * @param *t    SHELLCMD pointer to shell command to be updated
 *
 * @param *path    char pointer to character array containing the path
 *                 of the command to be executed
 *
 * @return void
 */

void update_shellcmd(SHELLCMD *t, char *path)
{
    // strdup null-terminates the char array cmd
    char *cmd = strdup(t->argv[0]);

    // so we only account for an additional byte (for '/' or '\0')
    t->argv[0] = realloc(t->argv[0], sizeof(cmd) + sizeof(path) + 1);

    // memory allocation failed
    if (t->argv[0] == NULL)
    {
        perror("realloc()");
        return;
    }

    // update shell command
    sprintf(t->argv[0], "%s/%s", path, cmd);
}
