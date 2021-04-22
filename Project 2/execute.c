#include "myshell.h"

/*
 * CITS2002 Project 2 2017
 * Name(s):		        Mark Boon, Aaron Norrish
 * Student number(s):	21750965, 21972015
 * Date:		        2/11/2017
 */


/**
 * Function: execute_shellcmd
 * ----------------------------
 * @brief    Traverses the command-tree and executes the commands that it holds
 *
 * @param *t    SHELLCMD pointer to shell command to be executed
 *
 * @return exitstatus    Integer exit status of executed shell command
 */
void sighand(int signum)
{
    int pid = getpid();

    fprintf(stderr, "wtf%d\n", pid);
    // exit(0);
}

int execute_shellcmd(SHELLCMD *t)
{
    signal(SIGTERM, SIG_IGN);
    int exitstatus;


    // error: shell command is null
    if (t == NULL)
    {
        fprintf(stderr, "%s: shell command == NULL\n", argv0);
        exitstatus = EXIT_FAILURE;
    }
    else
    {
        switch (t->type)
        {
            // executes a single command
            case CMD_COMMAND:
                // check and execute any internal commands
                if (!strcmp(t->argv[0], "exit"))
                {
                    run_exit(t);

                    // failed to exit
                    exitstatus = EXIT_FAILURE;
                    break;
                }

                else
                if (!strcmp(t->argv[0], "cd"))
                {
                    exitstatus = run_cd(t);
                    break;
                }

                else
                if (!strcmp(t->argv[0], "time"))
                {
                    exitstatus = run_time(t);
                    break;
                }

                // check if directory path is specified, otherwise search PATH
                else
                if (!strchr(t->argv[0], '/'))
                {
                    update_shellcmd(t, search_path(t->argv[0], PATH));
                }

                exitstatus = execute_command(t);
                break;

            // executes left and then right subtree unconditionally
            case CMD_SEMICOLON:
                exitstatus = execute_shellcmd(t->left);
                exitstatus = execute_shellcmd(t->right);
                break;

            // executes right subtree if left subtree executes successfully
            case CMD_AND:
                exitstatus = execute_shellcmd(t->left);
                wait(NULL);

                if (exitstatus == EXIT_SUCCESS)
                {
                    exitstatus = execute_shellcmd(t->right);
                }
                break;

            // executes right subtree if left subtree executes unsuccesfully
            case CMD_OR:
                exitstatus = execute_shellcmd(t->left);

                if (exitstatus == EXIT_FAILURE)
                {
                    exitstatus = execute_shellcmd(t->right);
                }
                break;

            // executes shell command within a sub copy of myshell
            case CMD_SUBSHELL:
                // handles an edge-case in subshell
                signal(SIGUSR1, SIG_IGN);

                exitstatus = create_subshell(t);
                break;

            // pipes stdout of left subtree to stdin of right subtree
            case CMD_PIPE:
                exitstatus = open_pipeline(t->left, t->right);
                break;

            // executes left subtree in backgrond
            // and executes right subtree if exists
            case CMD_BACKGROUND:
                exitstatus = run_background(t->left);

                if (t->right)
                {
                    exitstatus = execute_shellcmd(t->right);
                }
                break;

            // error: shell command has no recognised type
            default:
                fprintf(stderr, "%s: command type not recognised\n", argv0);
                exitstatus = EXIT_FAILURE;
                break;
        }
    }

    return exitstatus;
}
