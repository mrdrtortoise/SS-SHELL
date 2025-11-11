#include "s3.h"

int main(int argc, char *argv[])
{

    /// Stores the command line input
    char line[MAX_LINE];

    /// Storage for Pipeline operations
    int idx[MAX_ARGS];
    idx[0] = 0;
    int idx_count;
    bool sel = false;

    /// The last (previous) working directory
    char lwd[MAX_PROMPT_LEN - 6];

    init_s3();
    init_lwd(lwd); /// Implement this function: initializes lwd with the cwd (using getcwd)

    // Stores pointers to command arguments.
    /// The first element of the array is the command name.

    /// Stores pointers to command arguments.
    /// The first element of the array is the command name.
    char *args[MAX_ARGS];

    /// Stores the number of arguments
    int argsc;

    /// status is referenced to check whether parsed command is valid or not
    int status;

    /// Stores Input and Output file states for IO redirection
    char *outfile = NULL;
    char *infile = NULL;
    int append = 0;

    while (1)
    {

        read_command_line(line, lwd); /// Notice the additional parameter (required for prompt construction)

        if (command_with_pipes(line))
        {
            sel = true;
            idx_count = 1;
            status = parse_command(line, args, &argsc, idx, &idx_count, &sel);
            if (!status)
            {
                run_pipeline(args, &argc, idx, &idx_count);
            }
        }
        else if (is_cd(line))
        { /// Command cd
            parse_command(line, args, &argsc, NULL, NULL, NULL);
            run_cd(args, argsc, lwd);
        }
        else if (command_with_redirection(line, NULL, 0))
        { /// Command with redirection
            status = parse_command_with_redirection(line, args, &argsc, &outfile, &infile, &append);
            if (!status)
            {
                launch_program_with_redirection(args, argsc, &outfile, &infile, &append);
                reap();
            }
        }
        else /// Basic command
        {
            parse_command(line, args, &argsc, NULL, NULL, NULL);
            launch_program(args, argsc);
            reap();
        }
    }

    return 0;
}
