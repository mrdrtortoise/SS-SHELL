#include "s3.h"

int main(int argc, char *argv[])
{

    /// Is updated throughout the program with the new line while iterating through batched commands
    char line[MAX_LINE];
    // command is construced once from read_command_line at the beginning of the program
    char command[MAX_LINE];
    // stores the index of the start for the next batched command
    int curr_idx = 0;

    /// Storage for Pipeline operations
    int idx[MAX_ARGS];
    idx[0] = 0;
    int idx_count;
    bool sel = false;
    // calling from main, not from the launch_pipeline function
    bool from_pipeline = false;

    /// The last (previous) working directory
    char lwd[MAX_PROMPT_LEN - 6];

    // init_s3();
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

    // stores the number of commands (if batched input)
    int batch_count = 0;

    while (1)
    {
        outfile = NULL;
        infile = NULL;
        read_command_line(line, lwd); /// Notice the additional parameter (required for prompt construction)

        batch_count = batched_command(line) + 1;
        if (batch_count > 1)
        {
            for (int k = 0; k < batch_count; k++)
            {
                get_next_command(line, command, &curr_idx);
                if (command_with_pipes(command))
                {
                    sel = true;
                    idx_count = 1;
                    status = parse_command(command, args, &argsc, idx, &idx_count, &sel);
                    if (!status)
                    {
                        run_pipeline(args, &argc, idx, &idx_count);
                        reap_all();
                    }
                }
                else if (command_with_redirection(command, NULL, 0))
                { /// Command with redirection
                    status = parse_command_with_redirection(command, args, &argsc, &outfile, &infile, &append, 1);
                    if (!status)
                    {
                        // pass NULL as pid_pipeline because we are running it from main. Fork will happen inside this function
                        // and not inside launch_pipeline
                        launch_program_with_redirection(args, argsc, &outfile, &infile, &append, &from_pipeline, NULL);
                        reap();
                    }
                }
                else if (is_cd(command))
                { /// Command cd
                    parse_command(command, args, &argsc, NULL, NULL, NULL);
                    run_cd(args, argsc, lwd);
                    reap();
                }
                else /// Basic command
                {
                    if (parse_command(command, args, &argsc, NULL, NULL, NULL) == 0)
                    {
                        // pass NULL as pid_pipeline because we are running it from main. Fork will happen inside this function
                        // and not inside launch_pipeline
                        launch_program(args, argsc, &from_pipeline, NULL);
                        reap();
                    }
                }
            }
        }
        else
        {
            if (command_with_pipes(line))
            {
                sel = true;
                idx_count = 1;
                status = parse_command(line, args, &argsc, idx, &idx_count, &sel);
                if (!status)
                {
                    run_pipeline(args, &argc, idx, &idx_count);
                    reap_all();
                }
            }
            else if (command_with_redirection(line, NULL, 0))
            { /// Command with redirection
                status = parse_command_with_redirection(line, args, &argsc, &outfile, &infile, &append, 1);
                if (!status)
                {
                    // pass NULL as pid_pipeline because we are running it from main. Fork will happen inside this function
                    // and not inside launch_pipeline
                    launch_program_with_redirection(args, argsc, &outfile, &infile, &append, &from_pipeline, NULL);
                    reap();
                }
            }
            else if (is_cd(line))
            { /// Command cd
                parse_command(line, args, &argsc, NULL, NULL, NULL);
                run_cd(args, argsc, lwd);
                reap();
            }
            else /// Basic command
            {
                if (parse_command(line, args, &argsc, NULL, NULL, NULL) == 0)
                {
                    // pass NULL as pid_pipeline because we are running it from main. Fork will happen inside this function
                    // and not inside launch_pipeline
                    launch_program(args, argsc, &from_pipeline, NULL);
                    reap();
                }
            }
        }
    }

    return 0;
}
