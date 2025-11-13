#include "s3.h"

void init_s3()
{
    printf("\n\n  █████████   ████████      █████████  █████   █████ ██████████ █████       █████      \n"
           "███▒▒▒▒▒███ ███▒▒▒▒███    ███▒▒▒▒▒███▒▒███   ▒▒███ ▒▒███▒▒▒▒▒█▒▒███       ▒▒███       \n"
           "▒███    ▒▒▒ ▒▒▒    ▒███   ▒███    ▒▒▒  ▒███    ▒███  ▒███  █ ▒  ▒███        ▒███       \n"
           "▒▒█████████    ██████▒    ▒▒█████████  ▒███████████  ▒██████    ▒███        ▒███       \n"
           "▒▒▒▒▒▒▒▒███  ▒▒▒▒▒▒███    ▒▒▒▒▒▒▒▒███ ▒███▒▒▒▒▒███  ▒███▒▒█    ▒███        ▒███       \n"
           "███    ▒███ ███   ▒███    ███    ▒███ ▒███    ▒███  ▒███ ▒   █ ▒███      █ ▒███      █\n"
           "▒▒█████████ ▒▒████████    ▒▒█████████  █████   █████ ██████████ ███████████ ███████████\n"
           "▒▒▒▒▒▒▒▒▒   ▒▒▒▒▒▒▒▒      ▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒   ▒▒▒▒▒ ▒▒▒▒▒▒▒▒▒▒ ▒▒▒▒▒▒▒▒▒▒▒ ▒▒▒▒▒▒▒▒▒▒▒ \n\n");
}

/// Wrapper Functions for SystemCalls Implementation
pid_t Fork(void)
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("Fork Failed");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void Execvp(char *file, char *argv[])
{
    if (!file || !argv)
    {
        fprintf(stderr, "Execvp has invalid arguments\n");
        exit(EXIT_FAILURE);
    }
    int rc = execvp(file, argv);
    if (rc == -1)
    {
        perror("Execvp Failed");
        exit(EXIT_FAILURE);
    }
}

void Pipe(int pipefd[2])
{
    int rc = pipe(pipefd);
    if (rc == -1)
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
}

int Dup2(int oldfile, int newfile)
{
    int rc = dup2(oldfile, newfile);
    if (rc == -1)
    {
        perror("dup2 failed");
        exit(EXIT_FAILURE);
    }
    return rc;
}
/// Init lwd
void init_lwd(char lwd[])
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcpy(lwd, cwd);
    }
    else
    {
        perror("Error When Getting CWD");
    }
}

/// Simple for now, but will be expanded in a following section
void construct_shell_prompt(char shell_prompt[])
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcpy(shell_prompt, "[");
        strcat(shell_prompt, cwd);
        strcat(shell_prompt, " s3]$> ");
    }
    else
    {
        perror("Error When Getting CWD");
    }
}

/// Prints a shell prompt and reads input from the user
void read_command_line(char line[], char lwd[])
{
    char shell_prompt[MAX_PROMPT_LEN];
    construct_shell_prompt(shell_prompt);
    printf("%s", shell_prompt);

    /// See man page of fgets(...)
    if (fgets(line, MAX_LINE, stdin) == NULL)
    {
        perror("fgets failed");
        exit(1);
    }
    /// Remove newline (enter)
    line[strlen(line) - 1] = '\0';
}

void get_next_command(char *line, char *command, int *curr_idx)
{
    int i;
    if (*curr_idx == 0)
    {
        i = (*curr_idx);
    }
    else
    {
        i = (*curr_idx) + 1;
    }
    int j = 0;

    while (line[i] != ';' && line[i] != '\0' && j < MAX_LINE - 1)
    {
        command[j++] = line[i++];
    }
    command[j] = '\0';

    if (line[i] = ';')
    {
        i++;
    }
    *curr_idx = i;
}
/// Check Whether or not the Command has Input/Output Redirection.
/// Returns 1 if there is Redirection and 0 if none.
int command_with_redirection(char *line, char *args[], int sel)
{
    int found = 0;
    if (sel == 0)
    {
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] == '<')
            {
                found = 1;
                return found;
            }
            else if (line[i] == '>')
            {
                found = 1;
                return found;
            }
        }
        return found;
    }
    else if (sel == 1)
    {
        for (int i = 0; args[i] != NULL; i++)
        {
            if (strcmp(args[i], "<") == 0)
            {
                found = 1;
                return found;
            }
            else if (strcmp(args[i], ">") == 0)
            {
                found = 1;
                return found;
            }
            else if (strcmp(args[i], ">>") == 0)
            {
                found = 1;
                return found;
            }
        }
        found = 0;
        return found;
    }
    else
    {
        fprintf(stderr, "command_with_redirection has recieved third argument being different from 0 or 1");
        return -1;
    }
}

/// Check Whether or not the Command is a Cd command
/// Returns 1 if it is, 0 otherwise
int is_cd(char line[])
{
    if (strncmp(line, "cd", 2) == 0 && (line[2] == ' ' || line[2] == '\n' || line[2] == '\0'))
    {
        return 1;
    }
    return 0;
}

/// Check Whether or not the Command has Pipes
/// Returns 1 if yes, 0 otherwise
int command_with_pipes(char *line)
{
    int i = 0;
    while (line[i] != '\0')
    {
        if (line[i] == '|')
        {
            return 1;
        }
        i++;
    }
    return 0;
}

int batched_command(char *line)
{
    int i = 0;
    int count = 0;
    while (line[i] != '\0')
    {
        if (line[i] == ';')
        {
            count++;
        }
        i++;
    }
    return count;
}

int parse_command(char line[], char *args[], int *argsc, int *idx, int *idx_count, bool *sel)
{
    /// Implements simple tokenization (space delimited)
    /// Note: strtok puts '\0' (null) characters within the existing storage,
    /// to split it into logical cstrings.
    /// There is no dynamic allocation.

    /// See the man page of strtok(...)
    if (strlen(line) == 0)
    {
        return 1;
    }
    char *token = strtok(line, " ");
    *argsc = 0;
    while (token != NULL && *argsc < MAX_ARGS - 1)
    {
        args[(*argsc)++] = token;
        token = strtok(NULL, " ");
    }

    args[*argsc] = NULL; /// args must be null terminated

    if (sel != NULL && idx != NULL && idx_count != NULL)
    {
        for (int i = 0; args[i] != NULL; i++)
        {
            if (strcmp(args[i], "|") == 0)
            {
                if (args[i + 1] == NULL)
                {
                    fprintf(stderr, "Syntax Error: Expected Token after '|'\n");
                    return 1;
                }
                args[i] = NULL;
                idx[(*idx_count)++] = i + 1;
            }
        }
        idx[*idx_count] = -1;
    }
    return 0;
}

/// returns 0 if there was no error. 1 if there has been an error
// set sel to 1 to parse the line[]. set sel to 0 to skip that and directly figure out the input/output files
// for pipeline application where slice is already tokenized
int parse_command_with_redirection(char *line, char *args[], int *argsc, char **outfile, char **infile, int *append, int sel)
{
    if (sel)
    {
        if (line == NULL)
        {
            fprintf(stderr, "line cannot be NULL if sel is 1\n");
            return 1;
        }
        char *token = strtok(line, " ");
        *argsc = 0;
        while (token != NULL && *argsc < MAX_ARGS - 1)
        {
            args[(*argsc)++] = token;
            token = strtok(NULL, " ");
        }

        args[*argsc] = NULL;
    }
    *append = 0;
    for (int i = 0; i < *argsc; i++)
    {
        if (strcmp(args[i], "<") == 0)
        {
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "Syntax Error: No Input File Provided\n");
                return 1;
            }
            *infile = args[i + 1];
            args[i] = NULL;
            i++;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "Syntax Error: No Output File For Appending Provided\n");
                return 1;
            }
            *outfile = args[i + 1];
            args[i] = NULL;
            *append = 1;
            i++;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "Syntax Error: No Output File For Trucating Provided\n");
                return 1;
            }
            *outfile = args[i + 1];
            args[i] = NULL;
            *append = 0;
            i++;
        }
    }
    return 0;
}

/// Launch related functions
void child(char *args[], int argsc)
{
    /// Implement this function:

    /// Use execvp to load the binary
    /// of the command specified in args[ARG_PROGNAME].
    /// For reference, see the code in lecture 3.
    Execvp(args[0], args);
}

void launch_program(char *args[], int argsc, bool *from_pipeline, pid_t *pid_pipeline)
{
    /// Implement this function:

    /// fork() a child process.
    /// In the child part of the code,
    /// call child(args, argv)
    /// For reference, see the code in lecture 2.

    /// Handle the 'exit' command;
    /// so that the shell, not the child process,
    /// exits.
    int status;
    if (strcmp(args[0], "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }

    pid_t pid;
    if (!(*from_pipeline))
    {
        pid = Fork();
    }
    else
    {
        pid = (*pid_pipeline);
    }

    if (pid == 0)
    {
        child(args, argsc);
    }
}

void launch_program_with_redirection(char *args[], int argsc, char **outfile, char **infile, int *append, bool *from_pipeline, pid_t *pid_pipeline)
{
    // TODO
    // Make the Fork Call
    // call child_with_redirection
    // should get as input the outfile and infile so
    // it can adjust the STDIN and STDOUT accordingly
    int status;
    pid_t pid;
    if (!(*from_pipeline))
    {
        pid = Fork();
    }
    else
    {
        pid = *pid_pipeline;
    }
    if (pid == 0)
    {
        if (*infile != NULL && *outfile != NULL)
        {
            child_with_IaO_redirection(args, argsc, *outfile, *infile, *append);
        }
        else if (*infile != NULL && *outfile == NULL)
        {
            child_with_input_redirection(args, argsc, *infile);
        }
        else
        {
            child_with_output_redirection(args, argsc, *outfile, *append);
        }
    }
}

void child_with_output_redirection(char *args[], int argsc, char *outfile, int append)
{
    // TODO
    // change the file descritors using dup2() as given by outfile
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int fd = open(outfile, flags, 0644);
    if (fd < 0)
    {
        perror("Error when opening output\n");
        exit(EXIT_FAILURE);
    }
    Dup2(fd, STDOUT_FILENO);
    close(fd);
    Execvp(args[0], args);
}

void child_with_input_redirection(char *args[], int argsc, char *infile)
{
    // TODO
    // change the file descritors using dup2() as given by infile
    int fd = open(infile, O_RDONLY);
    if (fd < 0)
    {
        perror("Error when opening input\n");
        exit(EXIT_FAILURE);
    }
    Dup2(fd, STDIN_FILENO);
    close(fd);
    Execvp(args[0], args);
}

void child_with_IaO_redirection(char *args[], int argsc, char *outfile, char *infile, int append)
{
    // TODO
    // change the file descritors using dup2() as given by outfile AND infile
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int fd_out = open(outfile, flags, 0644);
    int fd_in = open(infile, O_RDONLY);
    if (fd_out < 0 || fd_in < 0)
    {
        perror("Error when opening output/input\n");
        exit(EXIT_FAILURE);
    }
    Dup2(fd_out, STDOUT_FILENO);
    Dup2(fd_in, STDIN_FILENO);
    close(fd_out);
    close(fd_in);
    Execvp(args[0], args);
}

void run_cd(char *args[], int argsc, char lwd[])
{

    char prev_cwd[PATH_MAX];
    if (getcwd(prev_cwd, sizeof(prev_cwd)) == NULL)
    {
        perror("Error When Getting CWD");
    }

    if (args[1] == NULL)
    {
        if (chdir("/") != 0)
        {
            perror("Could not change directory to root\n");
        }
    }
    else
    {
        if (strcmp(args[1], "-") == 0)
        {
            if (chdir(lwd) != 0)
            {
                perror("Could not change directory to last directory");
            }
        }
        else if (chdir(args[1]) != 0)
        {
            perror("Could not change current directory\n");
        }
    }
    strcpy(lwd, prev_cwd);
}

void create_pipes(int pipes[][2], char *args[], int *argsc, int *idx, int *idx_count)
{
    for (int i = 0; i < ((*idx_count) - 1); i++)
    {
        Pipe(pipes[i]);
    }
}

// Decides on either redirection or no redirection launch calls for each pipeline stage
void launch_pipelined_program(int pipes[][2], int i, int idx_count, char *args[], int argsc)
{
    char *outfile = NULL;
    char *infile = NULL;
    int append = 0;
    int slice_argsc = argsc;
    int n = idx_count - 1;
    bool from_pipeline = true;
    // actually need to make the fork call here in order to change the FD to fdin and fdout
    // that would mean that I have to change the launch_program_with_direction
    // to include handle children that have already been forked.
    // here, the only purpose of launch_program_with_redirection is overwriting
    // the file descriptors so that the output/input goes to wherever it is redirected
    // and not the next /previous pipeline stage
    pid_t pid = Fork();
    if (pid == 0)
    {
        if (i == 0)
        {
            Dup2(pipes[0][1], STDOUT_FILENO);
        }
        else if (i == n)
        {
            Dup2(pipes[n - 1][0], STDIN_FILENO);
        }
        else
        {
            Dup2(pipes[i - 1][0], STDIN_FILENO);
            Dup2(pipes[i][1], STDOUT_FILENO);
        }
        for (int k = 0; k < n; k++)
        {
            close(pipes[k][0]);
            close(pipes[k][1]);
        }

        if (command_with_redirection(NULL, args, 1))
        {
            parse_command_with_redirection(NULL, args, &slice_argsc, &outfile, &infile, &append, 0);
            launch_program_with_redirection(args, argsc, &outfile, &infile, &append, &from_pipeline, &pid);
        }
        else
        {
            launch_program(args, argsc, &from_pipeline, &pid);
        }
    }
}

void run_pipeline(char *args[], int *argsc, int *idx, int *idx_count)
{
    char *slice[MAX_ARGS];
    // pipes is initialized to half*2 the max number of args (if you have 128 args and every second is a pipe then you would have max 64 pipes * 2 = 128)
    int pipes[(*idx_count) - 1][2];
    int slice_count;
    create_pipes(pipes, args, argsc, idx, idx_count);
    /*for (int i = 0; pipes[i] != -1; i++)
    {
        printf("[idx %d] of pipes is : %d\n", i, pipes[i]);
    }*/
    for (int i = 0; i < (*idx_count); i++)
    {
        slice_count = 0;
        for (int j = (idx[i]); args[j] != NULL; j++)
        {
            slice[slice_count] = args[j];
            slice_count++;
        }
        // every slice is now a single pseudo arg char* array that contains the command between pipelines
        // Now we need to do the same as in main. check whether this slice is a command with redirection, or a basic command
        // then parse the char * array based on that.
        // then we need to implement the pipelineb between those commands
        // maybe we could directly fork idx_count children (thats how many programs will be running in the pipeline)
        // then do the pipeline input output connections before execvp.
        // how would I then overwrite pipeline output and input redirection?
        slice[slice_count] = NULL;
        launch_pipelined_program(pipes, i, (*idx_count), slice, slice_count);
    }
    for (int i = 0; i < (*idx_count) - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}