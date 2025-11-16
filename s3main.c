#include "s3.h"

int main(int argc, char *argv[])
{
    /// Is updated throughout the program with the new line while iterating through batched commands
    char line[MAX_LINE] = "";
    // command is construced once from read_command_line at the beginning of the program
    char command[MAX_LINE];
    // stores the index of the start for the next batched command
    int curr_idx = 0;

    /// The last (previous) working directory

    bool is_subshell = false;
    int i = 2;
    if(argc > 1){
        if(strcmp(argv[1], "-c") == 0){
            is_subshell = true;
            while(argv[i] != NULL){
                strcat(line, argv[i]);
                strcat(line, " ");
                i++;
            }
        }
    }

    char lwd[MAX_PROMPT_LEN - 6];
    if(!is_subshell){
        init_s3();
    }
    init_lwd(lwd); /// Implement this function: initializes lwd with the cwd (using getcwd)

    int batch_count = 0;

    while (1)
    {
        if(!is_subshell){
            read_command_line(line, lwd); /// Notice the additional parameter (required for prompt construction)
        }

        batch_count = batched_command(line) + 1;
        if (batch_count > 1)
        {
            for (int k = 0; k < batch_count; k++)
            {
                memset(command, 0, sizeof(command));
                get_next_command(line, command, &curr_idx);
                run_command(command, argc, lwd);
            }
        }
        else
        {
            run_command(line, argc, lwd);
        }
        if(is_subshell){
            break;
        }
    }

    return 0;
}
