#ifndef _S3_H_
#define _S3_H_

/// See reference for what these libraries provide
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <stdbool.h>

/// Constants for array sizes, defined for clarity and code readability
#define MAX_LINE 1024
#define MAX_ARGS 128
#define MAX_PROMPT_LEN 256

/// Enum for readable argument indices (use where required)
enum ArgIndex
{
    ARG_PROGNAME,
    ARG_1,
    ARG_2,
    ARG_3,
};

/// With inline functions, the compiler replaces the function call
/// with the actual function code;
/// inline improves speed and readability; meant for short functions (a few lines).
/// the static here avoids linker errors from multiple definitions (needed with inline).
static inline void reap()
{
    wait(NULL);
}
static inline void reap_all()
{
    while (wait(NULL) > 0)
        ;
}

/// Text Art
void init_s3();

/// Shell I/O and related functions (add more as appropriate)
void read_command_line(char line[], char lwd[]);
void construct_shell_prompt(char shell_prompt[]);
void get_next_command(char *line, char *command, int *curr_idx);

int batched_command(char *line);
int command_with_pipes(char *line);
int command_with_redirection(char *line, char *args[], int sel);
int parse_command_with_redirection(char line[], char *args[], int *argsc, char **outfile, char **infile, int *append, int sel);
int parse_command(char line[], char *args[], int *argsc, int *idx, int *idx_count, bool *sel);

/// functions for pipe stages
void run_pipeline(char *args[], int *argsc, int *idx, int *idx_count);
void create_pipes(int pipes[][2], char *args[], int *argsc, int *idx, int *idx_count);

/// Child functions (add more as appropriate)
void child(char *args[], int argsc);
void child_with_input_redirection(char *args[], int argsc, char *infile);
void child_with_output_redirection(char *args[], int argsc, char *outfile, int append);
void child_with_IaO_redirection(char *args[], int argsc, char *outfile, char *infile, int append);

/// Program launching functions (add more as appropriate)
void launch_program(char *args[], int argsc, bool *from_pipeline, pid_t *pid_pipeline);
void launch_program_with_redirection(char *args[], int argsc, char **outfile, char **infile, int *append, bool *from_pipeline, pid_t *pid_pipeline);
// i is the index of the command in the overall comman typed by the user
// pipes has all the file descriptors for the pipelines so that each child can close all the ones it doesnt need
void launch_pipelined_program(int pipes[][2], int i, int idx_count, char *args[], int argsc);

/// Wrapper Functions for Systemcalls
pid_t Fork(void);
void Execvp(char *file, char *argv[]);
void Pipe(int pipefd[2]);
int Dup2(int oldfile, int newfile);

/// Functions for cd implementation
void init_lwd(char lwd[]);
int is_cd(char line[]);
void run_cd(char *args[], int argsc, char lwd[]);
#endif